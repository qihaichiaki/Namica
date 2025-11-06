#pragma once

// 单一对象类, 禁止拷贝和赋值
#define CLASS_DISABLE_COPY_MOVE(className)           \
    className(const className&) = delete;            \
    className(className&&) = delete;                 \
    className& operator=(const className&) = delete; \
    className& operator=(const className&&) = delete;