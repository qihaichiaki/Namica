#include <numbers>

#include "runtime/utils/TestUtils.h"
#include "namica/math/Matrix.h"

using namespace namica;

// 测试矩阵乘法正确性

namespace
{

constexpr Float kEpsilon{1e-6f};

template <Int D>
void ExpectVectorNear(Vector<Float, D> const& _actual,
                      Vector<Float, D> const& _expected,
                      Float const _epsilon = kEpsilon)
{
    for (Int index{0}; index < D; ++index)
    {
        EXPECT_NEAR(_actual[index], _expected[index], _epsilon) << "component = " << index;
    }
}

void ExpectMat4Near(Mat4 const& _actual, Mat4 const& _expected, Float const _epsilon = kEpsilon)
{
    for (Int row{0}; row < 4; ++row)
    {
        for (Int column{0}; column < 4; ++column)
        {
            EXPECT_NEAR(_actual(row, column), _expected(row, column), _epsilon)
                << "row = " << row << ", column = " << column;
        }
    }
}

template <typename V>
concept Normalizable = requires(V& _value, V const& _constValue) {
    { _value.normalize() } -> std::same_as<V&>;
    { _constValue.normalized() } -> std::same_as<V>;
};

static_assert(Normalizable<Vec2>);
static_assert(Normalizable<Vec3>);
static_assert(Normalizable<Vec4>);

// 当前 normalize 只允许浮点向量调用。
static_assert(!Normalizable<Veci2>);

}  // namespace

TEST_F(NamicaRuntimeTest, IdentityMatrixLeftMultiplyVector)
{
    // I * v = v
    // v视作为 [4, 1]的列向量

    Mat4 const identity{1.0f};
    Vec4 const vector{1.0f, 2.0f, 3.0f, 4.0f};
    Vec4 const result{identity * vector};

    ExpectVectorNear(result, vector);
    EXPECT_TRUE(result.isEqual(vector));
}

// TEST_F(NamicaRuntimeTest, VectorRightMultiplyIdentityMatrix)
// {
//     // v * I = v
//     // v视作为 [1, 4]的行向量

//     Mat4 const identity{};
//     Vec4 const vector{1.0f, 2.0f, 3.0f, 4.0f};
//     Vec4 const result{vector * identity};

//     ExpectVectorNear(result, vector);
//     EXPECT_TRUE(result.isEqual(vector));
// }

/**
 * @brief 验证对角矩阵左乘列向量。
 *
 * 矩阵：
 *
 *     [ 2  0  0  0 ]
 *     [ 0  3  0  0 ]
 *     [ 0  0  4  0 ]
 *     [ 0  0  0  5 ]
 *
 * 向量：
 *
 *     [ 1 ]
 *     [ 2 ]
 *     [ 3 ]
 *     [ 4 ]
 *
 * 结果：
 *
 *     [  2 ]
 *     [  6 ]
 *     [ 12 ]
 *     [ 20 ]
 */
TEST_F(NamicaRuntimeTest, DiagonalMatrixLeftMultiplyVector)
{
    Mat4 matrix{1.0f};

    matrix(0, 0) = 2.0f;
    matrix(1, 1) = 3.0f;
    matrix(2, 2) = 4.0f;
    matrix(3, 3) = 5.0f;

    Vec4 const vector{1.0f, 2.0f, 3.0f, 4.0f};

    Vec4 const result{matrix * vector};
    Vec4 const expected{2.0f, 6.0f, 12.0f, 20.0f};

    ExpectVectorNear(result, expected);
    EXPECT_TRUE(result.isEqual(expected));
}

/**
 * @brief 验证行向量右乘对角矩阵。
 *
 * 对角矩阵左右乘时都会分别缩放对应分量，因此结果相同：
 *
 *     [1 2 3 4] * diag(2, 3, 4, 5)
 *       = [2 6 12 20]
 */
// TEST_F(NamicaRuntimeTest, VectorRightMultiplyDiagonalMatrix)
// {
//     Mat4 matrix{};

//     matrix(0, 0) = 2.0f;
//     matrix(1, 1) = 3.0f;
//     matrix(2, 2) = 4.0f;
//     matrix(3, 3) = 5.0f;

//     Vec4 const vector{1.0f, 2.0f, 3.0f, 4.0f};

