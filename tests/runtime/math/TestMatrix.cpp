#include <numbers>

#include "runtime/utils/TestUtils.h"
#include "runtime/math/utils/MathTestUtils.h"

#include <namica/core/Types.h>

using namespace namica;

// 测试矩阵乘法正确性

namespace
{

template <typename V>
concept Normalizable = requires(V& _value, V const& _constValue) {
    {
        _value.normalize()
    } -> std::same_as<V&>;
    {
        _constValue.normalized()
    } -> std::same_as<V>;
};

static_assert(Normalizable<Vec2>);
static_assert(Normalizable<Vec3>);
static_assert(Normalizable<Vec4>);

// 当前 normalize 只允许浮点向量调用。
static_assert(!Normalizable<Veci2>);

}  // namespace

// 用单位矩阵左乘包含不同分量的列向量，验证单位变换逐分量保持原值。
// 同时检查近似比较与 isEqual，避免矩阵乘向量的索引或维度处理发生偏移。
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

// 验证 data() 借用向量实际存储：地址应等于首元素，连续四个值按分量顺序排列。
// 通过返回指针修改 z 后，索引、命名访问器和 const data() 都应看到同一个值。
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

// 以行视图给出 1 到 16 的矩阵，检查 data() 暴露的连续内存按列主序排列。
// 同时核对每列向量与首地址，防止构造参数顺序与底层存储顺序被误当作相同。
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

// 通过 column * 4 + row 的偏移修改零矩阵中的一个非对角元素。
// 矩阵调用、列索引与 const data() 应读到同一修改，验证指针没有指向副本。
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

// 从单位矩阵产生含正数、负数和小数位移的平移矩阵。
// 期望仅最后一列携带位移且齐次分量为 1，用显式矩阵核对平移位置。
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

// 对元素各不相同的一般矩阵施加零位移，期望所有元素保持不变。
// 使用非单位矩阵，避免只能在初始化状态下满足恒等操作。
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

// 对一般矩阵施加混合符号位移，验证结果遵循 matrix * translation 的后乘约定。
// 期望前 3 列不变，最后一列按局部基向量线性组合；下方显式列出计算结果。
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

// 给带非均匀缩放和已有平移的矩阵追加局部位移 (1, 2, 3)。
// 新平移应为 (12, 26, 42)，而非简单逐分量相加；再用原点变换核对该位置。
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

// 对 xyz 相同但 w 分别为 1 和 0 的两个向量施加相同平移。
// 点应加上位移，方向应保持不变，以区分齐次坐标的点与方向语义。
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

// 在单位基下连续施加两个含正负分量的平移，验证位移按分量累加。
// 结果应为 (-3, 3, 3.5)，其余单位矩阵元素保持不变。
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

// 在带缩放和平移的矩阵上调用返回副本的 translated()。
// 源矩阵应保持完整原值，结果副本的平移列应反映新增局部位移。
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

// 用长度为 5 的 (3, 0, 4) 验证 normalize() 原地除以长度。
// 期望分量变为 (0.6, 0, 0.8)、平方长度为 1，并返回原对象引用供链式调用。
TEST_F(NamicaRuntimeTest, NormalizeMutatesVectorAndReturnsSelf)
{
    Vec3 vector{3.0f, 0.0f, 4.0f};

    Vec3& returned{vector.normalize()};

    EXPECT_EQ(&returned, &vector);

    ExpectVectorNear(vector, Vec3{0.6f, 0.0f, 0.8f});

    EXPECT_NEAR(vector.dot(vector), 1.0f, kEpsilon);
}

// 对含负分量且长度为 5 的向量调用 normalized()，检查得到单位长度副本。
// 源向量必须保持 (0, -3, 4)，不能因生成副本而被原地修改。
TEST_F(NamicaRuntimeTest, NormalizedReturnsUnitCopyAndPreservesSource)
{
    Vec3 const source{0.0f, -3.0f, 4.0f};

    Vec3 const result{source.normalized()};

    ExpectVectorNear(source, Vec3{0.0f, -3.0f, 4.0f});

    ExpectVectorNear(result, Vec3{0.0f, -0.6f, 0.8f});

    EXPECT_NEAR(result.dot(result), 1.0f, kEpsilon);
}

