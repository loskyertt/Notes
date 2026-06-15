# GDB 快速参考手册（现代版）

> 原文：GDB Quick Reference (GDB Version 5)
> 来源：Free Software Foundation, Inc. | 作者：Roland H. Pesch
> 翻译、更新并补充说明（基于 GDB 7.x ~ 15.x 现代版本）

---

## ⚡ 现代化说明

本文档在原版（GDB 5）基础上，系统替换或补充了已废弃、过时或有更优替代的命令，并新增了现代调试工作流中常用的功能。每处变更均附有**对比说明**，解释旧命令的问题及新命令的优势。

---

## 基本命令

| 命令 | 说明 |
|------|------|
| `gdb program [core]` | 调试程序（可指定 core dump 文件） |
| `run [arglist]` | 启动程序（可带参数列表） |
| `bt` | 回溯（backtrace）：显示程序调用栈 |
| `p expr` | 显示表达式的值 |
| `c` | 继续运行程序 |
| `n` | 下一行，步过函数调用 |
| `s` | 下一行，步入函数调用 |
| `quit` | 退出 GDB（也可用 `q` 或 `Ctrl-D`） |

> **补充说明**：`n`（next）和 `s`（step）的区别：遇到函数调用时，`n` 将整个函数当作一步执行，`s` 会进入函数内部逐行执行。

---

## 启动 GDB

| 命令 | 说明 |
|------|------|
| `gdb` | 启动 GDB，不加载任何调试文件 |
| `gdb program` | 开始调试指定程序 |
| `gdb program core` | 调试程序并使用 core dump 文件 |
| `gdb program pid` | 附加到正在运行的进程（指定进程 ID） |
| `gdb -tui program` | 以 TUI 模式启动，同时显示源码和调试信息窗口 |
| `gdb --args program arg1 arg2` | 启动时直接传入程序参数（推荐替代 `set args`） |
| `gdb -ex "cmd" program` | 启动后立即执行指定 GDB 命令 |
| `gdb -batch -ex "cmd" program` | 批处理模式（非交互，适合脚本自动化） |

> **🔄 现代化变更**：
>
> | 旧方式 | 新方式 | 原因 |
> |--------|--------|------|
> | 启动后 `set args ...` | `gdb --args program arg1 arg2` | 更直观，参数不会跨会话保留 |
> | 无 `-ex` 选项 | `gdb -ex "break main" -ex "run" program` | 启动时即可注入命令，适合自动化场景 |
> | `-tui` 需额外记忆 | 也可在运行中用 `tui enable` 随时切换 | GDB 7.0+ 支持运行中动态切换 TUI |

---

## 停止 GDB

| 命令 | 说明 |
|------|------|
| `quit` | 退出 GDB（也可用 `q` 或 `Ctrl-D`） |
| `INTERRUPT`（如 `Ctrl-C`） | 终止当前命令，或向运行中的进程发送中断信号 |

---

## 获取帮助

| 命令 | 说明 |
|------|------|
| `help` | 列出命令分类 |
| `help class` | 显示某类命令的单行描述 |
| `help command` | 描述指定命令的用法 |
| `apropos keyword` | 搜索所有包含关键词的命令（GDB 7.3+） |
| `complete prefix` | 列出以 prefix 开头的所有可补全命令 |

> **🔄 现代化变更**：
>
> | 旧方式 | 新方式 | 原因 |
> |--------|--------|------|
> | 逐一 `help` 查找 | `apropos keyword` | 类似 `man -k`，可按关键词搜索命令，效率更高 |
> | 无 | `complete` | 可查看 Tab 补全候选项，调试脚本时很有用 |

---

## 执行程序

| 命令 | 说明 |
|------|------|
| `run arglist` | 使用指定参数列表启动程序 |
| `run` | 使用当前参数启动程序 |
| `run ... <inf >outf` | 启动程序并重定向输入/输出 |
| `start [arglist]` | 启动程序并在 `main` 入口处自动暂停（GDB 6.4+） |
| `starti [arglist]` | 启动程序并在第一条机器指令处暂停（GDB 8.1+，适合无 `main` 的场景） |
| `kill` | 终止运行中的程序 |
| `set args arglist` | 为下次运行指定参数列表 |
| `set args` | 指定空参数列表 |
| `show args` | 显示参数列表 |
| `show env` | 显示所有环境变量 |
| `show env var` | 显示指定环境变量的值 |
| `set env var string` | 设置环境变量 |
| `unset env var` | 从环境中移除变量 |

> **🔄 现代化变更**：
>
> | 旧方式 | 新方式 | 原因 |
> |--------|--------|------|
> | `run` + 手动在 `main` 设断点 | `start` | 自动在 `main` 暂停，省去额外操作 |
> | 无 | `starti` | 调试汇编、嵌入式或无 `main` 符号的程序时必要 |
> | `tty dev` | 多已废弃，改用 `run < dev` 重定向 | 更通用，兼容性更好 |

