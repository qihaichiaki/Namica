#include "runtime/utils/TestUtils.h"

#include <namica/math/Quaternion.h>
#include <namica/math/Utils.h>

using namespace namica;

namespace
{

void ExpectQuatNear(Quat const& _actual, Quat const& _expected, Float const _epsilon = kEpsilon)
{
    EXPECT_NEAR(_actual.w, _expected.w, _epsilon) << "component = w";
    EXPECT_NEAR(_actual.x, _expected.x, _epsilon) << "component = x";
    EXPECT_NEAR(_actual.y, _expected.y, _epsilon) << "component = y";
    EXPECT_NEAR(_actual.z, _expected.z, _epsilon) << "component = z";
}

static_assert(noexcept(Quat{}));
static_assert(noexcept(Quat{1.0f, 2.0f, 3.0f, 4.0f}));
static_assert(noexcept(Quat{}.lengthSquared()));
static_assert(noexcept(Quat{}.length()));
static_assert(noexcept(Quat{}.normalize()));
static_assert(noexcept(Quat{}.normalized()));
static_assert(noexcept(Quat{} * Quat{}));
static_assert(noexcept(Quat{} * Vec3{}));
static_assert(noexcept(Quat{}.toMatrix()));
static_assert(noexcept(Quat::angleAxis(0.0f, Vec3{})));
static_assert(noexcept(Quat::fromEuler(Vec3{})));

}  // namespace

// 验证默认构造得到单位四元数。
TEST_F(NamicaRuntimeTest, QuaternionDefaultConstructsIdentity)
{
    Quat const quaternion{};

    ExpectQuatNear(quaternion, Quat{1.0f, 0.0f, 0.0f, 0.0f});
    EXPECT_FLOAT_EQ(quaternion.lengthSquared(), 1.0f);
    EXPECT_FLOAT_EQ(quaternion.length(), 1.0f);

    Vec3 const vector{1.0f, -2.0f, 3.0f};
    ExpectVectorNear(quaternion * vector, vector);
    ExpectMat4Near(quaternion.toMatrix(), Mat4{1.0f});
}

// 验证分量构造、模长平方和模长。
TEST_F(NamicaRuntimeTest, QuaternionComponentConstructorAndLengthPreserveValues)
{
    Quat const quaternion{1.0f, 2.0f, 2.0f, 4.0f};

    EXPECT_FLOAT_EQ(quaternion.w, 1.0f);
    EXPECT_FLOAT_EQ(quaternion.x, 2.0f);
    EXPECT_FLOAT_EQ(quaternion.y, 2.0f);
    EXPECT_FLOAT_EQ(quaternion.z, 4.0f);
    EXPECT_FLOAT_EQ(quaternion.lengthSquared(), 25.0f);
    EXPECT_FLOAT_EQ(quaternion.length(), 5.0f);
}

// 验证 normalize() 原地归一化并返回自身引用。
TEST_F(NamicaRuntimeTest, QuaternionNormalizeMutatesAndReturnsSelf)
{
    Quat quaternion{1.0f, 2.0f, 2.0f, 4.0f};

    Quat& returned{quaternion.normalize()};

    EXPECT_EQ(&returned, &quaternion);
    ExpectQuatNear(quaternion, Quat{0.2f, 0.4f, 0.4f, 0.8f});
    EXPECT_NEAR(quaternion.length(), 1.0f, kEpsilon);
}

// 验证 normalized() 返回副本且不修改源对象。
TEST_F(NamicaRuntimeTest, QuaternionNormalizedReturnsCopyAndPreservesSource)
{
    Quat const source{1.0f, 2.0f, 2.0f, 4.0f};
    Quat const before{source};

    Quat const result{source.normalized()};

    ExpectQuatNear(source, before);
    ExpectQuatNear(result, Quat{0.2f, 0.4f, 0.4f, 0.8f});
    EXPECT_NEAR(result.lengthSquared(), 1.0f, kEpsilon);
}

