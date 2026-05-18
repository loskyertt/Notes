---
title: Python 爬虫教程 09
date: 2024-10-21 14:35:56
excerpt: "http post 请求的数据爬取"
categories: "Python爬虫"
---


# 1.代码示例

```Python
import requests
from lxml import etree

url = "https://www.spiderbuf.cn/playground/s08"

headers = {
    "User-Agent": "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36",
}
html = requests.get(url=url, headers=headers).text

f = open('./课程/08course/08.html', 'w', encoding='utf-8')
f.write(html)
f.close()

root = etree.HTML(html)
trs = root.xpath('//tr')

f = open('./课程/08course/data08.txt', 'w', encoding='utf-8')
for tr in trs:
    tds = tr.xpath('./td')
    s = ''
    for td in tds:
        s = s + str(td.xpath('string(.)')) + '|'
    print(s)
    if s!= '':
        f.write(s + '\n')
```
直接运行这段代码是不会解析出任何数据的，同时可以看到抓取到的网页与我们想要的不一样。

# 2,网页分析

打开浏览器控制台，选择`network`：
[![控制台.png](https://s21.ax1x.com/2024/10/21/pAa40dx.png)](https://imgse.com/i/pAa40dx)
可以发现，请求方式是`post`。所以我们就得在代码中采用`post`请求方式：
```Python
import requests
from lxml import etree

url = "https://www.spiderbuf.cn/playground/s08"

headers = {
    "User-Agent": "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36",
}
# 传入 post 请求中的数据
payload = {'level': '8'}
# post 请求
html = requests.post(url=url, headers=headers, data=payload).text

f = open('./课程/08course/08.html', 'w', encoding='utf-8')
f.write(html)
f.close()

root = etree.HTML(html)
trs = root.xpath('//tr')

f = open('./课程/08course/data08.txt', 'w', encoding='utf-8')
for tr in trs:
    tds = tr.xpath('./td')
    s = ''
    for td in tds:
        s = s + str(td.xpath('string(.)')) + '|'
    print(s)
    if s!= '':
        f.write(s + '\n')
```

`payload`是在这里：
[![payload.png](https://s21.ax1x.com/2024/10/21/pAa4oY8.png)](https://imgse.com/i/pAa4oY8)

可以参考这里：[更加复杂的 POST 请求](https://requests.readthedocs.io/projects/cn/zh-cn/latest/user/quickstart.html#post)