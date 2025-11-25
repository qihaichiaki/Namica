#include "namica/scene/Scene.h"
#include "namica/scene/Entity.h"
#include "namica/scene/Components.h"
#include "namica/core/Log.h"
#include "namica/renderer/Renderer.h"
#include "namica/renderer/EditorCamera.h"
#include "namica/renderer/Renderer2D.h"
#include "namica/scene/ScriptableEntity.h"

#include <glm/glm.hpp>

namespace Namica
{

// Copy 组件数据部分

// 依据_enttMap中记录的src entity中的UUID, 对应上dst中新Entity, 随后拷贝其src中的entity中各个组件
template <typename Component>
static void copyComponent(entt::registry& _dstRegistry,
                          entt::registry& _srcRegistry,
                          std::unordered_map<entt::entity, entt::entity> const& _enttMap)
{
    auto view = _srcRegistry.view<Component>();
    for (auto enid : view)
    {
        NAMICA_CORE_ASSERT(_enttMap.find(enid) != _enttMap.end(), "未知的UUID对象");

        entt::entity newEnid = _enttMap.at(enid);
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
                           std::unordered_map<entt::entity, entt::entity> const& _enttMap)>;

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
                                std::unordered_map<entt::entity, entt::entity> const& _enttMap) {
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
                          std::unordered_map<entt::entity, entt::entity> const& _enttMap)
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

void Scene::onCameraComponentAdded(entt::registry& _registry, entt::entity _enid)
{
    CameraComponent& camera{_registry.get<CameraComponent>(_enid)};
    camera.camera.updateViewportSize(m_viewportWidth, m_viewportHeight);
}

void Scene::onRigidbody2DComponentAdded(entt::registry& _registry, entt::entity _enid)
{
    if (m_physicsWorld.isRunning())
    {
        Rigidbody2DComponent& rigidbody2D{_registry.get<Rigidbody2DComponent>(_enid)};
        TransformComponent& transform{_registry.get<TransformComponent>(_enid)};
        rigidbody2D.bodyId =
            m_physicsWorld.createBody(rigidbody2D.type,
                                      {transform.translation.x, transform.translation.y},
                                      transform.rotation.z,
                                      rigidbody2D.fixedRotation);
    }
}

void Scene::onBoxCollider2DComponentAdded(entt::registry& _registry, entt::entity _enid)
{
    if (m_physicsWorld.isRunning())
    {
        BoxCollider2DComponent& boxCollider2D{_registry.get<BoxCollider2DComponent>(_enid)};
        NAMICA_CORE_ASSERT(m_registry.all_of<Rigidbody2DComponent>(_enid),
                           "添加box2d碰撞器必须存在刚体组件!");
        Rigidbody2DComponent& rigidbody2D{_registry.get<Rigidbody2DComponent>(_enid)};
        TransformComponent& transform{_registry.get<TransformComponent>(_enid)};

        m_physicsWorld.attachBodyBoxShape(
            rigidbody2D.bodyId,
            boxCollider2D.offset,
            {boxCollider2D.size.x * transform.scale.x, boxCollider2D.size.y * transform.scale.y},
            boxCollider2D.rotation,
            boxCollider2D.physicalMaterials);
    }
}

void Scene::onCircleCollider2DComponentAdded(entt::registry& _registry, entt::entity _enid)
{
    if (m_physicsWorld.isRunning())
    {
        CircleCollider2DComponent& circleCollider2D{
            _registry.get<CircleCollider2DComponent>(_enid)};
        NAMICA_CORE_ASSERT(m_registry.all_of<Rigidbody2DComponent>(_enid),
                           "添加Circle碰撞器必须存在刚体组件!");
        Rigidbody2DComponent& rigidbody2D{_registry.get<Rigidbody2DComponent>(_enid)};

        m_physicsWorld.attacheBodyCircleShape(rigidbody2D.bodyId,
                                              circleCollider2D.offset,
                                              circleCollider2D.radius,
                                              circleCollider2D.physicalMaterials);
    }
}

Scene::Scene(std::string const& _sceneName) : m_name{_sceneName}
{
    // 组件注册表, 控制组件在场景中的相关行为
    // TODO: 添加SceneManger后统一管理
    static bool isInit{false};
    if (!isInit)
    {
        ComponentRegistry::componentRegister<TransformComponent>();
        ComponentRegistry::componentRegister<RelationshipComponent>();
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
    // 刚体组件添加行为
    m_registry.on_construct<Rigidbody2DComponent>().connect<&Scene::onRigidbody2DComponentAdded>(
        this);
    // 碰撞体组件添加行为
    m_registry.on_construct<BoxCollider2DComponent>()
        .connect<&Scene::onBoxCollider2DComponentAdded>(this);
    m_registry.on_construct<CircleCollider2DComponent>()
        .connect<&Scene::onCircleCollider2DComponentAdded>(this);
}

Scene::~Scene()
{
}

Ref<Scene> Scene::create(std::string const& _sceneName)
{
    return createRef<Scene>(_sceneName);
}

Ref<Scene> Scene::copy(Ref<Scene> const& _other)
{
    Ref<Scene> newScene{createRef<Scene>(_other->m_name)};
    // 常规的其他属性
    newScene->m_viewportWidth = _other->m_viewportWidth;
    newScene->m_viewportHeight = _other->m_viewportHeight;

    std::unordered_map<entt::entity, entt::entity> enttMap;
    auto otherEntitiesView = _other->m_registry.view<IDComponent>();
    for (auto enid : otherEntitiesView)
    {
        IDComponent& idComponent{_other->m_registry.get<IDComponent>(enid)};
        TagComponent& tagComponent{_other->m_registry.get<TagComponent>(enid)};
        enttMap.emplace(enid, newScene->createEntity(idComponent.id, tagComponent.name));
    }

    for (Entity& otherEntity : _other->m_rootEntities)
    {
        newScene->m_rootEntities.emplace_back(Entity{enttMap.at(otherEntity), newScene.get()});
    }

    // 待删除实体列表不复制

    // 复制组件
    ComponentRegistry::sceneCopy(newScene->m_registry, _other->m_registry, enttMap);

    return newScene;
}

std::string const& Scene::getName() const
{
    return m_name;
}

bool Scene::containsEntity(Entity _entity) const
{
    return _entity.isValid() && &(_entity.getScene()) == this;
}

Entity Scene::createEntity(std::string const& _name)
{
    Entity entity{m_registry.create(), this};

    entity.addComponent<IDComponent>();
    entity.addComponent<TagComponent>(_name.empty() ? "新实体" : _name);
    entity.addComponent<TransformComponent>();

    m_rootEntities.emplace_back(entity);

    return entity;
}

Entity Scene::createEntity(const UUID& _uuid, std::string const& _name)
{
    Entity entity{m_registry.create(), this};

    entity.addComponent<IDComponent>(_uuid);
    entity.addComponent<TagComponent>(_name.empty() ? "新实体" : _name);
    entity.addComponent<TransformComponent>();

    m_rootEntities.emplace_back(entity);

    return entity;
}

// 判断_ancestor 是否是 _enid的祖先节点
static bool isAncestor(entt::registry& _registry, entt::entity _ancestor, entt::entity _enid)
{
    while (_enid != entt::null)
    {
        if (_enid == _ancestor)
        {
            return true;
        }

        if (!_registry.any_of<RelationshipComponent>(_enid))
        {
            break;
        }

        _enid = _registry.get<RelationshipComponent>(_enid).parent;
    }
    return false;
}

bool Scene::addChildEntity(Entity _parent, Entity _child)
{
    if (!containsEntity(_parent) || !containsEntity(_child))
    {
        NAMICA_CORE_WARN(
            "实体-{}或者实体-{}并不属于当前场景-{}", _parent.getName(), _child.getName(), m_name);
        return false;
    }

    if (isAncestor(m_registry, _child, _parent))
    {
        NAMICA_CORE_WARN(
            "实体-{}是实体-{}的祖先节点, 不允许形成环!", _child.getName(), _parent.getName());
        return false;
    }

    RelationshipComponent& childRelationship{_child.hasComponent<RelationshipComponent>()
                                                 ? _child.getComponent<RelationshipComponent>()
                                                 : _child.addComponent<RelationshipComponent>()};
    RelationshipComponent& parentRelationship{_parent.hasComponent<RelationshipComponent>()
                                                  ? _parent.getComponent<RelationshipComponent>()
                                                  : _parent.addComponent<RelationshipComponent>()};

    if (childRelationship.parent != entt::null)
    {
        if (childRelationship.parent == _parent)
        {
            NAMICA_CORE_WARN(
                "实体-{}已经是实体-{}的孩子节点了!", _child.getName(), _parent.getName(), m_name);
            return false;
        }

        // 子节点存在其他父节点, 其他父节点进行移除
        RelationshipComponent& oldParentRelationship{
            m_registry.get<RelationshipComponent>(childRelationship.parent)};
        std::erase(oldParentRelationship.children, _child);
    }
    else
    {
        // 子节点不存在父节点, 则将其从root列表中移除
        std::erase(m_rootEntities, _child);
    }

    // 更新父子关系
    childRelationship.parent = _parent;
    parentRelationship.children.emplace_back(_child);

    return true;
}

bool Scene::removeChildEntity(Entity _parent, Entity _child)
{
    if (!containsEntity(_parent) || !containsEntity(_child))
    {
        NAMICA_CORE_WARN(
            "实体-{}或者实体-{}并不属于当前场景-{}", _parent.getName(), _child.getName(), m_name);
        return false;
    }

    RelationshipComponent& childRelationship{_child.getComponent<RelationshipComponent>()};
    RelationshipComponent& parentRelationship{_parent.getComponent<RelationshipComponent>()};

    if (childRelationship.parent != _parent)
    {
        NAMICA_CORE_ASSERT(
            "实体-{}并不是实体-{}的直接子节点!", _child.getName(), _parent.getName());
        return false;
    }

    childRelationship.parent = entt::null;
    std::erase(parentRelationship.children, _child);
    // TODO: 此处可设置插位
    m_rootEntities.emplace_back(_child);

    return true;
}

bool Scene::destoryEntity(Entity _entity)
{
    if (containsEntity(_entity))
    {
        m_enidsToDestroy.insert(_entity);
        return true;
    }
    else
    {
        NAMICA_CORE_WARN("删除了非当前场景的实体对象: {}", _entity.getName());
        return false;
    }
}

void Scene::destroyEntityRecursive(entt::entity _enid)
{
    if (m_registry.any_of<NativeScriptComponent>(_enid))
    {
        NativeScriptComponent& nativeScript{m_registry.get<NativeScriptComponent>(_enid)};
        if (nativeScript.instance)
        {
            nativeScript.instance->onDestroy();
            nativeScript.destroyScript(nativeScript.instance);
            nativeScript.instance = nullptr;
        }
    }

    if (m_registry.any_of<Rigidbody2DComponent>(_enid) && m_physicsWorld.isRunning())
    {
        // 如果存在刚体组件, 需要从物理世界中进行移除
        m_physicsWorld.destroyBody(m_registry.get<Rigidbody2DComponent>(_enid).bodyId);
    }

    // 删除子节点
    if (m_registry.any_of<RelationshipComponent>(_enid))
    {
        RelationshipComponent& relationship{m_registry.get<RelationshipComponent>(_enid)};
        if (relationship.parent != entt::null &&
            m_enidsToDestroy.find(relationship.parent) == m_enidsToDestroy.end())
        {
            // 如果其父节点不处于待删除列表, 则父节点
            std::erase(m_registry.get<RelationshipComponent>(relationship.parent).children, _enid);
        }

        if (relationship.parent == entt::null)
        {
            // 从rootEnids中进行删除
            std::erase(m_rootEntities, _enid);
        }

        for (auto childEnid : relationship.children)
        {
            destroyEntityRecursive(childEnid);
        }
    }
    else
    {
        std::erase(m_rootEntities, _enid);
    }

    m_registry.destroy(_enid);
}

// 延迟删除, 真正删除entity的地方
void Scene::flushDestroyQueue()
{
    if (m_enidsToDestroy.size() > 0)
    {
        for (entt::entity enid : m_enidsToDestroy)
        {
            destroyEntityRecursive(enid);
        }

        m_enidsToDestroy.clear();
    }
}

Entity Scene::copyEntity(Entity _entity)
{
    Entity newEntity{};
    if (containsEntity(_entity))
    {
        newEntity = createEntity(_entity.getName());
        ComponentRegistry::entityCopy(newEntity, _entity);

        // TODO：可以增加index指定插入位置
        if (newEntity.hasComponent<RelationshipComponent>())
        {
            RelationshipComponent& relationship{newEntity.getComponent<RelationshipComponent>()};
            if (relationship.parent == entt::null)
            {
                m_rootEntities.emplace_back(newEntity);
            }

            std::vector<entt::entity> tempChildren{};
            tempChildren.resize(relationship.children.size());
            for (entt::entity childEnid : relationship.children)
            {
                Entity childEntity{childEnid, this};
                Entity copyChildEntity{copyEntity(childEntity)};
                copyChildEntity.getComponent<RelationshipComponent>().parent = _entity;
                tempChildren.emplace_back(copyChildEntity);
            }
            relationship.children = tempChildren;
        }
        else
        {
            m_rootEntities.emplace_back(newEntity);
        }
    }
    else
    {
        NAMICA_CORE_WARN("场景-{}拷贝了不存在的实体-{}", m_name, _entity.getName());
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

void Scene::onRenderer(glm::mat4 const& _projectionView)
{
    // 渲染更新
    Renderer::beginRender(_projectionView);

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

    if (m_isDrawColliders2D)
    {
        m_registry.view<TransformComponent, BoxCollider2DComponent>().each(
            [](entt::entity _enid,
               TransformComponent& _transform,
               BoxCollider2DComponent& _boxCollider2D) {
                Renderer2D::drawRect(
                    _transform.translation +
                        glm::vec3{_boxCollider2D.offset.x, _boxCollider2D.offset.y, 0.0f},
                    {_transform.scale.x * _boxCollider2D.size.x,
                     _transform.scale.y * _boxCollider2D.size.y},
                    {0.0f, 1.0f, 0.0f, 1.f},
                    static_cast<int>(_enid));
            });

        m_registry.view<TransformComponent, CircleCollider2DComponent>().each(
            [](entt::entity _enid,
               TransformComponent& _transform,
               CircleCollider2DComponent& _circleCollider2D) {
                TransformComponent circleCollider2DTransform{_transform};
                circleCollider2DTransform.translation +=
                    glm::vec3{_circleCollider2D.offset.x, _circleCollider2D.offset.y, 0.0f};
                circleCollider2DTransform.scale = glm::vec3{
                    _circleCollider2D.radius * 2.0f, _circleCollider2D.radius * 2.0f, 1.0f};
                Renderer2D::drawCircle(circleCollider2DTransform.getTransform(),
                                       {0.0f, 1.0f, 0.0f, 1.f},
                                       0.01f,
                                       0.005f,
                                       static_cast<int>(_enid));
            });
    }

    Renderer::endRender();
}

void Scene::onUpdateEditor(Timestep _ts, EditorCamera const& _editorCamera)
{
    onRenderer(_editorCamera.getProjectionView());

    // 延迟删除对象
    flushDestroyQueue();
}

void Scene::onStartRuntime()
{
    // 创建物理世界, 为所有存在刚体组件的对象准备物理世界对象
    m_physicsWorld.init();

    // 创建物理对象, 和附加形状
    m_registry.group<Rigidbody2DComponent>().each([this](entt::entity _enid,
                                                         Rigidbody2DComponent& _rigidbody2D) {
        Entity entity{_enid, this};

        TransformComponent& transform{entity.getComponent<TransformComponent>()};
        _rigidbody2D.bodyId =
            m_physicsWorld.createBody(_rigidbody2D.type,
                                      {transform.translation.x, transform.translation.y},
                                      transform.rotation.z,
                                      _rigidbody2D.fixedRotation);

        if (entity.hasComponent<BoxCollider2DComponent>())
        {
            BoxCollider2DComponent& boxCollider2D{entity.getComponent<BoxCollider2DComponent>()};
            m_physicsWorld.attachBodyBoxShape(_rigidbody2D.bodyId,
                                              boxCollider2D.offset,
                                              {boxCollider2D.size.x * transform.scale.x,
                                               boxCollider2D.size.y * transform.scale.y},
                                              boxCollider2D.rotation,
                                              boxCollider2D.physicalMaterials);
        }

        if (entity.hasComponent<CircleCollider2DComponent>())
        {
            CircleCollider2DComponent& circleCollider2D{
                entity.getComponent<CircleCollider2DComponent>()};
            m_physicsWorld.attacheBodyCircleShape(_rigidbody2D.bodyId,
                                                  circleCollider2D.offset,
                                                  circleCollider2D.radius,
                                                  circleCollider2D.physicalMaterials);
        }
    });
}

void Scene::onUpdateRuntime(Timestep _ts)
{
    // 游戏脚本更新
    m_registry.group<NativeScriptComponent>().each(
        [this, _ts](entt::entity _enid, NativeScriptComponent& _nativeScript) {
            if (!_nativeScript.instance)
            {
                if (_nativeScript.instantiateScript)
                {
                    _nativeScript.instance = _nativeScript.instantiateScript();
                    _nativeScript.instance->m_entity = {_enid, this};
                    _nativeScript.instance->onCreate();
                }
            }

            if (_nativeScript.instance)
            {
                _nativeScript.instance->onUpdate(_ts);
            }
        });

    // 物理世界更新
    m_physicsWorld.onUpdate(_ts);

    // 更新刚体组件的对象transform
    m_registry.group<Rigidbody2DComponent>().each(
        [this](entt::entity _enid, Rigidbody2DComponent& _rigidbody2D) {
            Entity entity{_enid, this};
            TransformComponent& transform{entity.getComponent<TransformComponent>()};
            m_physicsWorld.getBodyTransform(
                _rigidbody2D.bodyId, transform.translation, transform.rotation.z);
        });

    // 寻找运行时的主相机
    Camera const* camera{nullptr};
    glm::mat4 cameraTransform{1.0f};

    auto view = m_registry.view<TransformComponent, CameraComponent>();
    for (auto enid : view)
    {
        TransformComponent& _transform{view.get<TransformComponent>(enid)};
        CameraComponent& _camera{view.get<CameraComponent>(enid)};
        if (_camera.primary)
        {
            camera = &_camera.camera;
            cameraTransform = _transform.getTransform();
            break;
        }
    }

    if (camera)
    {
        onRenderer(camera->getProjection() * glm::inverse(cameraTransform));
    }

    // 延迟删除对象
    flushDestroyQueue();
}

void Scene::onStopRuntime()
{
    m_physicsWorld.shutdown();
}

void Scene::setDrawColliders2D(bool _enable)
{
    m_isDrawColliders2D = _enable;
}

std::vector<Entity>& Scene::getRootEntities()
{
    return m_rootEntities;
}

}  // namespace Namica