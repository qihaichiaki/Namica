#pragma once

#include "namica/core/Base.h"

namespace namica
{

struct ApplicationConfig;

class Application
{
public:
    CLASS_DISABLE_COPY_MOVE(Application)

    /**
     * @brief 初始化namica应用程序
     *
     * @param _config 应用程序初始化参数
     *
     * @throw logic_error Application在当前进程只能存在一份
     */
    NAMICA_API Application(ApplicationConfig const& _config);

    NAMICA_API virtual ~Application();

public:
    /**
     * @brief 获取当前进程的唯一实例
     */
    NAMICA_API static Application* get() noexcept;

public:
    NAMICA_API void run();

private:
};

}  // namespace namica