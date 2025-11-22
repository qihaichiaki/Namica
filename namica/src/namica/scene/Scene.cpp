#include "namica/scene/Scene.h"
#include "namica/scene/Entity.h"
#include "namica/scene/Components.h"
#include "namica/core/Log.h"
#include "namica/renderer/Renderer.h"
#include "namica/renderer/EditorCamera.h"
#include "namica/renderer/Renderer2D.h"

namespace Namica
{

// Copy 组件数据部分

// 依据_enttMap中记录的src entity中的UUID, 对应上dst中新Entity, 随后拷贝其src中的entity中各个组件
template <typename Component>
static void copyComponent(entt::registry& _dstRegistry,
                          entt::registry& _srcRegistry,
                          std::unordered_map<UUID, entt::entity> const& _enttMap)
{
    auto view = _srcRegistry.view<Component>();
    for (auto enid : view)
    {
        UUID id = _srcRegistry.get<IDComponent>(enid).id;
        NAMICA_CORE_ASSERT(_enttMap.find(id) != _enttMap.end(), "未知的UUID对象");

        entt::entity newEnid = _enttMap.at(id);
        _dstRegistry.emplace_or_replace<Component>(
            newEnid, view.template get<Component>(enid));  // 调用各个组件的拷贝构造函数
    }
}

template <typename Component>
static void copyComponent(Entity _dst, Entity _src)
{
    if (_src.hasComponent<Component>())
    {
        _dst.addComponent<Component>(_src.getComponent<Component>());
    }
}

struct ComponentInfo
{
    using EntityCopyFunc = std::function<void(Entity _dst, Entity _src)>;
    using SceneCopyFunc =
        std::function<void(entt::registry& _dstRegistry,
                           entt::registry& _srcRegistry,
                           std::unordered_map<UUID, entt::entity> const& _enttMap)>;

    EntityCopyFunc entityCopyFunc;
    SceneCopyFunc sceneCopyFunc;
};

// component注册表, 可以统一管理各种类型component的统一行为
class ComponentRegistry
{
public:
    template <typename Component>
    static void componentRegister()
    {
        ComponentInfo info{};
        info.entityCopyFunc = [](Entity _dst, Entity _src) {
            copyComponent<Component>(_dst, _src);
        };
        info.sceneCopyFunc = [](entt::registry& _dstRegistry,
                                entt::registry& _srcRegistry,
                                std::unordered_map<UUID, entt::entity> const& _enttMap) {
            copyComponent<Component>(_dstRegistry, _srcRegistry, _enttMap);
        };

        get().m_components.emplace_back(info);
    }

    static void entityCopy(Entity _dst, Entity _src)
    {
        for (auto& info : get().m_components)
        {
            info.entityCopyFunc(_dst, _src);
        }
    }

