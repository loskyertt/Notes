# GDB 快速参考手册

> 原文：GDB Quick Reference (GDB Version 5)
> 来源：Free Software Foundation, Inc. | 作者：Roland H. Pesch
> 翻译并补充说明

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

> **补充说明**：`n`（next）和 `s`（step）的区别在于：当遇到函数调用时，`n` 会将整个函数当作一步执行完毕，而 `s` 会进入函数内部逐行执行。这是调试中最常用的两个命令。

---

## 启动 GDB

| 命令 | 说明 |
|------|------|
| `gdb` | 启动 GDB，不加载任何调试文件 |
| `gdb program` | 开始调试指定程序 |
| `gdb program core` | 调试程序并使用 core dump 文件 |
| `gdb program pid` | 附加到正在运行的进程（指定进程 ID） |

> **补充说明**：调试正在运行的进程时，也可以在 GDB 内部使用 `attach pid` 命令。使用 `gdb -tui program` 可以启动 TUI（文本用户界面）模式，在终端中同时显示源代码和调试信息。

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

> **补充说明**：GDB 的帮助系统非常完善，`help` 后面可以跟任何命令名，包括缩写。例如 `help b` 和 `help break` 效果相同。不确定命令时，善用 `help` 是最高效的学习方式。

---

## 执行程序

| 命令 | 说明 |
|------|------|
| `run arglist` | 使用指定参数列表启动程序 |
| `run` | 使用当前参数启动程序 |
| `run ... <inf >outf` | 启动程序并重定向输入/输出 |
| `kill` | 终止运行中的程序 |
| `tty dev` | 将 dev 设为下次运行时的标准输入和输出 |
| `set args arglist` | 为下次运行指定参数列表 |
| `set args` | 指定空参数列表 |
| `show args` | 显示参数列表 |
| `show env` | 显示所有环境变量 |
| `show env var` | 显示指定环境变量的值 |
| `set env var string` | 设置环境变量 |
| `unset env var` | 从环境中移除变量 |

> **补充说明**：`set args` 设置的参数会在每次 `run` 时使用，直到重新设置。如果程序需要从文件读取输入，可以使用 `run < input.txt` 的方式重定向输入。

---

## Shell 命令

| 命令 | 说明 |
|------|------|
| `cd dir` | 切换工作目录到 dir |
| `pwd` | 显示当前工作目录 |
| `make ...` | 调用 `make` 构建程序 |
| `shell cmd` | 执行任意 shell 命令 |

> **补充说明**：在 GDB 中可以直接执行 shell 命令而不必退出，例如 `shell ls` 查看目录内容。也可以用 `!cmd` 的简写形式，如 `!ls`。

---

## 断点与观察点

| 命令 | 说明 |
|------|------|
| `break [file:]line` | 在指定行号设置断点（可指定文件） |
| `break [file:]func` | 在指定函数入口设置断点 |
| `break *addr` | 在指定地址设置断点 |
| `break` | 在下一条指令处设置断点 |
| `break ... if expr` | 条件断点：当 expr 非零时中断 |
| `tbreak ...` | 临时断点，触发一次后自动禁用 |
| `rbreak [file:]regex` | 在所有匹配正则表达式的函数处设置断点 |
| `watch expr` | 为表达式设置观察点（值变化时中断） |
| `catch event` | 捕获事件（如 throw、catch、fork、vfork、load 等） |
| `info break` | 显示已定义的断点 |
| `info watch` | 显示已定义的观察点 |
| `clear` | 删除下一条指令处的断点 |
| `clear [file:]fun` | 删除函数入口处的断点 |
| `clear [file:]line` | 删除指定源码行的断点 |
| `delete [n]` | 删除断点（或指定编号的断点） |
| `disable [n]` | 禁用断点（或指定编号的断点） |
| `enable [n]` | 启用断点（或指定编号的断点） |
| `enable once [n]` | 启用断点，触发一次后自动禁用 |
| `enable del [n]` | 启用断点，触发一次后自动删除 |
| `ignore n count` | 忽略断点 n 共 count 次 |
| `cond n [expr]` | 在断点 n 上设置新的条件表达式；不带 expr 则取消条件 |
| `commands n` | 每次断点 n 触发时执行 GDB 命令列表 |
| `end` | 命令列表结束 |
| `[silent]` | 在命令列表中抑制默认显示 |