---

## Shell 命令

| 命令 | 说明 |
|------|------|
| `cd dir` | 切换工作目录到 dir |
| `pwd` | 显示当前工作目录 |
| `make ...` | 调用 `make` 构建程序 |
| `shell cmd` | 执行任意 shell 命令 |
| `!cmd` | `shell cmd` 的简写（GDB 7.0+） |
| `pipe cmd \| shell_cmd` | 将 GDB 命令输出通过管道传给 shell（GDB 9.1+） |

> **🔄 现代化变更**：
>
> | 旧方式 | 新方式 | 原因 |
> |--------|--------|------|
> | `shell cmd` | `!cmd` | 更简洁的简写形式 |
> | 无 | `pipe info registers \| grep rsp` | `pipe`（或 `\|`）可将 GDB 输出传给外部工具过滤，GDB 9.1+ 支持 |

---

## 断点与观察点

| 命令 | 说明 |
|------|------|
| `break [file:]line` | 在指定行号设置断点（可指定文件） |
| `break [file:]func` | 在指定函数入口设置断点 |
| `break *addr` | 在指定地址设置断点 |
| `break` | 在下一条指令处设置断点 |
| `break ... if expr` | 条件断点：当 expr 非零时中断 |
| `tbreak ...` | 临时断点，触发一次后自动删除 |
| `rbreak [file:]regex` | 在所有匹配正则表达式的函数处设置断点 |
| `watch expr` | 为表达式设置写观察点（值变化时中断） |
| `rwatch expr` | 为表达式设置读观察点（值被读取时中断，GDB 6.0+） |
| `awatch expr` | 为表达式设置读写观察点（读或写时均中断，GDB 6.0+） |
| `watch expr if cond` | 带条件的观察点（GDB 7.0+） |
| `catch event` | 捕获事件（throw、catch、syscall、fork、load 等） |
| `catch syscall [name/num]` | 捕获指定系统调用（GDB 7.0+） |
| `info break` | 显示已定义的断点和观察点 |
| `delete [n]` | 删除断点（或指定编号的断点） |
| `disable [n]` | 禁用断点（或指定编号的断点） |
| `enable [n]` | 启用断点（或指定编号的断点） |
| `enable once [n]` | 启用断点，触发一次后自动禁用 |
| `enable del [n]` | 启用断点，触发一次后自动删除 |
| `ignore n count` | 忽略断点 n 共 count 次 |
| `cond n [expr]` | 在断点 n 上设置新的条件表达式；不带 expr 则取消条件 |
| `commands n` | 每次断点 n 触发时执行 GDB 命令列表 |
| `save breakpoints file` | 将断点保存到文件（GDB 7.2+） |
| `source file` | 从文件恢复断点 |
| `end` | 命令列表结束 |

> **🔄 现代化变更**：
>
> | 旧方式 | 新方式 | 原因 |
> |--------|--------|------|
> | `info watch`（独立命令） | `info break`（统一显示） | 现代 GDB 已将观察点合并到 `info break` 中显示 |
> | `watch` 仅监控写操作 | `rwatch`/`awatch` | 可精确控制读或读写，避免遗漏 |
> | `clear [file:]line` | 更推荐 `delete n`（按编号） | 用编号删除更精确，不易误删同行断点 |
> | 无 | `save breakpoints file` | 调试会话恢复时无需重设所有断点，适合长期调试项目 |
> | `catch event`（旧版事件有限） | `catch syscall name` | 可精确捕获指定系统调用，如 `catch syscall write` |

---

## 执行控制

| 命令 | 说明 |
|------|------|
| `continue [count]` | 继续运行；若指定 count，则跳过当前断点 count 次 |
| `step [count]` | 步入执行，到达另一行时停止；可指定重复次数 |
| `stepi [count]` | 按机器指令单步步入 |
| `next [count]` | 步过执行，包括函数调用 |
| `nexti [count]` | 按机器指令单步步过 |
| `until [location]` | 运行直到当前行之后的位置（用于跳出循环） |
| `advance location` | 运行到指定位置（GDB 7.0+，比 `until` 更灵活） |
| `finish` | 运行直到当前栈帧返回 |
| `return [expr]` | 强制从当前栈帧返回（可指定返回值） |
| `signal num` | 以信号 num 恢复执行（0 表示无信号） |
| `jump line` | 在指定行号恢复执行 |
| `jump *addr` | 在指定地址恢复执行 |
| `set variable var=expr` | 修改程序变量的值（推荐写法） |
| `skip function func` | 步入时跳过指定函数（GDB 7.4+） |
| `skip file file` | 步入时跳过指定文件中的所有函数（GDB 7.4+） |