//     Vec4 const result{vector * matrix};
//     Vec4 const expected{2.0f, 6.0f, 12.0f, 20.0f};

//     ExpectVectorNear(result, expected);
//     EXPECT_TRUE(result.isEqual(expected));
// }

/**
 * @brief 验证一般矩阵左乘列向量。
 *
 * 矩阵：
 *
 *     [  1   2   3   4 ]
 *     [  5   6   7   8 ]
 *     [  9  10  11  12 ]
 *     [ 13  14  15  16 ]
 *
 * 列向量：
 *
 *     [ 1 ]
 *     [ 2 ]
 *     [ 3 ]
 *     [ 4 ]
 *
 * 每一个结果分量由“矩阵的一行点乘列向量”得到：
 *
 * result.x = 1×1 + 2×2 + 3×3 + 4×4 = 30
 * result.y = 5×1 + 6×2 + 7×3 + 8×4 = 70
 * result.z = 9×1 + 10×2 + 11×3 + 12×4 = 110
 * result.w = 13×1 + 14×2 + 15×3 + 16×4 = 150
 */
TEST_F(NamicaRuntimeTest, GeneralMatrixLeftMultiplyVector)
{
    Mat4 matrix{1.0f};

    matrix(0, 0) = 1.0f;
    matrix(0, 1) = 2.0f;
    matrix(0, 2) = 3.0f;
    matrix(0, 3) = 4.0f;

    matrix(1, 0) = 5.0f;
    matrix(1, 1) = 6.0f;
    matrix(1, 2) = 7.0f;
    matrix(1, 3) = 8.0f;

    matrix(2, 0) = 9.0f;
    matrix(2, 1) = 10.0f;
    matrix(2, 2) = 11.0f;
    matrix(2, 3) = 12.0f;

    matrix(3, 0) = 13.0f;
    matrix(3, 1) = 14.0f;
    matrix(3, 2) = 15.0f;
    matrix(3, 3) = 16.0f;

    Vec4 const vector{1.0f, 2.0f, 3.0f, 4.0f};

    Vec4 const result{matrix * vector};
    Vec4 const expected{30.0f, 70.0f, 110.0f, 150.0f};

    ExpectVectorNear(result, expected);
    EXPECT_TRUE(result.isEqual(expected));
}

/**
 * @brief 验证行向量右乘一般矩阵。
 *
 * 行向量：
 *
 *     [1 2 3 4]
 *
 * 右乘矩阵时，每个结果分量由“行向量点乘矩阵的一列”得到：
 *
 * result.x = 1×1 + 2×5 + 3×9 + 4×13 = 90
 * result.y = 1×2 + 2×6 + 3×10 + 4×14 = 100
 * result.z = 1×3 + 2×7 + 3×11 + 4×15 = 110
 * result.w = 1×4 + 2×8 + 3×12 + 4×16 = 120
 */
// TEST_F(NamicaRuntimeTest, VectorRightMultiplyGeneralMatrix)
// {
//     Mat4 matrix{};

//     matrix(0, 0) = 1.0f;
//     matrix(0, 1) = 2.0f;
//     matrix(0, 2) = 3.0f;
//     matrix(0, 3) = 4.0f;

//     matrix(1, 0) = 5.0f;
//     matrix(1, 1) = 6.0f;
//     matrix(1, 2) = 7.0f;
//     matrix(1, 3) = 8.0f;

//     matrix(2, 0) = 9.0f;
//     matrix(2, 1) = 10.0f;
//     matrix(2, 2) = 11.0f;
//     matrix(2, 3) = 12.0f;

//     matrix(3, 0) = 13.0f;
//     matrix(3, 1) = 14.0f;
//     matrix(3, 2) = 15.0f;
//     matrix(3, 3) = 16.0f;

//     Vec4 const vector{1.0f, 2.0f, 3.0f, 4.0f};

//     Vec4 const result = vector * matrix;
//     Vec4 const expected{90.0f, 100.0f, 110.0f, 120.0f};

//     ExpectVectorNear(result, expected);
//     EXPECT_TRUE(result.isEqual(expected));
// }

/**
 * @brief 验证矩阵与向量乘法通常不满足交换律。
 *
 * 对同一个矩阵和向量：
 *
 *     matrix * vector
 *
 * 与：
 *
 *     vector * matrix
 *
 * 一般具有不同结果。
 */
