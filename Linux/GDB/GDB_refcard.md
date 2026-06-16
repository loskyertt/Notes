---
title: GDB 参考卡
date: 2026-06-16
tags:
  - GDB
  - Linux
  - 调试
  - C/C++
aliases:
  - gdb-refcard
  - gdb-cheatsheet
---

# 1. 基本命令速查

| 命令 | 说明 | 示例 |
|------|------|------|
| `gdb program [core]` | 调试程序 [使用核心转储文件] | `gdb ./myapp core.1234` |
| `run [arglist]` | 启动程序 [带参数列表] | `run --verbose input.txt` |
| `bt` | 回溯：显示程序调用栈 | `bt` |
| `p expr` | 显示表达式的值 | `p *ptr@10` |
| `c` | 继续运行程序 | `c` |
| `n` | 下一行，跳过函数调用 | `n` |
| `s` | 下一行，进入函数调用 | `s` |

> [!tip] 记忆技巧
> `n` = **n**ext（跳过），`s` = **s**tep（进入），`c` = **c**ontinue（继续），`bt` = **b**ack**t**race（回溯）。

---

# 2. 启动 GDB

| 命令 | 说明 | 示例 |
|------|------|------|
| `gdb` | 启动 GDB，不加载调试文件 | `gdb` |
| `gdb program` | 开始调试 program | `gdb ./myapp` |
| `gdb program core` | 调试 program，使用核心转储文件 core | `gdb ./myapp core.1234` |
| `gdb --pid 1234` | 直接附加到运行中的进程 | `gdb --pid 1234` |
| `gdb --args program arg1 arg2` | 在 `--args` 后指定程序及其参数 | `gdb --args ./myapp -v --output=result.txt` |
| `gdb -tui program` | 启动内置文本用户界面（TUI 模式） | `gdb -tui ./myapp` |

> [!info] 现代替代说明
> - `gdb --pid`（或 `-p`）替代旧版 `gdb program 1234`，更明确、更安全，避免将 PID 误认为核心文件
> - `gdb --args` 替代旧版在 `run` 中传参的方式，让参数传递更清晰，避免 shell 引号解析问题
> - `gdb -tui` 启动内置 TUI 模式，在终端内提供类似 IDE 的源码/汇编/寄存器分屏视图

---

# 3. 停止 GDB

| 命令 | 说明 | 示例 |
|------|------|------|
| `quit` | 退出 GDB（也可用 `q` 或 `Ctrl+D`） | `quit` |
| `Ctrl+C` | 终止当前命令，或向运行中的进程发送中断信号 | 运行中按 `Ctrl+C` |

---

# 4. 获取帮助

| 命令 | 说明 | 示例 |
|------|------|------|
| `help` | 列出命令分类 | `help` |
| `help class` | 列出某分类下命令的单行描述 | `help breakpoints` |
| `help command` | 描述指定命令 | `help break` |
| `apropos keyword` | 搜索与关键词相关的所有命令和文档 | `apropos thread` |
| `info gdb` | 在终端中查看 GDB 完整手册（info 格式） | `info gdb` |

> [!tip] 记忆技巧
> `apropos` 比逐类 `help` 浏览更高效，支持模糊搜索，是发现新命令的最佳方式。

---

# 5. 运行程序

## 5.1 启动与参数

| 命令 | 说明 | 示例 |
|------|------|------|
| `run arglist` | 使用参数列表启动程序 | `run -c config.ini --debug` |
| `run` | 使用当前参数启动程序 | `run` |
| `run ... <inf >outf` | 启动程序并重定向输入/输出 | `run < input.txt > output.txt` |
| `kill` | 终止运行中的程序 | `kill` |
| `tty dev` | 使用 dev 作为下次运行的 stdin 和 stdout | `tty /dev/pts/2` |
| `start` | 在 `main` 函数处设置临时断点并运行 | `start` |

> [!tip] `start` 命令
> `start` 相当于 `break main` + `run` 的快捷方式，省去两步操作。

## 5.2 参数与环境变量

| 命令 | 说明 | 示例 |
|------|------|------|
| `set args arglist` | 为下次运行指定参数列表 | `set args -v input.dat` |
| `set args` | 指定空参数列表 | `set args` |
| `show args` | 显示参数列表 | `show args` |
| `set env var string` | 设置环境变量 | `set env LD_LIBRARY_PATH /usr/local/lib` |
| `show env` | 显示所有环境变量 | `show env` |
| `show env var` | 显示指定环境变量 | `show env HOME` |
| `unset env var` | 从环境中移除变量 | `unset env MY_FLAG` |

---

# 6. Shell 命令

| 命令 | 说明 | 示例 |
|------|------|------|
| `cd dir` | 将工作目录切换到 dir | `cd /tmp/test` |
| `pwd` | 显示当前工作目录 | `pwd` |
| `make ...` | 调用 `make` 构建程序 | `make clean && make` |
| `shell cmd` | 执行任意 shell 命令 | `shell ls -la /tmp/core*` |

