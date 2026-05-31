# 1. 步骤一：完成 CLI 的本地身份注册

运行注册命令，并用 `--name` 给你的本地客户端起一个名字（比如叫 `sky-agent`）：

```bash
npx @lobehub/market-cli register --name sky-agent
```

_执行后，它会在你的终端里输出成功信息，并在后台自动生成你缺失的 `MARKET_CLIENT_ID`。_

---

# 2. 步骤二：重新安装 PDF 技能

注册成功后，认证锁就被解开了。此时再次运行你最初需要的安装命令（记得把链接填完整，或者直接用官方市场的包名）：

- 方式 A（使用官方市场包名，最推荐）：

```bash
npx @lobehub/market-cli skills install anthropics-skills-pdf --agent codex
```

- 方式 B（如果你一定要用指定的 GitHub 裸链接，注意不能只写 githubusercontent.com，必须写完完整的 json 路径）：

```bash
npx @lobehub/market-cli skills install https://githubusercontent.com --agent codex
```