// TEST_F(NamicaRuntimeTest, LeftAndRightMultiplicationProduceDifferentResults)
// {
//     Mat4 matrix{};

//     matrix(0, 0) = 1.0f;
//     matrix(0, 1) = 2.0f;
//     matrix(0, 2) = 3.0f;
//     matrix(0, 3) = 4.0f;

//     matrix(1, 0) = 5.0f;
//     matrix(1, 1) = 6.0f;
//     matrix(1, 2) = 7.0f;
//     matrix(1, 3) = 8.0f;

//     matrix(2, 0) = 9.0f;
//     matrix(2, 1) = 10.0f;
//     matrix(2, 2) = 11.0f;
//     matrix(2, 3) = 12.0f;

//     matrix(3, 0) = 13.0f;
//     matrix(3, 1) = 14.0f;
//     matrix(3, 2) = 15.0f;
//     matrix(3, 3) = 16.0f;

//     Vec4 const vector{1.0f, 2.0f, 3.0f, 4.0f};

//     Vec4 const leftResult = matrix * vector;
//     Vec4 const rightResult = vector * matrix;

//     // 两种乘法的结果应分别为：
//     // matrix * vector = (30, 70, 110, 150)
//     // vector * matrix = (90, 100, 110, 120)
//     ExpectVectorNear(leftResult, Vec4{30.0f, 70.0f, 110.0f, 150.0f});
//     ExpectVectorNear(rightResult, Vec4{90.0f, 100.0f, 110.0f, 120.0f});

//     // 至少部分分量不同，证明左右乘法不是同一种运算。
//     EXPECT_NE(leftResult[0], rightResult[0]);
//     EXPECT_NE(leftResult[1], rightResult[1]);
//     EXPECT_NE(leftResult[3], rightResult[3]);

//     EXPECT_FALSE(leftResult.isEqual(rightResult));
// }

// 验证 Vec4::data()
TEST_F(NamicaRuntimeTest, Vec4DataReflectsElements)
{
    Vec4 vector{1.0f, 2.0f, 3.0f, 4.0f};

    Float* const vectorData{vector.data()};

    ASSERT_NE(vectorData, nullptr);
    EXPECT_EQ(vectorData, &vector[0]);

    EXPECT_FLOAT_EQ(vectorData[0], 1.0f);
    EXPECT_FLOAT_EQ(vectorData[1], 2.0f);
    EXPECT_FLOAT_EQ(vectorData[2], 3.0f);
    EXPECT_FLOAT_EQ(vectorData[3], 4.0f);

    vectorData[2] = 10.0f;

    EXPECT_FLOAT_EQ(vector[2], 10.0f);
    EXPECT_FLOAT_EQ(vector.z(), 10.0f);

    Vec4 const& constVector{vector};
    Float const* const constData{constVector.data()};

    EXPECT_EQ(constData, vectorData);
    EXPECT_FLOAT_EQ(constData[2], 10.0f);
}

// 验证 Mat4::data() 列主序
TEST_F(NamicaRuntimeTest, Mat4DataUsesColumnMajorOrder)
{
    Mat4 matrix{1.0f,
                2.0f,
                3.0f,
                4.0f,
                5.0f,
                6.0f,
                7.0f,
                8.0f,
                9.0f,
                10.0f,
                11.0f,
                12.0f,
                13.0f,
                14.0f,
                15.0f,
                16.0f};

    Float const expected[16]{1.0f,
                             5.0f,
                             9.0f,
                             13.0f,
                             2.0f,
                             6.0f,
                             10.0f,
                             14.0f,
                             3.0f,
                             7.0f,
                             11.0f,
                             15.0f,
                             4.0f,
                             8.0f,
                             12.0f,
                             16.0f};

    Float* const matrixData{matrix.data()};

    ASSERT_NE(matrixData, nullptr);
    EXPECT_EQ(matrixData, matrix[0].data());

    for (UInt index{0}; index < 16; ++index)
    {
        EXPECT_FLOAT_EQ(matrixData[index], expected[index]) << "列主序索引错误, index = " << index;
    }

    ExpectVectorNear(matrix[0], Vec4{1.0f, 5.0f, 9.0f, 13.0f});
    ExpectVectorNear(matrix[1], Vec4{2.0f, 6.0f, 10.0f, 14.0f});
    ExpectVectorNear(matrix[2], Vec4{3.0f, 7.0f, 11.0f, 15.0f});
    ExpectVectorNear(matrix[3], Vec4{4.0f, 8.0f, 12.0f, 16.0f});
}