---

# 7. 断点与观察点

## 7.1 设置断点

| 命令 | 说明 | 示例 |
|------|------|------|
| `break [file:]line` | 在指定行号设置断点 | `break main.c:42` |
| `b [file:]line` | 同上缩写 | `b utils.c:15` |
| `break [file:]func` | 在函数入口设置断点 | `break parse_args` |
| `b [file:]function` | 同上缩写 | `b mylib::init` |
| `break +offset` | 在当前位置偏移 offset 行处设置断点 | `break +5` |
| `break *addr` | 在地址 addr 处设置断点 | `break *0x4005d6` |
| `break` | 在下一条指令处设置断点 | `break` |
| `break ... if expr` | 条件断点，当 expr 非零时中断 | `break loop.c:20 if i == 100` |
| `tbreak ...` | 临时断点；命中后自动禁用 | `tbreak main` |
| `rbreak [file:]regex` | 在所有匹配正则的函数处设置断点 | `rbreak file.c:^handle_` |
| `hbreak` | 设置硬件断点 | `hbreak *0x7ffff7a1b000` |
| `thbreak` | 临时硬件断点，命中后自动删除 | `thbreak *0x4005d6` |
| `dprintf location,fmt,args` | 动态 printf，不修改源码插入格式化输出 | `dprintf main.c:42,"x=%d\\n",x` |

> [!warning] 硬件断点
> `hbreak` 适用于调试 ROM 代码或断点数量超出软件断点限制的场景。硬件断点数量有限（通常 4 个），请谨慎使用。

## 7.2 观察点

| 命令 | 说明 | 示例 |
|------|------|------|
| `watch expr` | 为表达式设置写观察点 | `watch global_counter` |
| `rwatch expr` | 为表达式设置读观察点 | `rwatch config->mode` |
| `awatch expr` | 为表达式设置读写观察点 | `awatch shared_data->flag` |
| `info watch` | 显示已定义的观察点 | `info watch` |

> [!tip] 观察点用途
> - `watch`：当变量**被修改**时中断 → 追踪"谁改了这个变量"
> - `rwatch`：当变量**被读取**时中断 → 追踪"谁读了这个变量"
> - `awatch`：当变量**被读取或修改**时中断 → 追踪"谁碰了这个变量"

## 7.3 捕获事件

| 命令 | 说明 | 示例 |
|------|------|------|
| `catch event` | 捕获事件断点 | `catch throw` |
| `catch throw` | C++ 异常抛出时中断 | `catch throw` |
| `catch catch` | C++ 异常捕获时中断 | `catch catch` |
| `catch exec` | exec 调用时中断 | `catch exec` |
| `catch fork` | fork 调用时中断 | `catch fork` |
| `catch vfork` | vfork 调用时中断 | `catch vfork` |
| `catch load` | 共享库加载时中断 | `catch load` |
| `catch unload` | 共享库卸载时中断 | `catch unload` |

## 7.4 查看断点

| 命令 | 说明 | 示例 |
|------|------|------|
| `info break` | 显示已定义的断点 | `info break` |
| `info break n` | 显示指定断点详情 | `info break 3` |

---

# 8. 断点管理

| 命令 | 说明 | 示例 |
|------|------|------|
| `cond n [expr]` | 在断点 n 上设置条件；不带 expr 则取消条件 | `cond 2 x > 0` / `cond 2` |
| `clear` | 删除下一条指令处的断点 | `clear` |
| `clear [file:]fun` | 删除函数入口处的断点 | `clear parse_args` |
| `clear [file:]line` | 删除源码行上的断点 | `clear main.c:42` |
| `delete [n]` | 删除断点 [或指定断点 n] | `delete 1 3` / `delete`（删除全部） |
| `disable [n]` | 禁用断点 | `disable 2` |
| `enable [n]` | 启用断点 | `enable 2` |
| `enable once [n]` | 启用断点 n；命中后自动禁用 | `enable once 3` |
| `enable del [n]` | 启用断点 n；命中后自动删除 | `enable del 4` |
| `ignore n count` | 忽略断点 n 接下来 count 次命中 | `ignore 1 99`（前 99 次不中断） |
| `commands n` | 每当断点 n 命中时执行 GDB 命令列表 | 见下方示例 |
| `end` | 命令列表结束标记 | — |

**`commands` 使用示例**：

```gdb
(gdb) break loop.c:20 if i == 50
(gdb) commands 1
Type commands for breakpoint(s) 1, one per line.
End with a line saying just "end".
>silent
>printf "i=%d, sum=%d\n", i, sum
>continue
>end
```

> [!tip] `commands` vs `dprintf`
> - `dprintf` 更简洁，适合简单的打印需求：`dprintf main.c:42,"x=%d\n",x`
> - `commands` 更灵活，适合复杂的多步骤操作（如条件判断、修改变量等）

---

# 9. 执行控制

