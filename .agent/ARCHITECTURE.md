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

### 类构造与拷贝约束宏（2026-10-31 建议，未定案）

用户希望复用禁止实例化、拷贝等约束，询问宏的文件归属。当前建议集中放在已有 `runtime/include/namica/core/Base.h`，与基础编译、声明辅助宏一起管理；Types.h 继续以类型定义为主。这类宏只需头文件，没有对应 cpp。后续基础宏数量与职责确实增加时，再考虑抽出 `core/Defines.h`，不预建更细目录。

直接依据：当日核对 O3DE `development` 分支 [AzCore/base.h](https://github.com/o3de/o3de/blob/development/Code/Framework/AzCore/AzCore/base.h)，其中定义 `AZ_DISABLE_COPY_MOVE` 等特殊成员函数辅助宏。这是开发分支参考，未固定提交，不代表所有成熟引擎都采用相同组织。

语义需要区分：

- 禁止外部直接创建、内部仍可创建：私有构造函数，适用于当前 Engine 这种单例意图；不能将其默认构造函数删除。
- 允许创建，但禁止拷贝与移动：分别删除拷贝构造、拷贝赋值、移动构造、移动赋值，可集中为 `NAMICA_DISABLE_COPY_MOVE(Type)`。
- 没有实例的纯静态工具类：可用 `NAMICA_STATIC_CLASS(Type)` 删除默认构造并复用上述约束，前提是没有另行提供可用构造函数。宏本身不会让其他成员自动成为 static。

Namica 使用 C++20，建议以 `= delete` 明确禁止相应操作；private 仅限制访问，不禁止类成员或友元调用。依据为 [C++ deleted definitions](https://eel.is/c++draft/dcl.fct.def.delete)（在线标准草案）。宏内不要隐藏 `public:` / `private:` 切换，由调用方明确放置，避免影响后续成员的可见性。上述宏名称为建议；用户随后在 Base.h 中自行添加了 `CLASS_DISABLE_COPY_MOVE`，实际状态见 PROJECT。

结合日志设计：若 Log 是纯静态访问入口，可禁止实例化；若 Logger 是要创建 Runtime/App 两个实例的对象，就应保留受控构造能力，按需要限制拷贝/移动。用户随后在 Logger.h 使用了上述宏并声明私有构造函数，创建入口尚未实现，不能将完整日志接口视为已经定案。

### 智能指针别名与内存池的边界（2026-10-31 澄清，方案未定案）

用户回顾了“前期用 using 包装标准智能指针，方便以后替换内存池”的建议。本轮明确：类型别名可以统一名称，但不会产生新的所有权语义、改变内存分配方式，也不保证未来只改一行即可替换类型或接入内存池。应分别理解所有权、对象创建、对象销毁及存储分配/回收。

成熟引擎依据：当日查阅 O3DE `development` 的 [AZStd unique_ptr.h](https://github.com/o3de/o3de/blob/development/Code/Framework/AzCore/AzCore/std/smart_ptr/unique_ptr.h)，它通过 `using std::unique_ptr` 复用标准实现，另以 `make_unique` 根据类型是否具有类分配器选择 aznew/new。参考点是集中创建策略，并不要求重写智能指针；开发分支未固定提交。Unreal 的 [智能指针文档](https://dev.epicgames.com/documentation/en-us/unreal-engine/smart-pointers-in-unreal-engine)（页面标注 UE 5.8）则说明其自有智能指针实现，不能据此推导 Namica 也必须自研。

面向 Namica 的建议：

- 可选用 `UniquePtr<T, Deleter = std::default_delete<T>>`、`SharedPtr<T>`、`WeakPtr<T>` 等明确表达所有权的别名；UniquePtr 保留删除器参数，兼容不同资源的释放要求。直接使用 std 也是有效起点，不能将别名封装列为必要前置工作。
- 如果希望统一扩展入口，可将别名与 `makeUnique` / `makeShared` 等创建函数集中于一个 `core/Memory.h`；当前尚未创建，不预建多层 memory 目录。初期创建函数可调用标准库；未来调整创建/释放策略时仍需检查相关接口，不能承诺零改动迁移。
- 接入对象池时，unique_ptr 常用池创建函数配匹配的删除器，或类型自身配套 operator new/delete；unique_ptr 本身不提供 allocator 模板参数。直接从池获取的存储不能在缺乏配套释放机制时交给普通 delete。删除器类型变化也会改变 unique_ptr 类型。
- shared_ptr 可通过 `std::allocate_shared` 接入满足要求的分配器，对象和引用计数控制块的生命周期都需考虑；池的有效期不能短于相关回收操作。参考 [标准智能指针](https://eel.is/c++draft/unique.ptr)、[allocate_shared](https://eel.is/c++draft/util.smartptr.shared.create)、[别名模板](https://eel.is/c++draft/temp.alias)（在线标准草案）。
- 当前 [FileSystem.cpp](../runtime/src/io/FileSystem.cpp) 已使用 unique_ptr 配 stbi_image_free 管理由 stb 分配的图片内存，是所有权与分配/释放来源分开的实际案例；无需因此重写所有权指针。
- 当前先明确日志对象由谁持有、谁创建/释放，不引入内存池或自研智能指针任务。Logger 的私有构造权限也不会因 using 或普通创建函数包装自动改变。

### 裸指针接口、集中销毁与批量回收（2026-10-31 讨论，未定案）

用户进一步提出：不使用智能指针，对外返回裸指针并设置明确回收机制；或依靠内存池自动回收，必要时写辅助类。当前属于探索，不能记录为已经决定全项目禁用智能指针或实现通用内存池。

当日核对的成熟引擎依据：

- Godot stable 的 [Node::queue_free](https://docs.godotengine.org/en/stable/classes/class_node.html#class-node-method-queue-free) 在帧末删除节点及其子节点；[RefCounted](https://docs.godotengine.org/en/stable/classes/class_refcounted.html) 则按引用计数自动释放。两种生命周期策略共存，Node 的序列化 owner 属性不应混同于内存所有权。
- Unreal 5.8 的 [FMemStackBase](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Core/FMemStackBase) 与 [FMemMark](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Core/FMemMark) 记录分配位置并批量弹出后续分配，是按范围统一回收的实际参考。
- Unreal 5.8 的 [UObject 回收机制](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-object-handling-in-unreal-engine) 使用引用图与根集合判断可回收对象；这类可达性追踪是额外系统，不能从“使用内存池”自动得到。
- Godot [Object 有效性说明](https://docs.godotengine.org/en/stable/classes/class_object.html) 和 Unreal [Object Pointers](https://dev.epicgames.com/documentation/en-us/unreal-engine/object-pointers-in-unreal-engine) 均区分对象存活与持有引用；普通裸指针不会自动跟踪销毁。Godot 的实例有效性检测属于引擎机制，不能直接套用到任意 C++ 裸指针。

面向 Namica 的建议与边界：

- 允许所属系统统一持有对象，对外返回借用的 T* / T&，调用方不承担 delete。内部可按值存储、使用标准智能指针或池；外部裸指针接口不等于内部必须完全禁用智能指针，也不要求每个对象都动态分配。
- 内存池管理存储分配与复用，仍需确定“何时可以回收”。一帧临时数据可在所有消费者完成后统一重置；场景独占对象可在场景卸载时统一清理；需要提前删除的对象仍需销毁请求、安全时机与槽位回收。单纯等程序退出再回收，不足以解决长时间运行中的内存持续占用。
- 批量归还存储不能替代非平凡对象的析构或外部资源释放。可以在对象池/拥有者中额外记录并执行销毁动作；尤其要先处理文件、GPU 资源等，再回收对象存储。参考标准 [monotonic_buffer_resource::release](https://eel.is/c++draft/mem.res.monotonic.buffer)，其职责是释放存储，不包含按对象类型逐个析构。
- 除遗漏回收外，还需避免重复释放、悬空引用和地址复用。已发布裸指针的对象需要稳定地址及明确有效期；场景删除对象后，编辑器保存的选中指针可能仍非空。长时间保存、可能被独立销毁的对象，后续可考虑带代数的句柄或弱引用；这只是未来场景需求的建议，不是现在开发通用对象系统的任务。
- 用户提出的辅助类若在析构中调用销毁/回收函数，就是 RAII 资源管理；可做专用拥有者或作用域守卫，明确禁止复制、按需允许移动，并覆盖初始化失败和提前退出。也可复用标准工具，不必为了避免 std 类型重做等价通用指针。参考 [C++ Core Guidelines R.1 / R.3](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rr-raii)（当日在线版）：自动资源管理与借用指针接口可以并存，这是设计约定而非语言强制。
- 当前 Log 只需确定由日志服务持有两个 Logger、对外提供借用访问、在使用者结束后统一关闭；不需要先开发对象池或 GC。仓库 Engine::getInstance 返回函数局部静态对象的地址，已经是“返回裸指针但不转移所有权”的例子。

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

- 作为 runtime 的基础能力，先用 `runtime/include/namica/core/Log.h`、`runtime/src/core/Log.cpp`，测试放 `tests/runtime/core/TestLog.cpp`；用户已创建空的 Log.h，其余是建议路径，尚未创建。等级枚举和少量配置可以同头文件，不要求独立日志构建目标或大量类。
- 支持 Debug / Info / Warning / Error、简单类别标签、格式化消息、最低输出等级；先同步控制台输出，再补一个可选文件输出。输出实现留在 cpp，未来编辑器日志面板通过输出扩展接入，runtime 不依赖 editor。
- 定清初始化、退出刷新、文件打开失败的行为；无需先实现异步队列、日志轮转、远程输出或复杂类别注册。Log 不替代业务错误返回，也不应在输出失败时递归调用自己报错。
- 测试关注可观察结果：等级过滤、类别和参数保留、UTF-8 文本、文件刷新后内容、输出失败行为。可在无图形窗口的测试程序中验证，再接入现有窗口/Shader 报错点。具体 API、格式化库与线程安全承诺仍未定。

关于等级过滤的长期职责（2026-11-01 建议，尚未定案）：

- 用户强调长期规划先行，当前实现只是功能子集。以下是目标职责，不要求立即建立对应类或文件。
- 当日重新核对 [Unreal 5.8 日志文档](https://dev.epicgames.com/documentation/en-us/unreal-engine/logging-in-unreal-engine) 与 [FLogCategoryBase 构造参数](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Core/FLogCategoryBase/__ctor)：按类别设置 verbosity，区分 DefaultVerbosity 与 CompileTimeVerbosity，支持命令行和运行中控制台调整。UE 的 verbosity 排序及 Fatal 行为不能直接套用到自定义严重程度枚举。
- 补充实现参考为固定版本 spdlog v1.15.3 的 [logger.h](https://github.com/gabime/spdlog/blob/v1.15.3/include/spdlog/logger.h) 和 [logger-inl.h](https://github.com/gabime/spdlog/blob/v1.15.3/include/spdlog/logger-inl.h)：logger 入口检查及各 sink 的独立检查。它是日志库参考，不是成熟引擎案例，也不代表项目已选择此依赖。
- Namica 可采用严重程度递增的 Trace / Debug / Info / Warning / Error / Critical；Off 仅作关闭配置，不作为消息等级。等级、来源类别、输出目标应独立表达；Fatal/崩溃是另行定义的控制流程，不让普通错误日志隐式终止程序，也不让关闭日志取消必要的失败终止行为。
- 目标职责分为：编译期决定调用点是否保留；运行期根据 logger/类别决定是否接收；各输出目标决定是否写入；编辑器视图在已收集记录中筛选显示。上游丢弃的记录不能由下游恢复，视图隐藏不等于禁止文件记录。
- 建议将运行期有效阈值定义为“类别显式覆盖 > logger 默认 > 引擎默认”的一次解析，而不是让默认值和覆盖值重复作硬门槛；实际配置形式待定。消息达到有效最低等级才接收，各输出目标仍可独立提高阈值。例如入口 Debug、文件 Debug、控制台 Warning；若入口已为 Warning，文件不能恢复 Debug。默认值、配置文件、命令行和运行时修改的覆盖顺序需明确，与编译期保留范围分开。
- 日志数据应保留等级和类别等字段，不能让 editor 靠解析格式化文本判断等级。运行期拒绝尽量早于格式化和异步入队；普通函数内部过滤仍不能阻止调用参数求值，昂贵调试数据可由 shouldLog 在调用处保护或使用惰性接口。不要让日志参数承担必须执行的业务副作用。
- 沿现有 Logger.h 承载等级与 logger 过滤接口，Log.h 承载公共访问/初始化入口，输出实现留 runtime，日志面板筛选归 editor。目录是否再细分由实际体量决定，不需要先增加 LogLevel.h / LogFilter.h 等空壳。当前可仅实现运行期阈值，但长期字段和职责不因此消失。
- 建议验证等于阈值也放行、Off、两个 logger 独立、类别覆盖、输出目标组合、被过滤记录不格式化/入队、编辑器隐藏后可重新显示已保存记录；编译期裁剪和参数求值需单独验证。此次只给方案，未改源码或运行测试。

关于编译期裁剪与发行配置（2026-11-01 继续讨论，尚未定案）：

- 用户进一步提出通过宏让正式发行版本忽略 Debug 及更低等级。建议长期同时保留编译期裁剪与运行期过滤：前者确定可用的日志调用范围，后者仅在该范围内调整接收策略。
- 当日重新核对 Unreal 5.8 的 FLogCategoryBase（上文链接），其 CompileTimeVerbosity / DefaultVerbosity 分别控制编译期和运行期；[构建配置文档](https://dev.epicgames.com/documentation/en-us/unreal-engine/build-configurations-reference-for-unreal-engine) 区分启用优化的 Development 与 Shipping。不能把 CMake 的 Release 优化配置天然等同于正式发行策略。
- [spdlog v1.15.3 的 spdlog.h](https://github.com/gabime/spdlog/blob/v1.15.3/include/spdlog/spdlog.h) 展示 SPDLOG_ACTIVE_LEVEL 和逐等级宏：禁用时展开为 (void)0；这会移除该调用及其中的实参表达式，不依赖运行期分支或优化器推测。普通 debug 函数调用不会因为这些调用点宏被关闭而自动消失。
- Namica 可规划独立的 NAMICA_LOG_COMPILED_MIN_LEVEL 数值宏，由构建配置提供，日志门面宏按等级决定转发或空操作；枚举保留给 C++ 接口，预处理条件使用整数宏，不使用 enum class / constexpr 名称。运行期阈值继续由 Logger/类别配置控制。宏仅承担调用点裁剪及必要的上下文捕获，格式化、输出和资源管理留在普通 C++ 实现。
- 默认策略建议：开发构建保留 Trace 及以上；内部优化构建仍可保留 Debug 或 Trace；正式发行默认保留 Info 及以上，也可按实际需求提高到 Warning。此处是建议，不是已选定的阈值，也不要求现在新增 Shipping 构建类型。需详细现场信息时可提供保留详细日志的诊断构建，运行期无法恢复已裁掉的调用。
- 普通函数内部的 if 或 if constexpr 不能阻止调用前的实参求值；宏仍启用但运行期过滤时，昂贵参数也需要调用点判断或惰性求值。日志参数不能承担必须执行的业务副作用。裁剪只消除调用中的表达式，调用前已执行的计算不会因此自动取消。
- 构建配置必须覆盖实际使用日志宏的 runtime、editor 和游戏翻译单元；已编译 DLL 内的调用不会因游戏程序修改宏而改变，需重编对应目标。不要在各源文件随意定义阈值；公共内联/模板定义亦需保持一致。被预处理丢弃的日志代码不接受常规 C++ 类型检查，应保留启用完整日志的构建验证。
- 本轮读取现有 CMakePresets、runtime/CMakeLists 和 Base/Logger 头文件：有 Debug/Release 预设，但未见专用日志裁剪宏；未修改代码、CMake 或执行构建。

关于静态生命周期与两个 logger（2026-10-31 讨论，尚未定案）：

- 用户询问能否让日志随程序存活，用 static 保存，并在 runtime 内保留 editor logger 声明。该问题不是已确认实现方案。
- Agent 建议采用每个进程一套日志服务，覆盖应用的启动、运行和退出；可以用 static 保存状态或持有对象，同时由程序入口控制初始化与关闭。结束各日志使用者的工作、释放它们的资源后，再刷新并关闭日志。第一次只做控制台日志时函数局部 static 自动构造/析构也可行，但要约束静态对象退出阶段的日志调用。
- 成熟引擎依据：当日 Godot `master` 的 [OS::OS / OS::~OS](https://raw.githubusercontent.com/godotengine/godot/master/core/os/os.cpp) 创建并释放 logger，[Main::setup 中的文件日志配置](https://raw.githubusercontent.com/godotengine/godot/master/main/main.cpp) 显式添加 RotatedFileLogger。由此可参考“全局访问入口与受控资源生命周期可以并存”，不要求 Namica 复制 Godot 的 OS 架构。
- C++ 依据：[静态局部变量初始化](https://eel.is/c++draft/stmt.dcl) 与 [正常退出时的销毁](https://eel.is/c++draft/basic.start.term)；静态存储不自动表达所有业务依赖的关闭顺序。初始化前、关闭后的调用要有明确约定，必要时只走最简 stderr 后备路径，不能再次访问已销毁的 logger。此处引用的是在线标准草案。
- 两个 logger 可以区分 Runtime 与应用侧，并共享输出设施；它们不要求成为两套日志系统。Unreal 的 [Category 与 Verbosity](https://dev.epicgames.com/documentation/en-us/unreal-engine/logging-in-unreal-engine) 支持按来源区分日志；如果仅需来源标签，也可以使用一个后端加类别。具体是否两个实例仍由用户选择。
- 在 runtime 内声明一个名字为 Editor 的通用 logger，本身不会导致反向链接 editor。Agent 更推荐长期将第二个入口称为 App，编辑器启动时命名 Editor，独立游戏启动时命名 Game；或把 Editor 专用声明放在 editor，复用 runtime 通用能力。通用实现可以统一在 runtime，避免引入编辑器类型/UI 依赖。此命名调整只是建议。
- Namica 当前为 DLL：共享状态在 runtime 的 cpp 内唯一定义，经 NAMICA_API 导出的非内联入口访问。不要在公共头中定义命名空间级 static 实例，也不要仅凭 header-only/inline 声明假定 EXE 与 DLL 共用同一份状态。参考 [spdlog DLL 说明](https://github.com/gabime/spdlog/wiki/How-to-use-spdlog-in-DLLs)（页面最后编辑于 2022，针对 header-only registry 的情形，当日重新查阅）；这不是将 spdlog 选为项目依赖的决定。
- 若两个 logger 写同一文件，建议共享同一个文件输出对象；[spdlog sinks 文档](https://github.com/gabime/spdlog/wiki/Sinks) 展示 logger 持有可共享的输出对象，作为职责参考，不要求引入同等类层次。

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
