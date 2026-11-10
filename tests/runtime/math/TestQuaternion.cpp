#include "runtime/utils/TestUtils.h"
#include "runtime/math/utils/MathTestUtils.h"

#include <namica/core/Types.h>

#include <namica/math/Quaternion.h>
#include <namica/math/Utils.h>

using namespace namica;

namespace
{

// 仅比较四元数各分量；q 与 -q 的旋转等价性由专门用例验证，不能在此折叠符号。
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

// 默认构造四元数应得到 (1, 0, 0, 0) 和单位模长。
// 再旋转非零向量并转换为矩阵，两者均应表现为恒等变换。
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

// 以 (1, 2, 2, 4) 构造四元数，检查构造不重排或归一化输入分量。
// 平方模长应为 25，模长应为 5，为后续归一化用例提供可手算基准。
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

// 将模长为 5 的四元数原地归一化，各分量应除以 5，最终模长为 1。
// 同时检查返回值仍引用原对象，保证原地接口可用于链式调用。
TEST_F(NamicaRuntimeTest, QuaternionNormalizeMutatesAndReturnsSelf)
{
    Quat quaternion{1.0f, 2.0f, 2.0f, 4.0f};

    Quat& returned{quaternion.normalize()};

    EXPECT_EQ(&returned, &quaternion);
    ExpectQuatNear(quaternion, Quat{0.2f, 0.4f, 0.4f, 0.8f});
    EXPECT_NEAR(quaternion.length(), 1.0f, kEpsilon);
}

// 对非单位四元数调用 normalized()，验证得到单位模长副本。
// 源对象应保持原分量，结果应为逐分量除以原模长，不能偷偷原地修改。
TEST_F(NamicaRuntimeTest, QuaternionNormalizedReturnsCopyAndPreservesSource)
{
    Quat const source{1.0f, 2.0f, 2.0f, 4.0f};
    Quat const before{source};

    Quat const result{source.normalized()};

    ExpectQuatNear(source, before);
    ExpectQuatNear(result, Quat{0.2f, 0.4f, 0.4f, 0.8f});
    EXPECT_NEAR(result.lengthSquared(), 1.0f, kEpsilon);
}

// 对零四元数调用原地和副本归一化，检查零模长边界保持零值。
// 原地接口仍返回自身引用，两个结果均不能因除零产生非有限分量。
TEST_F(NamicaRuntimeTest, QuaternionNormalizeZeroLeavesItUnchanged)
{
    Quat quaternion{0.0f, 0.0f, 0.0f, 0.0f};

    Quat& returned{quaternion.normalize()};
    Quat const copied{quaternion.normalized()};

    EXPECT_EQ(&returned, &quaternion);
    ExpectQuatNear(quaternion, Quat{0.0f, 0.0f, 0.0f, 0.0f});
    ExpectQuatNear(copied, quaternion);
}

// 以两组不同的非单位分量计算 Hamilton product，核对四个已知结果分量。
// 不只用旋转性质间接验证，从而能够定位乘法公式中的符号或交叉项错误。
TEST_F(NamicaRuntimeTest, QuaternionMultiplicationUsesHamiltonProduct)
{
    Quat const left{1.0f, 2.0f, 3.0f, 4.0f};
    Quat const right{5.0f, 6.0f, 7.0f, 8.0f};

    Quat const result{left * right};

    ExpectQuatNear(result, Quat{-60.0f, 12.0f, 30.0f, 24.0f});
}

// 分别将四元数与单位元左右相乘，结果都应保持原分量。
// 再用纯虚单位 i、j 验证 i*j=k 而 j*i=-k，明确乘法不可交换。
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

// 使用单位 Z 轴和 90° 角生成四元数，验证半角公式的已知分量。
// w、z 应为 sqrt(2)/2，x、y 为 0，结果应保持单位模长。
TEST_F(NamicaRuntimeTest, QuaternionAngleAxisCreatesExpectedQuarterTurn)
{
    constexpr Float angle{Constants::PI * 0.5f};
    constexpr Float halfSqrt2{0.70710678118f};

    Quat const rotation{Quat::angleAxis(angle, Vec3{0.0f, 0.0f, 1.0f})};

    ExpectQuatNear(rotation, Quat{halfSqrt2, 0.0f, 0.0f, halfSqrt2});
    EXPECT_NEAR(rotation.length(), 1.0f, kEpsilon);
}

// 分别绕单位 X、Y、Z 正轴旋转 90°，用三个基向量验证右手方向。
// 期望依次得到 +Z、+X、+Y，防止旋转符号或轴分量顺序出错。
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

// 用归一化的一般轴生成单位旋转，作用于含正负分量的非单位向量。
// 旋转前后点积平方长度应相等，验证旋转不会引入缩放。
TEST_F(NamicaRuntimeTest, QuaternionRotationPreservesVectorLength)
{
    Quat const rotation{Quat::angleAxis(0.73f, Vec3{1.0f, -2.0f, 3.0f}.normalized())};
    Vec3 const vector{4.0f, -5.0f, 6.0f};

    Vec3 const result{rotation * vector};

    EXPECT_NEAR(result.dot(result), vector.dot(vector), 1e-5f);
}

// 绕同一单位轴构造角度互为相反数的旋转，验证两种乘积均为单位四元数。
// 再让向量依次经历正向和逆向旋转，结果应回到原向量。
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

// 取一个单位四元数及其所有分量的相反数，验证旋转的双重表示。
// 二者直接旋转同一向量和转换出的矩阵均应一致，不能把分量不同等同于旋转不同。
TEST_F(NamicaRuntimeTest, QuaternionAndNegationProduceSameRotation)
{
    Quat const quaternion{Quat::angleAxis(1.13f, Vec3{1.0f, 2.0f, -3.0f}.normalized())};
    Quat const negated{-quaternion.w, -quaternion.x, -quaternion.y, -quaternion.z};
    Vec3 const vector{2.0f, -4.0f, 5.0f};

    ExpectVectorNear(quaternion * vector, negated * vector, 1e-5f);
    ExpectMat4Near(quaternion.toMatrix(), negated.toMatrix(), 1e-5f);
}

// 将 X 轴与 Z 轴的 90° 旋转相乘，验证右侧旋转先作用于向量。
// 组合结果应等于显式逐次旋转，并得到已知 +Z 方向以避免两条路径共同出错。
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

// 把绕单位 Z 轴正向 90° 的四元数转换成矩阵，与显式已知旋转矩阵比较。
// 检查旋转块的符号、方向及齐次行列，避免转置约定不一致。
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

// 用一般单位旋转分别直接旋转三维向量和以矩阵变换 w=0 的方向。
// xyz 应一致且 w 保持 0，验证两种公开旋转入口遵循相同约定。
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

// 对同一角度和轴分别生成四元数矩阵与 Rodrigues 轴角旋转矩阵。
// 四元数入口显式使用单位轴，两份矩阵应一致，用独立实现交叉检查转换结果。
TEST_F(NamicaRuntimeTest, QuaternionToMatrixMatchesMatrixAxisAngleRotation)
{
    constexpr Float angle{0.73f};
    Vec3 const axis{1.0f, -2.0f, 3.0f};
    Vec3 const normalizedAxis{axis.normalized()};

    Mat4 const quaternionMatrix{Quat::angleAxis(angle, normalizedAxis).toMatrix()};
    Mat4 const matrixRotation{Mat4{1.0f}.rotated(angle, axis)};

    ExpectMat4Near(quaternionMatrix, matrixRotation, 1e-5f);
}

// 将三个 Euler 角都设为零，生成的四元数应为单位四元数。
// 此边界保证没有旋转时不引入分量偏置或角度顺序副作用。
TEST_F(NamicaRuntimeTest, QuaternionFromZeroEulerCreatesIdentity)
{
    ExpectQuatNear(Quat::fromEuler(Vec3{}), Quat{});
}

// 每次只保留一个非零 Euler 角，分别覆盖 X、Y、Z 轴及负角度。
// 各结果应等价于对应单位轴的 angleAxis()，验证角分量与轴的对应关系。
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

// 使用三个均非零的 Euler 角，检查组合为 qZ*qY*qX，即先 X、再 Y、最后 Z。
// 同时比较独立逐次构造的矩阵，防止单轴用例无法识别的旋转顺序错误。
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

// 使用三个不同且含负值的 Euler 角生成一般组合旋转。
// 模长及平方模长均应接近 1，验证组合不会引入缩放；不声称覆盖所有浮点输入。
TEST_F(NamicaRuntimeTest, QuaternionFromEulerProducesUnitQuaternion)
{
    Quat const quaternion{Quat::fromEuler(Vec3{1.17f, -0.63f, 2.31f})};

    EXPECT_NEAR(quaternion.lengthSquared(), 1.0f, 1e-5f);
    EXPECT_NEAR(quaternion.length(), 1.0f, 1e-5f);
}