| 命令 | 说明 | 示例 |
|------|------|------|
| `continue [count]` | 继续运行；若指定 count，则忽略此断点接下来 count 次 | `continue 5` |
| `c [count]` | 同上缩写 | `c 10` |
| `step [count]` | 单步执行（进入函数）；指定 count 则重复 count 次 | `step 3` |
| `s [count]` | 同上缩写 | `s 2` |
| `stepi [count]` | 按机器指令单步执行 | `stepi` |
| `si [count]` | 同上缩写 | `si` |
| `next [count]` | 下一行（跳过函数调用） | `next 5` |
| `n [count]` | 同上缩写 | `n 3` |
| `nexti [count]` | 下一条机器指令 | `nexti` |
| `ni [count]` | 同上缩写 | `ni` |
| `until [location]` | 运行直到下一条指令（或指定位置），常用于跳出循环 | `until 50` / `until loop.c:50` |
| `finish` | 运行直到当前栈帧返回 | `finish` |
| `return [expr]` | 从当前栈帧直接返回 [带返回值]，不执行剩余代码 | `return 0` |
| `signal num` | 以信号 num 恢复执行（0 表示无信号） | `signal 2`（发送 SIGINT） |
| `jump line` | 在指定行号处恢复执行 | `jump 30` |
| `jump *address` | 在指定地址处恢复执行 | `jump *0x4005d6` |
| `set var=expr` | 修改程序变量 | `set x = 42` |

**常用场景示例**：

```gdb
# 跳出当前循环
(gdb) until loop.c:50

# 强制函数提前返回
(gdb) return -1

# 修改变量后继续
(gdb) set i = 99
(gdb) continue
```

> [!warning] `jump` 与 `return` 的风险
> - `jump` 跳过中间代码，可能导致程序状态不一致
> - `return` 跳过函数剩余代码，栈上局部变量不会被正确清理
> - 两者仅用于调试，切勿期望程序继续正常运行

## 9.1 现代执行控制补充

| 命令 | 说明 | 示例 |
|------|------|------|
| `advance location` | 继续运行直到到达指定位置 | `advance parse_args` |
| `reverse-continue` | 反向继续执行（需 `record` 支持） | `reverse-continue` |
| `reverse-step` | 反向单步（进入函数） | `reverse-step` |
| `reverse-next` | 反向下一行（跳过函数） | `reverse-next` |
| `record` | 开启执行记录（基于 btrace，性能开销小） | `record` |
| `record full` | 开启完整执行记录（记录所有内存和寄存器变化） | `record full` |

**反向调试示例**：

```gdb
(gdb) break main
(gdb) run
(gdb) record          # 开启执行记录
(gdb) continue        # 让程序运行一段时间
(gdb) reverse-step    # 回退一步
(gdb) reverse-continue # 反向运行到上一个断点
```

> [!info] 反向调试
> 反向执行需要进程记录支持（GDB 7.0+），通过 `record` 命令开启。特别适合调试偶发 bug：先正常执行到出错点，再反向逐步回溯定位原因。

---

# 10. 程序栈

| 命令 | 说明 | 示例 |
|------|------|------|
| `backtrace [n]` | 打印栈中所有帧的回溯；或 n 帧 | `backtrace` / `bt 3`（最内 3 帧） |
| `bt [n]` | 同上缩写 | `bt -2`（最外 2 帧） |
| `bt full` | 打印回溯时同时显示每个栈帧的局部变量值 | `bt full` |
| `frame [n]` | 选择第 n 帧；无参数则显示当前帧 | `frame 2` |
| `up n` | 向上选择 n 帧（向调用者方向） | `up 2` |
| `down n` | 向下选择 n 帧（向被调用者方向） | `down 1` |
| `info frame [addr]` | 描述选中的帧（或 addr 处的帧） | `info frame` |
| `frame apply all cmd` | 对所有栈帧执行指定 GDB 命令 | `frame apply all info locals` |
| `select-frame n` | 静默切换栈帧（不打印帧信息） | `select-frame 3` |

**调用栈分析示例**：

```gdb
(gdb) bt
#0  parse_args (argc=3, argv=0x7fffffffe3a8) at main.c:42
#1  0x0000000000400586 in main (argc=3, argv=0x7fffffffe3a8) at main.c:15
(gdb) frame 1
#1  0x0000000000400586 in main (argc=3, argv=0x7fffffffe3a8) at main.c:15
15          parse_args(argc, argv);
(gdb) info locals
config = {mode = 0, verbose = false}
```

> [!tip] `bt full` 的效率
> `bt full` 一次性显示所有帧的局部变量，比单独 `bt` + 逐帧 `info locals` 效率更高。

---

# 11. 显示

## 11.1 print 命令

| 命令 | 说明 | 示例 |
|------|------|------|
| `print [/f] [expr]` | 按格式 f 显示表达式 expr 的值 | `print/x $rax` |
| `p [/f] [expr]` | 同上缩写 | `p *head` |

**常用打印示例**：

