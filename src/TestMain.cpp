#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <lua.hpp>
#include <pugixml.hpp>
#include <cJSON.h>

#include <chrono>
#include <iostream>
#include <thread>

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);          // 初始化SDL下全部的子系统
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);  // 初始化SDL图像解码 - jpg和png
    Mix_Init(MIX_INIT_MP3);                 // 初始化SDL音频解码 - mp3
    TTF_Init();                             // 初始化SDL字体解码

    auto logger = spdlog::stdout_color_mt("Namica");
    logger->info("测试");

    lua_State *lua_state = luaL_newstate();
    logger->info("lua加载地址: {}", (void *)(lua_state));

    // xml三方依赖测试
    pugi::xml_document doc;
    // cJson三方依赖测试
    cJSON *json_root = cJSON_Parse("{}");
    if (!json_root) {
        logger->info("json简单解析成功...");
    } else {
        logger->warn("json解析失败...");
    }

    int windowWidth = 900;
    int windowHeight = 720;
    // 创建窗口
    // 窗口高DPI(更加清晰的显示) | 可以调整大小
    SDL_Window *window =
        SDL_CreateWindow("Namica", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth,
                         windowHeight, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);

    // 创建渲染器
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // IMGUI根据SDL2初始化
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    // io配置
    ImGuiIO &io = ImGui::GetIO();
    // 加载字体, 设置大小, 默认文字配置数据, 加载完整的中文字符集
    io.Fonts->AddFontFromFileTTF(EDIT_RES_PATH "fonts/msyh.ttc", 18.0f, nullptr,
                                 io.Fonts->GetGlyphRangesChineseFull());
    // stype配置
    // ......

    // 帧时间设置
    using namespace std::chrono;
    // 帧率默认为144，帧间隔表示了每秒跑多少帧
    const nanoseconds frameDuration{1'000'000'000 / 144};
    steady_clock::time_point lastTick = steady_clock::now();

    bool show_demo_window = true;

    bool isQuit = false;
    while (!isQuit) {
        // 事件处理
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // 交给IMGUI处理事件
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                isQuit = true;
            }
        }

        // 计算当前帧和上一帧之间的帧间隔
        steady_clock::time_point frameStart = steady_clock::now();
        float delta = duration<float>{frameStart - lastTick}.count();
        lastTick = frameStart;

        // IMGUI开启新帧
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // 更新
        if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

        // 渲染
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // 设置绘制颜色
        SDL_RenderClear(renderer);                       // 使用上述颜色清屏

        ImGui::Render();  // 生成绘制数据
        // 调用SDL渲染器绘制
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);  // 应用渲染到窗口上

        // 查看是否存在剩余时间将CPU时间片放出
        nanoseconds sleepDuration = frameDuration - (steady_clock::now() - frameStart);
        if (sleepDuration > nanoseconds{0}) {
            std::this_thread::sleep_for(sleepDuration);
        }
    }

    // 退出释放资源
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();

    logger->info("拜拜啦~可爱的世界创建者(●'◡'●)");
    return 0;
}