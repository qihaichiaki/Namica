#pragma once

#include <gtest/gtest.h>

class NamicaRuntimeTest : public testing::Test
{
public:
    void SetUp() override;
    void TearDown() override;
};
