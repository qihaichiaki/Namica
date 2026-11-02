# 应用入口与示例

## 当前应用入口

app 模块目前提供 `Engine::getInstance()`，返回已有引擎实例的指针；调用者不负责销毁它。当前尚无公开的初始化、帧循环或关闭接口，取得实例不会自动创建窗口或运行游戏。

`NamicaEditor` 也只是应用入口，目前等待约三秒后退出。体验窗口、渲染和游戏应运行下面的实验程序。

## 构建交互实验

先按[构建与接入](GettingStarted.md)准备依赖和 MSVC x64 环境，然后在仓库根目录执行：

```shell
cmake --preset x64-debug -DNAMICA_BUILD_TESTS=ON -DBUILD_PLAYGROUND=ON
cmake --build build/x64-debug --target test_snake_game test_window_render
```

实验需要能够创建 OpenGL 上下文的图形环境。它们使用 GoogleTest 作为入口，但包含交互窗口和持续运行的循环，需要手动体验与关闭。

## 运行贪吃蛇

```shell
./build/x64-debug/bin/test_snake_game.exe --gtest_filter=TestSnakeGame.SnakeGameMain
```

在开始界面按任意键进入游戏，使用方向键控制移动，游戏结束后按 R 重新开始；关闭窗口退出。

源码：[TestSnakeGame.cpp](../../tests/playground/snake_game/TestSnakeGame.cpp)。这是独立实验，尚不是由编辑器创建和打包的游戏项目。

## 运行渲染示例

先列出可运行的示例，再选择一个启动，避免依次进入全部交互程序：

```shell
./build/x64-debug/bin/test_window_render.exe --gtest_list_tests
./build/x64-debug/bin/test_window_render.exe --gtest_filter=TestWindowRender.cube_render
```

还可以选择 `TestWindowRender.file_render` 查看资源加载相关示例。此类实验使用配置时指定的仓库 `assets/` 目录；直接复制可执行文件到其他位置不等于获得了完整可发布程序。

入口与源码：[Engine.h](../../runtime/include/namica/app/Engine.h) · [编辑器入口](../../editor/src/Main.cpp) · [渲染实验](../../tests/playground/window_render/)。返回[用户指南](README.md)。
