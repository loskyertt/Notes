---
title: Python 爬虫教程 06
date: 2024-10-18 16:38:02
excerpt: "网页图片的爬取及本地保存"
categories: "Python爬虫"
---


# 1.分析网站

第一步还是要先向网站发送请求，然后获得请求的内容：
```Python
import requests
from lxml import etree

url = "https://www.spiderbuf.cn/playground/s05"

headers = {
    "User-Agent": "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36",
}

html = requests.get(url= url, headers=headers).text
print(html)

f = open('./课程/05course/05.html', 'w', encoding='utf-8')
f.write(html)
f.close()
```

然后开始解析网页源码，在浏览器中，要爬取的网站页面点击右键，然后选择 **查看网页源代码** ：
[![如图操作.png](https://s21.ax1x.com/2024/10/18/pAUDMm4.png)](https://imgse.com/i/pAUDMm4)

找到要爬取的图片部分：
[![图片部分.png](https://s21.ax1x.com/2024/10/18/pAUD8t1.png)](https://imgse.com/i/pAUD8t1)

点击里面的链接可以进入到图片页面：
[![图片.png](https://s21.ax1x.com/2024/10/18/pAUD8t1.png)](https://imgse.com/i/pAUD8t1)

注意这幅图中左上角的链接`https://www.spiderbuf.cn/static/images/beginner/1kwfkui2.jpg`。这条链接分成两部分来看：`https://www.spiderbuf.cn` + `/static/images/beginner/1kwfkui2.jpg`

也就是说，要爬取图片的话，只需要解析出后半部分的链接即可。

# 2.解析图片链接

代码示例：
```Python
root = etree.HTML(html)
imgs = root.xpath('//img/@src')
for item in imgs:
    img_data = requests.get('https://www.spiderbuf.cn' + item, headers=headers).content     # content 表示以二进制解析内容
    img = open("./课程/05course/" + str(item).replace('/', ''), 'wb')   # b 表示以二进制的方式写入
    img.write(img_data)
    img.close()
```

**为什么这里是写成`//img/@src`，而不是`//img/[@src]/text()`？**

在 XPath 中，`//img/@src` 和 `//img[@src]/text()` 的含义和使用场景不同，主要区别在于它们提取的是元素的属性值还是元素的文本内容。

1. `//img/@src`: 提取属性值

- **作用** ：`//img/@src` 用于选择所有 `<img>` 标签的 `src` 属性值。
- **解释** ：
  - `//img`：选择文档中所有的 `<img>` 元素。
  - `@src`：选择每个 `<img>` 元素的 `src` 属性值。
- **返回值** ：它直接返回属性的值，比如 `/static/images/beginner/9cwjdins.jpg`，即图片的 URL。

2. `//img[@src]/text()`: 提取元素文本

- **作用** ：`//img[@src]/text()` 用于选择所有具有 `src` 属性的 `<img>` 元素的 **文本内容** 。
- **解释** ：
  - `//img[@src]`：选择所有带有 `src` 属性的 `<img>` 元素。
  - `text()`：提取该元素的文本内容。
- **返回值** ：它会返回元素的文本内容，而不是属性的值。

在这里的`<img>` 标签是一个 **自闭合标签** ，通常不会包含任何文本内容。因此，`//img[@src]/text()` 通常不会返回任何有效的结果，因为 `<img>` 标签中**没有文本**。