// 验证通过 data() 修改矩阵
TEST_F(NamicaRuntimeTest, Mat4DataMutationUpdatesColumnMajorElement)
{
    Mat4 matrix{0.0f};

    constexpr UInt row{2};
    constexpr UInt column{1};
    constexpr UInt dataIndex{column * 4 + row};

    Float* const matrixData{matrix.data()};
    matrixData[dataIndex] = 42.0f;

    EXPECT_FLOAT_EQ(matrix(2, 1), 42.0f);
    EXPECT_FLOAT_EQ(matrix[1][2], 42.0f);

    Mat4 const& constMatrix{matrix};
    Float const* const constData{constMatrix.data()};

    EXPECT_EQ(constData, matrixData);
    EXPECT_FLOAT_EQ(constData[dataIndex], 42.0f);
}

TEST_F(NamicaRuntimeTest, TranslateIdentityMatrixCreatesExpectedTranslation)
{
    Mat4 const identity{1.0f};
    Vec3 const offset{2.0f, -3.0f, 4.5f};

    Mat4 const actual{identity.translated(offset)};

    Mat4 const expected{1.0f,
                        0.0f,
                        0.0f,
                        2.0f,
                        0.0f,
                        1.0f,
                        0.0f,
                        -3.0f,
                        0.0f,
                        0.0f,
                        1.0f,
                        4.5f,
                        0.0f,
                        0.0f,
                        0.0f,
                        1.0f};

    // 验证T一致
    ExpectMat4Near(actual, expected);

    // 列主序下，第 3 列应该保存平移。
    ExpectVectorNear(actual[3], Vec4{2.0f, -3.0f, 4.5f, 1.0f});
}

TEST_F(NamicaRuntimeTest, TranslateWithZeroOffsetPreservesMatrix)
{
    // clang-format off
    Mat4 const matrix{
         1.0f,  2.0f,  3.0f,  4.0f,
         5.0f,  6.0f,  7.0f,  8.0f,
         9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    };
    // clang-format on

    Mat4 const actual{matrix.translated(Vec3{0.0f, 0.0f, 0.0f})};

    ExpectMat4Near(actual, matrix);
}

TEST_F(NamicaRuntimeTest, TranslatePostMultipliesGeneralMatrix)
{
    // clang-format off
    Mat4 const matrix{
         1.0f,  2.0f,  3.0f,  4.0f,
         5.0f,  6.0f,  7.0f,  8.0f,
         9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    };
    // clang-format on

    Vec3 const offset{2.0f, -1.0f, 0.5f};

    Mat4 const actual{matrix.translated(offset)};

    // clang-format off
    // result[3] =
    //     matrix[0] * 2.0
    //   + matrix[1] * -1.0
    //   + matrix[2] * 0.5
    //   + matrix[3]
    Mat4 const expected{
         1.0f,  2.0f,  3.0f,  5.5f,
         5.0f,  6.0f,  7.0f, 15.5f,
         9.0f, 10.0f, 11.0f, 25.5f,
        13.0f, 14.0f, 15.0f, 35.5f
    };
    // clang-format on

    ExpectMat4Near(actual, expected);

    // translate 只改变第 3 列。
    ExpectVectorNear(actual[0], matrix[0]);
    ExpectVectorNear(actual[1], matrix[1]);
    ExpectVectorNear(actual[2], matrix[2]);
    ExpectVectorNear(actual[3], Vec4{5.5f, 15.5f, 25.5f, 35.5f});
}