> **补充说明**：
> - 条件断点非常实用，例如 `break main.c:42 if x > 10` 只在变量 x 大于 10 时中断。
> - `tbreak` 适用于只需停一次的场景，如循环的第一次迭代。
> - `watch` 用于监控变量变化，但注意观察点会显著降低程序运行速度。
> - `commands` 可以配合断点实现自动化调试，例如在断点处自动打印变量值然后继续执行。

---

## 执行控制

| 命令 | 说明 |
|------|------|
| `continue [count]` | 继续运行；若指定 count，则跳过当前断点 count 次 |
| `step [count]` | 步入执行，到达另一行时停止；可指定重复次数 |
| `stepi [count]` | 按机器指令单步步入 |
| `next [count]` | 步过执行，包括函数调用 |
| `nexti [count]` | 按机器指令单步步过 |
| `until [location]` | 运行直到下一指令（或指定位置） |
| `finish` | 运行直到当前栈帧返回 |
| `return [expr]` | 强制从当前栈帧返回（可指定返回值） |
| `signal num` | 以信号 num 恢复执行（0 表示无信号） |
| `jump line` | 在指定行号恢复执行 |
| `jump *addr` | 在指定地址恢复执行 |
| `set var=expr` | 计算表达式但不显示结果；用于修改程序变量 |

> **补充说明**：
> - `finish` 是调试函数时非常有用的命令，它会执行完当前函数并停在返回点。
> - `until` 在循环中特别有用：当你在循环体内停住时，`until` 可以让你跳出循环（执行到循环结束）。
> - `set var=x=5` 可以直接修改变量值来测试不同的执行路径，无需重新编译。
> - `stepi` 和 `nexti` 主要用于调试没有源码的情况（如库函数），可以逐条汇编指令执行。

---

## 程序栈

| 命令 | 说明 |
|------|------|
| `backtrace [n]` | 打印栈中所有栈帧的追踪信息；或 n 个栈帧（n>0 为最内层，n<0 为最外层） |
| `frame [n]` | 选择第 n 号栈帧或指定地址的栈帧；不带 n 则显示当前栈帧 |
| `up n` | 向上选择 n 层栈帧（向调用者方向） |
| `down n` | 向下选择 n 层栈帧（向被调用者方向） |
| `info frame [addr]` | 描述选中的栈帧或指定地址的栈帧 |
| `info args` | 显示选中栈帧的函数参数 |
| `info locals` | 显示选中栈帧的局部变量 |
| `info all-reg [rn]` | 显示选中栈帧的寄存器值（all-reg 包含浮点寄存器） |

> **补充说明**：
> - `bt full` 可以显示每一层栈帧的局部变量值，比 `bt` 更详细。
> - 在多线程程序中，`thread apply all bt` 可以显示所有线程的调用栈，是排查死锁等问题的利器。
> - `frame` 命令切换栈帧后，就可以用 `p` 查看该层函数的局部变量。

---

## 显示

| 命令 | 说明 |
|------|------|
| `print [/f] [expr]` | 按格式 f 显示表达式 expr 的值（或上一个值 `$`） |
| `call [/f] expr` | 类似 print，但不显示 void 返回值 |
| `x [/Nuf] expr` | 检查地址 expr 处的内存；格式说明如下 |

### print 格式说明