```gdb
(gdb) p x              # 打印变量 x
$1 = 42
(gdb) p/x x            # 十六进制打印
$2 = 0x2a
(gdb) p *arr@10        # 打印数组 arr 的前 10 个元素
$3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
(gdb) p/t flags        # 二进制打印
$4 = 10110
(gdb) p str            # 打印字符串
$5 = 0x400684 "hello world"
(gdb) p node->next->data  # 链表遍历
$6 = 7
```

## 11.2 打印格式

| 格式符 | 说明 | 示例 |
|--------|------|------|
| `x` | 十六进制 | `p/x 255` → `0xff` |
| `d` | 有符号十进制 | `p/d 0xff` → `-1`（若为 char） |
| `u` | 无符号十进制 | `p/u -1` → `4294967295` |
| `o` | 八进制 | `p/o 8` → `010` |
| `t` | 二进制 | `p/t 6` → `110` |
| `a` | 地址（绝对和相对） | `p/a 0x4005d6` → `0x4005d6 <main+20>` |
| `c` | 字符 | `p/c 65` → `65 'A'` |
| `f` | 浮点数 | `p/f 0x40490000` → `3.1400001` |

## 11.3 call 与内存检查

| 命令 | 说明 | 示例 |
|------|------|------|
| `call [/f] expr` | 类似 `print`，但不显示 `void` 返回值 | `call printf("x=%d\n", x)` |
| `x [/Nuf] expr` | 检查地址 expr 处的内存 | `x/10xw 0x7fffffffe300` |
| `disassem [addr]` | 将内存显示为机器指令 | `disassem main` |

**`x` 命令示例**：

```gdb
(gdb) x/10xw $rsp             # 查看栈上 10 个字（十六进制）
0x7fffffffe300: 0x00000001    0x00007fff    0x00000002    0x00400586
0x7fffffffe310: 0x00000003    0x00007fff    0x00000004    0x00000000
0x7fffffffe320: 0x00000005    0x00000000

(gdb) x/20i main              # 查看 main 函数的前 20 条指令
0x400572 <main>:    push   %rbp
0x400573 <main+1>:  mov    %rsp,%rbp

(gdb) x/s 0x400684            # 查看地址处的字符串
0x400684:       "hello world"
```

### 内存检查格式 `/Nuf`

| 字段 | 说明 | 示例 |
|------|------|------|
| `N` | 显示单元数量 | `10` → 显示 10 个单元 |
| `u` | 单元大小：`b`=字节，`h`=半字(2B)，`w`=字(4B)，`g`=巨字(8B) | `x/4gw` → 4 个 8 字节单元 |
| `f` | 打印格式（同 `print` 格式，或 `s`=字符串，`i`=指令） | `x/10i` → 10 条指令 |

> [!tip] `x` 命令记忆
> 格式 `/Nuf`：**N** 个 **u** 大小的单元，按 **f** 格式显示。例如 `x/16xb buf` = 以十六进制查看 buf 的前 16 字节。

## 11.4 现代显示补充

| 命令 | 说明 | 示例 |
|------|------|------|
| `parray len arr` | 以美观格式打印数组（GDB 10+） | `parray 10 arr` |

---

# 12. 自动显示

| 命令 | 说明 | 示例 |
|------|------|------|
| `display [/f] expr` | 每次程序停止时自动显示 expr 的值 | `display/x i` |
| `display` | 显示所有已启用的自动显示表达式 | `display` |
| `undisplay n` | 从自动显示列表中移除编号 n | `undisplay 1` |
| `disable disp n` | 禁用编号 n 的自动显示 | `disable disp 2` |
| `enable disp n` | 启用编号 n 的自动显示 | `enable disp 2` |
| `info display` | 列出自动显示表达式的编号列表 | `info display` |

**自动显示示例**：

```gdb
(gdb) display/x i          # 每次停止时自动以十六进制显示 i
1: x/i = 0x0
(gdb) display *head        # 每次停止时自动显示链表头节点
2: *head = {data = 1, next = 0x602010}
(gdb) n
1: x/i = 0x1
2: *head = {data = 1, next = 0x602010}
```

> [!tip] 自动显示用途
> 在循环调试中，`display` 可以让你每次单步时自动看到关键变量的值，无需反复手动 `p`。

---

# 13. 表达式

## 13.1 表达式语法

| 表达式 | 说明 | 示例 |
|--------|------|------|
| `expr` | C、C++ 或 Modula-2 表达式 | `p x + y * 2` |
| `addr@len` | 从地址 addr 开始、长度为 len 的数组 | `p *arr@10` |
| `file::nm` | 文件 file 中定义的变量或函数 nm | `p mylib.c::global_var` |
| `{type}addr` | 以指定类型读取地址 addr 处的内存 | `p {int}0x7fffffffe300` |

**表达式使用示例**：

