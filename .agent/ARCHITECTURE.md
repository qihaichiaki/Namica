# 当前架构思路

整理：2026-11-03。以下保留仍有用的设计讨论，**建议不等于已批准接口或实施任务**。项目选择见 [DECISIONS.md](DECISIONS.md)，当前实现见 [PROJECT.md](PROJECT.md)。

参考来源保留原核对范围；再次据此提出新建议前，按 [协作规则](AGENTS.md#建议必须有成熟引擎依据)复核适用性。

## 整体流程与职责

```text
源素材与导入设置 → 内容处理 → 编辑场景与资源引用 → 准备发布内容
游戏代码与运行库 → 编译准备 ───────────────────→ 发布目录 → 独立运行
```

保存场景是其中一步，游戏行为还需要运行逻辑。第一版可简化为带版本的场景数据、松散资源和必要程序，复制交付目录到源码目录之外验证运行。

runtime 提供通用运行能力，tooling 处理制作和发布，editor 提供应用交互。独立构建目标不等于必须独立 DLL；tooling 的库形式尚未确定。编辑器试玩与独立游戏应复用游戏逻辑，静态接入可作为第一版候选。

按稳定子系统及复用、生命周期和依赖决定粒度：已有 core/app/math/io 可继续演进，窗口、输入、资源、渲染和场景按实际需求补齐。保留公开 include / 私有 src 边界；通用模块避免依赖编辑器 UI 或具体游戏对象，不预先拆出大量空类。

文件路径查询可集中保留，图片数据/解码、GPU 资源创建和场景状态则有不同职责。glTF 导入与 GPU 网格创建也应分别理解。场景读写先归相应模块，有实际复用再提取通用序列化设施。

制作数据应区分资源 ID、持久对象 ID 与临时运行句柄；磁盘引用不依赖地址或 GL 编号。保留数据版本，编辑内容与试玩可变状态隔离。资源格式、具体类名、反射或 ECS 尚未选定。

依据：2026-10-31 核对的 [Godot 架构](https://docs.godotengine.org/en/stable/engine_details/architecture/godot_architecture_diagram.html)、[编辑器依赖方向](https://docs.godotengine.org/en/stable/engine_details/editor/introduction_to_editor_development.html)（stable 浮动文档），以及 [Unreal 构建流程](https://dev.epicgames.com/documentation/en-us/unreal-engine/build-operations-cooking-packaging-deploying-and-running-projects-in-unreal-engine)（核对时页面标注 5.8）。它们支持职责与流程比较；Namica 的目录和阶段安排是项目适配。

## 构造约束与所有权

- private 构造限制外部创建，`= delete` 禁止对应操作，两者含义不同。基础声明宏可留在 Base.h，宏不隐藏访问级别切换；不因某个类使用约束宏就推广到全部类。
- 类型别名统一名称，不改变所有权或分配方式。对象创建、析构、存储回收和引用有效期需要分别设计，不能承诺更换别名就能无缝接入内存池。
- 所属系统可以持有对象并对外提供借用 T* / T&；这是所有权契约，不能阻止调用方显式接管裸地址或对引用取地址后接管。SharedPtr 从裸指针显式构造会建立独立控制块，不会加入原持有者的共享所有权；对受管对象这样操作会造成提前销毁、悬空访问或重复释放。
- SharedPtr& 直接暴露持有槽，调用方可 reset/替换，复制它则合法加入原控制块并延长对象寿命；const SharedPtr& 仅限制修改持有槽，仍可复制。借用接口不自动交出这些权力，但也不提供语言层面的完整防误用保障。
- 若需阻止外部直接 delete 或默认 SharedPtr 接管，可考虑 private 析构和内部删除器；现有 Logger::create 也需适配，工厂成员的访问权限不会自动传给标准库删除逻辑。该约束不意味着绝对禁止任何 shared_ptr 包装。对普通日志调用，更直接的候选是仅公开 Log 提交入口，使受管 Logger 不对外暴露；不为此默认增加句柄体系。
- 内存池管理存储，不提供自动可达性回收；批量回收前仍须处理非平凡析构和外部资源。池、删除器及 shared_ptr 控制块的有效期需匹配。
- 长期借用对象需要明确销毁时机与地址稳定性；对象可独立销毁时再考虑弱引用或句柄。辅助类若在析构中回收资源就是 RAII，不必重写等价的通用所有权工具。

当前先解决日志对象的持有与关闭，不以开发通用对象池或 GC 为前提。FileSystem 的定制删除器是已有的资源管理例子。

依据：2026-10-31 核对的 [O3DE 基础宏](https://github.com/o3de/o3de/blob/development/Code/Framework/AzCore/AzCore/base.h)（development，未固定提交）、Godot [Node 回收](https://docs.godotengine.org/en/stable/classes/class_node.html#class-node-method-queue-free)与 [RefCounted](https://docs.godotengine.org/en/stable/classes/class_refcounted.html)（stable），以及 [C++ RAII 与借用指针约定](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rr-raii)。这些是不同生命周期策略的参考，尚未选定 Namica 的全局方案。

借用边界补充依据：2026-11-03 核对 [shared_ptr 构造规则](https://eel.is/c++draft/util.smartptr.shared.const)，裸指针构造与复制 shared_ptr 具有不同的所有权效果；同日复核 [Ogre Singleton](https://github.com/OGRECave/ogre/blob/master/OgreMain/include/OgreSingleton.h) 的指针/引用接口（master，未固定提交），仅作为借用访问实例，不据此声称能禁止调用方销毁对象。上述封装加强方案仍待讨论。

## Log 的近期扩展

已有过滤、同步消息格式化和布局失败契约作为基础。建议按以下职责演进，不要求每项独立成类：

```text
Log 访问与生命周期 → Logger 过滤和消息格式化 → 记录与分发
                                               → 控制台 / 文件 / 编辑器接收端
```

1. **记录与输出边界**：保留时间、等级、来源和正文；颜色与布局归各输出端，编辑器直接使用字段。保存或延迟消费记录时需拥有正文，不能延长借用格式化参数的寿命。
2. **入口与生命周期**：Core/App Log 由 Application 初始化，已确认使用前提见 DECISIONS；构造失败回滚和关闭边界仍需收敛，见下方[生命周期建议](#日志生命周期待讨论)。以后是否增加其他来源按实际诊断需求决定。
3. **同步文件输出**：UTF-8 文本不含控制台颜色码，目录可写且不依赖源码路径，定义刷新与创建/写入失败行为。优先评估成熟输出设施；spdlog 仍是候选，现有 std::format 布局需适配，不能直接当作 spdlog pattern。
4. **失败处理**：布局配置失败与消息提交失败分别约定。建议后备诊断独立且不递归，失败后正常记录仍能继续；不能仅添加 noexcept 宣称可靠。外部错误文本通过固定 `"{}"` 参数或纯文本入口提交。
5. **业务接入**：从窗口、Shader 和资源读取错误开始，记录操作、来源、相关路径及底层原因，保留原业务返回和恢复流程。GLFW 回调文字如需长期保存必须复制。

DLL 中的共享状态应有明确归属，候选方式是在 runtime cpp 中定义并经导出入口访问；不能仅凭公共头中的 static/inline 或 header-only registry 假定 EXE 与 DLL 共享状态。多个 Logger 若写同一文件，可共享输出设施。

建议的完成场景是：资源失败可在控制台和文件定位，文件不可写或消息格式错误有反馈，正常退出完成刷新，现有行为回归保持。具体失败契约应先讨论再测试。编辑器缓存、筛选、订阅解除及 UI 线程投递随编辑器阶段补齐，异步、远程输出和崩溃采集按需求加入。

依据：2026-11-02 核对的 Godot [Logging](https://docs.godotengine.org/en/stable/tutorials/scripting/logging.html)、[Logger 回调](https://docs.godotengine.org/en/stable/classes/class_logger.html)（stable 浮动文档）、[输出类型](https://raw.githubusercontent.com/godotengine/godot/master/core/io/logger.h)与 [OS 生命周期](https://raw.githubusercontent.com/godotengine/godot/master/core/os/os.cpp)（master，未固定提交）。
补充参考：[spdlog sinks](https://github.com/gabime/spdlog/wiki/Sinks)、[DLL registry 说明](https://github.com/gabime/spdlog/wiki/How-to-use-spdlog-in-DLLs)（后者页面最后编辑于 2022，当日复核）、[GLFW 错误回调](https://www.glfw.org/docs/latest/intro_guide.html#error_handling)。这些来源不表示已决定引入相应库或照搬其结构。

### Application 与运行库（待讨论）

Application 作为应用框架协调 WindowSystem、LayerStack 等应用子系统，具体交互程序由应用基类扩展；Log 强依赖 Application 的初始化。当前应用骨架见 PROJECT，其他职责与接口仍在讨论，未授权 agent 修改源码。

- 建议 runtime 保留为运行库/模块名称，Application 表达库中的应用生命周期与主循环框架。EditorApplication、GameApplication、ViewerApplication 可以采用薄继承，通过配置、少量钩子和 Layer 组合业务；不要求所有工具或嵌入式调用都继承它。
- Application 负责启动、退出与帧阶段；WindowSystem 管理窗口和原生事件；LayerStack 管理层顺序、事件传播与更新/绘制回调，GPU 资源和渲染后端仍由渲染模块承担。ImGui 等 UI 应可选，不作为应用基类固定依赖。
- Layer 的持有、移除与 onDetach 契约需明确。倾向由应用的层栈统一拥有应用级 Layer；借用式设计则必须保证被引用对象活得足够久，不能只存裸指针便假定会自动清理。
- Layer/UI 的脱离与资源释放发生在其依赖的渲染和窗口关闭前，初始化失败也要清理已建立资源。派生钩子在完整对象期间调度，不从基类构造/析构中期待派生覆盖。成员按声明顺序初始化、逆序销毁；不能仅凭构造/析构函数体中的调用顺序保证资源覆盖整个对象生命周期。
- 以 Application 承担应用生命周期的方向继续收敛，避免再叠加职责重复的 Runtime/Engine 入口；完整的子系统初始化和清理尚未实现。

依据：2026-11-03 核对 O3DE [ToolsApplication](https://github.com/o3de/o3de/blob/development/Code/Framework/AzToolsFramework/AzToolsFramework/Application/ToolsApplication.h)继承 AzFramework::Application 并扩展 Start/Stop，[Application 实现](https://github.com/o3de/o3de/blob/development/Code/Framework/AzFramework/AzFramework/Application/Application.cpp)集中系统启动、停止和主循环（development，未固定提交）。这支持应用框架分层的比较，不代表 O3DE 使用 Namica 所议的 LayerStack 结构。

### Application 入口与唯一实例（待讨论）

用户提出由 Namica 固定启动流程、用户创建派生 Application；该方向可采用，但入口组织、对象所有权和实例数量约束需分别实现。

- 建议初版同一进程内最多存在一个 Application 作用域，涵盖构建、运行和清理；不同派生类型共享约束，多个窗口、场景或编辑器试玩不另建 Application。完整关闭后能否再次启动须单独验证，不把唯一实例理解为进程历史上只能构造一次。
- 默认启动流程调用用户提供的创建函数，接收 UniquePtr<Application> 后负责运行和销毁；用户选择派生类型与配置，Application 负责子系统生命周期。UniquePtr 表达单一对象的独占所有权，不保证全进程只有一个 Application；禁止复制/移动、protected 构造也不能限制多次独立构造。
- 唯一性检查放在 Application 内部，先于共享子系统初始化，覆盖绕过默认入口的构造路径；重复创建应明确失败，不能覆盖旧实例或只靠可能禁用的 assert。初版生命周期操作限定主线程，检查状态统一保存在 runtime .cpp；仅为数量检查不必公开 getInstance。
- 用 RAII 管理实例占用，构造失败能释放占用，清理完成后再解除；不可只在构造体设置静态指针、依赖析构体复位。公开可用状态若存在，应与正在构造/清理的占用状态区分；run 的重复进入也需单独约束。
- 默认入口建议作为编入可执行目标一次的 Main.cpp/启动模板，runtime DLL 不定义用户程序的 main。EntryPoint.h 定义 main 可作为便捷接入，但只能包含在一个翻译单元，pragma once 不跨翻译单元去重。保留自定义 main 接入能力，便于测试、工具和嵌入；两条路径复用相同生命周期约束，不重复实现初始化。
- 固定入口可负责创建与销毁 Application，但日志初始化职责仍属于 Application；使用自定义 main 也必须先建立应用再调用 Log。输出接入与退出约束见下方日志生命周期建议。

依据：2026-11-03 核对 Ogre [应用示例](https://github.com/OGRECave/ogre/blob/master/Samples/Tutorials/BasicTutorial2.cpp)，用户 main 在栈上创建派生 ApplicationContext 并调用初始化、渲染循环和关闭；Godot [平台入口](https://github.com/godotengine/godot/blob/master/platform/windows/godot_windows.cpp)由引擎组织 Main::setup/start、运行与 cleanup（均为 master 浮动源码，未固定提交）。补充比较 Qt [QApplication](https://doc.qt.io/qt-6.8/qapplication.html)（6.8 文档，核对时页面标注 6.8.9），公开构造并要求一个应用实例，示例由用户 main 持有；Qt 是应用框架而非游戏引擎。[构造失败清理规则](https://eel.is/c++draft/except.ctor)同日核对。Namica 的默认入口仍为建议，唯一实例当前实现与剩余边界见 PROJECT。

### 异常类型与处理边界（待讨论）

异常使用方向见 DECISIONS；以下为适配建议，尚未形成完整公开错误契约。

- 先使用标准异常：重复创建 Application 可用 std::logic_error；违反参数约定可用 std::invalid_argument；不能建立必要运行环境可用 std::runtime_error，有底层错误码时考虑 std::system_error。常规可处理的失败仍可返回结果，现有 setLogFmt 的 Bool 契约不因采用异常而改变。
- 自定义异常仍使用 C++ throw/catch。需要只捕获 Namica 某类失败，或携带资源路径、后端错误码等可程序化读取的字段时再扩展合适的标准异常基类；不按模块预建大量空异常类，不靠解析 what() 文本判断错误。异常类型属于公开契约，变化需考虑调用方已有的 catch。
- 构造失败时保持原有 Application 可用、撤销新对象已取得的资源和实例占用；RAII 承担清理。异常按值抛出、通常按 const 引用捕获，只在能够恢复或结束当前操作的边界处理，不逐层重复记录后再抛出，也不统一改写为丢失原类型的 runtime_error。
- 默认入口捕获未处理的启动/运行异常，使用可靠后备诊断并返回失败退出码；不捕获后无条件继续运行可能失效的应用。工作线程与外部回调需各自明确异常边界，不能假定主线程 main 的 catch 能接住它们。
- 析构和回滚不得让异常逃逸；可能失败的构造/初始化不随意加 noexcept。noexcept 不消除异常，异常逃逸会触发 terminate。异常对象的构造和 what() 不负责调用 Log，防止日志自身失败或尚未启动时产生递归依赖。

依据：2026-11-03 核对 Ogre [Exception](https://ogrecave.github.io/ogre/api/latest/class_ogre_1_1_exception.html)与[类型层级](https://ogrecave.github.io/ogre/api/latest/hierarchy.html)（latest 浮动文档，Exception 页面标注 14.5），在 std::exception 之上提供领域类型与描述、来源、文件和行号；spdlog [spdlog_ex](https://github.com/gabime/spdlog/blob/v1.x/include/spdlog/common.h)直接派生 std::exception 并提供 what()（v1.x，未固定提交）。这说明定制深度随需求变化，不意味着 Namica 需复制其体系。[C++ Core Guidelines E.14–E.18](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Re-exception-types)与 [MSVC noexcept 规则](https://learn.microsoft.com/en-us/cpp/cpp/exception-specifications-throw-cpp?view=msvc-170)同日核对。

### Application 配置（待讨论）

用户提出将子系统初始化选项统一到 Application；建议采用 ApplicationConfig 组合各模块的强类型 Config，尚未确认具体字段或实施接口。

- 各模块定义配置字段、默认值和模块内校验，只接收自己的 Config；Application 负责组合、跨模块约束、依赖顺序和失败清理。同一选择只保留一个权威字段，例如渲染后端由渲染配置定义，窗口侧需求由应用协调。
- 汇总应用启动所需的配置；资源、场景及运行中新建对象的参数仍由相应创建接口接收。EditorConfig/GameConfig 可组合 ApplicationConfig，编辑器布局和游戏规则不进入 runtime 的公共配置。LayerStack 没有实际选项时不预建空 Config。
- 初始化输入作为有明确所有权的配置快照；调用方修改原配置不会自动影响运行对象。区分请求值与实际生效状态，运行期修改通过模块接口处理；日志等级可动态调整，渲染后端等选项可能需要重建。
- Application 初始化本次启用的模块，未来无窗口工具等场景再明确可选表达；不要让所有程序必须开启全部子系统。文件可保存的数据与回调、外部对象等代码注入参数分清，普通配置结构体不自动获得 DLL 二进制兼容性。
- 当前先用代码组装和默认值。后续由上层合并项目文件、用户覆盖和启动参数，明确优先级后形成同一份配置，再校验并启动系统；子系统不各自读取全局配置。Log 初始化前的配置加载错误需由入口返回、异常或独立后备诊断处理，不能调用尚未有效的 Log。动态注册、反射与配置中心等到实际扩展需求出现再引入。
- 日志配置可作为 ApplicationConfig 的子配置，由 Application 传入 Log 初始化；具体字段尚未确定。运行期等级调整与启动配置分开，不因参数集中到应用配置就向其他子系统暴露全部字段。

依据：2026-11-03 核对 Bevy [窗口配置](https://github.com/bevyengine/bevy/blob/main/examples/window/window_settings.rs)和[日志配置](https://github.com/bevyengine/bevy/blob/main/examples/app/logs.rs)，由应用组装各插件的独立配置（main，未固定提交）；O3DE [ComponentApplication](https://github.com/o3de/o3de/blob/development/Code/Framework/AzCore/AzCore/Component/ComponentApplication.h)区分可加载的 Descriptor 与代码提供的 StartupParameters（development，未固定提交），[Settings Registry](https://docs.o3de.org/docs/user-guide/settings/)支持配置文件合并与命令行等入口（浮动文档）。Namica 的聚合结构和实现阶段是适配建议，不照搬插件系统或注册中心。

### 日志生命周期（待讨论）

Log 依赖 Application 初始化是已确认契约，见 DECISIONS；下列为在此契约内仍需细化的边界，不要求 Log 在应用创建前自动可用。

- **启动与失败**：Application 先建立 Log，再启动需要日志的子系统。初始化失败时撤销应用占用并清理日志半初始化状态；Log 尚未有效时不能递归调用自身记录失败。
- **配置与所有权**：Application 协调日志配置与使用期，Log 内部持有 Core/App Logger，通过引用提供借用。独立 Logger::create 的所有权与 Log 使用前提分开；创建独立 Logger 不初始化公共 Log。
- **关闭顺序**：建议 Log 覆盖其他应用子系统的清理过程，停止日志生产者后再刷新和关闭输出；编辑器接收端先于 UI 销毁解除订阅。当前静态对象的留存不等于已实现应用关闭协议，重建应用的语义仍需定义。
- **扩展边界**：先保持同步输出，未来文件或异步线程也需纳入明确的关闭顺序。借用引用不跨对象销毁缓存；runtime DLL 卸载前结束调用，不承诺卸载或应用关闭后仍可用。

应用协调与清理顺序参考上文 O3DE Application 和 Godot Main 的生命周期资料；具体 Log 依赖与关闭契约是 Namica 的项目选择，不声称参考引擎具有相同接口。

### 日志访问与后备诊断（待讨论）

未满足 Application 初始化前提的 Log 调用不受支持。若需要诊断初始化失败或退出故障，可在入口提供不依赖 Log 的 stderr 后备路径，不能用 Log 自己报告未就绪；是否提供统一失败接口尚未确定。格式错误、输出失败和并发配置仍需各自约定，不能仅加 noexcept 宣称不会失败。

依据：2026-11-03 核对 Godot [错误输出](https://github.com/godotengine/godot/blob/master/core/error/error_macros.cpp)，打印设施未就绪、OS 不存在或错误输出重入时通过 fprintf(stderr, ...) 后备输出（master，未固定提交）。这里只参考诊断失败的独立处理，不改变 Namica 的 Log 初始化前提。

### 过滤与发行策略

长期应区分编译期裁剪、Logger/来源的运行期阈值、输出目标阈值和编辑器视图筛选。入口已丢弃的记录不能由文件输出或编辑器恢复；视图隐藏则可保留缓存中的记录。

普通函数内部过滤省去格式化，但无法阻止调用前的实参求值。昂贵参数需要调用处判断或惰性接口，日志参数不应承担必须执行的业务副作用。调用点宏可用于编译期裁剪，资源管理和输出仍留在普通 C++ 中。

Release 优化配置与发行日志策略分开；具体默认阈值与是否提供 Off 尚未确定。已确认 DLL 与各调用方独立设置宏，PRIVATE 不传播符合该策略，无需强制 PUBLIC；调用方只控制自身编译出的宏调用，无法改变 DLL 已保留或裁掉的调用。宏开关不改变当前 Logger 类定义或 DLL ABI。

当前关闭的 Trace/Debug 与 Assert 采用 (void)0，不执行参数或条件。建议保留启用这些宏的构建，检查被裁掉调用的合法性；业务所需副作用不放入日志或断言参数。多语句宏应整体作为一条语句，结尾分号由调用处提供。

依据：2026-11-01 核对的 Unreal [FLogCategoryBase](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Core/FLogCategoryBase/__ctor) 与[构建配置](https://dev.epicgames.com/documentation/en-us/unreal-engine/build-configurations-reference-for-unreal-engine)（页面标注 5.8），以及固定版本 [spdlog v1.15.3 调用点宏](https://github.com/gabime/spdlog/blob/v1.15.3/include/spdlog/spdlog.h)。不照搬 Unreal 等级排序与 Fatal 行为。

2026-11-03 复核上述 spdlog 固定版本：关闭的 Trace/Debug 宏采用 (void)0；同日核对 [CMake 定义传播](https://cmake.org/cmake/help/latest/command/target_compile_definitions.html)。Godot [DEV_ASSERT](https://github.com/godotengine/godot/blob/master/core/error/error_macros.h)（master，未固定提交）在关闭时移除条件，启用时通过完整 if/else 保持语句边界且不自带末尾分号；Namica 采用 do/while(0) 时也应保留调用方加分号的习惯，不据此照搬 Godot 的所有错误处理策略。

### 接口访问与 DLL 导出（待讨论）

- C++ 的 public/protected/private 与 friend 决定源码访问权限；NAMICA_API 决定 DLL 的符号导入导出；公开头文件决定正式提供哪些声明。非导出不能替代访问控制，公开模板、inline 或函数指针等调用路径也不能靠省略导出宏一概阻止。
- 仅限内部调用的操作可用 private 与必要友元限制；若调用与定义均在 runtime 的 .cpp 内，可同时不导出。友元权限不由派生类自动继承。protected 只用于有意允许派生类访问的扩展接口；当前 Log 私有 init 由 Application 友元调用。
- private 成员也可能需要导出：当前 Logger::log 是公开模板，会在客户端生成对 private println 的调用；println 不能因此去掉导出。exported class 的私有成员同样不会变成 public。
- 完全内部的函数和类型放在 .cpp 或 runtime/src 私有头中，公开头只保留必要接口；需要隐藏公开类的实现结构时再考虑 PImpl，不默认给所有类增加间接层。不要用构建宏裁出两份不同的公开类定义。
- 权限需覆盖间接修改：返回可修改的 SharedPtr& 会允许调用方替换/清空公共实例。当前 Log 已采用 Logger& 借用接口，仍需明确有效期，引用不转移所有权。

依据：2026-11-03 核对 [MSVC 导出规则](https://learn.microsoft.com/en-us/cpp/cpp/using-dllimport-and-dllexport-in-cpp-classes?view=msvc-170)、[C++ 访问与友元规则](https://eel.is/c++draft/class.access)；Ogre [Root](https://github.com/OGRECave/ogre/blob/master/OgreMain/include/OgreRoot.h)整体标记导出，同时保留 private 的插件生命周期方法和 RenderSystem 友元（master，未固定提交）。具体 Log 权限方案是 Namica 的适配建议。

### 接口演进与回归

现有 create、log 和 setLogMinLevel 的职责可以延续；setLogFmt 是默认输出的便利配置，不意味着未来所有输出端共用布局。测试绑定公开行为，输出采集可随后端演进调整。

非公开实现的测试建议：

- 默认经正式入口验证可观察行为。私有构造经工厂或所属系统创建；Logger 的校验和 println 经 setLogFmt/log 验证；Application 的私有实例守卫经应用创建、失败和清理行为验证。现有 Logger 测试已采用此方式，不要求每个私有函数有同名测试，也不以读取内部标记替代行为断言。
- 若复杂内部算法有独立、稳定的职责且端到端触发成本高，可在私有实现模块中单独测试；不为几个简单辅助函数强行拆类。难触发的资源失败可通过实际依赖边界替换或注入失败来覆盖，不给公开配置增加纯测试开关。
- 少数确需直接访问的内部行为可使用范围有限的 friend 测试访问器或 GoogleTest FRIEND_TEST；测试夹具的友元权限不自动传给 TEST_F 生成的派生类，需要夹具访问方法或明确友元。不要用 #define private public、测试宏改类的访问布局或直接包含 .cpp 作为常规方案。
- Namica 的外部测试目前链接 runtime DLL，友元/内部头文件只解决源码可见性，不解决未导出符号的链接。必要的内部测试可用独立目标链接同一份生产实现的对象或静态模块；避免同一测试进程又链接 DLL 又编译同一组全局状态。保留通过正式 DLL 的公开接口回归，不为测试直接扩大正式导出面。
- Application 的异常类型、失败后状态及是否支持关闭后重建需先明确，再写相应回归；当前仅讨论测试方式，未添加或执行测试。覆盖率用于寻找遗漏，不要求为不可观察的实现细节逐一增加测试。

测试依据：2026-11-03 核对 [GoogleTest 私有代码测试](https://google.github.io/googletest/advanced.html#testing-private-code)（浮动官方文档），优先公开接口，并提供内部模块与友元方案；Godot [单元测试](https://docs.godotengine.org/en/stable/engine_details/architecture/unit_testing.html#testing-failure-paths)（stable 浮动文档）通过 Color::html 的无效输入检查失败结果，并提供场景树、渲染和音频等测试环境支持。仅参考行为验证与依赖隔离，Namica 继续使用已有 GoogleTest。

布局校验与实际格式化应保持参数数量、顺序、类型一致，支持标准格式语法；不自写括号计数或正则解析器替代格式化实现。配置成功不保证后续每次输出都不会发生资源故障，Bool 返回值、nodiscard 诊断和 noexcept 是独立选择。

动态格式与字面量检查分开：被运行期过滤的动态消息不应触发格式化；未来对错误字面量进行编译检查不违背这一行为。现有契约及实际覆盖见 PROJECT，避免将未定恢复策略提前固化进测试。

依据：2026-11-01 核对的在线 C++ 草案[格式规则](https://eel.is/c++draft/format.string)与[错误报告](https://eel.is/c++draft/format.err.report)；Namica 只采用当前项目标准已提供的能力，不假定草案新增接口可用。

## 后续阶段建议

日志基础 → 窗口、输入与必要渲染 → 场景及保存加载 → 编辑与试玩 → 简化发布。每段交付可使用的子集，由下游实际使用推动完善；不等 Render/Scene 全部完成才接入简易编辑器。

围绕贪吃蛇收敛最小编辑能力与数据格式，试玩结束不破坏编辑内容，发布时包含必要程序、运行库、配置和资源，并脱离源码目录验证。具体编辑属性、数据格式和实现技术仍待决定。