| 格式符 | 说明 |
|--------|------|
| `x` | 十六进制 |
| `d` | 有符号十进制 |
| `u` | 无符号十进制 |
| `o` | 八进制 |
| `t` | 二进制 |
| `a` | 地址，绝对和相对 |
| `c` | 字符 |
| `f` | 浮点数 |

### x 命令格式说明

`x /Nuf expr` 中：

- **N**：显示的单元数量
- **u**：单元大小

| 单元符 | 说明 |
|--------|------|
| `b` | 单个字节 |
| `h` | 半字（两个字节） |
| `w` | 字（四个字节） |
| `g` | 双字（八个字节） |

- **f**：打印格式（同 print 格式，另加 `s` 和 `i`）

| 格式符 | 说明 |
|--------|------|
| `s` | 以 null 结尾的字符串 |
| `i` | 机器指令 |

> **补充说明**：
> - `p/x variable` 以十六进制显示变量值，`p/d variable` 以十进制显示。
> - `x/10xw 0xaddr` 表示从地址 addr 开始，以十六进制显示 10 个 word（4 字节）。
> - `x/20i $pc` 可以查看从当前程序计数器开始的 20 条汇编指令，非常实用。
> - `p *ptr@10` 可以打印指针 ptr 指向的数组的前 10 个元素。

---

## 自动显示

| 命令 | 说明 |
|------|------|
| `display [/f] expr` | 每次程序停止时自动按格式 f 显示 expr 的值 |
| `display` | 显示所有已启用的自动显示表达式 |
| `undisplay n` | 从自动显示列表中移除编号 n |
| `disable disp n` | 禁用编号 n 的自动显示 |
| `enable disp n` | 启用编号 n 的自动显示 |
| `info display` | 显示自动显示表达式的编号列表 |

> **补充说明**：`display` 命令适合在每次断点停下时都需要查看的变量。例如 `display i` 会在每次程序暂停时自动打印变量 i 的值，省去反复输入 `p i` 的麻烦。

---

## 表达式

| 表达式 | 说明 |
|--------|------|
| `expr` | C、C++ 或 Modula-2 表达式（包括函数调用） |
| `addr@len` | 从地址 addr 开始的 len 个元素的数组 |
| `file::nm` | 文件 file 中定义的变量或函数 nm |
| `{type}addr` | 以指定类型读取地址 addr 处的内存 |

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

| 命令 | 说明 |
|------|------|
| `show values [n]` | 显示最近 10 个值（或围绕 $n 的值） |
| `show conv` | 显示所有便利变量 |

> **补充说明**：便利变量可以用来保存中间结果。例如 `set $i = 0` 定义一个便利变量，然后在 `commands` 断点脚本中使用。`$` 变量在调试时特别方便，可以直接引用上次 `print` 的结果。

---

## 控制 GDB

| 命令 | 说明 |
|------|------|
| `set param value` | 设置 GDB 内部参数 |
| `show param` | 显示参数的当前设置 |

### 常用参数

| 参数 | 说明 |
|------|------|
| `complaint limit` | 异常符号的警告消息数量上限 |
| `confirm on/off` | 启用或禁用确认提示 |
| `editing on/off` | 控制命令行编辑功能 |
| `height lpp` | 显示暂停前的行数 |
| `language lang` | GDB 表达式语言（auto、c 或 modula-2） |
| `prompt str` | 使用 str 作为 GDB 提示符 |
| `radix base` | 数值基数（八进制、十进制或十六进制） |
| `verbose on/off` | 加载符号时的控制消息 |
| `width cpl` | 折行前的字符数 |
| `write on/off` | 允许或禁止修改二进制文件和 core 文件 |
| `print address on/off` | 在栈和值中打印内存地址 |
| `print array on/off` | 数组的紧凑或美观格式 |
| `print asm-dem on/off` | 机器指令输出中的 C++ 符号反修饰 |
| `print demangl on/off` | C++ 符号的源码形式（反修饰）或内部形式 |
| `print elements limit` | 数组元素显示数量上限 |
| `print object on/off` | 打印 C++ 对象的派生类型 |
| `print pretty on/off` | 结构体显示：紧凑或缩进 |
| `print union on/off` | 联合体成员的显示 |
| `print vtbl on/off` | C++ 虚函数表的显示 |