// 验证 _offset 是沿输入矩阵局部坐标轴进行平移的。如果矩阵带有缩放，平移量也会受到基向量影响。
TEST_F(NamicaRuntimeTest, TranslateAppliesOffsetInMatrixLocalBasis)
{
    // clang-format off
    Mat4 const matrix{
        2.0f, 0.0f, 0.0f, 10.0f,
        0.0f, 3.0f, 0.0f, 20.0f,
        0.0f, 0.0f, 4.0f, 30.0f,
        0.0f, 0.0f, 0.0f,  1.0f
    };
    // clang-format on

    Mat4 const actual{matrix.translated(Vec3{1.0f, 2.0f, 3.0f})};

    // clang-format off
    Mat4 const expected{
        2.0f, 0.0f, 0.0f, 12.0f,
        0.0f, 3.0f, 0.0f, 26.0f,
        0.0f, 0.0f, 4.0f, 42.0f,
        0.0f, 0.0f, 0.0f,  1.0f
    };
    // clang-format on

    ExpectMat4Near(actual, expected);

    // 局部原点转换后应落在新的平移位置。
    Vec4 const transformedOrigin{actual * Vec4{0.0f, 0.0f, 0.0f, 1.0f}};

    ExpectVectorNear(transformedOrigin, Vec4{12.0f, 26.0f, 42.0f, 1.0f});
}

// 验证齐次坐标语义：平移影响 w == 1 的点，不影响 w == 0 的方向。
TEST_F(NamicaRuntimeTest, TranslateAffectsPointButNotDirection)
{
    Mat4 const translated{Mat4{1.0f}.translate(Vec3{3.0f, -2.0f, 5.0f})};

    Vec4 const point{1.0f, 2.0f, 3.0f, 1.0f};
    Vec4 const direction{1.0f, 2.0f, 3.0f, 0.0f};

    Vec4 const transformedPoint{translated * point};
    Vec4 const transformedDirection{translated * direction};

    ExpectVectorNear(transformedPoint, Vec4{4.0f, 0.0f, 8.0f, 1.0f});
    ExpectVectorNear(transformedDirection, Vec4{1.0f, 2.0f, 3.0f, 0.0f});
}

TEST_F(NamicaRuntimeTest, TranslateCompositionAccumulatesOffsets)
{
    Mat4 const first{Mat4{1.0f}.translate(Vec3{1.0f, -2.0f, 3.0f})};
    Mat4 const second{first.translated(Vec3{-4.0f, 5.0f, 0.5f})};

    // clang-format off
    Mat4 const expected{
        1.0f, 0.0f, 0.0f, -3.0f,
        0.0f, 1.0f, 0.0f,  3.0f,
        0.0f, 0.0f, 1.0f,  3.5f,
        0.0f, 0.0f, 0.0f,  1.0f
    };
    // clang-format on

    ExpectMat4Near(second, expected);
}

// 输入矩阵不被修改
TEST_F(NamicaRuntimeTest, TranslateDoesNotModifySourceMatrix)
{
    // clang-format off
    Mat4 matrix{
        2.0f, 0.0f, 0.0f, 10.0f,
        0.0f, 3.0f, 0.0f, 20.0f,
        0.0f, 0.0f, 4.0f, 30.0f,
        0.0f, 0.0f, 0.0f,  1.0f
    };
    // clang-format on

    Mat4 const before{matrix};
    Mat4 const translated{matrix.translated(Vec3{1.0f, 2.0f, 3.0f})};

    ExpectMat4Near(matrix, before);
    ExpectVectorNear(translated[3], Vec4{12.0f, 26.0f, 42.0f, 1.0f});
}

// 验证vec normalize

// 验证原地修改和返回引用
TEST_F(NamicaRuntimeTest, NormalizeMutatesVectorAndReturnsSelf)
{
    Vec3 vector{3.0f, 0.0f, 4.0f};

    Vec3& returned{vector.normalize()};

    EXPECT_EQ(&returned, &vector);

    ExpectVectorNear(vector, Vec3{0.6f, 0.0f, 0.8f});

    EXPECT_NEAR(vector.dot(vector), 1.0f, kEpsilon);
}

// 验证 normalized() 返回副本且不修改源向量
TEST_F(NamicaRuntimeTest, NormalizedReturnsUnitCopyAndPreservesSource)
{
    Vec3 const source{0.0f, -3.0f, 4.0f};

    Vec3 const result{source.normalized()};

    ExpectVectorNear(source, Vec3{0.0f, -3.0f, 4.0f});

    ExpectVectorNear(result, Vec3{0.0f, -0.6f, 0.8f});

    EXPECT_NEAR(result.dot(result), 1.0f, kEpsilon);
}