```gdb
(gdb) p *arr@5                  # 打印数组前 5 个元素
$1 = {10, 20, 30, 40, 50}

(gdb) p mylib.c::counter        # 访问指定文件中的全局变量
$2 = 42

(gdb) p {char [10]}0x602010     # 以 char[10] 类型读取内存
$3 = "hello"

(gdb) p strcmp(s1, s2)          # 调用函数
$4 = 0
```

## 13.2 便利变量

| 变量 | 说明 | 示例 |
|------|------|------|
| `$` | 最近一次显示的值 | `p $` → 重复上一次结果 |
| `$n` | 第 n 次显示的值 | `p $1` → 显示第 1 次结果 |
| `$$` | $ 之前显示的值 | `p $$` |
| `$$n` | 往前第 n 次显示的值 | `p $$2` |
| `$_` | 用 `x` 检查的最后一个地址 | `p $_` |
| `$__` | 地址 `$_` 处的值 | `p $__` |
| `$var` | 便利变量；可赋任意值 | `set $idx = 0` |
| `$_exitcode` | 程序退出时的退出码 | `p $_exitcode` |
| `$_thread` | 当前线程编号 | `p $_thread` |

| 命令 | 说明 | 示例 |
|------|------|------|
| `show values [n]` | 显示最近 10 个值 [或 $n 附近的值] | `show values 5` |
| `show conv` | 显示所有便利变量 | `show conv` |

**便利变量使用示例**：

```gdb
(gdb) p head->next
$1 = (struct node *) 0x602010
(gdb) set $node = $1           # 保存指针到便利变量
(gdb) p $node->data
$2 = 7
(gdb) p $node->next->data
$3 = 13

# 循环遍历链表
(gdb) set $cur = head
(gdb) p $cur->data
$4 = 1
(gdb) set $cur = $cur->next
(gdb) p $cur->data
$5 = 7
```

---

# 14. 控制 GDB

## 14.1 通用参数

| 命令 | 说明 | 示例 |
|------|------|------|
| `set param value` | 设置 GDB 内部参数 | `set pagination off` |
| `show param` | 显示参数当前设置 | `show pagination` |

## 14.2 主要参数列表

| 参数 | 说明 | 示例 |
|------|------|------|
| `confirm on/off` | 启用或禁用确认提示 | `set confirm off` |
| `editing on/off` | 控制 readline 命令行编辑 | `set editing on` |
| `height lpp` | 显示暂停前的行数 | `set height 0`（禁用分页） |
| `language lang` | GDB 表达式语言 | `set language c` |
| `listsize n` | `list` 命令显示的行数 | `set listsize 20` |
| `prompt str` | 使用 str 作为 GDB 提示符 | `set prompt (mygdb) ` |
| `radix base` | 数值的进制表示 | `set radix 16` |
| `verbose on/off` | 控制加载符号时的消息 | `set verbose off` |
| `width cpl` | 折行前的字符数 | `set width 200` |
| `write on/off` | 允许或禁止修补二进制文件 | `set write on` |

## 14.3 现代参数补充

| 参数 | 说明 | 示例 |
|------|------|------|
| `pagination on/off` | 控制分页（替代 `set height`） | `set pagination off` |
| `print pretty on` | 美化结构体输出 | `set print pretty on` |
| `print object on` | 显示 C++ 对象的实际（动态）类型 | `set print object on` |
| `print array on` | 数组美化输出 | `set print array on` |
| `print elements 0` | 取消数组/字符串显示长度限制 | `set print elements 0` |
| `style enabled on` | 启用终端样式（颜色高亮，GDB 9+） | `set style enabled on` |

**输出美化对比示例**：

```gdb
# 默认输出
(gdb) p my_struct
$1 = {name = "hello", value = 42, next = 0x602010}

# 美化输出
(gdb) set print pretty on
(gdb) p my_struct
$2 = {
  name = "hello",
  value = 42,
  next = 0x602010
}
```

---

# 15. 源文件

## 15.1 源文件搜索路径

| 命令 | 说明 | 示例 |
|------|------|------|
| `dir names` | 将目录名添加到源文件搜索路径前端 | `dir /home/user/project/src` |
| `dir` | 清除源文件搜索路径 | `dir` |
| `show dir` | 显示当前源文件搜索路径 | `show dir` |

## 15.2 查看源码

| 命令 | 说明 | 示例 |
|------|------|------|
| `list` | 显示接下来 10 行源码 | `list` |
| `list -` | 显示之前 10 行源码 | `list -` |
| `list lines` | 显示指定行号周围的源码 | `list main.c:42` |
| `info line num` | 显示源码行对应的编译代码起止地址 | `info line 42` |
| `info source` | 显示当前源文件名 | `info source` |
| `info sources` | 列出所有使用中的源文件 | `info sources` |

### `list` 行号指定方式

| 格式 | 说明 | 示例 |
|------|------|------|
| `[file:]num` | 行号 [在指定文件中] | `list main.c:42` |
| `[file:]function` | 函数开头 [在指定文件中] | `list parse_args` |
| `+off` | 上次打印位置之后 off 行 | `list +20` |
| `-off` | 上次打印位置之前 off 行 | `list -10` |
| `*address` | 包含地址的行 | `list *0x400586` |
| `f,l` | 从第 f 行到第 l 行 | `list 10,30` |

