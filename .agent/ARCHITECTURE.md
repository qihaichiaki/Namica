# 架构理解与建议

整理日期：2026-10-31。本文总结当前讨论，**推荐结构不等于已实现或已批准的具体 API**。用户确认项与历史纠正见 [DECISIONS.md](DECISIONS.md)。

依据 D09，每次以本文为基础提出建议前，应按 [协作指引](AGENTS.md#建议必须有成熟引擎依据) 核对相关成熟引擎的官方文档或源码。下面的历史讨论与参考列表不等于所有建议已经逐项获得当前证据支持；再次推荐时需给出直接相关的来源，区分引擎事实与面向 Namica 的简化和推导。

## 整体制作与运行流程

```text
源素材 + 导入设置
  → 导入与处理
  → 编辑场景、组件与资源引用
  → 构建场景和资源数据 ─┐
游戏代码 → 编译/准备 ───┼→ 打包发布 → 独立游戏 → 加载内容并执行逻辑
引擎运行时 ───────────┘
```

编辑器保存场景只是其中一段。游戏行为来自游戏代码或脚本，运行时还会创建和销毁对象。导入、内容构建、程序编译与打包可分阶段理解，第一版不需要每个阶段都是独立类或程序。

## 已确认的顶层边界

- `runtime/`：游戏执行需要的通用能力。
- `tooling/`：独立顶层目录、独立构建目标；负责制作侧的导入、内容处理和发布能力。
- `editor/`：编辑器应用，使用 runtime 和 tooling。
- 正式游戏通常使用 runtime、游戏逻辑和构建后的内容，不依赖 tooling。需要运行时导入外部内容时，按实际功能决定哪些解析能力进入游戏。

其余顶层位置是参考约定：`tools/builder/` 为工具程序入口，`projects/` 放游戏工程，`assets/` 放引擎默认资源，`tests/` 放验证和实验，`build/` 放编译输出，`dist/` 放交付内容。它们并不要求立即创建。

建议的依赖关系（不是全部现有目标）：

```text
NamicaEditor  → NamicaTooling → Namica
NamicaEditor  → Namica
NamicaBuilder → NamicaTooling
FirstGame    → Namica + 游戏逻辑
```

`Namica` 目前是 DLL；tooling 初期可用静态库。独立构建目标并不等于必须独立 DLL。编辑器试玩需要接入同一份游戏逻辑，第一版可静态链接当前游戏逻辑，未来再考虑可加载模块。

## 用户特别强调：控制拆分粒度

用户认为早期建议过细，要求参考成熟引擎的实际源码。当前设计建议改为：**按稳定子系统划分，内部集中提供完整能力，根据复用、生命周期、依赖和复杂度再细分。**

| 早期细分建议 | 当前收敛建议；仍需结合实现确认 |
| --- | --- |
| Base 拆出多个只有少量宏的头文件 | `core/Base.h` 可保留平台判断与导出宏，类型放 `Types.h` |
| `SystemPaths` | 程序目录、临时目录查询可留在 `FileSystem` |
| `AssetPaths` | 资源根目录和搜索路径可由 `AssetManager` 管理；管理器出现前不必急着搬现有方法 |
| 公开 `ImageData` 和 `ImageLoader` 两套入口 | 先以 `Image` 提供数据及加载接口，格式加载器可作为内部实现 |
| 独立公开 AssetCatalog、AssetManager 等多层对象 | 先让 `AssetManager` 提供统一入口，索引作为内部数据 |
| 提前创建通用 serialization 子系统 | 场景读写归 scene，通用文件支持归 io；出现跨领域复用再提取 |
| BuildPipeline、ContentBuilder、GamePackager 多层公开类 | 初期 `GameBuilder` 内部按阶段组织函数 |
| 所有配置、事件、枚举独立文件 | 可与主要关联类同文件，广泛复用后再拆 |
| 强制将数学 Utils 改名 Angle | `math/Utils.h` 可以保留，限定为数学领域 |

收敛不代表把图片解码、GPU 资源、场景对象和游戏规则全部塞进 FileSystem 或 Engine。稳定子系统边界仍然重要。

## Runtime 的参考模块

```text
runtime/include/namica/
├── core/       Base、Types、日志、时间等基础设施
├── app/        Engine、Application 和生命周期组织
├── math/       向量、矩阵、四元数、数学常量与工具
├── io/         文件、目录和常用路径操作
├── platform/   窗口等平台能力的公开接口
├── input/      输入查询与输入类型
├── asset/      资源标识、查找、加载、缓存，Image 等内容数据
├── render/     Renderer、Texture、ShaderProgram、Material、Mesh
└── scene/      Scene、Entity 及组件
```

音频、物理、动画、游戏 UI、脚本等按小游戏需求增加。模块名称本身不构成强制重命名理由；不必为了模仿 Urho3D 把 app/asset/render 改成 Engine/Resource/Graphics。

- 公开 `include` 与私有 `src` 的现有 CMake 边界保留，私有头文件也可以放 src。
- `.inl` 中调用方需要可见的模板/内联实现继续随公开头文件提供。
- GLFW/OpenGL 等实现细节收在 src，避免公共接口不必要地暴露第三方类型。
- 图片第一版若仍发布为 PNG，runtime 需要解码；tooling 的导入策略可复用该能力，不能把所有图片代码一概移出 runtime。
- glTF 源素材导入与 GPU 网格创建在概念上分开；当前实验 `Mesh::load` 混合两者，迁入正式模块时再明确边界。
- 场景中的渲染系统把对象数据提交给 Renderer，物理集成系统同步物理与场景；底层系统尽量不依赖具体游戏对象。

## 工具与编辑器的参考粒度

Tooling 近期可围绕 `Project`、`AssetDatabase`、`AssetImporter`、`GameBuilder` 几个入口组织，具体导入器放私有实现。工具程序负责入口，工具库提供可复用能力。

编辑器可以先使用 `EditorApplication` 和少量面板，管理当前工程、场景与试玩状态。无需立即引入 ProjectSession / SceneDocument / PlaySession 等独立类。

## 值得提前理解的数据和生命周期约定

以下是建议原则，不代表已经选定实现框架：

- 资源、实体、场景配置、运行状态与玩家存档是不同概念。多个实体可以共享资源，但各自持有可变游戏状态。
- AssetId、持久实体 ID 与临时运行句柄区分；不要把内存地址、易变数组下标或 GL 对象编号作为磁盘持久引用。
- 场景和资源数据保留格式版本；源素材与导入设置可重建缓存。
- 编辑内容与试玩时的可变状态隔离；可用独立场景实例，不强制只有一个全局 Scene。
- 游戏类型可手动注册，属性面板可手写；反射、脚本热重载、通用 ECS 并非第一版前提。
- 先明确谁创建、持有、更新和销毁对象。运行时可先采用单线程显式调度，关闭按依赖顺序释放资源。

## 小游戏闭环建议

| 阶段 | 可检查的成果 |
| --- | --- |
| 独立运行 | 一个游戏入口使用 runtime 跑通基础玩法 |
| 场景数据 | 修改场景文件能够改变游戏内容 |
| 编辑保存 | 编辑器修改、保存，关闭重开后保持一致 |
| 试玩 | 使用相同游戏逻辑，结束试玩不破坏编辑内容 |
| 构建发布 | 生成程序、必要运行库、运行配置及资源 |
| 脱离源码运行 | 复制交付目录到其他位置，仍能加载内容并游玩 |

第一版可使用带版本的 JSON 场景、PNG 图片、松散文件和简单资源索引。可以复制清单内的全部资源，后续再做依赖裁剪、压缩、专用格式、增量构建。应检查动态引用的资源是否被包含。

当前 [项目日记](../doc/life/梦醒再梦.md) 已确定第一阶段使用贪吃蛇验证完整流程。2D 收集与避障游戏只是曾提出的案例，不是当前目标。后续建议应围绕贪吃蛇确定最小编辑能力，例如棋盘配置、初始状态和资源选择；这些具体功能仍是建议，不能自动视为已批准需求。

## 成熟引擎参考与证据

### Log 与近期开发路线（2026-10-31 建议，未定案）

用户计划下一步开发 Log，并提出先补齐渲染之外的基础能力、各自配简单测试，再进入 render、scene、editor。Agent 认可先做日志，建议将基础阶段限定为支撑首个可运行场景所需的部分；各阶段交付最小可运行版本，再由下游实际使用推动完善。

当日查证的依据：

- [Unreal 日志文档](https://dev.epicgames.com/documentation/en-us/unreal-engine/logging-in-unreal-engine)（页面标注 UE 5.8）：日志有类别、级别、格式化消息，支持文件及编辑器展示，自定义输出可使用 FOutputDevice。
- [Godot core/io/logger.h](https://raw.githubusercontent.com/godotengine/godot/master/core/io/logger.h)（master 开发分支，未固定提交）：Logger、StdLogger、RotatedFileLogger、CompositeLogger 区分消息输出与具体输出目标。此处参考职责，不要求照搬类层次。
- [Godot stable 编辑器开发说明](https://docs.godotengine.org/en/stable/engine_details/editor/introduction_to_editor_development.html)：编辑器复用引擎渲染/UI，规定 editor → scene → servers → core 的依赖方向。该文档说明依赖关系，不是官方历史开发顺序；下面阶段安排是结合 Namica 目标的推导。
- [Unreal 构建发布流程](https://dev.epicgames.com/documentation/en-us/unreal-engine/build-operations-cooking-packaging-deploying-and-running-projects-in-unreal-engine)（页面标注 UE 5.8）：Build、Cook、Stage、Package、Deploy、Run 区分程序、内容与交付阶段，Stage 会整理到独立目录。Namica 第一版可简化为准备程序和资源、复制到交付目录、独立运行验证。

Log 第一版建议：

- 作为 runtime 的基础能力，先用 `runtime/include/namica/core/Log.h`、`runtime/src/core/Log.cpp`，测试放 `tests/runtime/core/TestLog.cpp`；这些是建议路径，尚未创建。等级枚举和少量配置可以同头文件，不要求独立日志构建目标或大量类。
- 支持 Debug / Info / Warning / Error、简单类别标签、格式化消息、最低输出等级；先同步控制台输出，再补一个可选文件输出。输出实现留在 cpp，未来编辑器日志面板通过输出扩展接入，runtime 不依赖 editor。
- 定清初始化、退出刷新、文件打开失败的行为；无需先实现异步队列、日志轮转、远程输出或复杂类别注册。Log 不替代业务错误返回，也不应在输出失败时递归调用自己报错。
- 测试关注可观察结果：等级过滤、类别和参数保留、UTF-8 文本、文件刷新后内容、输出失败行为。可在无图形窗口的测试程序中验证，再接入现有窗口/Shader 报错点。具体 API、格式化库与线程安全承诺仍未定。

建议阶段与完成线：

| 阶段 | 最小成果 |
| --- | --- |
| 必要基础 | Log、时间/帧间隔、现有文件读写、启动与关闭流程，覆盖关键正常与失败路径 |
| 平台与输入 | 从现有实验提取窗口、事件轮询和必要输入，能打开、响应输入、正常退出 |
| Render 第一版 | 画出贪吃蛇需要的棋盘、蛇与食物；现有 OpenGL 实验作为实现来源 |
| Scene 第一版 | 对象及必要组件、场景保存加载与资源引用，独立程序可用相同数据运行 |
| Editor 与简单 Tooling | 视口、少量属性编辑、保存和试玩；准备独立游戏程序及资源，移到仓库外验证运行 |

基础阶段暂不以覆盖所有非渲染领域为目标；音频、物理、动画、脚本、任务调度、通用反射和复杂资源管理按游戏需求再加入。各阶段除局部测试外，还应用一个小程序串联验证启动、使用和关闭。Scene 能加载和显示后即可让简易 Editor 参与验证，后续继续迭代 Render/Scene；不必等两者功能完善才接 Editor。`NAMICA_TOL` 继续遵守 D10，保持现状。

### 目录规划与其他参考

2026-10-31 针对用户本轮 core/app 迁移重新核对：

- [Godot stable 架构说明](https://docs.godotengine.org/en/stable/engine_details/architecture/godot_architecture_diagram.html) 区分 Core 基础能力与 Main 的启动、关闭、主循环职责。由此支持 Namica 将基础定义与未来生命周期组织分开；`app/Engine` 是 Namica 的命名和适配方案，不代表 Godot 的同名 Engine 类位于 main，也不表示 Namica 已实现生命周期管理。
- 当日查阅的 Godot `master` 源码 [core/typedefs.h](https://raw.githubusercontent.com/godotengine/godot/master/core/typedefs.h) 提供通用基础定义；[core/math/math_defs.h](https://raw.githubusercontent.com/godotengine/godot/master/core/math/math_defs.h) 放置数学常量及比较容差。这里的 `master` 是开发分支参考，未固定提交，不当作特定稳定发行版内容。
- 曾建议将 `NAMICA_TOL` 从 `Types.h` 归入已有 `math/Constants.h`；用户于 2026-10-31 明确延后此事（D10）。当前保持位置、名称与数值不变，不将其列为本轮目录整理的待修问题，也不反复提醒。后续实际涉及容差问题时再核对成熟引擎依据并讨论，未来方案尚未确定。

以下页面在前期讨论中已查阅。以具体接口和目录为参考，不把所有成熟引擎描述成同一种架构；比较当前版本时重新核对。

- [Godot core 源码目录](https://github.com/godotengine/godot/tree/master/core)：将 math、io、input、os 等归在 core 下。
- [Godot editor 源码目录](https://github.com/godotengine/godot/tree/master/editor)：包含 import、export 等制作能力。
- [Godot Image](https://raw.githubusercontent.com/godotengine/godot/master/core/io/image.h) 与 [ImageLoader](https://raw.githubusercontent.com/godotengine/godot/master/core/io/image_loader.h)：Image 提供数据与加载接口，格式扩展可以在内部继续分工。
- [Godot 依赖规则](https://docs.godotengine.org/en/stable/engine_details/editor/introduction_to_editor_development.html)：编辑器向场景和底层依赖，不应反向引入编辑器头文件。
- [Urho3D 子系统目录](https://github.com/urho3d/urho3d/tree/master/Source/Urho3D)：Core、Engine、IO、Resource、Graphics、Scene 等；仓库已归档，此处作为历史结构参考。
- [Urho3D FileSystem](https://raw.githubusercontent.com/urho3d/urho3d/master/Source/Urho3D/IO/FileSystem.h)：文件操作与程序、临时目录查询集中在同一系统。
- [Urho3D ResourceCache](https://github.com/urho3d/Urho3D/blob/master/Source/Urho3D/Resource/ResourceCache.h)：资源目录、包、加载、缓存形成统一入口。
- [Unreal 目录说明](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-directory-structure)：参考 Runtime、Editor、Developer、Programs 的层次，具体目标边界仍按 Namica 需求设计。
- [Unreal 构建流程](https://dev.epicgames.com/documentation/en-us/unreal-engine/build-operations-cooking-packaging-deploying-and-running-projects-in-unreal-engine)：区分程序构建、内容处理、组织发布目录、打包与运行。
- [Unity 资源数据库](https://docs.unity3d.com/ja/current/Manual/asset-database-contents.html)：源素材、导入产物、元数据与依赖的区别。

这些参考支持设计取舍，但不是照抄目录的依据，也不意味着用户已经授权引入对应框架或依赖。