// 验证零四元数归一化时保持不变。
TEST_F(NamicaRuntimeTest, QuaternionNormalizeZeroLeavesItUnchanged)
{
    Quat quaternion{0.0f, 0.0f, 0.0f, 0.0f};

    Quat& returned{quaternion.normalize()};
    Quat const copied{quaternion.normalized()};

    EXPECT_EQ(&returned, &quaternion);
    ExpectQuatNear(quaternion, Quat{0.0f, 0.0f, 0.0f, 0.0f});
    ExpectQuatNear(copied, quaternion);
}

// 验证 Hamilton product 的各分量。
TEST_F(NamicaRuntimeTest, QuaternionMultiplicationUsesHamiltonProduct)
{
    Quat const left{1.0f, 2.0f, 3.0f, 4.0f};
    Quat const right{5.0f, 6.0f, 7.0f, 8.0f};

    Quat const result{left * right};

    ExpectQuatNear(result, Quat{-60.0f, 12.0f, 30.0f, 24.0f});
}

// 验证单位元和乘法不可交换性。
TEST_F(NamicaRuntimeTest, QuaternionMultiplicationHasIdentityAndIsNotCommutative)
{
    Quat const identity{};
    Quat const quaternion{1.0f, 2.0f, 3.0f, 4.0f};

    ExpectQuatNear(identity * quaternion, quaternion);
    ExpectQuatNear(quaternion * identity, quaternion);

    Quat const i{0.0f, 1.0f, 0.0f, 0.0f};
    Quat const j{0.0f, 0.0f, 1.0f, 0.0f};

    ExpectQuatNear(i * j, Quat{0.0f, 0.0f, 0.0f, 1.0f});
    ExpectQuatNear(j * i, Quat{0.0f, 0.0f, 0.0f, -1.0f});
}

// 验证 angleAxis() 的已知 90 度旋转分量。
TEST_F(NamicaRuntimeTest, QuaternionAngleAxisCreatesExpectedQuarterTurn)
{
    constexpr Float angle{Constants::PI * 0.5f};
    constexpr Float halfSqrt2{0.70710678118f};

    Quat const rotation{Quat::angleAxis(angle, Vec3{0.0f, 0.0f, 1.0f})};

    ExpectQuatNear(rotation, Quat{halfSqrt2, 0.0f, 0.0f, halfSqrt2});
    EXPECT_NEAR(rotation.length(), 1.0f, kEpsilon);
}

// 验证 X、Y、Z 正向 90 度旋转符合右手定则。
TEST_F(NamicaRuntimeTest, QuaternionPositiveQuarterTurnsFollowRightHandRule)
{
    constexpr Float angle{Constants::PI * 0.5f};

    Quat const rotationX{Quat::angleAxis(angle, Vec3{1.0f, 0.0f, 0.0f})};
    Quat const rotationY{Quat::angleAxis(angle, Vec3{0.0f, 1.0f, 0.0f})};
    Quat const rotationZ{Quat::angleAxis(angle, Vec3{0.0f, 0.0f, 1.0f})};

    // +Y 绕 +X 旋转 90° -> +Z。
    ExpectVectorNear(rotationX * Vec3{0.0f, 1.0f, 0.0f}, Vec3{0.0f, 0.0f, 1.0f});

    // +Z 绕 +Y 旋转 90° -> +X。
    ExpectVectorNear(rotationY * Vec3{0.0f, 0.0f, 1.0f}, Vec3{1.0f, 0.0f, 0.0f});

    // +X 绕 +Z 旋转 90° -> +Y。
    ExpectVectorNear(rotationZ * Vec3{1.0f, 0.0f, 0.0f}, Vec3{0.0f, 1.0f, 0.0f});
}