> **🔄 现代化变更**：
>
> | 旧方式 | 新方式 | 原因 |
> |--------|--------|------|
> | `set var=expr` | `set variable var=expr` | 旧写法与 `set` 参数命名空间有冲突风险；新写法更明确，不易误操作 |
> | 无 | `advance location` | `until` 只能前进到当前行之后，`advance` 可以跳到任意位置（包括其他函数），更灵活 |
> | 无 | `skip function` | 调试时不想进入某些库函数（如 `std::vector::push_back`），用 `skip` 比 `next` 更精确 |

---

## 程序栈

| 命令 | 说明 |
|------|------|
| `backtrace [n]` | 打印栈帧追踪；n>0 为最内层 n 帧，n<0 为最外层 n 帧 |
| `bt full` | 显示每层栈帧的局部变量（比 `bt` 更详细） |
| `bt -frame-arguments all` | 显示每层栈帧的函数参数值（GDB 7.1+） |
| `frame [n]` | 选择第 n 号栈帧；不带 n 则显示当前栈帧 |
| `up n` | 向上选择 n 层栈帧（向调用者方向） |
| `down n` | 向下选择 n 层栈帧（向被调用者方向） |
| `info frame [addr]` | 描述选中的栈帧或指定地址的栈帧 |
| `info args` | 显示选中栈帧的函数参数 |
| `info locals` | 显示选中栈帧的局部变量 |
| `info registers` | 显示通用寄存器（推荐替代 `info all-reg`） |
| `info all-registers` | 显示所有寄存器（含浮点/向量寄存器） |
| `thread apply all bt` | 显示所有线程的调用栈 |

> **🔄 现代化变更**：
>
> | 旧方式 | 新方式 | 原因 |
> |--------|--------|------|
> | `info all-reg` | `info registers` / `info all-registers` | 旧缩写 `all-reg` 依赖不稳定的前缀匹配；现代 GDB 推荐使用完整命令名 |
> | `bt` 只显示帧 | `bt full` 或 `bt -frame-arguments all` | 可直接看到每帧的局部变量和参数，无需逐帧切换 |

---

## 显示

| 命令 | 说明 |
|------|------|
| `print [/f] [expr]` | 按格式 f 显示表达式 expr 的值（或上一个值 `$`） |
| `print -pretty -- expr` | 美化输出结构体（GDB 7.0+） |
| `print -array -- expr` | 以数组模式显示（GDB 7.0+） |
| `call [/f] expr` | 调用函数并显示返回值，不显示 void |
| `x [/Nuf] expr` | 检查地址 expr 处的内存 |
| `output expr` | 打印值但不显示换行和 `$N =` 前缀（适合脚本） |

### print 格式说明

| 格式符 | 说明 |
|--------|------|
| `x` | 十六进制 |
| `d` | 有符号十进制 |
| `u` | 无符号十进制 |
| `o` | 八进制 |
| `t` | 二进制 |
| `a` | 地址（绝对和相对） |
| `c` | 字符 |
| `f` | 浮点数 |
| `z` | 带前导零的十六进制（GDB 7.1+） |
| `r` | 原始格式，不应用 pretty-printer（GDB 7.1+） |

### x 命令格式说明

`x /Nuf expr` 中：N 为单元数量，u 为单元大小，f 为格式。

| 单元符 | 说明 |
|--------|------|
| `b` | 单个字节 |
| `h` | 半字（两字节） |
| `w` | 字（四字节） |
| `g` | 双字（八字节） |

| 格式符 | 说明 |
|--------|------|
| `s` | 以 null 结尾的字符串 |
| `i` | 机器指令 |

> **🔄 现代化变更**：
>
> | 旧方式 | 新方式 | 原因 |
> |--------|--------|------|
> | `set print pretty on` 后全局生效 | `print -pretty -- expr` | 可对单次输出应用，不影响全局设置 |
> | 无 `/z` 格式 | `p/z var` | 输出带前导零的十六进制，寄存器调试更直观 |
> | 无 `/r` 格式 | `p/r var` | 跳过 Python pretty-printer，查看原始内存值时有用 |

---

## 自动显示

| 命令 | 说明 |
|------|------|
| `display [/f] expr` | 每次程序停止时自动按格式 f 显示 expr 的值 |
| `display` | 显示所有已启用的自动显示表达式 |
| `undisplay n` | 从自动显示列表中移除编号 n |
| `disable display n` | 禁用编号 n 的自动显示 |
| `enable display n` | 启用编号 n 的自动显示 |
| `info display` | 显示自动显示表达式的编号列表 |

> **🔄 现代化变更**：
>
> | 旧方式 | 新方式 | 原因 |
> |--------|--------|------|
> | `disable disp n` | `disable display n` | 旧缩写依赖前缀匹配，新版 GDB 推荐完整写法以避免歧义 |