> **补充说明**：
> - `set print pretty on` 可以让结构体以缩进格式显示，更易阅读。
> - `set print array on` 可以让数组元素逐行显示。
> - `set radix 16` 可以将默认输出基数改为十六进制，适合底层调试。

---

## 源文件

| 命令 | 说明 |
|------|------|
| `dir names` | 将目录名添加到源文件搜索路径前端 |
| `dir` | 清空源文件搜索路径 |
| `show dir` | 显示当前源文件搜索路径 |
| `list` | 显示接下来十行源码 |
| `list -` | 显示之前十行源码 |
| `list lines` | 显示指定行周围的源码 |

### list 行指定方式

| 格式 | 说明 |
|------|------|
| `[file:]num` | 指定文件中的行号 |
| `[file:]function` | 函数开头 |
| `+off` | 最后打印位置之后 off 行 |
| `-off` | 最后打印位置之前 off 行 |
| `*address` | 包含该地址的行 |
| `f,l` | 从第 f 行到第 l 行 |

| 命令 | 说明 |
|------|------|
| `listsize n` | `list` 命令显示的行数 |
| `info line num` | 显示源码行 num 对应的编译代码的起止地址 |
| `info source` | 显示当前源文件名 |
| `info sources` | 列出所有使用中的源文件 |
| `forw regex` | 向后搜索匹配正则表达式的源码行 |
| `rev regex` | 向前搜索匹配正则表达式的源码行 |

> **补充说明**：`list` 命令支持缩写为 `l`。在 TUI 模式下，源码会自动显示在上方窗口，不需要手动 `list`。

---

## 符号表

| 命令 | 说明 |
|------|------|
| `info address s` | 显示符号 s 的存储位置 |
| `info func [regex]` | 显示已定义函数的名称和类型（可匹配正则） |
| `info var [regex]` | 显示全局变量的名称和类型（可匹配正则） |
| `whatis [expr]` | 显示表达式（或 `$`）的类型，不深入 |
| `ptype [expr]` | 显示表达式的类型，更详细 |
| `ptype type` | 描述类型、结构体、联合体或枚举 |

> **补充说明**：`ptype` 比 `whatis` 更详细。例如对于结构体，`whatis` 只显示 `struct MyStruct`，而 `ptype` 会列出所有成员。调试复杂数据结构时，`ptype` 非常有用。

---

## GDB 脚本

| 命令 | 说明 |
|------|------|
| `source script` | 从文件读取并执行 GDB 命令 |
| `define cmd` | 创建新的 GDB 命令 cmd |
| `end` | 命令列表结束 |
| `document cmd` | 为新命令创建在线帮助文档 |
| `end` | 帮助文本结束 |

> **补充说明**：GDB 脚本可以实现自动化调试流程。例如创建一个 `.gdb` 文件，其中包含一系列断点设置和显示命令，然后用 `source debug.gdb` 加载。也可以在 `define` 中定义自定义命令，如：

```
define bp_and_print
    break $arg0
    commands
      print $arg1
      continue
    end
end
```

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
| `h exp off/on` | 禁用/启用 readline 历史扩展 |
| `h file filename` | 记录 GDB 命令历史的文件 |
| `h size size` | 保留的历史命令数量 |
| `h save off/on` | 控制是否使用外部文件保存历史 |

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

> **补充说明**：调试信号相关问题时，`handle` 命令至关重要。例如 `handle SIGPIPE nostop noprint pass` 可以让 GDB 不在 SIGPIPE 信号处停止，但仍让程序处理该信号。默认情况下，GDB 会拦截大多数信号。

---

## 工作文件