// 验证单位四元数旋转保持向量长度。
TEST_F(NamicaRuntimeTest, QuaternionRotationPreservesVectorLength)
{
    Quat const rotation{Quat::angleAxis(0.73f, Vec3{1.0f, -2.0f, 3.0f}.normalized())};
    Vec3 const vector{4.0f, -5.0f, 6.0f};

    Vec3 const result{rotation * vector};

    EXPECT_NEAR(result.dot(result), vector.dot(vector), 1e-5f);
}

// 验证旋转与逆旋转恢复原向量。
TEST_F(NamicaRuntimeTest, QuaternionRotationFollowedByInverseRestoresVector)
{
    constexpr Float angle{0.83f};
    Vec3 const axis{Vec3{1.0f, -2.0f, 3.0f}.normalized()};
    Vec3 const vector{4.0f, -5.0f, 6.0f};

    Quat const rotation{Quat::angleAxis(angle, axis)};
    Quat const inverse{Quat::angleAxis(-angle, axis)};

    ExpectQuatNear(rotation * inverse, Quat{}, 1e-5f);
    ExpectQuatNear(inverse * rotation, Quat{}, 1e-5f);
    ExpectVectorNear(inverse * (rotation * vector), vector, 1e-5f);
}

// 验证 q 与 -q 表示同一个旋转。
TEST_F(NamicaRuntimeTest, QuaternionAndNegationProduceSameRotation)
{
    Quat const quaternion{Quat::angleAxis(1.13f, Vec3{1.0f, 2.0f, -3.0f}.normalized())};
    Quat const negated{-quaternion.w, -quaternion.x, -quaternion.y, -quaternion.z};
    Vec3 const vector{2.0f, -4.0f, 5.0f};

    ExpectVectorNear(quaternion * vector, negated * vector, 1e-5f);
    ExpectMat4Near(quaternion.toMatrix(), negated.toMatrix(), 1e-5f);
}

// 验证四元数乘法按右侧旋转先应用的顺序组合。
TEST_F(NamicaRuntimeTest, QuaternionMultiplicationComposesRotationsInOrder)
{
    constexpr Float angle{Constants::PI * 0.5f};

    Quat const rotationX{Quat::angleAxis(angle, Vec3{1.0f, 0.0f, 0.0f})};
    Quat const rotationZ{Quat::angleAxis(angle, Vec3{0.0f, 0.0f, 1.0f})};
    Vec3 const vector{0.0f, 1.0f, 0.0f};

    Vec3 const composed{(rotationZ * rotationX) * vector};
    Vec3 const sequential{rotationZ * (rotationX * vector)};

    ExpectVectorNear(composed, sequential);
    ExpectVectorNear(composed, Vec3{0.0f, 0.0f, 1.0f});
}

// 验证 toMatrix() 的已知 Z 轴 90 度旋转矩阵。
TEST_F(NamicaRuntimeTest, QuaternionToMatrixCreatesExpectedQuarterTurnMatrix)
{
    constexpr Float angle{Constants::PI * 0.5f};
    Quat const rotation{Quat::angleAxis(angle, Vec3{0.0f, 0.0f, 1.0f})};

    // clang-format off
    Mat4 const expected{
        0.0f, -1.0f, 0.0f, 0.0f,
        1.0f,  0.0f, 0.0f, 0.0f,
        0.0f,  0.0f, 1.0f, 0.0f,
        0.0f,  0.0f, 0.0f, 1.0f
    };
    // clang-format on

    ExpectMat4Near(rotation.toMatrix(), expected);
}

// 验证 toMatrix() 与四元数直接旋转向量一致。
TEST_F(NamicaRuntimeTest, QuaternionToMatrixMatchesDirectVectorRotation)
{
    Quat const rotation{Quat::angleAxis(0.73f, Vec3{1.0f, -2.0f, 3.0f}.normalized())};
    Vec3 const vector{4.0f, -5.0f, 6.0f};

    Vec3 const quaternionResult{rotation * vector};
    Vec4 const matrixResult{rotation.toMatrix() * Vec4{vector.x(), vector.y(), vector.z(), 0.0f}};

    ExpectVectorNear(
        quaternionResult, Vec3{matrixResult.x(), matrixResult.y(), matrixResult.z()}, 1e-5f);
    EXPECT_NEAR(matrixResult.w(), 0.0f, kEpsilon);
}

