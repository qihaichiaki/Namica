#pragma once

#include <gtest/gtest.h>

#include <namica/math/Matrix.h>

class NamicaRuntimeTest : public testing::Test
{
public:
    void SetUp() override;
    void TearDown() override;
};

constexpr namica::Float kEpsilon{1e-6f};

template <namica::Int D>
void ExpectVectorNear(namica::Vector<namica::Float, D> const& _actual,
                      namica::Vector<namica::Float, D> const& _expected,
                      namica::Float const _epsilon = kEpsilon)
{
    for (namica::Int index{0}; index < D; ++index)
    {
        EXPECT_NEAR(_actual[index], _expected[index], _epsilon) << "component = " << index;
    }
}

void ExpectMat4Near(namica::Mat4 const& _actual,
                    namica::Mat4 const& _expected,
                    namica::Float const _epsilon = kEpsilon);
