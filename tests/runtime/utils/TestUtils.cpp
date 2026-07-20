#include "runtime/utils/TestUtils.h"

void NamicaRuntimeTest::SetUp()
{
}

void NamicaRuntimeTest::TearDown()
{
}

void ExpectMat4Near(namica::Mat4 const& _actual,
                    namica::Mat4 const& _expected,
                    namica::Float const _epsilon)
{
    for (namica::Int row{0}; row < 4; ++row)
    {
        for (namica::Int column{0}; column < 4; ++column)
        {
            EXPECT_NEAR(_actual(row, column), _expected(row, column), _epsilon)
                << "row = " << row << ", column = " << column;
        }
    }
}