## 15.3 源码搜索

| 命令 | 说明 | 示例 |
|------|------|------|
| `forward-search regex` | 向前搜索匹配正则的源码行 | `forward-search while` |
| `reverse-search regex` | 向后搜索匹配正则的源码行 | `reverse-search return` |

> [!tip] TUI 模式
> 通过 `gdb -tui` 或在 GDB 中按 `Ctrl+X A` 启动 TUI 模式，提供分屏源码浏览：
> - `Ctrl+X 2`：分割窗口（源码+汇编/寄存器）
> - `Ctrl+L`：刷新屏幕
> - `/`：在 TUI 中搜索
>
> 完全替代逐行 `list` 的低效方式。

---

# 16. 符号表

| 命令 | 说明 | 示例 |
|------|------|------|
| `info address s` | 显示符号 s 存储的位置 | `info address global_var` |
| `info func [regex]` | 显示已定义函数的名称和类型 | `info func ^parse` |
| `info var [regex]` | 显示全局变量的名称和类型 | `info var ^config` |
| `whatis [expr]` | 显示表达式的类型（不深入展开） | `whatis ptr` → `type = int *` |
| `ptype [expr]` | 显示表达式的详细类型定义 | `ptype node_t` |
| `ptype /o type` | 输出结构体的偏移量信息 | `ptype /o struct config` |
| `ptype /mtt type` | 显示类型的方法、虚表、类型信息（C++） | `ptype /mtt MyClass` |
| `info types [regex]` | 按正则表达式搜索类型定义 | `info types ^node` |

**符号表查询示例**：

```gdb
# 查看变量类型
(gdb) whatis head
type = struct node *

# 查看结构体详细定义
(gdb) ptype struct node
type = struct node {
    int data;
    struct node *next;
}

# 查看结构体内存布局（偏移量）
(gdb) ptype /o struct config
/* offset      |    size */  type = struct config {
/*      0      |       4 */    int mode;
/*      4      |       1 */    _Bool verbose;
/* XXX  3-byte hole      */
/*      8      |       8 */    char *filename;
/*     16      |       4 */    int count;
/*     20      |       4 */    int padding;
/* total size (bytes):   24 */
}

# 搜索所有以 handle_ 开头的函数
(gdb) info func ^handle_
All functions matching regular expression "^handle_":

File handler.c:
void handle_request(struct request *);
void handle_error(int);
```

> [!tip] `ptype /o` 的价值
> `ptype /o` 显示结构体成员的偏移量和大小，包括填充字节（hole），对理解内存布局和对齐问题极为有用。

---

# 17. GDB 脚本

## 17.1 基本脚本命令

| 命令 | 说明 | 示例 |
|------|------|------|
| `source script` | 从文件读取并执行 GDB 命令 | `source my_macros.gdb` |
| `define cmd` | 创建新的 GDB 命令 | 见下方示例 |
| `end` | 命令列表结束标记 | — |
| `document cmd` | 为新命令创建在线帮助文档 | 见下方示例 |
| `show commands` | 显示最近 10 条命令 | `show commands` |
| `show commands n` | 显示编号 n 附近的 10 条命令 | `show commands 50` |
| `show commands +` | 显示接下来 10 条命令 | `show commands +` |

**自定义命令示例**：

```gdb
# 定义一个打印链表的命令
(gdb) define print_list
Type commands for definition of "print_list", one per line.
End with a line saying just "end".
>set $cur = $arg0
>while $cur != 0
 >printf "node@%p: data=%d\n", $cur, $cur->data
 >set $cur = $cur->next
>end
>end

# 为命令添加帮助文档
(gdb) document print_list
Print all nodes in a linked list starting from the given head pointer.
Usage: print_list <head_pointer>
end

# 使用自定义命令
(gdb) print_list head
node@0x602000: data=1
node@0x602010: data=7
node@0x602020: data=13
```

## 17.2 Python 脚本

GDB 7.0+ 内置 Python 解释器，远超旧版 `define` 脚本能力。

**Python 脚本示例**：

```gdb
# 在 GDB 中直接执行 Python 代码
(gdb) python print("Hello from Python")

# 使用 GDB Python API
(gdb) python
>frame = gdb.selected_frame()
>print("Current function:", frame.name())
>print("Current line:", frame.find_sal().line)
>end

# 从文件加载 Python 脚本
(gdb) source my_script.py
```

## 17.3 .gdbinit 文件

GDB 启动时自动加载的配置文件，可放置常用设置和自定义命令。

**`.gdbinit` 示例**：

```gdb
# ~/.gdbinit
set print pretty on
set print array on
set pagination off
set history save on
set history filename ~/.gdb_history

# 允许项目级 .gdbinit 自动加载
set auto-load safe-path /

# 常用别名
define cls
    shell clear
end
```