---

## 表达式

| 表达式 | 说明 |
|--------|------|
| `expr` | C、C++、Fortran、Ada 等语言表达式（含函数调用） |
| `addr@len` | 从地址 addr 开始的 len 个元素的数组 |
| `file::nm` | 文件 file 中定义的变量或函数 nm |
| `{type}addr` | 以指定类型读取地址 addr 处的内存 |

> **🔄 现代化变更**：原文档仅提及 C/C++/Modula-2。现代 GDB 已广泛支持 Fortran、Ada、Go、Rust（部分），Modula-2 已基本不再使用。

### 便利变量

| 变量 | 说明 |
|------|------|
| `$` | 最近一次显示的值 |
| `$n` | 第 n 次显示的值 |
| `$$` | 倒数第二次显示的值 |
| `$$n` | 倒数第 n 次显示的值 |
| `$_` | 最后用 `x` 检查的地址 |
| `$__` | 地址 `$_` 处的值 |
| `$var` | 便利变量；可赋任意值 |
| `$_thread` | 当前线程编号（GDB 7.0+） |
| `$_exitcode` | 程序退出码（GDB 8.1+） |
| `$_exception` | 当前捕获的异常对象（C++ 异常调试，GDB 7.0+） |

| 命令 | 说明 |
|------|------|
| `show values [n]` | 显示最近 10 个值（或围绕 $n 的值） |
| `show convenience` | 显示所有便利变量（完整命令，旧缩写 `show conv` 仍可用） |

> **🔄 现代化变更**：
>
> | 旧方式 | 新方式 | 原因 |
> |--------|--------|------|
> | `show conv` | `show convenience` | 旧缩写依赖前缀匹配，补全写法更安全 |
> | 无 `$_thread` | `$_thread` | 多线程调试中可在条件断点中使用，如 `break foo if $_thread == 2` |

---

## 控制 GDB

| 命令 | 说明 |
|------|------|
| `set param value` | 设置 GDB 内部参数 |
| `show param` | 显示参数的当前设置 |

### 常用参数

| 参数 | 说明 |
|------|------|
| `confirm on/off` | 启用或禁用危险操作确认提示 |
| `editing on/off` | 控制命令行编辑功能（readline） |
| `height lpp` | 显示暂停前的行数（0 表示不分页） |
| `width cpl` | 折行前的字符数（0 表示不折行） |
| `language lang` | GDB 表达式语言（auto、c、c++、fortran 等） |
| `prompt str` | 使用 str 作为 GDB 提示符 |
| `radix base` | 数值基数（8、10 或 16） |
| `verbose on/off` | 加载符号时显示控制消息 |
| `write on/off` | 允许或禁止修改二进制文件和 core 文件 |
| `print address on/off` | 在栈和值中打印内存地址 |
| `print array on/off` | 数组的紧凑或美观格式 |
| `print array-indexes on/off` | 打印数组元素时显示下标（GDB 7.1+） |
| `print demangle on/off` | C++ 符号的反修饰显示 |
| `print asm-demangle on/off` | 汇编输出中的 C++ 符号反修饰 |
| `print elements limit` | 数组元素显示数量上限（0 表示无限制） |
| `print object on/off` | 打印 C++ 对象的派生类型 |
| `print pretty on/off` | 结构体显示：紧凑或缩进 |
| `print union on/off` | 联合体成员的显示 |
| `print vtbl on/off` | C++ 虚函数表的显示 |
| `print symbol on/off` | 打印地址时附带符号名（GDB 7.1+） |
| `pagination on/off` | 控制输出分页（等同于调整 `height`） |

> **🔄 现代化变更**：
>
> | 旧方式 | 新方式 | 原因 |
> |--------|--------|------|
> | `complaint limit` | 已弃用，不再推荐使用 | 现代 GDB 改进了符号加载，此参数意义不大 |
> | `print asm-dem on/off` | `print asm-demangle on/off` | 旧缩写依赖前缀匹配，现代 GDB 推荐完整参数名 |
> | `print demangl on/off` | `print demangle on/off` | 同上 |
> | 无 `print array-indexes` | `set print array-indexes on` | 打印 `arr[0]=1, arr[1]=2` 形式，调试数组更清晰 |
> | 无 `pagination` | `set pagination off` | 比 `set height 0` 语义更直接 |

---

## 源文件

