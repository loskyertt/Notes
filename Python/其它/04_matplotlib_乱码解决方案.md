# 1.出现的问题

以下面这个代码为例：
```Python
import numpy as np
import matplotlib.pyplot as plt

x = np.arange(1, 12)
y = x ** 2 + 4
plt.title("Matplotlib demo")
plt.xlabel("时间(分钟)")
plt.ylabel("金额($)")
plt.plot(x,y)
plt.show()
```


在 Linux 下使用 Python 的 `matplotlib` 包，中文字符默认是会出现乱码的，或者在 Windows 下报这种类似错误（缺失字体）：

```bash
d:\Dev\Forest_FSL\preprocess\check_data.py:133: UserWarning: Glyph 23545 (\N{CJK UNIFIED IDEOGRAPH-5BF9}) missing from font(s) DejaVu Sans.

  plt.tight_layout()
```

归根结底是 Matplotlib 字体配置问题（CJK 字体缺失），不是数据或绘图逻辑错误。

---

# 2.解决方法

## 2.1 Linux 平台

首先需要下载字体，网上常用的中文字体是 `SimHei`，[下载地址](https://github.com/StellarCN/scp_zh/blob/master/fonts/SimHei.ttf)

### 2.1.1 方式一：直接在代码中引入（推荐）

```python
import matplotlib.font_manager as fm

font_path = "/home/sky/下载/SimHei.ttf"  # 设置字体路径（或者其他中文字体的路径）
my_font = fm.FontProperties(fname=font_path)

# 手动注册字体
fm.fontManager.addfont(font_path)

# 获取字体名称
font_name = my_font.get_name()
print(f"真实字体名称: {font_name}")

# 设置 Matplotlib 识别该字体
plt.rcParams["font.sans-serif"] = [font_name]
plt.rcParams["axes.unicode_minus"] = False  # 解决负号显示问题
```

### 2.2.2 方式二：修改配置文件

1. **查找存放字体的文件夹**

```Python
import matplotlib
 
print(matplotlib.matplotlib_fname())   # 查找字体路径
```

运行这段代码会打印出字体存放路径，如：

```txt
/home/sky/miniconda3/envs/test/lib/python3.12/site-packages/matplotlib/mpl-data/matplotlibrc
```

`matplotlibrc` 是配置文件，一会儿要修改的。然后进入存放字体的目录下：

```bash
cd /home/sky/miniconda3/envs/test/lib/python3.12/site-packages/matplotlib/mpl-data/fonts/ttf
```

把下载好的字体文件 `SimHei.ttf` 复制到 `ttf` 目录下：

```bash
cp ~/下载/SimHei.ttf .  
```

然后回到 `mpl-data` 目录下，修改 `matplotlibrc` 文件：

```bash
nano matplotlibrc
```

找到 `font.serif`，`font.sans-serif` 所在位置，如下如所示。在冒号后面加入 `SimHei`，保存退出：

- 原来的：

```txt
#font.serif:      DejaVu Serif, Bitstream Vera Serif, Computer Modern Roman, New Century Schoolbook, Century Schoolbook L, Utopia, ITC Bookman, Bookman, Nimbus Roman No9 L, Times New Roman, Times, Palatino, Charter, serif
#font.sans-serif: DejaVu Sans, Bitstream Vera Sans, Computer Modern Sans Serif, Lucida Grande, Verdana, Geneva, Lucid, Arial, Helvetica, Avant Garde, sans-serif
```

- 修改后的：

```txt
#font.serif:      SimHei, DejaVu Serif, Bitstream Vera Serif, Computer Modern Roman, New Century Schoolbook, Century Schoolbook L, Utopia, ITC Bookman, Bookman, Nimbus Roman No9 L, Times New Roman, Times, Palatino, Charter, serif
#font.sans-serif: SimHei, DejaVu Sans, Bitstream Vera Sans, Computer Modern Sans Serif, Lucida Grande, Verdana, Geneva, Lucid, Arial, Helvetica, Avant Garde, sans-serif
```

2. **清理缓存**

查找缓存位置：

```Python
import matplotlib    
print(matplotlib.get_cachedir())
```

把缓存文件删除即可：

```bash
rm ~/.cache/matplotlib -rf
```

---

## 2.2 Windows 平台

在代码中手动指定中文字体就行：

```python
import matplotlib.pyplot as plt

plt.rcParams["font.sans-serif"] = ["SimHei"]  # 黑体
plt.rcParams["axes.unicode_minus"] = False  # 解决负号显示问题
```

> [!note]
> Linux 上也可以这样手动指定字体名称，前提是该字体得支持中文字体，如果出现乱码的情况，建议先删除 `~/.cache/matplotlib/fontlist-v390.json` 这个缓存文件，该文件包含系统字体的索引缓存。

---

# 3.负号乱码解决

只要在前面加以段这个代码：

```Python
# 解决负号显示问题
plt.rcParams["axes.unicode_minus"] = False
```

就能正常显示负号。

---

# 4. 参考教程

- [【Deepin20系统】Linux系统中永久解决matplotlib画图中文乱码问题和使用seaborn中文乱码问题](https://developer.aliyun.com/article/1577567)
- [解决Python使用matplotlib绘图时出现的中文乱码问题](https://cloud.tencent.com/developer/article/1877673)