---

# 18. 历史记录

| 命令 | 说明 | 示例 |
|------|------|------|
| `set history expansion on/off` | 启用/禁用历史扩展 | `set history expansion on` |
| `set history filename file` | 设置历史文件 | `set history filename ~/.gdb_history` |
| `set history size n` | 设置历史大小 | `set history size 10000` |
| `set history save on/off` | 控制是否保存到文件 | `set history save on` |

> [!info] 旧版写法对照
> 旧版参考卡中使用 `h exp off/on`、`h file filename` 等写法，现代 GDB 推荐使用 `set history ...` 全称形式。

---

# 19. 信号处理

| 命令 | 说明 | 示例 |
|------|------|------|
| `handle signal act` | 指定 GDB 对信号的处理动作 | `handle SIGUSR1 nostop pass` |
| `info signals` | 显示信号表及 GDB 对各信号的处理动作 | `info signals` |
| `info handle` | 与 `info signals` 等价，名称更直观 | `info handle` |

### 处理动作

| 动作 | 说明 | 示例 |
|------|------|------|
| `print` | 通知信号 | `handle SIGPIPE print` |
| `nostop` | 不停止执行 | `handle SIGPIPE nostop` |
| `stop` | 停止执行 | `handle SIGSEGV stop` |
| `pass` | 允许程序处理信号 | `handle SIGUSR1 pass` |
| `nopass` | 不允许程序看到信号 | `handle SIGUSR1 nopass` |

**信号处理示例**：

```gdb
# 不中断调试但允许程序接收 SIGUSR1
(gdb) handle SIGUSR1 nostop pass

# 忽略 SIGPIPE（常见于网络编程）
(gdb) handle SIGPIPE nostop nopass

# 对 SIGSEGV 停止并通知
(gdb) handle SIGSEGV stop print

# 查看所有信号处理设置
(gdb) info signals
```

> [!tip] 常见用法
> `handle SIGUSR1 nostop pass` —— 不中断调试但允许程序接收信号，这是最常见的用法模式。

---

# 20. 在编辑器中使用 GDB

## 20.1 GNU Emacs

| 快捷键 | 说明 | 示例 |
|--------|------|------|
| `M-x gdb` | 在 Emacs 中启动 GDB | `M-x gdb RET ./myapp RET` |
| `M-s` | 单步执行（step） | 在源码缓冲区按 `M-s` |
| `M-n` | 下一行（next） | 在源码缓冲区按 `M-n` |
| `M-i` | 下一指令（stepi） | 在源码缓冲区按 `M-i` |
| `C-c C-f` | 完成当前栈帧（finish） | 在源码缓冲区按 `C-c C-f` |
| `M-c` | 继续（cont） | 在源码缓冲区按 `M-c` |
| `M-u` | 向上 n 帧（up） | 在源码缓冲区按 `M-u` |
| `M-d` | 向下 n 帧（down） | 在源码缓冲区按 `M-d` |
| `C-x &` | 从光标处复制数字，插入到命令末尾 | 在源码缓冲区按 `C-x &` |
| `C-x SPC` | 在光标位置设置断点 | 在源码行上按 `C-x SPC` |

## 20.2 现代替代方案

| 工具 | 说明 | 适用场景 |
|------|------|----------|
| **VS Code + CodeLLDB** | 图形化断点管理、变量监视、调用栈可视化 | 日常开发调试 |
| **Neovim + nvim-dap** | Debug Adapter Protocol 插件 | Neovim 用户 |
| **GDB TUI 模式** | 内置终端界面（`gdb -tui`） | 无 GUI 环境 |
| **GDB Dashboard** | 第三方 Python 脚本，更美观的实时调试界面 | 增强终端体验 |
| **GEF / pwndbg / peda** | 安全研究和逆向工程增强插件 | 安全研究、CTF |

---

# 21. 工作文件

| 命令 | 说明 | 示例 |
|------|------|------|
| `file [file]` | 使用 file 同时作为符号和可执行文件 | `file ./myapp` |
| `core [file]` | 读取 file 作为核心转储文件 | `core core.1234` |
| `exec [file]` | 仅使用 file 作为可执行文件 | `exec ./myapp` |
| `symbol [file]` | 使用 file 的符号表 | `symbol ./myapp.debug` |
| `load file` | 动态链接文件并添加其符号 | `load /usr/lib/libmylib.so` |
| `add-sym file addr` | 从文件读取额外符号，该文件在地址 addr 处动态加载 | `add-sym mylib.so 0x7ffff7dc0000` |
| `info files` | 显示使用中的工作文件和目标 | `info files` |
| `set debug-file-directory dirs` | 设置 debuginfo 文件搜索目录 | `set debug-file-directory /usr/lib/debug` |
| `info share` | 列出当前加载的共享库名称 | `info share` |
| `set solib-search-path dirs` | 设置共享库搜索路径 | `set solib-search-path /opt/lib` |

**工作文件使用示例**：

