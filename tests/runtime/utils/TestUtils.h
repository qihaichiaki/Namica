#pragma once

#include <gtest/gtest.h>

// runtime 各模块共用的 GoogleTest 夹具；模块专用比较与资源工具留在各模块 utils。
class NamicaRuntimeTest : public testing::Test
{
public:
    void SetUp() override;
    void TearDown() override;
};