// 验证零向量保持不变
TEST_F(NamicaRuntimeTest, NormalizeZeroVectorLeavesItUnchanged)
{
    Vec3 vector{};

    Vec3& returned{vector.normalize()};
    Vec3 const copied{Vec3{}.normalized()};

    EXPECT_EQ(&returned, &vector);

    ExpectVectorNear(vector, Vec3{});
    ExpectVectorNear(copied, Vec3{});

    EXPECT_FLOAT_EQ(vector.dot(vector), 0.0f);
    EXPECT_FLOAT_EQ(copied.dot(copied), 0.0f);
}

// 覆盖 Vec2 和 Vec4
TEST_F(NamicaRuntimeTest, NormalizeSupportsVec2AndVec4)
{
    Vec2 const normalized2{Vec2{3.0f, 4.0f}.normalized()};

    Vec4 const normalized4{Vec4{1.0f, -2.0f, 2.0f, 0.0f}.normalized()};

    ExpectVectorNear(normalized2, Vec2{0.6f, 0.8f});

    ExpectVectorNear(normalized4, Vec4{1.0f / 3.0f, -2.0f / 3.0f, 2.0f / 3.0f, 0.0f});

    EXPECT_NEAR(normalized2.dot(normalized2), 1.0f, kEpsilon);

    EXPECT_NEAR(normalized4.dot(normalized4), 1.0f, kEpsilon);
}

// TODO: normalize 实现 max-abs 预缩放后移除 DISABLED_。
TEST_F(NamicaRuntimeTest, DISABLED_NormalizedMaximumMagnitudeVectorAvoidsOverflow)
{
    constexpr Float maximum{std::numeric_limits<Float>::max()};

    constexpr Float inverseSqrt3{0.57735026919f};

    Vec3 const result{Vec3{maximum, -maximum, maximum}.normalized()};

    ExpectVectorNear(result, Vec3{inverseSqrt3, -inverseSqrt3, inverseSqrt3}, 1e-5f);

    EXPECT_NEAR(result.dot(result), 1.0f, 1e-5f);
}

// TODO: normalize 实现稳定归一化后移除 DISABLED_。下溢测试
TEST_F(NamicaRuntimeTest, DISABLED_NormalizedSmallestNonZeroVectorAvoidsUnderflow)
{
    constexpr Float minimum{std::numeric_limits<Float>::min()};

    Vec3 const result{Vec3{minimum, 0.0f, 0.0f}.normalized()};

    ExpectVectorNear(result, Vec3{1.0f, 0.0f, 0.0f});

    EXPECT_NEAR(result.dot(result), 1.0f, kEpsilon);
}

// 验证mat rotate

// 验证右手系旋转方向
TEST_F(NamicaRuntimeTest, RotatePositiveQuarterTurnsFollowRightHandRule)
{
    constexpr Float angle{std::numbers::pi_v<Float> * 0.5f};  // 旋转角度: 90

    Mat4 const identity{1.0f};

    Mat4 const rotationX{identity.rotated(angle, Vec3{1.0f, 0.0f, 0.0f})};

    Mat4 const rotationY{identity.rotated(angle, Vec3{0.0f, 1.0f, 0.0f})};

    Mat4 const rotationZ{identity.rotated(angle, Vec3{0.0f, 0.0f, 1.0f})};

    // +Y 绕 +X 旋转 90° -> +Z。
    ExpectVectorNear(rotationX * Vec4{0.0f, 1.0f, 0.0f, 0.0f}, Vec4{0.0f, 0.0f, 1.0f, 0.0f});

    // +Z 绕 +Y 旋转 90° -> +X。
    ExpectVectorNear(rotationY * Vec4{0.0f, 0.0f, 1.0f, 0.0f}, Vec4{1.0f, 0.0f, 0.0f, 0.0f});

    // +X 绕 +Z 旋转 90° -> +Y。
    ExpectVectorNear(rotationZ * Vec4{1.0f, 0.0f, 0.0f, 0.0f}, Vec4{0.0f, 1.0f, 0.0f, 0.0f});
}