| 命令 | 说明 |
|------|------|
| `directory names` | 将目录名添加到源文件搜索路径前端（可用 `dir` 简写） |
| `directory` | 清空源文件搜索路径 |
| `show directories` | 显示当前源文件搜索路径 |
| `set substitute-path from to` | 将源码路径前缀 from 替换为 to（GDB 7.0+） |
| `show substitute-path` | 显示路径替换规则 |
| `unset substitute-path [from]` | 删除路径替换规则 |
| `list` | 显示接下来十行源码 |
| `list -` | 显示之前十行源码 |
| `list lines` | 显示指定行周围的源码 |
| `set listsize n` | `list` 命令显示的行数（旧为 `listsize n`） |
| `info line num` | 显示源码行 num 对应的编译代码的起止地址 |
| `info source` | 显示当前源文件名及语言 |
| `info sources` | 列出所有使用中的源文件 |
| `search regex` | 向后搜索匹配正则表达式的源码行（旧：`forw regex`） |
| `reverse-search regex` | 向前搜索匹配正则表达式的源码行（旧：`rev regex`） |

> **🔄 现代化变更**：
>
> | 旧方式 | 新方式 | 原因 |
> |--------|--------|------|
> | `forw regex` | `search regex` | `forw` 是极旧的别名，语义不直观；`search` 更清晰 |
> | `rev regex` | `reverse-search regex` | 同上 |
> | `listsize n`（裸命令） | `set listsize n` | 旧写法在现代 GDB 中已不被推荐，应通过 `set` 命名空间设置 |
> | `show dir` | `show directories` | 完整命令名更明确 |
> | 手动修改路径 | `set substitute-path /build/path /local/path` | 当二进制编译路径与本地路径不同时（如 CI 构建），无需移动源文件即可正确映射 |

---

## 符号表

| 命令 | 说明 |
|------|------|
| `info address s` | 显示符号 s 的存储位置 |
| `info functions [regex]` | 显示已定义函数的名称和类型（可匹配正则） |
| `info variables [regex]` | 显示全局变量的名称和类型（可匹配正则） |
| `info types [regex]` | 显示已定义类型（GDB 4.6+，结构体、枚举等） |
| `whatis [expr]` | 显示表达式的类型（不展开 typedef） |
| `ptype [expr]` | 显示表达式的完整类型（展开结构体成员） |
| `ptype /o type` | 显示结构体每个成员的偏移量和大小（GDB 8.1+） |
| `demangle name` | 反修饰 C++ 符号名 |

> **🔄 现代化变更**：
>
> | 旧方式 | 新方式 | 原因 |
> |--------|--------|------|
> | `info func` | `info functions` | 补全写法，避免依赖前缀匹配缩写 |
> | `info var` | `info variables` | 同上 |
> | `ptype type`（只显示成员） | `ptype /o type` | GDB 8.1+ 新增 `/o` 选项，可显示每个字段的字节偏移量和大小，分析内存布局时极为有用 |

---

## GDB 脚本

| 命令 | 说明 |
|------|------|
| `source script` | 从文件读取并执行 GDB 命令 |
| `source -v script` | 执行脚本并打印每条命令（调试脚本时有用） |
| `define cmd` | 创建新的 GDB 命令 cmd |
| `end` | 命令列表结束 |
| `document cmd` | 为新命令创建在线帮助文档 |
| `end` | 帮助文本结束 |

> **🔄 现代化变更（Python 脚本支持）**：
>
> GDB 7.0+ 内置 Python 解释器，支持用 Python 编写更强大的扩展和命令，远超 GDB 原生脚本能力：
>
> ```python
> # 在 GDB Python 环境中：
> import gdb
>
> class PrintLocalsPretty(gdb.Command):
>     """打印当前帧所有局部变量（美化输出）"""
>     def __init__(self):
>         super().__init__("plocals", gdb.COMMAND_DATA)
>
>     def invoke(self, arg, from_tty):
>         frame = gdb.selected_frame()
>         block = frame.block()
>         for sym in block:
>             if sym.is_variable:
>                 val = frame.read_var(sym)
>                 print(f"  {sym.name} = {val}")
>
> PrintLocalsPretty()
> ```
>
> 加载方式：`source myscript.py`  
> 也可在 `~/.gdbinit` 中 `source` 自动加载。

---

## 历史命令

| 命令 | 说明 |
|------|------|
| `show commands` | 显示最近 10 条命令 |
| `show commands n` | 显示编号 n 附近的 10 条命令 |
| `show commands +` | 显示接下来 10 条命令 |

### 历史选项

| 选项 | 说明 |
|------|------|
| `set history expansion on/off` | 启用/禁用 readline 历史扩展（完整写法） |
| `set history filename file` | 记录 GDB 命令历史的文件（完整写法） |
| `set history size size` | 保留的历史命令数量 |
| `set history save on/off` | 控制是否使用外部文件保存历史 |

> **🔄 现代化变更**：
>
> | 旧方式 | 新方式 | 原因 |
> |--------|--------|------|
> | `h exp off/on` | `set history expansion on/off` | 旧写法 `h exp` 是极度缩写，可读性差；现代 GDB 推荐完整 `set history` 命名空间 |
> | `h file filename` | `set history filename file` | 同上 |
> | `h size size` | `set history size size` | 同上 |
> | `h save off/on` | `set history save on/off` | 同上 |

