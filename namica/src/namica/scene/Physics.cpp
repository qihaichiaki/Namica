#include "namica/scene/Physics.h"
#include "namica/core/Log.h"
#include <box2d/box2d.h>

namespace Namica
{

namespace Physics2D
{

static inline WorldHandle toWorldHandle(b2WorldId const& _worldID)
{
    return {_worldID.index1, _worldID.generation};
}

static inline b2WorldId tob2WorldId(WorldHandle const& _worldHandle)
{
    return {_worldHandle.index1, _worldHandle.generation};
}

static inline b2BodyType tob2BodyType(RigidbodyType _bodyType)
{
    switch (_bodyType)
    {
        case RigidbodyType::Static:
            return b2BodyType::b2_staticBody;
        case RigidbodyType::Dynamic:
            return b2BodyType::b2_dynamicBody;
        case RigidbodyType::Kinematic:
            return b2BodyType::b2_kinematicBody;
    }
    NAMICA_CORE_ASSERT(false, "未知Rigidbody2DComponent::BodyType类型");
    return b2BodyType::b2_staticBody;
}

static inline BodyHandle toBodyHandle(b2BodyId const& _b2BodyId)
{
    return {_b2BodyId.index1, _b2BodyId.world0, _b2BodyId.generation};
}

static inline b2BodyId tob2BodyId(BodyHandle const& _bodyHandle)
{
    return {_bodyHandle.index1, _bodyHandle.world0, _bodyHandle.generation};
}

void PhysicsWorld::init()
{
    b2WorldDef worldDef{b2DefaultWorldDef()};
    m_worldHandle = toWorldHandle(b2CreateWorld(&worldDef));

    m_isRunning = true;
}

void PhysicsWorld::onUpdate(Timestep _ts)
{
    b2World_Step(tob2WorldId(m_worldHandle), _ts, 4);
}

void PhysicsWorld::shutdown()
{
    b2DestroyWorld(tob2WorldId(m_worldHandle));

    m_isRunning = false;
}

bool PhysicsWorld::isRunning() const
{
    return m_isRunning;
}

BodyHandle PhysicsWorld::createBody(RigidbodyType _type,
                                    glm::vec2 const& _position,
                                    float _rotation,
                                    bool _fixedRotation)
{
    b2BodyDef bodyDef{b2DefaultBodyDef()};
    bodyDef.type = tob2BodyType(_type);
    bodyDef.motionLocks.angularZ = _fixedRotation;
    bodyDef.position = {_position.x, _position.y};
    bodyDef.rotation = b2MakeRot(_rotation);

    return toBodyHandle(b2CreateBody(tob2WorldId(m_worldHandle), &bodyDef));
}

void PhysicsWorld::attachBodyBoxShape(BodyHandle const& _body,
                                      glm::vec2 const& _offset,
                                      glm::vec2 const& _halfSize,
                                      float _rotation,
                                      Materials const& _materials)
{
    b2BodyId body{tob2BodyId(_body)};
    // 创建多边形形状
    b2Polygon boxPolygon{
        b2MakeOffsetBox(_halfSize.x, _halfSize.y, {_offset.x, _offset.y}, b2MakeRot(_rotation))};
    b2ShapeDef boxShapeDef{b2DefaultShapeDef()};
    boxShapeDef.density = _materials.density;                   // 密度
    boxShapeDef.material.friction = _materials.friction;        // 摩擦力
    boxShapeDef.material.restitution = _materials.restitution;  // 反弹系数
    // TODO: 反弹恢复阈值

    b2CreatePolygonShape(body, &boxShapeDef, &boxPolygon);
}

void PhysicsWorld::attacheBodyCircleShape(BodyHandle const& _body,
                                          glm::vec2 const& _offset,
                                          float _radius,
                                          Materials const& _materials)
{
    b2BodyId body{tob2BodyId(_body)};
    b2Circle circle{{_offset.x, _offset.y}, _radius};
    b2ShapeDef boxShapeDef{b2DefaultShapeDef()};
    boxShapeDef.density = _materials.density;                   // 密度
    boxShapeDef.material.friction = _materials.friction;        // 摩擦力
    boxShapeDef.material.restitution = _materials.restitution;  // 反弹系数
    // TODO: 反弹恢复阈值

    b2CreateCircleShape(body, &boxShapeDef, &circle);
}

void PhysicsWorld::getBodyTransform(BodyHandle const& _body, glm::vec3& _position, float& _rotation)
{
    b2BodyId body{tob2BodyId(_body)};
    auto pos = b2Body_GetPosition(body);
    auto rot = b2Body_GetRotation(body);
    _position = {pos.x, pos.y, _position.z};
    _rotation = atan2(rot.s, rot.c);
}

}  // namespace Physics2D

}  // namespace Namica