| 命令 | 说明 |
|------|------|
| `file [file]` | 同时使用文件作为符号表和可执行文件；不带参数则丢弃两者 |
| `core [file]` | 将文件作为 core dump 读取；或不带参数丢弃 |
| `exec [file]` | 仅将文件作为可执行文件；或不带参数丢弃 |
| `symbol [file]` | 从文件读取符号表；或不带参数丢弃 |
| `load file` | 动态链接文件并添加其符号 |
| `add-sym file addr` | 从动态加载的文件中读取额外符号（指定地址） |
| `info files` | 显示当前使用的工作文件和目标 |
| `path dirs` | 将目录添加到可执行文件和符号文件的搜索路径前端 |
| `show path` | 显示可执行文件和符号文件的搜索路径 |
| `info share` | 列出当前加载的共享库名称 |

> **补充说明**：`file` 命令可以在 GDB 运行中切换调试目标程序。调试动态加载的共享库时，`info share` 可以查看哪些库已被加载。

---

## 调试目标

| 命令 | 说明 |
|------|------|
| `target type param` | 连接到目标机器、进程或文件 |
| `help target` | 显示可用的目标类型 |
| `attach param` | 连接到另一个进程 |
| `detach` | 从 GDB 控制中释放目标 |

> **补充说明**：`target` 命令用于远程调试。例如 `target remote localhost:1234` 可以连接到在 1234 端口等待的远程调试存根。`attach` 用于调试已运行的进程，`detach` 用于分离而不终止进程。

---

## 在 GNU Emacs 中使用 GDB

| 快捷键 | 说明 |
|--------|------|
| `M-s` | 单步执行一行（step） |
| `M-n` | 单步执行一行（next） |
| `M-i` | 单步执行一条指令（stepi） |
| `M-c` | 继续运行（cont） |
| `C-c C-f` | 完成当前栈帧（finish） |
| `M-u` | 向上移动 arg 层栈帧（up） |
| `M-d` | 向下移动 arg 层栈帧（down） |
| `C-x &` | 从光标处复制数字，插入到行尾 |
| `C-x SPC` | （在源文件中）在光标处设置断点 |

> **补充说明**：Emacs 的 GDB 模式（`M-x gdb`）提供了图形化的调试界面，可以在源码窗口中直接设置断点、查看变量，比纯命令行更直观。

---

## 反汇编

| 命令 | 说明 |
|------|------|
| `disassem [addr]` | 将内存显示为机器指令 |

> **补充说明**：`disassem` 可以简写为 `disas`。常用方式：
> - `disas` 反汇编当前函数
> - `disas 0xaddr1,0xaddr2` 反汇编指定地址范围
> - `disas /m func_name` 混合显示源码和汇编（GDB 7.0+）

---

## 许可证

| 命令 | 说明 |
|------|------|
| `show copying` | 显示 GNU 通用公共许可证 |
| `show warranty` | 显示无担保声明 |

GDB 是自由软件，欢迎在 GNU 通用公共许可证条款下分发副本。GDB 不提供任何担保。

---

## 常用调试技巧补充

### 1. 多线程调试

```gdb
info threads          # 列出所有线程
thread n              # 切换到线程 n
thread apply all bt   # 显示所有线程的调用栈
set scheduler-locking on  # 锁定只运行当前线程
```

### 2. 远程调试

```gdb
# 目标机器上启动 gdbserver
gdbserver :1234 program

# 本地 GDB 连接
target remote host:1234
```

### 3. 调试 fork 的子进程

```gdb
set follow-fork-mode child   # 跟踪子进程
set follow-fork-mode parent  # 跟踪父进程（默认）
```

### 4. 反向调试（GDB 7.0+）

```gdb
record          # 开始记录执行历史
reverse-step    # 反向单步
reverse-next    # 反向步过
reverse-continue # 反向继续
```

### 5. 查看内存映射

```gdb
info proc mappings   # 显示进程的内存映射
```
