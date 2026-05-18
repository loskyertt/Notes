---
title: Python 爬虫教程 05
date: 2024-10-17 17:31:52
excerpt: "分页参数分析及翻页爬取"
categories: "Python爬虫"
---


# 1.网页分析

有时候要爬取的网页是需要翻页的，如图所示：

[![第一页.png](https://s21.ax1x.com/2024/10/17/pAUAa1e.png)](https://imgse.com/i/pAUAa1e)

设置有时候尽管网页需要翻页，但是在执行翻页操作时，地址栏（网址）没有发生变化，这时候可能隐藏在浏览器控制台的`network`中，如图所示：
[![控制台.png](https://s21.ax1x.com/2024/10/17/pAUAwXd.png)](https://imgse.com/i/pAUAwXd)

# 2.知道明确页数的情况

示例代码：
```Python
import requests
from lxml import etree

base_url = "https://www.spiderbuf.cn/playground/s04?pageno={}"

headers = {
    "User-Agent": "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36",
}

# 实现翻页爬取
for i in range(1, 6):
    url = base_url.format(i)
    html = requests.get(url=url, headers=headers).text

    f = open('./课程/04course/04_%d.html' % i, 'w', encoding='utf-8')
    f.write(html)
    f.close()

    root = etree.HTML(html)
    trs = root.xpath('//tr')

    f = open('./课程/04course/data04_%d.txt' % i, 'w', encoding='utf-8')
    for tr in trs:
        tds = tr.xpath('./td')
        s = ''
        for td in tds:
            s = s + str(td.xpath('string(.)')) + '|'
        print(s)
        if s!= '':
            f.write(s + '\n')
```

# 3.不知道明确页数的情况

可以通过查看网页源码找寻总页数，然后先用`xpath`解析网页里`总页数`的这个内容，再通过正则表达式解析出里面的数字：
```Python
import requests
from lxml import etree
import re       # 正则表达式库

base_url = "https://www.spiderbuf.cn/playground/s04?pageno={}"

headers = {
    "User-Agent": "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36",
}

html_1 = requests.get(url= base_url.format(1), headers=headers).text
root = etree.HTML(html_1)
pages_text = root.xpath('.//li/span/text()')    # 返回的是一个列表
print(pages_text[0])
# 正则表达式解析，提取数字
pages = re.findall('[0-9]', pages_text[0])      # 返回的是一个列表
print(pages)
```

输出结果：
```txt
共5页
['5']
```

感觉这个方法并不怎么实用，因为这种情况的话是能直接在网页看到有多少页的，再解析就是多此一举了。提这个也只是为了引出正则表达式，关于正则表达式的教程，可以结合 AI 来学习。