# 当前架构思路

整理：2026-11-02。以下保留仍有用的设计讨论，**建议不等于已批准接口或实施任务**。项目选择见 [DECISIONS.md](DECISIONS.md)，当前实现见 [PROJECT.md](PROJECT.md)。

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
- 所属系统可以持有对象并对外提供借用 T* / T&；内部按值、智能指针或池存储均可。裸指针接口不自动转移销毁责任，也不意味着禁用标准智能指针。
- 内存池管理存储，不提供自动可达性回收；批量回收前仍须处理非平凡析构和外部资源。池、删除器及 shared_ptr 控制块的有效期需匹配。
- 长期借用对象需要明确销毁时机与地址稳定性；对象可独立销毁时再考虑弱引用或句柄。辅助类若在析构中回收资源就是 RAII，不必重写等价的通用所有权工具。

当前先解决日志对象的持有与关闭，不以开发通用对象池或 GC 为前提。Engine 的借用指针与 FileSystem 的定制删除器是已有例子。

依据：2026-10-31 核对的 [O3DE 基础宏](https://github.com/o3de/o3de/blob/development/Code/Framework/AzCore/AzCore/base.h)（development，未固定提交）、Godot [Node 回收](https://docs.godotengine.org/en/stable/classes/class_node.html#class-node-method-queue-free)与 [RefCounted](https://docs.godotengine.org/en/stable/classes/class_refcounted.html)（stable），以及 [C++ RAII 与借用指针约定](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rr-raii)。这些是不同生命周期策略的参考，尚未选定 Namica 的全局方案。

## Log 的近期扩展

已有过滤、同步消息格式化和布局失败契约作为基础。建议按以下职责演进，不要求每项独立成类：

```text
Log 访问与生命周期 → Logger 过滤和消息格式化 → 记录与分发
                                               → 控制台 / 文件 / 编辑器接收端
```

1. **记录与输出边界**：保留时间、等级、来源和正文；颜色与布局归各输出端，编辑器直接使用字段。保存或延迟消费记录时需拥有正文，不能延长借用格式化参数的寿命。
2. **入口与生命周期**：明确创建者和持有者，在窗口/资源初始化前可用，在全部使用者结束后刷新关闭。static 保存与受控生命周期可以并存，但需约定初始化前、关闭后的调用行为。单入口加来源字段或多个 Logger 均未定案。
3. **同步文件输出**：UTF-8 文本不含控制台颜色码，目录可写且不依赖源码路径，定义刷新与创建/写入失败行为。优先评估成熟输出设施；spdlog 仍是候选，现有 std::format 布局需适配，不能直接当作 spdlog pattern。
4. **失败处理**：布局配置失败与消息提交失败分别约定。建议后备诊断独立且不递归，失败后正常记录仍能继续；不能仅添加 noexcept 宣称可靠。外部错误文本通过固定 `"{}"` 参数或纯文本入口提交。
5. **业务接入**：从窗口、Shader 和资源读取错误开始，记录操作、来源、相关路径及底层原因，保留原业务返回和恢复流程。GLFW 回调文字如需长期保存必须复制。

DLL 中的共享状态应有明确归属，候选方式是在 runtime cpp 中定义并经导出入口访问；不能仅凭公共头中的 static/inline 或 header-only registry 假定 EXE 与 DLL 共享状态。多个 Logger 若写同一文件，可共享输出设施。

建议的完成场景是：资源失败可在控制台和文件定位，文件不可写或消息格式错误有反馈，正常退出完成刷新，现有行为回归保持。具体失败契约应先讨论再测试。编辑器缓存、筛选、订阅解除及 UI 线程投递随编辑器阶段补齐，异步、远程输出和崩溃采集按需求加入。

依据：2026-11-02 核对的 Godot [Logging](https://docs.godotengine.org/en/stable/tutorials/scripting/logging.html)、[Logger 回调](https://docs.godotengine.org/en/stable/classes/class_logger.html)（stable 浮动文档）、[输出类型](https://raw.githubusercontent.com/godotengine/godot/master/core/io/logger.h)与 [OS 生命周期](https://raw.githubusercontent.com/godotengine/godot/master/core/os/os.cpp)（master，未固定提交）。
补充参考：[spdlog sinks](https://github.com/gabime/spdlog/wiki/Sinks)、[DLL registry 说明](https://github.com/gabime/spdlog/wiki/How-to-use-spdlog-in-DLLs)（后者页面最后编辑于 2022，当日复核）、[GLFW 错误回调](https://www.glfw.org/docs/latest/intro_guide.html#error_handling)。这些来源不表示已决定引入相应库或照搬其结构。

### 过滤与发行策略

长期应区分编译期裁剪、Logger/来源的运行期阈值、输出目标阈值和编辑器视图筛选。入口已丢弃的记录不能由文件输出或编辑器恢复；视图隐藏则可保留缓存中的记录。

普通函数内部过滤省去格式化，但无法阻止调用前的实参求值。昂贵参数需要调用处判断或惰性接口，日志参数不应承担必须执行的业务副作用。调用点宏可用于编译期裁剪，资源管理和输出仍留在普通 C++ 中。

Release 优化配置与发行日志策略分开；具体等级、默认阈值、覆盖顺序与是否提供 Off 尚未确定。宏配置需覆盖实际调用方及 DLL 的编译单元，运行期无法恢复已裁掉的调用；保留完整日志构建以检查被裁掉代码的合法性。

依据：2026-11-01 核对的 Unreal [FLogCategoryBase](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Core/FLogCategoryBase/__ctor) 与[构建配置](https://dev.epicgames.com/documentation/en-us/unreal-engine/build-configurations-reference-for-unreal-engine)（页面标注 5.8），以及固定版本 [spdlog v1.15.3 调用点宏](https://github.com/gabime/spdlog/blob/v1.15.3/include/spdlog/spdlog.h)。不照搬 Unreal 等级排序与 Fatal 行为。

### 接口演进与回归

现有 create、log 和 setLogMinLevel 的职责可以延续；setLogFmt 是默认输出的便利配置，不意味着未来所有输出端共用布局。测试绑定公开行为，输出采集可随后端演进调整。

布局校验与实际格式化应保持参数数量、顺序、类型一致，支持标准格式语法；不自写括号计数或正则解析器替代格式化实现。配置成功不保证后续每次输出都不会发生资源故障，Bool 返回值、nodiscard 诊断和 noexcept 是独立选择。

动态格式与字面量检查分开：被运行期过滤的动态消息不应触发格式化；未来对错误字面量进行编译检查不违背这一行为。现有契约及实际覆盖见 PROJECT，避免将未定恢复策略提前固化进测试。

依据：2026-11-01 核对的在线 C++ 草案[格式规则](https://eel.is/c++draft/format.string)与[错误报告](https://eel.is/c++draft/format.err.report)；Namica 只采用当前项目标准已提供的能力，不假定草案新增接口可用。

## 后续阶段建议

日志基础 → 窗口、输入与必要渲染 → 场景及保存加载 → 编辑与试玩 → 简化发布。每段交付可使用的子集，由下游实际使用推动完善；不等 Render/Scene 全部完成才接入简易编辑器。

围绕贪吃蛇收敛最小编辑能力与数据格式，试玩结束不破坏编辑内容，发布时包含必要程序、运行库、配置和资源，并脱离源码目录验证。具体编辑属性、数据格式和实现技术仍待决定。
