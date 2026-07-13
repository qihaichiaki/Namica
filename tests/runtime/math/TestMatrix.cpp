#include "runtime/utils/TestUtils.h"

#include "namica/math/Matrix.h"

using namespace namica;

// 测试矩阵乘法正确性

constexpr float kEpsilon{1e-6f};

void ExpectVec4Near(Vec4 const& _actual, Vec4 const& _expected, float _epsilon = kEpsilon)
{
    EXPECT_NEAR(_actual[0], _expected[0], _epsilon);
    EXPECT_NEAR(_actual[1], _expected[1], _epsilon);
    EXPECT_NEAR(_actual[2], _expected[2], _epsilon);
    EXPECT_NEAR(_actual[3], _expected[3], _epsilon);
}

TEST_F(NamicaRuntimeTest, IdentityMatrixLeftMultiplyVector)
{
    // I * v = v
    // v视作为 [4, 1]的列向量

    Mat4 const identity{};
    Vec4 const vector{1.0f, 2.0f, 3.0f, 4.0f};
    Vec4 const result{identity * vector};

    ExpectVec4Near(result, vector);
    EXPECT_TRUE(result.isEqual(vector));
}

TEST_F(NamicaRuntimeTest, VectorRightMultiplyIdentityMatrix)
{
    // v * I = v
    // v视作为 [1, 4]的行向量

    Mat4 const identity{};
    Vec4 const vector{1.0f, 2.0f, 3.0f, 4.0f};
    Vec4 const result{vector * identity};

    ExpectVec4Near(result, vector);
    EXPECT_TRUE(result.isEqual(vector));
}

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
    Mat4 matrix{};

    matrix(0, 0) = 2.0f;
    matrix(1, 1) = 3.0f;
    matrix(2, 2) = 4.0f;
    matrix[3][3] = 5.0f;

    Vec4 const vector{1.0f, 2.0f, 3.0f, 4.0f};

    Vec4 const result{matrix * vector};
    Vec4 const expected{2.0f, 6.0f, 12.0f, 20.0f};

    ExpectVec4Near(result, expected);
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
TEST_F(NamicaRuntimeTest, VectorRightMultiplyDiagonalMatrix)
{
    Mat4 matrix{};

    matrix[0][0] = 2.0f;
    matrix[1][1] = 3.0f;
    matrix[2][2] = 4.0f;
    matrix[3][3] = 5.0f;

    Vec4 const vector{1.0f, 2.0f, 3.0f, 4.0f};

    Vec4 const result{vector * matrix};
    Vec4 const expected{2.0f, 6.0f, 12.0f, 20.0f};

    ExpectVec4Near(result, expected);
    EXPECT_TRUE(result.isEqual(expected));
}

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
    Mat4 matrix{};

    matrix[0][0] = 1.0f;
    matrix[0][1] = 2.0f;
    matrix[0][2] = 3.0f;
    matrix[0][3] = 4.0f;

    matrix[1][0] = 5.0f;
    matrix[1][1] = 6.0f;
    matrix[1][2] = 7.0f;
    matrix[1][3] = 8.0f;

    matrix[2][0] = 9.0f;
    matrix[2][1] = 10.0f;
    matrix[2][2] = 11.0f;
    matrix[2][3] = 12.0f;

    matrix[3][0] = 13.0f;
    matrix[3][1] = 14.0f;
    matrix[3][2] = 15.0f;
    matrix[3][3] = 16.0f;

    Vec4 const vector{1.0f, 2.0f, 3.0f, 4.0f};

    Vec4 const result{matrix * vector};
    Vec4 const expected{30.0f, 70.0f, 110.0f, 150.0f};

    ExpectVec4Near(result, expected);
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
TEST_F(NamicaRuntimeTest, VectorRightMultiplyGeneralMatrix)
{
    Mat4 matrix{};

    matrix[0][0] = 1.0f;
    matrix[0][1] = 2.0f;
    matrix[0][2] = 3.0f;
    matrix[0][3] = 4.0f;

    matrix[1][0] = 5.0f;
    matrix[1][1] = 6.0f;
    matrix[1][2] = 7.0f;
    matrix[1][3] = 8.0f;

    matrix[2][0] = 9.0f;
    matrix[2][1] = 10.0f;
    matrix[2][2] = 11.0f;
    matrix[2][3] = 12.0f;

    matrix[3][0] = 13.0f;
    matrix[3][1] = 14.0f;
    matrix[3][2] = 15.0f;
    matrix[3][3] = 16.0f;

    Vec4 const vector{1.0f, 2.0f, 3.0f, 4.0f};

    Vec4 const result = vector * matrix;
    Vec4 const expected{90.0f, 100.0f, 110.0f, 120.0f};

    ExpectVec4Near(result, expected);
    EXPECT_TRUE(result.isEqual(expected));
}

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
TEST_F(NamicaRuntimeTest, LeftAndRightMultiplicationProduceDifferentResults)
{
    Mat4 matrix{};

    matrix[0][0] = 1.0f;
    matrix[0][1] = 2.0f;
    matrix[0][2] = 3.0f;
    matrix[0][3] = 4.0f;

    matrix[1][0] = 5.0f;
    matrix[1][1] = 6.0f;
    matrix[1][2] = 7.0f;
    matrix[1][3] = 8.0f;

    matrix[2][0] = 9.0f;
    matrix[2][1] = 10.0f;
    matrix[2][2] = 11.0f;
    matrix[2][3] = 12.0f;

    matrix[3][0] = 13.0f;
    matrix[3][1] = 14.0f;
    matrix[3][2] = 15.0f;
    matrix[3][3] = 16.0f;

    Vec4 const vector{1.0f, 2.0f, 3.0f, 4.0f};

    Vec4 const leftResult = matrix * vector;
    Vec4 const rightResult = vector * matrix;

    // 两种乘法的结果应分别为：
    // matrix * vector = (30, 70, 110, 150)
    // vector * matrix = (90, 100, 110, 120)
    ExpectVec4Near(leftResult, Vec4{30.0f, 70.0f, 110.0f, 150.0f});
    ExpectVec4Near(rightResult, Vec4{90.0f, 100.0f, 110.0f, 120.0f});

    // 至少部分分量不同，证明左右乘法不是同一种运算。
    EXPECT_NE(leftResult[0], rightResult[0]);
    EXPECT_NE(leftResult[1], rightResult[1]);
    EXPECT_NE(leftResult[3], rightResult[3]);

    EXPECT_FALSE(leftResult.isEqual(rightResult));
}