// 分别对零向量使用原地与副本归一化接口，验证零长度边界保持零值。
// 检查返回引用和平方长度，防止除零使结果变为 NaN 或无穷值。
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

// 用已知长度为 5 和 3 的 Vec2、Vec4 检查归一化模板覆盖不同维数。
// 显式核对各分量与单位长度，确保四维版本没有遗漏末尾分量或错误使用三维公式。
TEST_F(NamicaRuntimeTest, NormalizeSupportsVec2AndVec4)
{
    Vec2 const normalized2{Vec2{3.0f, 4.0f}.normalized()};

    Vec4 const normalized4{Vec4{1.0f, -2.0f, 2.0f, 0.0f}.normalized()};

    ExpectVectorNear(normalized2, Vec2{0.6f, 0.8f});

    ExpectVectorNear(normalized4, Vec4{1.0f / 3.0f, -2.0f / 3.0f, 2.0f / 3.0f, 0.0f});

    EXPECT_NEAR(normalized2.dot(normalized2), 1.0f, kEpsilon);

    EXPECT_NEAR(normalized4.dot(normalized4), 1.0f, kEpsilon);
}

// 待实现稳定归一化：输入最大有限浮点值构成的向量，方向应仍为 (1, -1, 1)。
// 期望单位向量分量为正负 1/sqrt(3)；当前平方求和可能溢出，因此保持 DISABLED_。
TEST_F(NamicaRuntimeTest, DISABLED_NormalizedMaximumMagnitudeVectorAvoidsOverflow)
{
    constexpr Float maximum{std::numeric_limits<Float>::max()};

    constexpr Float inverseSqrt3{0.57735026919f};

    Vec3 const result{Vec3{maximum, -maximum, maximum}.normalized()};

    ExpectVectorNear(result, Vec3{inverseSqrt3, -inverseSqrt3, inverseSqrt3}, 1e-5f);

    EXPECT_NEAR(result.dot(result), 1.0f, 1e-5f);
}

// 待实现稳定归一化：输入最小正正规浮点值沿 X 轴的非零向量。
// 结果应为单位 X 方向；当前长度平方可能下溢到零，因此保持 DISABLED_。
TEST_F(NamicaRuntimeTest, DISABLED_NormalizedSmallestNonZeroVectorAvoidsUnderflow)
{
    constexpr Float minimum{std::numeric_limits<Float>::min()};

    Vec3 const result{Vec3{minimum, 0.0f, 0.0f}.normalized()};

    ExpectVectorNear(result, Vec3{1.0f, 0.0f, 0.0f});

    EXPECT_NEAR(result.dot(result), 1.0f, kEpsilon);
}

// 验证mat rotate

// 分别绕 X、Y、Z 正轴旋转 90 度，检查对应基向量按右手定则转向。
// 三个轴都用已知结果验证，避免转置或正负号错误只在单个轴下被掩盖。
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

// 分别调用 rotate() 与 rotated()，比较同一旋转下原地结果和副本结果。
// 原地接口应返回自身引用，副本接口应保留源矩阵，二者计算出的矩阵应一致。
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

// 对元素不同的一般矩阵传入零旋转轴，检查该无方向输入的既有行为。
// 原地与副本接口都应保持原矩阵；原地接口仍返回自身引用。
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

// 用同方向且相差 7 倍长度的两个轴施加同一个旋转角。
// 两份结果矩阵应近似一致，验证旋转由轴方向决定而非轴长度决定。
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

// 在含非均匀缩放和平移的矩阵上追加 Z 轴 90 度旋转。
// 显式期望 matrix * rotation 的结果且平移列不变，以区分局部后乘与世界前乘。
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

// 以同一非单位轴构造正角和负角旋转，分别检查两个相乘顺序。
// 两种乘积均应回到单位矩阵，验证轴归一化后旋转与逆旋转正确抵消。
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

