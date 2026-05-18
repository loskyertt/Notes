---
title: Python 爬虫教程 07
date: 2024-10-19 09:28:18
excerpt: "带 iframe 的页面源码分析及数据提取"
categories: "Python爬虫"
---


# 1.示例

示例代码：
```Python
import requests
from lxml import etree

# 注意这条链建
url = "https://www.spiderbuf.cn/playground/s06"

headers = {
    "User-Agent": "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36",
}

html = requests.get(url=url, headers=headers).text

f = open('./课程/06course/06.html', 'w', encoding='utf-8')
f.write(html)
f.close()

print(html)

root = etree.HTML(html)
trs = root.xpath('//tr')

f = open('./课程/06course/data06.txt', 'w', encoding='utf-8')
for tr in trs:
    tds = tr.xpath('./td')
    s = ''
    for td in tds:
        s = s + str(td.xpath('string(.)')) + '|'
    print(s)
    if s!= '':
        f.write(s + '\n')
```
运行这段代码，会发现是解析不出我们想要的内容的。虽然`url`、解析逻辑看起来填写的都是正确的，那么问题是出在哪儿呢？其实就是出在`url`，这个`url`只是看起来正确，但并不是真正的`url`（我们想要的，包含内容的）。

# 2.分析页面

这时候我们来分析下网页源代码，发现在目标页面代开源码是没有我们想要的内容的，如下图所示：
[![网页源码.png](https://s21.ax1x.com/2024/10/19/pAUHxVs.png)](https://imgse.com/i/pAUHxVs)
那么这时候我们就得找到 **真实链接** 。

按`F12`打开浏览器控制台，选择`network`：
[![控制台.png](https://s21.ax1x.com/2024/10/19/pAUbP2T.png)](https://imgse.com/i/pAUbP2T)

先看左边（红色框）部分，选中一个，看一下它的`preview`，如图所示：
[![preview.png](https://s21.ax1x.com/2024/10/19/pAUbkMF.png)](https://imgse.com/i/pAUbkMF)

可以发现没有我们想要的大内容，说明原始的`url`不对，我们得通过这种方式查找出包含内容的`url`。可以把后缀是`.js`和`.css`的排除掉，这些都是指网页样式。

如下图所示：
[![pAUbQxO.png](https://s21.ax1x.com/2024/10/19/pAUbQxO.png)](https://imgse.com/i/pAUbQxO)
这个`inner`才是我们想要的，再看它的`headers`，我们就能找到它的正确的`url = https://www.spiderbuf.cn/playground/inner`

把这个`url`替换掉原始`url`就行了：
```Python
url = "https://www.spiderbuf.cn/playground/inner"

headers = {
    "User-Agent": "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36",
}

html = requests.get(url=url, headers=headers).text

'''剩余部分的代码不变......'''
```

# 3.总结

如图所示：
[![控制台.png](https://s21.ax1x.com/2024/10/19/pAUb8qH.png)](https://imgse.com/i/pAUb8qH)

可以发现在`html`下还有一个`html`，称作`iframe`。就是网页里面嵌套了一个浏览器再打开另一个网页，只有在控制台看它后台的一个请求，才能找出真实的`url`。