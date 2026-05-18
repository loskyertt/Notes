---
title: Python 爬虫教程 04
date: 2024-10-17 17:04:34
excerpt: "lxml 库进阶使用"
categories: "Python爬虫"
---


# 1.测试案例

```Python
import requests
from lxml import etree

url = "https://spiderbuf.cn/playground/s03"

headers = {
    "User-Agent": "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36",
}

html = requests.get(url=url, headers=headers).text

f = open('./课程/03course/03.html', 'w', encoding='utf-8')
f.write(html)
f.close()

# print(html)
root = etree.HTML(html)
trs = root.xpath('//tr')

f = open('./课程/03course/data03.txt', 'w', encoding='utf-8')
for tr in trs:
    tds = tr.xpath('./td')
    s = ''
    for td in tds:
        s = s + str(td.text) + ' | '
    print(s)
    if s!= '':
        f.write(s + '\n')
```

直接运行上面这段代码，会发现输出结果是这样的：
```txt
1 | None | CD-82-76-71-65-75 | 堡垒机 | 服务器 | Windows10 | 80,22,443 | 
                             | 
2 | None | E6-84-22-55-44-BE | 摄像头 | 摄像头 | HUAWEI | 80,22,443 | 
                             | 
3 | None | 37-01-AE-BE-5E-C0 | 文件服务器 | 服务器 | Linux | 80,22,443 | 
                             | 
4 | None | 84-A9-97-A8-B2-99 | 交换机 | 交换机 | HUAWEI | None | 
                             | 
5 | None | 8C-94-9D-85-6C-C1 | 堡垒机 | 服务器 | Windows10 | 80,22,443 | 
                             | 
6 | None | F2-10-E3-CA-DF-DC | 数据库服务器 | 服务器 | Windows10 | 80,22,443 | 
                             | 
7 | None | 46-9A-AF-F4-DC-33 | 数据库服务器 | 服务器 | Windows10 | 80,22,443 | 
                             | 
8 | None | B5-66-A6-0C-C6-57 | 堡垒机 | 服务器 | Linux | 80,22,443 | 
                             | 
9 | None | 5C-3F-8E-6E-D9-C5 | OA服务器 | 服务器 | Linux | 80,22,443 | 
                             | 
10 | None | EC-C6-79-88-4C-BA | 测试服务器 | 服务器 | Linux | 80,22,443 | 
                             | 
```
第一列的内容都是`None`，这是为什么呢？

# 2.分析页面源码结构

我们先来看下我们要爬取的页面的源码结构，是这段内容：
```html
                    <tr>
                        <td>1</td>
                        <td><a href="#">172.16.80.178</a></td>
                        <td>CD-82-76-71-65-75</td>
                        <td>堡垒机</td>
                        <td>服务器</td>
                        <td>Windows10</td>
                        <td>80,22,443</td>
                        <td>
                            <font color="green">在线</font>
                        </td>
                    </tr>
```

可以发现有两个部分：
- 第二个`td`：`<td><a href="#">172.16.80.178</a></td>`
- 最后一个`td`：`<td><font color="green">在线</font></td>`
这两个部分`<td>`里是不含内容的。要像这种`<td>文本</td>`才说名含有内容，对于这种`<td><a>文本<a></td>`是`<a>`含有内容，`<td>`不含。
所以我们要取出`<a>`节点里的内容应该怎么做呢？有一个思路是把`<td>`节点下的`<a>`节点的内容再取一遍，但这未免有点麻烦了。

这里有一个简单的方法，把解析部分的代码改成这样：
```Python
root = etree.HTML(html)
trs = root.xpath('//tr')

f = open('./课程/03course/data03.txt', 'w', encoding='utf-8')
for tr in trs:
    tds = tr.xpath('./td')
    s = ''
    for td in tds:
        s = s + str(td.xpath('string(.)')) + '|'        # 修改过后的解析操作
    print(s)
    if s!= '':
        f.write(s + '\n')
```
这种方式可以获取元素及其所有子元素的完整文本，而不仅仅是直接子节点的文本。`.`表示当前节点，这里就表示把当前节点的所有文本内容（包括子节点中的文本）提取出来。