---

## 信号处理

| 命令 | 说明 |
|------|------|
| `handle signal act` | 指定 GDB 对信号的处理方式 |
| `info signals` | 显示信号表及 GDB 对每个信号的操作 |

### handle 动作

| 动作 | 说明 |
|------|------|
| `print` | 通知信号发生 |
| `noprint` | 不通知信号 |
| `stop` | 停止执行 |
| `nostop` | 不停止执行 |
| `pass` | 允许程序处理该信号 |
| `nopass` | 不让程序看到该信号 |

> **补充说明**：`handle SIGPIPE nostop noprint pass` 可以让 GDB 不在 SIGPIPE 处停止，但仍让程序处理。调试信号密集型程序时，可以先 `info signals` 查看各信号默认设置，再针对性调整。

---

## 工作文件

| 命令 | 说明 |
|------|------|
| `file [file]` | 同时使用文件作为符号表和可执行文件；不带参数则丢弃 |
| `core-file [file]` | 将文件作为 core dump 读取（旧：`core [file]`） |
| `exec-file [file]` | 仅将文件作为可执行文件（旧：`exec [file]`） |
| `symbol-file [file]` | 从文件读取符号表（旧：`symbol [file]`） |
| `add-symbol-file file addr` | 从动态加载的文件中读取额外符号（旧：`add-sym`） |
| `load file` | 动态链接文件并添加其符号 |
| `info files` | 显示当前使用的工作文件和目标 |
| `info target` | 同 `info files`（别名） |
| `path dirs` | 将目录添加到可执行文件和符号文件的搜索路径前端 |
| `show path` | 显示可执行文件和符号文件的搜索路径 |
| `info sharedlibrary` | 列出当前加载的共享库（旧：`info share`） |

> **🔄 现代化变更**：
>
> | 旧方式 | 新方式 | 原因 |
> |--------|--------|------|
> | `core [file]` | `core-file [file]` | `core` 是旧别名，`core-file` 语义更清晰，与其他 `-file` 系列命令一致 |
> | `exec [file]` | `exec-file [file]` | 同上 |
> | `symbol [file]` | `symbol-file [file]` | 同上 |
> | `add-sym file addr` | `add-symbol-file file addr` | 旧缩写已废弃，完整命令名更明确 |
> | `info share` | `info sharedlibrary` | `info share` 仍有效但是旧缩写，完整命令名更具可读性 |

---

## 调试目标

| 命令 | 说明 |
|------|------|
| `target type param` | 连接到目标机器、进程或文件 |
| `target remote host:port` | 连接远程 GDB stub（如 gdbserver） |
| `target extended-remote host:port` | 扩展远程协议，支持 `run` 重启进程（推荐替代 `target remote`） |
| `help target` | 显示可用的目标类型 |
| `attach pid` | 附加到正在运行的进程 |
| `detach` | 从 GDB 控制中释放目标（不终止进程） |
| `disconnect` | 断开连接但不释放目标（适合 `extended-remote`） |

> **🔄 现代化变更**：
>
> | 旧方式 | 新方式 | 原因 |
> |--------|--------|------|
> | `target remote host:port` | `target extended-remote host:port` | `extended-remote` 支持在连接中执行 `run`、`file` 等命令，适合嵌入式和容器调试；`target remote` 在进程退出后就断开，`extended-remote` 可以保持连接并重启 |

---

## TUI 模式（现代终端调试界面）

GDB 7.0+ 内置 TUI（Text User Interface），可在终端中同时显示源码、汇编、寄存器等窗口，无需外部 IDE。

| 命令 | 说明 |
|------|------|
| `tui enable` | 进入 TUI 模式（也可用 `Ctrl-X A` 切换） |
| `tui disable` | 退出 TUI 模式 |
| `layout src` | 显示源码窗口 |
| `layout asm` | 显示汇编窗口 |
| `layout regs` | 显示寄存器窗口（+ 源码或汇编） |
| `layout split` | 同时显示源码和汇编窗口 |
| `focus src / asm / regs / cmd` | 切换焦点到指定窗口 |
| `refresh` | 刷新 TUI 显示（屏幕乱码时使用） |
| `winheight src +5` | 调整窗口高度 |
| `tui reg general` | 在寄存器窗口显示通用寄存器 |
| `tui reg float` | 在寄存器窗口显示浮点寄存器 |
| `tui reg sse` | 在寄存器窗口显示 SSE 寄存器 |

> **补充说明**：TUI 模式是原版文档中完全缺失的重要功能。对于纯命令行调试场景，TUI 可以显著提升效率，是现代 GDB 最实用的特性之一。`Ctrl-X A` 快捷键可随时切换进出 TUI 模式。