// 待实现稳定轴归一化：最大有限分量的 (1, 1, 1) 方向应与普通尺度同方向等价。
// 当前巨大轴的长度计算可能溢出，因此此回归保持 DISABLED_，不宣称行为已实现。
TEST_F(NamicaRuntimeTest, DISABLED_RotateMaximumMagnitudeAxisMatchesEquivalentAxis)
{
    constexpr Float maximum{std::numeric_limits<Float>::max()};

    constexpr Float angle{0.73f};

    Mat4 const identity{1.0f};

    Mat4 const actual{identity.rotated(angle, Vec3{maximum, maximum, maximum})};

    Mat4 const expected{identity.rotated(angle, Vec3{1.0f, 1.0f, 1.0f})};

    ExpectMat4Near(actual, expected, 1e-5f);
}

// 验证mat scale

// 对单位矩阵施加正数、负数和小数缩放，核对显式对角矩阵。
// 前三个对角元应等于缩放因子，齐次分量保持 1，其他元素保持 0。
TEST_F(NamicaRuntimeTest, ScaleIdentityMatrixCreatesExpectedScaleMatrix)
{
    Mat4 const actual{Mat4{1.0f}.scaled(Vec3{2.0f, -3.0f, 0.5f})};

    // clang-format off
    Mat4 const expected{
        2.0f,  0.0f, 0.0f, 0.0f,
        0.0f, -3.0f, 0.0f, 0.0f,
        0.0f,  0.0f, 0.5f, 0.0f,
        0.0f,  0.0f, 0.0f, 1.0f
    };
    // clang-format on

    ExpectMat4Near(actual, expected);
}

// 对一般矩阵施加 (1, 1, 1) 缩放，期望全部元素保持原值。
// 非单位输入可检出缩放实现错误重建矩阵或丢失已有变换。
TEST_F(NamicaRuntimeTest, ScaleWithUnitFactorsPreservesMatrix)
{
    // clang-format off
    Mat4 const matrix{
         1.0f,  2.0f,  3.0f,  4.0f,
         5.0f,  6.0f,  7.0f,  8.0f,
         9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    };
    // clang-format on

    Mat4 const actual{matrix.scaled(Vec3{1.0f, 1.0f, 1.0f})};

    ExpectMat4Near(actual, matrix);
}

// 对元素各异的一般矩阵施加混合符号缩放，检查后乘缩放逐列作用。
// 前三列分别乘对应因子，最后一列不变，并与显式期望矩阵交叉核对。
TEST_F(NamicaRuntimeTest, ScalePostMultipliesGeneralMatrix)
{
    // clang-format off
    Mat4 const matrix{
         1.0f,  2.0f,  3.0f,  4.0f,
         5.0f,  6.0f,  7.0f,  8.0f,
         9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    };

    Mat4 const expected{
         2.0f,  -6.0f, 1.5f,  4.0f,
        10.0f, -18.0f, 3.5f,  8.0f,
        18.0f, -30.0f, 5.5f, 12.0f,
        26.0f, -42.0f, 7.5f, 16.0f
    };
    // clang-format on

    Vec3 const factors{2.0f, -3.0f, 0.5f};

    Mat4 const actual{matrix.scaled(factors)};

    ExpectMat4Near(actual, expected);

    ExpectVectorNear(actual[0], matrix[0] * factors.x());

    ExpectVectorNear(actual[1], matrix[1] * factors.y());

    ExpectVectorNear(actual[2], matrix[2] * factors.z());

    // 后乘缩放不改变平移列。
    ExpectVectorNear(actual[3], matrix[3]);
}

// 对已有缩放和平移的矩阵施加含零因子的缩放，比较原地和副本接口。
// scaled() 应保留源对象，scale() 应返回自身引用；两种结果一致且平移列不变。
TEST_F(NamicaRuntimeTest, ScaleMutatesWhileScaledReturnsCopy)
{
    // clang-format off
    Mat4 const source{
        2.0f, 0.0f, 0.0f, 10.0f,
        0.0f, 3.0f, 0.0f, 20.0f,
        0.0f, 0.0f, 4.0f, 30.0f,
        0.0f, 0.0f, 0.0f,  1.0f
    };
    // clang-format on

    Vec3 const factors{0.5f, -2.0f, 0.0f};

    Mat4 const before{source};

    Mat4 const copied{source.scaled(factors)};

    // scaled() 不修改源矩阵。
    ExpectMat4Near(source, before);

    Mat4 mutableMatrix{source};

    Mat4& returned{mutableMatrix.scale(factors)};

    EXPECT_EQ(&returned, &mutableMatrix);
    ExpectMat4Near(mutableMatrix, copied);

    // 平移列始终保持不变。
    ExpectVectorNear(copied[3], source[3]);
}