// 验证 rotate() 原地修改，而 rotated() 返回副本
TEST_F(NamicaRuntimeTest, RotateMutatesWhileRotatedReturnsCopy)
{
    constexpr Float angle{std::numbers::pi_v<Float> * 0.5f};

    // clang-format off
    Mat4 const source{
        2.0f, 0.0f, 0.0f, 10.0f,
        0.0f, 3.0f, 0.0f, 20.0f,
        0.0f, 0.0f, 4.0f, 30.0f,
        0.0f, 0.0f, 0.0f,  1.0f
    };
    // clang-format on

    Mat4 const before{source};

    Mat4 const copied{source.rotated(angle, Vec3{0.0f, 0.0f, 1.0f})};

    // rotated() 不修改源矩阵。
    ExpectMat4Near(source, before);

    Mat4 mutableMatrix{source};

    Mat4& returned{mutableMatrix.rotate(angle, Vec3{0.0f, 0.0f, 1.0f})};

    EXPECT_EQ(&returned, &mutableMatrix);
    ExpectMat4Near(mutableMatrix, copied);
}

// 验证零旋转轴
TEST_F(NamicaRuntimeTest, RotateWithZeroAxisLeavesMatrixUnchanged)
{
    // clang-format off
    Mat4 matrix{
         1.0f,  2.0f,  3.0f,  4.0f,
         5.0f,  6.0f,  7.0f,  8.0f,
         9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    };
    // clang-format on

    Mat4 const before{matrix};

    Mat4& returned{matrix.rotate(0.75f, Vec3{})};

    Mat4 const copied{before.rotated(0.75f, Vec3{})};

    EXPECT_EQ(&returned, &matrix);
    ExpectMat4Near(matrix, before);
    ExpectMat4Near(copied, before);
}

// 验证旋转轴自动归一化
TEST_F(NamicaRuntimeTest, RotateNormalizesNonUnitAxis)
{
    constexpr Float angle{0.73f};

    Vec3 const axis{1.0f, -2.0f, 3.0f};
    Vec3 const scaledAxis{7.0f, -14.0f, 21.0f};

    Mat4 const identity{1.0f};

    Mat4 const withAxis{identity.rotated(angle, axis)};

    Mat4 const withScaledAxis{identity.rotated(angle, scaledAxis)};

    ExpectMat4Near(withScaledAxis, withAxis, 1e-5f);
}

// 验证当前为后乘、局部坐标轴旋转
TEST_F(NamicaRuntimeTest, RotatePostMultipliesInLocalCoordinateSystem)
{
    constexpr Float angle{std::numbers::pi_v<Float> * 0.5f};

    // clang-format off
    Mat4 const matrix{
        2.0f, 0.0f, 0.0f, 10.0f,
        0.0f, 3.0f, 0.0f, 20.0f,
        0.0f, 0.0f, 4.0f, 30.0f,
        0.0f, 0.0f, 0.0f,  1.0f
    };

    // matrix * rotationZ(90°)
    Mat4 const expected{
        0.0f, -2.0f, 0.0f, 10.0f,
        3.0f,  0.0f, 0.0f, 20.0f,
        0.0f,  0.0f, 4.0f, 30.0f,
        0.0f,  0.0f, 0.0f,  1.0f
    };
    // clang-format on

    Mat4 const actual{matrix.rotated(angle, Vec3{0.0f, 0.0f, 1.0f})};

    ExpectMat4Near(actual, expected);

    // 后乘不会改变原有平移列。
    ExpectVectorNear(actual[3], Vec4{10.0f, 20.0f, 30.0f, 1.0f});
}

// 验证旋转与逆旋转
TEST_F(NamicaRuntimeTest, RotateFollowedByInverseReturnsIdentity)
{
    constexpr Float angle{0.83f};
    Vec3 const axis{1.0f, -2.0f, 3.0f};

    Mat4 const identity{1.0f};

    Mat4 const rotation{identity.rotated(angle, axis)};

    Mat4 const inverse{identity.rotated(-angle, axis)};

    ExpectMat4Near(rotation * inverse, identity, 1e-5f);

    ExpectMat4Near(inverse * rotation, identity, 1e-5f);
}

// TODO: normalize 实现稳定归一化后移除 DISABLED_。
TEST_F(NamicaRuntimeTest, DISABLED_RotateMaximumMagnitudeAxisMatchesEquivalentAxis)
{
    constexpr Float maximum{std::numeric_limits<Float>::max()};

    constexpr Float angle{0.73f};

    Mat4 const identity{1.0f};

    Mat4 const actual{identity.rotated(angle, Vec3{maximum, maximum, maximum})};

    Mat4 const expected{identity.rotated(angle, Vec3{1.0f, 1.0f, 1.0f})};

    ExpectMat4Near(actual, expected, 1e-5f);
}