---

## 反汇编

| 命令 | 说明 |
|------|------|
| `disassemble [addr]` | 反汇编当前函数或指定地址（可用 `disas` 简写） |
| `disassemble /m [addr]` | 混合显示源码和汇编（GDB 7.0+，已在 GDB 10+ 弃用） |
| `disassemble /s [addr]` | 混合显示源码和汇编（GDB 10+ 推荐，替代 `/m`） |
| `disassemble /r [addr]` | 同时显示原始字节码 |
| `disassemble addr1,addr2` | 反汇编指定地址范围 |
| `set disassembly-flavor intel` | 使用 Intel 语法（默认 AT&T，GDB 7.0+） |
| `set disassembly-flavor att` | 使用 AT&T 语法（默认） |

> **🔄 现代化变更**：
>
> | 旧方式 | 新方式 | 原因 |
> |--------|--------|------|
> | `disassem` | `disassemble` / `disas` | `disassem` 是旧缩写，`disas` 是现代官方缩写 |
> | `disas /m` | `disas /s` | `/m` 在 GDB 10+ 已弃用，`/s` 是推荐替代，源码与汇编对应更准确 |
> | 无 flavor 设置 | `set disassembly-flavor intel` | AT&T 语法对很多人不直观；Intel 语法更接近 NASM/Visual Studio 风格 |

---

## 在 GNU Emacs 中使用 GDB

| 快捷键 | 说明 |
|--------|------|
| `M-x gdb` | 在 Emacs 中启动 GDB（进入 GUD 模式） |
| `M-s` | 单步执行一行（step） |
| `M-n` | 单步执行一行（next） |
| `M-i` | 单步执行一条指令（stepi） |
| `M-c` | 继续运行（cont） |
| `C-c C-f` | 完成当前栈帧（finish） |
| `M-u` | 向上移动栈帧（up） |
| `M-d` | 向下移动栈帧（down） |
| `C-x SPC` | 在光标处设置断点 |

> **补充说明**：Emacs 的 `M-x gdb` 提供多窗口 GDB 界面（GUD + 源码 + 局部变量 + 调用栈），比原版文档描述的单一命令行模式功能更丰富。VS Code、CLion 等现代 IDE 也内置了 GDB 集成，可作为替代方案。

---

## 许可证

| 命令 | 说明 |
|------|------|
| `show copying` | 显示 GNU 通用公共许可证 |
| `show warranty` | 显示无担保声明 |

GDB 是自由软件，欢迎在 GNU 通用公共许可证条款下分发副本。GDB 不提供任何担保。

---

## 现代调试技巧（补充与更新）

### 1. 多线程调试

```gdb
info threads                          # 列出所有线程
thread n                              # 切换到线程 n
thread apply all bt                   # 显示所有线程调用栈
thread apply all bt full              # 显示所有线程调用栈及局部变量
set scheduler-locking on              # 锁定只运行当前线程
set scheduler-locking step            # 单步时锁定，continue 时放开（推荐）
break foo if $_thread == 2            # 只在线程 2 触发的条件断点
```

> **🔄 变更**：`set scheduler-locking step`（GDB 7.0+）比 `on` 更实用：单步调试时只运行当前线程，`continue` 时恢复所有线程，避免死锁。

### 2. 远程调试

```bash
# 目标机器上启动 gdbserver
gdbserver :1234 program
gdbserver --attach :1234 pid       # 附加到已运行进程

# 本地 GDB 连接（推荐使用 extended-remote）
target extended-remote host:1234
```

### 3. 调试 fork/exec 的子进程

```gdb
set follow-fork-mode child           # 跟踪子进程（默认 parent）
set follow-exec-mode new             # exec 后在新 inferior 中继续（GDB 7.1+）
set detach-on-fork off               # 同时调试父子进程（GDB 7.1+）
info inferiors                       # 显示所有 inferior（进程）
inferior n                           # 切换到第 n 个 inferior
```

> **🔄 变更**：GDB 7.1+ 支持 `detach-on-fork off` + `inferior` 命令，可同时调试 fork 的父子进程，原版文档没有涉及此功能。

### 4. 反向调试（GDB 7.0+）

```gdb
target record-full                   # 开始完整记录（新写法，旧为 record）
target record-btrace                 # 硬件辅助分支追踪（需 CPU 支持，GDB 7.10+）
reverse-step                         # 反向单步进入
reverse-next                         # 反向单步跳过
reverse-continue                     # 反向继续执行
reverse-finish                       # 反向执行到函数调用处
set exec-direction reverse           # 设置默认执行方向为反向
```

> **🔄 变更**：`record` 现代推荐写法为 `target record-full`；`record-btrace` 利用 Intel PT/BTS 硬件，开销极低（`record-full` 开销巨大）。

### 5. 查看内存映射