    static void sceneCopy(entt::registry& _dstRegistry,
                          entt::registry& _srcRegistry,
                          std::unordered_map<UUID, entt::entity> const& _enttMap)
    {
        for (auto& info : get().m_components)
        {
            info.sceneCopyFunc(_dstRegistry, _srcRegistry, _enttMap);
        }
    }

private:
    static ComponentRegistry& get()
    {
        static ComponentRegistry instance;
        return instance;
    }

private:
    std::vector<ComponentInfo> m_components;
};

void Scene::onCameraComponentAdded(entt::registry& _registry, entt::entity _entity)
{
    CameraComponent& cameraComponent{_registry.get<CameraComponent>(_entity)};
    cameraComponent.camera.updateViewportSize(m_viewportWidth, m_viewportHeight);
}

Scene::Scene()
{
    // 组件注册表, 控制组件在场景中的相关行为
    // TODO: 添加SceneManger后统一管理
    static bool isInit{false};
    if (!isInit)
    {
        ComponentRegistry::componentRegister<TransformComponent>();
        ComponentRegistry::componentRegister<CameraComponent>();
        ComponentRegistry::componentRegister<SpriteRendererComponent>();
        ComponentRegistry::componentRegister<CircleRendererComponent>();
        ComponentRegistry::componentRegister<NativeScriptComponent>();
        ComponentRegistry::componentRegister<Rigidbody2DComponent>();
        ComponentRegistry::componentRegister<BoxCollider2DComponent>();
        ComponentRegistry::componentRegister<CircleCollider2DComponent>();
        isInit = true;
    }

    // 增加组件添加时行为

    // 相机组件添加时初始化行为
    m_registry.on_construct<CameraComponent>().connect<&Scene::onCameraComponentAdded>(this);
}

Scene::~Scene()
{
}

Ref<Scene> Scene::create()
{
    return createRef<Scene>();
}

Ref<Scene> Scene::copy(Ref<Scene> const& _other)
{
    Ref<Scene> newScene{createRef<Scene>()};
    // 常规的其他属性

    std::unordered_map<UUID, entt::entity> enttMap;
    auto otherEntitiesView = _other->m_registry.view<IDComponent>();
    for (auto enid : otherEntitiesView)
    {
        IDComponent& idComponent{_other->m_registry.get<IDComponent>(enid)};
        TagComponent& tagComponent{_other->m_registry.get<TagComponent>(enid)};
        enttMap.emplace(idComponent.id, newScene->createEntity(idComponent.id, tagComponent.name));
    }

    // 复制组件
    ComponentRegistry::sceneCopy(newScene->m_registry, _other->m_registry, enttMap);

    return newScene;
}

bool Scene::containsEntity(Entity _entity) const
{
    return &(_entity.getScene()) == this;
}

Entity Scene::createEntity(std::string const& _name)
{
    Entity entity{m_registry.create(), this};

    entity.addComponent<IDComponent>();
    entity.addComponent<TagComponent>(_name.empty() ? "新实体" : _name);
    entity.addComponent<TransformComponent>();

    return entity;
}

Entity Scene::createEntity(const UUID& _uuid, std::string const& _name)
{
    Entity entity{m_registry.create(), this};

    entity.addComponent<IDComponent>(_uuid);
    entity.addComponent<TagComponent>(_name.empty() ? "新实体" : _name);
    entity.addComponent<TransformComponent>();

    return entity;
}

bool Scene::destoryEntity(Entity _entity)
{
    if (containsEntity(_entity))
    {
        m_registry.destroy(_entity);
        return true;
    }
    else
    {
        NAMICA_CORE_WARN("删除了非当前场景的实体对象: {}", _entity.getName());
    }

    return false;
}

Entity Scene::copyEntity(Entity _entity)
{
    Entity newEntity{};
    if (_entity.isValid())
    {
        newEntity = createEntity(_entity.getName());
        ComponentRegistry::entityCopy(newEntity, _entity);
    }
    else
    {
        NAMICA_CORE_WARN("拷贝了不存在的实体");
    }

    return newEntity;
}

void Scene::onViewportResize(uint32_t _width, uint32_t _height)
{
    m_viewportWidth = _width;
    m_viewportHeight = _height;

    auto camerasView = m_registry.view<CameraComponent>();
    for (auto enid : camerasView)
    {
        CameraComponent& cameraComponent{camerasView.get<CameraComponent>(enid)};
        if (!cameraComponent.fixedAspectRatio)
        {
            cameraComponent.camera.updateViewportSize(m_viewportWidth, m_viewportHeight);
        }
    }
}

void Scene::onUpdateEditor(Timestep _ts, EditorCamera const& _editorCamera)
{
    // 渲染更新
    Renderer::beginRender(_editorCamera.getProjectionView());

    Renderer::clear();

    // SpriteRendererComponent
    m_registry.group<TransformComponent, SpriteRendererComponent>().each(
        [](entt::entity _enid,
           TransformComponent& _transform,
           SpriteRendererComponent& _spriteRenderer) {
            if (_spriteRenderer.texture)
            {
                Renderer2D::drawQuad(_transform.getTransform(),
                                     _spriteRenderer.texture,
                                     _spriteRenderer.color,
                                     _spriteRenderer.tilingFactor,
                                     static_cast<int>(_enid));
            }
            else
            {
                Renderer2D::drawQuad(
                    _transform.getTransform(), _spriteRenderer.color, static_cast<int>(_enid));
            }
        });

    // CircleRendererComponent
    m_registry.view<TransformComponent, CircleRendererComponent>().each(
        [](entt::entity _enid,
           TransformComponent& _transform,
           CircleRendererComponent& _circleRenderer) {
            Renderer2D::drawCircle(_transform.getTransform(),
                                   _circleRenderer.color,
                                   _circleRenderer.thickness,
                                   _circleRenderer.fade,
                                   static_cast<int>(_enid));
        });

    Renderer::endRender();
}

void Scene::onStartRuntime()
{
}

void Scene::onUpdateRuntime(Timestep _ts)
{
}

void Scene::onStopRuntime()
{
}

}  // namespace Namica