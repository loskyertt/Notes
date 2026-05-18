---
title: Python 爬虫教程 03
date: 2024-10-17 16:51:40
excerpt: "http 请求分析及头构造使用"
categories: "Python爬虫"
---


# 1.无请求头访问

如果不构建请求头，直接向目标网站发送请求：
```Python
import requests
from lxml import etree

url = "https://spiderbuf.cn/playground/s02"

html = requests.get(url=url).text

f = open('./课程/02course/02.html', 'w', encoding='utf-8')
f.write(html)
f.close()

print(html)
root = etree.HTML(html)
trs = root.xpath('//tr')

f = open('./课程/02course/data02.txt', 'w', encoding='utf-8')
for tr in trs:
    tds = tr.xpath('./td')
    s = ''
    for td in tds:
        # print(td.text)
        s = s + str(td.text) + ' | '
    print(s)
    if s!= '':
        f.write(s + '\n')
```

输出结果：
```txt
<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 2.0//EN">
<html>
<head><title>403 Forbidden</title></head>
<body>
<center><h1>403 Forbidden</h1></center>
<hr><center>tengine</center>
</body>
</html>
```
很容易被网站检测到是爬虫。

# 2.添加请求头

所以基本上在发送请求之前都会封装一个`http`请求的头部信息：
```Python
headers = {
    "User-Agent": "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36",
}

html = requests.get(url=url, headers=headers).text
```

有时候还需要往里面填入`Cookie`。甚至为了防止被检测到是爬虫，需要更换`User-Agent`，比如用火狐浏览器的等，或者浏览器不同版本的，这在网上可以查询到。