// 以 (0, -2, 0.5) 同时覆盖压扁轴、反向和缩小三类缩放。
// 同 xyz 的点和方向应得到相同缩放后的 xyz，各自的齐次 w 保持原值。
TEST_F(NamicaRuntimeTest, ScaleSupportsZeroNegativeAndFractionalFactors)
{
    Mat4 const scaling{Mat4{1.0f}.scaled(Vec3{0.0f, -2.0f, 0.5f})};

    Vec4 const point{3.0f, 4.0f, -6.0f, 1.0f};

    Vec4 const direction{3.0f, 4.0f, -6.0f, 0.0f};

    ExpectVectorNear(scaling * point, Vec4{0.0f, -8.0f, -3.0f, 1.0f});

    ExpectVectorNear(scaling * direction, Vec4{0.0f, -8.0f, -3.0f, 0.0f});
}

// 连续施加两组非均匀缩放，验证各轴因子相乘而不是相加。
// 期望合成为 (8, 6, 4)，并检查链式调用返回最初矩阵的引用。
TEST_F(NamicaRuntimeTest, ScaleCompositionMultipliesFactors)
{
    Mat4 matrix{1.0f};

    Mat4& returned{matrix.scale(Vec3{2.0f, -3.0f, 0.5f}).scale(Vec3{4.0f, -2.0f, 8.0f})};

    EXPECT_EQ(&returned, &matrix);

    // clang-format off
    Mat4 const expected{
        8.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 6.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 4.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    // clang-format on

    ExpectMat4Near(matrix, expected);
}

// 矩阵 平移、旋转、缩放组合测试

// 按 translate、rotate、scale 的调用顺序构造 T * Rz(90°) * S。
// 用显式矩阵及点、方向和原点的已知结果检查组合顺序，副本链应与原地链一致。
TEST_F(NamicaRuntimeTest, AffineTranslateRotateScaleComposesAsTrs)
{
    constexpr Float angle{std::numbers::pi_v<Float> * 0.5f};

    Vec3 const translation{10.0f, 20.0f, 30.0f};

    Vec3 const rotationAxis{0.0f, 0.0f, 1.0f};

    Vec3 const scaleFactors{2.0f, 3.0f, 4.0f};

    Mat4 actual{1.0f};

    Mat4& returned{actual.translate(translation).rotate(angle, rotationAxis).scale(scaleFactors)};

    EXPECT_EQ(&returned, &actual);

    // actual = T * Rz(90°) * S
    //
    // clang-format off
    Mat4 const expected{
        0.0f, -3.0f, 0.0f, 10.0f,
        2.0f,  0.0f, 0.0f, 20.0f,
        0.0f,  0.0f, 4.0f, 30.0f,
        0.0f,  0.0f, 0.0f,  1.0f
    };
    // clang-format on

    ExpectMat4Near(actual, expected, 1e-5f);

    // 非原地版本应得到相同结果。
    Mat4 const identity{1.0f};

    Mat4 const copied{
        identity.translated(translation).rotated(angle, rotationAxis).scaled(scaleFactors)};

    ExpectMat4Near(copied, expected, 1e-5f);

    // identity 不应被非原地接口修改。
    ExpectMat4Near(identity, Mat4{1.0f});

    Vec4 const point{1.0f, 2.0f, 3.0f, 1.0f};

    Vec4 const direction{1.0f, 2.0f, 3.0f, 0.0f};

    Vec4 const origin{0.0f, 0.0f, 0.0f, 1.0f};

    // 点的实际变换过程：
    //
    // Scale:
    //     (1, 2, 3) -> (2, 6, 12)
    //
    // Rotate Z 90°:
    //     (2, 6, 12) -> (-6, 2, 12)
    //
    // Translate:
    //     (-6, 2, 12) -> (4, 22, 42)
    ExpectVectorNear(actual * point, Vec4{4.0f, 22.0f, 42.0f, 1.0f}, 1e-5f);

    // w == 0 的方向向量不受平移影响。
    ExpectVectorNear(actual * direction, Vec4{-6.0f, 2.0f, 12.0f, 0.0f}, 1e-5f);

    // 局部原点最终移动到平移位置。
    ExpectVectorNear(actual * origin, Vec4{10.0f, 20.0f, 30.0f, 1.0f}, 1e-5f);
}

// 矩阵求逆测试

// 分别以零矩阵、单位矩阵、对角矩阵和一般矩阵验证行列式的基础数值。
// 再交换一般矩阵的两行，结果应只改变符号，以检查排列符号处理。
TEST_F(NamicaRuntimeTest, DeterminantHandlesBasicAndRowSwapCases)
{
    Mat4 const zero{};
    Mat4 const identity{1.0f};

    // clang-format off
    Mat4 const diagonal{
        2.0f,  0.0f, 0.0f, 0.0f,
        0.0f, -3.0f, 0.0f, 0.0f,
        0.0f,  0.0f, 4.0f, 0.0f,
        0.0f,  0.0f, 0.0f, 5.0f
    };

    Mat4 const general{
        1.0f, 2.0f, 0.0f, 0.0f,
        1.0f, 3.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 2.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 2.0f
    };

    // general 交换前两行，determinant 应改变符号。
    Mat4 const firstTwoRowsSwapped{
        1.0f, 3.0f, 1.0f, 0.0f,
        1.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 2.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 2.0f
    };
    // clang-format on

    EXPECT_FLOAT_EQ(zero.determinant(), 0.0f);
    EXPECT_FLOAT_EQ(identity.determinant(), 1.0f);
    EXPECT_FLOAT_EQ(diagonal.determinant(), -120.0f);
    EXPECT_FLOAT_EQ(general.determinant(), 1.0f);
    EXPECT_FLOAT_EQ(firstTwoRowsSwapped.determinant(), -1.0f);
}

// 对含正数、负数和小数对角元的可逆矩阵求逆，期望各对角元取倒数。
// 同时检查左右乘积为单位矩阵，逆矩阵的行列式为原行列式倒数。
TEST_F(NamicaRuntimeTest, InverseDiagonalMatrixUsesReciprocalDiagonal)
{
    // clang-format off
    Mat4 const matrix{
        2.0f,  0.0f, 0.0f, 0.0f,
        0.0f, -4.0f, 0.0f, 0.0f,
        0.0f,  0.0f, 0.5f, 0.0f,
        0.0f,  0.0f, 0.0f, 5.0f
    };

    Mat4 const expected{
        0.5f,  0.0f, 0.0f, 0.0f,
        0.0f, -0.25f, 0.0f, 0.0f,
        0.0f,  0.0f, 2.0f, 0.0f,
        0.0f,  0.0f, 0.0f, 0.2f
    };
    // clang-format on

    Mat4 const actual{matrix.inversed()};

    ExpectMat4Near(actual, expected);
    ExpectMat4Near(matrix * actual, Mat4{1.0f});
    ExpectMat4Near(actual * matrix, Mat4{1.0f});

    EXPECT_NEAR(actual.determinant(), 1.0f / matrix.determinant(), kEpsilon);
}

// 用显式已知逆矩阵检查一般矩阵求逆，避免仅验证互乘而掩盖共同索引错误。
// 同时核对左右逆、inversed() 不修改源对象，以及 inverse() 原地更新并返回自身。
TEST_F(NamicaRuntimeTest, InverseGeneralMatrixProducesExpectedResult)
{
    // clang-format off
    Mat4 source{
        1.0f, 2.0f, 0.0f, 0.0f,
        1.0f, 3.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 2.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 2.0f
    };

    Mat4 const expected{
         7.0f, -6.0f,  4.0f, -2.0f,
        -3.0f,  3.0f, -2.0f,  1.0f,
         2.0f, -2.0f,  2.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,  1.0f
    };
    // clang-format on

    Mat4 const before{source};

    // inversed() 返回副本，不修改源矩阵。
    Mat4 const copied{source.inversed()};

    ExpectMat4Near(source, before);
    ExpectMat4Near(copied, expected, 1e-5f);

    // 同时验证左逆和右逆。
    ExpectMat4Near(before * copied, Mat4{1.0f}, 1e-5f);
    ExpectMat4Near(copied * before, Mat4{1.0f}, 1e-5f);

    // inverse() 原地修改并返回自身引用。
    Mat4& returned{source.inverse()};

    EXPECT_EQ(&returned, &source);
    ExpectMat4Near(source, expected, 1e-5f);
}

// 对平移、Z 轴旋转和非均匀缩放组成的仿射矩阵求逆。
// 显式检查逆矩阵与行列式，并让点和方向往返变换，验证平移及齐次分量处理。
TEST_F(NamicaRuntimeTest, InverseAffineTrsMatrixRestoresPointAndDirection)
{
    constexpr Float angle{std::numbers::pi_v<Float> * 0.5f};

    Mat4 transform{1.0f};

    transform.translate(Vec3{10.0f, 20.0f, 30.0f})
        .rotate(angle, Vec3{0.0f, 0.0f, 1.0f})
        .scale(Vec3{2.0f, 3.0f, 4.0f});

    // transform = T * Rz(90°) * S
    // det(T) = 1，det(R) = 1，det(S) = 2 * 3 * 4。
    EXPECT_NEAR(transform.determinant(), 24.0f, 1e-4f);

    // clang-format off
    Mat4 const expectedInverse{
         0.0f,        0.5f, 0.0f, -10.0f,
        -1.0f / 3.0f, 0.0f, 0.0f,  10.0f / 3.0f,
         0.0f,        0.0f, 0.25f, -7.5f,
         0.0f,        0.0f, 0.0f,   1.0f
    };
    // clang-format on

    Mat4 const inverse{transform.inversed()};

    ExpectMat4Near(inverse, expectedInverse, 1e-5f);
    ExpectMat4Near(transform * inverse, Mat4{1.0f}, 1e-4f);
    ExpectMat4Near(inverse * transform, Mat4{1.0f}, 1e-4f);

    Vec4 const localPoint{1.0f, 2.0f, 3.0f, 1.0f};
    Vec4 const localDirection{1.0f, 2.0f, 3.0f, 0.0f};

    Vec4 const worldPoint{transform * localPoint};
    Vec4 const worldDirection{transform * localDirection};

    ExpectVectorNear(worldPoint, Vec4{4.0f, 22.0f, 42.0f, 1.0f}, 1e-5f);

    ExpectVectorNear(worldDirection, Vec4{-6.0f, 2.0f, 12.0f, 0.0f}, 1e-5f);

    ExpectVectorNear(inverse * worldPoint, localPoint, 1e-4f);
    ExpectVectorNear(inverse * worldDirection, localDirection, 1e-4f);
}

// 构造两行相同的奇异矩阵，先确认行列式为零，再调用两个求逆接口。
// 按当前契约结果保留原矩阵，原地接口返回自身；此测试不定义其他错误报告策略。
TEST_F(NamicaRuntimeTest, InverseSingularMatrixFollowsCurrentNoOpContract)
{
    // 前两行相同，因此不可逆。
    // clang-format off
    Mat4 singular{
        1.0f, 2.0f, 3.0f, 4.0f,
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    // clang-format on

    Mat4 const before{singular};

    EXPECT_FLOAT_EQ(singular.determinant(), 0.0f);

    Mat4 const copied{singular.inversed()};

    ExpectMat4Near(copied, before);
    ExpectMat4Near(singular, before);

    Mat4& returned{singular.inverse()};

    EXPECT_EQ(&returned, &singular);
    ExpectMat4Near(singular, before);
}

// 使用 0.01 倍单位矩阵，行列式虽仅为 1e-8，但条件数为 1，仍完全可逆。
// 期望逆为 100 倍单位矩阵，防止用固定行列式阈值误拒绝良态的小尺度矩阵。
TEST_F(NamicaRuntimeTest, InverseSmallWellConditionedMatrixIsNotRejected)
{
    // 4D 均匀缩放，condition number == 1，是完全可逆的矩阵。
    // 但 determinant == 1e-8，小于当前 NAMICA_TOL。
    Mat4 const matrix{0.01f};
    Mat4 const expected{100.0f};

    EXPECT_NEAR(matrix.determinant(), 1e-8f, 1e-12f);

    Mat4 const inverse{matrix.inversed()};

    ExpectMat4Near(inverse, expected, 1e-3f);
    ExpectMat4Near(matrix * inverse, Mat4{1.0f}, 1e-5f);
}

// perspective 相关测试

// 以垂直视场 90°、宽高比 2、近平面 1 和远平面 9 构造透视矩阵。
// 逐元素核对右手 OpenGL 约定下的显式矩阵，固定投影符号与深度映射。
TEST_F(NamicaRuntimeTest, PerspectiveCreatesExpectedRightHandedOpenGlMatrix)
{
    constexpr Float fov{std::numbers::pi_v<Float> / 2.0f};
    constexpr Float aspect{2.0f};
    constexpr Float zNear{1.0f};
    constexpr Float zFar{9.0f};

    Mat4 const actual{Mat4::perspective(fov, aspect, zNear, zFar)};

    // 以下按照矩阵的行视图书写。
    // clang-format off
    Mat4 const expected{
        0.5f, 0.0f,  0.0f,  0.0f,
        0.0f, 1.0f,  0.0f,  0.0f,
        0.0f, 0.0f, -1.25f, -2.25f,
        0.0f, 0.0f, -1.0f,  0.0f
    };
    // clang-format on

    ExpectMat4Near(actual, expected, 1e-5f);
}

// 将视线负 Z 轴上的近、远平面中心投影到裁剪空间，再做透视除法。
// NDC 深度应分别为 -1 和 1，clip.w 等于 -view.z；相机后方点的 w 应为负。
TEST_F(NamicaRuntimeTest, PerspectiveMapsNearAndFarPlanesToOpenGlNdcDepth)
{
    constexpr Float fov{std::numbers::pi_v<Float> / 2.0f};
    constexpr Float aspect{2.0f};
    constexpr Float zNear{1.0f};
    constexpr Float zFar{9.0f};

    Mat4 const projection{Mat4::perspective(fov, aspect, zNear, zFar)};

    // 右手观察空间中，相机前方是 -Z。
    Vec4 const nearClip{projection * Vec4{0.0f, 0.0f, -zNear, 1.0f}};

    Vec4 const farClip{projection * Vec4{0.0f, 0.0f, -zFar, 1.0f}};

    ExpectVectorNear(nearClip, Vec4{0.0f, 0.0f, -1.0f, 1.0f}, 1e-5f);

    ExpectVectorNear(farClip, Vec4{0.0f, 0.0f, 9.0f, 9.0f}, 1e-5f);

    // OpenGL NDC：near -> -1，far -> +1。
    EXPECT_NEAR(nearClip.z() / nearClip.w(), -1.0f, 1e-5f);

    EXPECT_NEAR(farClip.z() / farClip.w(), 1.0f, 1e-5f);

    // clip.w == -view.z。
    EXPECT_NEAR(nearClip.w(), zNear, kEpsilon);
    EXPECT_NEAR(farClip.w(), zFar, kEpsilon);

    // +Z 位于相机后方，因此 clip.w 为负。
    Vec4 const behindClip{projection * Vec4{0.0f, 0.0f, zNear, 1.0f}};

    EXPECT_LT(behindClip.w(), 0.0f);
}

// 选择近裁剪面两个对角点和远裁剪面角点，按已知视场计算其视空间坐标。
// 透视除法后角点应落到 NDC 立方体对应的正负 1 边界，验证视锥缩放关系。
TEST_F(NamicaRuntimeTest, PerspectiveMapsFrustumBoundariesToNdcCube)
{
    constexpr Float fov{std::numbers::pi_v<Float> / 2.0f};
    constexpr Float aspect{2.0f};
    constexpr Float zNear{1.0f};
    constexpr Float zFar{9.0f};

    Mat4 const projection{Mat4::perspective(fov, aspect, zNear, zFar)};

    auto const toNdc = [](Vec4 const& _clip) noexcept -> Vec3 {
        return Vec3{_clip.x() / _clip.w(), _clip.y() / _clip.w(), _clip.z() / _clip.w()};
    };

    // vertical FOV = 90°：
    //
    // near half-height = 1
    // near half-width  = 2
    Vec4 const nearUpperRight{projection * Vec4{2.0f, 1.0f, -zNear, 1.0f}};

    Vec4 const nearLowerLeft{projection * Vec4{-2.0f, -1.0f, -zNear, 1.0f}};

    // far half-height = 9
    // far half-width  = 18
    Vec4 const farUpperRight{projection * Vec4{18.0f, 9.0f, -zFar, 1.0f}};

    ExpectVectorNear(toNdc(nearUpperRight), Vec3{1.0f, 1.0f, -1.0f}, 1e-5f);

    ExpectVectorNear(toNdc(nearLowerLeft), Vec3{-1.0f, -1.0f, -1.0f}, 1e-5f);

    ExpectVectorNear(toNdc(farUpperRight), Vec3{1.0f, 1.0f, 1.0f}, 1e-5f);
}

// 比较宽高比变化及垂直视场从 90° 缩小为 60° 的投影矩阵。
// 宽高比只改变水平方向缩放；视场缩小增大两方向缩放，深度项应保持不变。
TEST_F(NamicaRuntimeTest, PerspectiveUsesVerticalFovAndWidthToHeightAspect)
{
    constexpr Float fov90{std::numbers::pi_v<Float> / 2.0f};

    constexpr Float fov60{std::numbers::pi_v<Float> / 3.0f};

    Mat4 const square{Mat4::perspective(fov90, 1.0f, 1.0f, 9.0f)};

    Mat4 const wide{Mat4::perspective(fov90, 2.0f, 1.0f, 9.0f)};

    Mat4 const narrowFov{Mat4::perspective(fov60, 2.0f, 1.0f, 9.0f)};

    constexpr Float sqrt3{std::numbers::sqrt3_v<Float>};

    // aspect 变大只减小水平方向缩放。
    EXPECT_NEAR(square(0, 0), 1.0f, 1e-5f);
    EXPECT_NEAR(square(1, 1), 1.0f, 1e-5f);

    EXPECT_NEAR(wide(0, 0), 0.5f, 1e-5f);
    EXPECT_NEAR(wide(1, 1), 1.0f, 1e-5f);

    // vertical FOV = 60°，1 / tan(30°) = sqrt(3)。
    EXPECT_NEAR(narrowFov(0, 0), sqrt3 / 2.0f, 1e-5f);

    EXPECT_NEAR(narrowFov(1, 1), sqrt3, 1e-5f);

    // aspect 和 FOV 不应改变深度映射。
    EXPECT_NEAR(square(2, 2), wide(2, 2), kEpsilon);
    EXPECT_NEAR(square(2, 3), wide(2, 3), kEpsilon);

    EXPECT_NEAR(wide(2, 2), narrowFov(2, 2), kEpsilon);
    EXPECT_NEAR(wide(2, 3), narrowFov(2, 3), kEpsilon);
}

// 投影不在坐标轴上的已知点 (0.75, -0.5, -4, 1)。
// 分别核对裁剪坐标与除以 w 后的 NDC，避免只测试平面中心而漏掉 x/y 项。
TEST_F(NamicaRuntimeTest, PerspectiveProducesExpectedClipAndNdcCoordinates)
{
    constexpr Float fov{std::numbers::pi_v<Float> / 2.0f};

    Mat4 const projection{Mat4::perspective(fov, 2.0f, 1.0f, 9.0f)};
    Vec4 const viewPoint{0.75f, -0.5f, -4.0f, 1.0f};
    Vec4 const clipPoint{projection * viewPoint};

    ExpectVectorNear(clipPoint, Vec4{0.375f, -0.5f, 2.75f, 4.0f}, 1e-5f);

    EXPECT_NEAR(clipPoint.w(), -viewPoint.z(), kEpsilon);

    Vec3 const ndcPoint{clipPoint.x() / clipPoint.w(),
                        clipPoint.y() / clipPoint.w(),
                        clipPoint.z() / clipPoint.w()};

    ExpectVectorNear(ndcPoint, Vec3{0.09375f, -0.125f, 0.6875f}, 1e-5f);
}