// 验证 toMatrix() 与独立的 Rodrigues 矩阵旋转实现一致。
TEST_F(NamicaRuntimeTest, QuaternionToMatrixMatchesMatrixAxisAngleRotation)
{
    constexpr Float angle{0.73f};
    Vec3 const axis{1.0f, -2.0f, 3.0f};
    Vec3 const normalizedAxis{axis.normalized()};

    Mat4 const quaternionMatrix{Quat::angleAxis(angle, normalizedAxis).toMatrix()};
    Mat4 const matrixRotation{Mat4{1.0f}.rotated(angle, axis)};

    ExpectMat4Near(quaternionMatrix, matrixRotation, 1e-5f);
}

// 验证零 Euler 角得到单位四元数。
TEST_F(NamicaRuntimeTest, QuaternionFromZeroEulerCreatesIdentity)
{
    ExpectQuatNear(Quat::fromEuler(Vec3{}), Quat{});
}

// 验证单轴 Euler 旋转分别等价于 angleAxis()。
TEST_F(NamicaRuntimeTest, QuaternionFromEulerSingleAxisMatchesAngleAxis)
{
    constexpr Float x{0.31f};
    constexpr Float y{-0.47f};
    constexpr Float z{0.83f};

    ExpectQuatNear(Quat::fromEuler(Vec3{x, 0.0f, 0.0f}),
                   Quat::angleAxis(x, Vec3{1.0f, 0.0f, 0.0f}));
    ExpectQuatNear(Quat::fromEuler(Vec3{0.0f, y, 0.0f}),
                   Quat::angleAxis(y, Vec3{0.0f, 1.0f, 0.0f}));
    ExpectQuatNear(Quat::fromEuler(Vec3{0.0f, 0.0f, z}),
                   Quat::angleAxis(z, Vec3{0.0f, 0.0f, 1.0f}));
}

// 验证 Euler 组合顺序为先 X、再 Y、最后 Z。
TEST_F(NamicaRuntimeTest, QuaternionFromEulerComposesXThenYThenZ)
{
    constexpr Float x{0.31f};
    constexpr Float y{-0.47f};
    constexpr Float z{0.83f};

    Quat const rotationX{Quat::angleAxis(x, Vec3{1.0f, 0.0f, 0.0f})};
    Quat const rotationY{Quat::angleAxis(y, Vec3{0.0f, 1.0f, 0.0f})};
    Quat const rotationZ{Quat::angleAxis(z, Vec3{0.0f, 0.0f, 1.0f})};

    Quat const fromEuler{Quat::fromEuler(Vec3{x, y, z})};
    Quat const composed{rotationZ * rotationY * rotationX};

    ExpectQuatNear(fromEuler, composed, 1e-5f);

    Mat4 expectedMatrix{1.0f};
    expectedMatrix.rotate(z, Vec3{0.0f, 0.0f, 1.0f});
    expectedMatrix.rotate(y, Vec3{0.0f, 1.0f, 0.0f});
    expectedMatrix.rotate(x, Vec3{1.0f, 0.0f, 0.0f});

    ExpectMat4Near(fromEuler.toMatrix(), expectedMatrix, 1e-5f);
}

// 验证任意 Euler 输入生成单位四元数。
TEST_F(NamicaRuntimeTest, QuaternionFromEulerProducesUnitQuaternion)
{
    Quat const quaternion{Quat::fromEuler(Vec3{1.17f, -0.63f, 2.31f})};

    EXPECT_NEAR(quaternion.lengthSquared(), 1.0f, 1e-5f);
    EXPECT_NEAR(quaternion.length(), 1.0f, 1e-5f);
}