```gdb
info proc mappings                   # 显示进程的内存映射（Linux）
info proc all                        # 显示进程详细信息
maintenance info sections            # 显示可执行文件的各段信息
```

### 6. Checkpoint（进程快照，GDB 7.0+）

```gdb
checkpoint                           # 保存当前进程状态快照
info checkpoints                     # 显示所有快照
restart n                            # 恢复到快照 n
delete checkpoint n                  # 删除快照 n
```

> **🔄 新增**：`checkpoint` 允许在不使用反向调试的情况下"时间旅行"：在某个状态打快照，继续执行后可随时恢复。比反向调试开销更小，适合确定性程序。

### 7. 动态 Printf（GDB 7.9+）

```gdb
dprintf location, "format", args...  # 在指定位置插入动态打印，无需修改源码
dprintf main.c:42, "x=%d\n", x       # 示例：在第 42 行打印变量 x
info breakpoints                      # dprintf 也出现在断点列表中
```

> **🔄 新增**：`dprintf` 相当于"不重新编译的 printf 调试"，在无法修改源码或重新编译时极为有用，且比条件断点 + `commands` 更简洁。

### 8. Python Pretty-Printer（GDB 7.0+）

GDB 7.0+ 支持 Python pretty-printer，可美化 STL 容器等复杂结构的输出。通常通过 `libstdc++-v3` 附带的 Python 脚本自动加载。

```gdb
info pretty-printer                  # 显示当前注册的 pretty-printer
enable pretty-printer global "stl"   # 启用 STL pretty-printer
disable pretty-printer global "stl"  # 禁用
print v                              # std::vector<int> 显示为 {1, 2, 3}（而非原始指针）
```

### 9. 结构体成员偏移分析（GDB 8.1+）

```gdb
ptype /o struct MyStruct
# 输出示例：
# /* offset    |  size */
# struct MyStruct {
# /*   0      |     4 */    int x;
# /*   4      |     4 */    int y;
# /*   8      |     8 */    double z;
# }
# /* total size (bytes):   16 */
```

> **🔄 新增**：`ptype /o` 是 GDB 8.1 新增的强大功能，可直接查看结构体的内存布局，对性能优化（避免 false sharing、分析 padding）非常有价值。

---

## 变更汇总对照表

| 旧命令（GDB 5 时代） | 现代替代命令 | 变更版本 | 主要原因 |
|---------------------|-------------|----------|----------|
| `gdb program` + `set args` | `gdb --args program arg1` | 任意 | 更直观，避免遗忘 |
| `run` + 手动断 `main` | `start` | GDB 6.4 | 自动在 main 暂停 |
| `info watch` | `info break`（统一） | GDB 7.x | 断点和观察点合并管理 |
| `info all-reg` | `info registers` / `info all-registers` | — | 避免不稳定的前缀匹配 |
| `set var=expr` | `set variable var=expr` | — | 明确命名空间，避免冲突 |
| `forw regex` | `search regex` | — | 语义更清晰 |
| `rev regex` | `reverse-search regex` | — | 语义更清晰 |
| `info func` | `info functions` | — | 完整命令名，避免歧义 |
| `info var` | `info variables` | — | 同上 |
| `add-sym file addr` | `add-symbol-file file addr` | — | 旧缩写已废弃 |
| `info share` | `info sharedlibrary` | — | 完整命令名 |
| `core [file]` | `core-file [file]` | — | 与命名体系统一 |
| `exec [file]` | `exec-file [file]` | — | 同上 |
| `symbol [file]` | `symbol-file [file]` | — | 同上 |
| `disassem` | `disas` | GDB 7.x | 官方推荐简写 |
| `disas /m` | `disas /s` | GDB 10 | `/m` 已弃用 |
| `h exp/file/size/save` | `set history expansion/filename/size/save` | — | 旧缩写可读性差 |
| `disable disp n` | `disable display n` | — | 完整命令名 |
| `show conv` | `show convenience` | — | 完整命令名 |
| `record` | `target record-full` | GDB 7.x | 明确目标类型 |
| `target remote` | `target extended-remote`（推荐） | GDB 6.x | 支持 run 重启，更强大 |
| `listsize n` | `set listsize n` | — | 通过 set 命名空间管理 |
| 无 | `dprintf` | GDB 7.9 | 动态 printf，无需重编译 |
| 无 | `skip function/file` | GDB 7.4 | 步入时跳过不关心的函数 |
| 无 | `checkpoint` / `restart` | GDB 7.0 | 进程快照与恢复 |
| 无 | `ptype /o` | GDB 8.1 | 结构体内存布局分析 |
| 无 | `catch syscall` | GDB 7.0 | 精确捕获系统调用 |
| 无 | TUI 模式 | GDB 7.0 | 终端内多窗口调试界面 |
