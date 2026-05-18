---
title: Python 爬虫教程 02
date: 2024-10-17 14:41:00
excerpt: "requests 库机器 lxml 入门"
categories: "Python爬虫"
---


# 1.测试网址

[测试网站](https://www.spiderbuf.cn/list)

# 2.编写代码

## 2.1 代码示例

```Python
import requests
from lxml import etree

url = "https://spiderbuf.cn/playground/s01"

html = requests.get(url=url).text

f = open('./课程/01course/01.html', 'w', encoding='utf-8')
f.write(html)
f.close()

# print(html)
root = etree.HTML(html)
trs = root.xpath('//tr')

f = open('./课程/01course/data01.txt', 'w', encoding='utf-8')
for tr in trs:
    tds = tr.xpath('./td')
    s = ''
    for td in tds:
        # 这里加 str 是为了防止 <td></td> 之间是空数据
        s = s + str(td.text) + ' | '
    print(s)
    # 保存解析到的数据到本地
    if s!= '':
        f.write(s + '\n')
```

在很多时候，建议先把请求到的网页内容保存到本地（如上面代码所示），因为有时候解析数据无法一次性解析成功，那就会再向网页发送请求，这样不断的去访问服务器，就容易被服务器检测到是爬虫。


# 3.lxml 基础使用

## 3.1 XPath 基本语法

- `/`：从根节点选择。根节点是一个 XML 或 HTML 文档的最顶层节点，所有其他元素都是它的子节点。
- `//`：选择文档中的所有符合条件的节点，不论它们在文档的什么位置。
- `.`：当前节点。
- `..`：父节点。
- `@`：选择属性。
- `[ ]`：筛选器，添加条件。
- `text()`：选择文本节点。

## 3.2 示例说明

假设有如下 HTML 结构，`<html>`是根节点：

```html
<html>
  <body>
    <table>
      <tr>
        <td class="status">在线</td>
        <td>123</td>
      </tr>
      <tr>
        <td class="status">离线</td>
        <td>456</td>
      </tr>
      <tr>
        <td class="status">在线</td>
        <td>789</td>
      </tr>
    </table>
  </body>
</html>
```

## 3.3 使用 XPath 提取数据

1. **提取所有行的 `<td>` 内容：**

```python
root = etree.HTML(html)
trs = root.xpath('//tr')
```

`//tr` 表示选择文档中所有的 `<tr>` 节点，不论它们在文档的哪个层级。

2. **提取某个特定列的数据：**

```python
status_list = root.xpath('//td[@class="status"]/text()')
print(status_list)
```

- `//td[@class="status"]`：选择所有 class 为 "status" 的 `<td>` 节点。
- `/text()`：选择这些节点的文本内容。

**结果：**

```python
['在线', '离线', '在线']
```

3. **结合 `for` 循环逐行提取多个列的内容：**

```python
trs = root.xpath('//tr')

for tr in trs:
    status = tr.xpath('./td[@class="status"]/text()')[0]  # 获取状态
    number = tr.xpath('./td[2]/text()')[0]  # 获取数字
    print(f"状态: {status}, 数字: {number}")
```

- `./td[@class="status"]/text()`：从当前行的 `<td>` 中选择 class 为 "status" 的列。
- `./td[2]/text()`：选择当前行的第二个 `<td>` 节点的文本内容。

**输出：**

```bash
状态: 在线, 数字: 123
状态: 离线, 数字: 456
状态: 在线, 数字: 789
```

## 3.4 XPath 实用技巧

1. **使用属性选择节点：**
   - `//td[@class="status"]` 选择所有 class 为 "status" 的 `<td>`。
   - `//a[@href]` 选择带有 `href` 属性的所有 `<a>` 标签。

2. **使用索引选择特定节点：**
   - `//tr[1]` 选择第一个 `<tr>` 节点。
   - `//td[2]` 选择当前 `<tr>` 中的第二个 `<td>` 节点。

3. **选择文本和属性：**
   - `//td/text()` 选择 `<td>` 内的文本。
   - `//a/@href` 选择所有 `<a>` 标签的 `href` 属性。

4. **条件筛选：**
   - `//tr[td[1]="在线"]` 选择第一列文本为“在线”的行。