```gdb
# 分别加载可执行文件和调试符号
(gdb) exec ./myapp
(gdb) symbol ./myapp.debug

# 分析核心转储
(gdb) file ./myapp
(gdb) core core.1234

# 查看加载的共享库
(gdb) info share
From                To                  Syms Read   Shared Object Library
0x00007ffff7dd0000  0x00007ffff7df5000  Yes         /lib64/ld-linux-x86-64.so.2
0x00007ffff7bb0000  0x00007ffff7bc5000  Yes         /lib64/libpthread.so.0
```

---

# 22. 调试目标

| 命令 | 说明 | 示例 |
|------|------|------|
| `target type param` | 连接到目标机器、进程或文件 | `target remote :1234` |
| `help target` | 显示可用的目标类型 | `help target` |
| `attach param` | 附加到另一个进程 | `attach 1234` |
| `detach` | 从 GDB 控制中释放目标 | `detach` |
| `show path` | 显示可执行和符号文件路径 | `show path` |
| `info share` | 列出当前加载的共享库名称 | `info share` |
| `path dirs` | 将 dirs 添加到搜索路径前端 | `path /opt/mylib` |

**远程调试示例**：

```gdb
# 在目标机器上启动 gdbserver
# $ gdbserver :1234 ./myapp

# 在开发机上连接
(gdb) file ./myapp
(gdb) target remote 192.168.1.100:1234
(gdb) break main
(gdb) continue
```

> [!tip] 远程调试
> `target remote :1234` 是最常用的远程调试方式。目标机器运行 `gdbserver`，开发机器通过 GDB 连接。

---

# 23. 多线程调试

| 命令 | 说明 | 示例 |
|------|------|------|
| `info threads` | 显示所有线程 | `info threads` |
| `thread n` | 切换到线程 n | `thread 3` |
| `thread apply all cmd` | 对所有线程执行命令 | `thread apply all bt` |
| `break file:line thread n` | 仅在线程 n 中设置断点 | `break worker.c:30 thread 2` |
| `set scheduler-locking on` | 锁定调度，仅当前线程执行 | `set scheduler-locking on` |
| `set scheduler-locking off` | 解除调度锁定 | `set scheduler-locking off` |
| `set scheduler-locking step` | 单步时锁定，其他时候不锁定 | `set scheduler-locking step` |

**多线程调试示例**：

```gdb
(gdb) info threads
  Id   Target Id         Frame
* 1    Thread 0x7ffff7fc1700 "myapp"  worker (arg=0x0) at worker.c:42
  2    Thread 0x7ffff7fc0700 "myapp"  producer (arg=0x0) at producer.c:28
  3    Thread 0x7ffff7fbf700 "myapp"  consumer (arg=0x0) at consumer.c:55

(gdb) thread 2
[Switching to thread 2 (Thread 0x7ffff7fc0700 "myapp")]

(gdb) thread apply all bt    # 查看所有线程的调用栈

(gdb) set scheduler-locking on   # 锁定：仅调试当前线程
(gdb) step
(gdb) set scheduler-locking off  # 解锁
```

> [!warning] 多线程调试陷阱
> 默认情况下 GDB 在单步时可能切换到其他线程，导致行为不可预期。使用 `set scheduler-locking on` 锁定调度可避免此问题，但会改变程序的真实执行时序。

---

# 24. 常用调试工作流

## 24.1 调试段错误

```gdb
$ gdb ./myapp core.1234
(gdb) bt                    # 查看崩溃时的调用栈
(gdb) frame 0               # 切换到崩溃帧
(gdb) info locals           # 查看局部变量
(gdb) p *ptr                # 检查可疑指针
(gdb) p/x ptr               # 十六进制查看指针值
```

## 24.2 调试循环

```gdb
(gdb) break loop.c:20 if i == 100   # 条件断点
(gdb) run
(gdb) display i                     # 自动显示 i
(gdb) display sum                   # 自动显示 sum
(gdb) n                             # 单步
(gdb) n                             # 单步
```

## 24.3 调试多线程竞态

```gdb
(gdb) set scheduler-locking on
(gdb) break worker.c:30 thread 2
(gdb) run
(gdb) p shared_data->counter
(gdb) set scheduler-locking step
(gdb) step
```

## 24.4 附加到运行中的进程

```gdb
$ gdb --pid 1234
(gdb) bt
(gdb) info threads
(gdb) thread apply all bt
(gdb) detach
```

---

> [!summary] GDB 核心工作流
> 1. **启动**：`gdb ./app` / `gdb --pid PID` / `gdb --args ./app arg1`
> 2. **设断点**：`break` / `tbreak` / `watch` / `catch`
> 3. **运行**：`run` / `start` / `continue`
> 4. **单步**：`n`（跳过）/ `s`（进入）/ `finish`（跳出）
> 5. **查看**：`p` / `x` / `bt` / `info`
> 6. **修改**：`set var = expr` / `return`
> 7. **退出**：`quit`
