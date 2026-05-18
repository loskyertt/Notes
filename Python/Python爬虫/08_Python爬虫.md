---
title: Python 爬虫教程 08
date: 2024-10-19 15:09:01
excerpt: "ajax 动态加载数据爬取"
categories: "Python爬虫"
---


# 1.解析网页

解析网页的步骤与**Python 爬虫教程 07** 的差不多，因为直接用原始的`url`是无法爬取到数据的，还是需要通过浏览器的控制台才能找到。

原始的`url = https://www.spiderbuf.cn/playground/s07`。

代码示例：
```Python
import requests
import json

url = "https://www.spiderbuf.cn/playground/iplist"

headers = {
    "User-Agent": "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36",
}

response = requests.get(url=url, headers=headers)
response.encoding = 'utf-8'  # 设置编码为 utf-8
json_data = response.text

# Save the raw HTML to a file
with open('./课程/07course/07.html', 'w', encoding='utf-8') as f:
    f.write(json_data)

ls = json.loads(json_data)

txt = open('./课程/07course/data07.txt', 'w', encoding='utf-8')
for item in ls:
    s = f'{item['ip']}|{item['mac']}|{item['name']}|{item['type']}|{item['manufacturer']}|{item['ports']}|{item['status']}\n'
    txt.write(s)

txt.close()
```

可以看出爬取到的数据是`json`格式，注意这里的`response.encoding = 'utf-8' `，不设置的话，爬取到的中文内容就是乱码。


# 2.说明（补充）

**AJAX** （Asynchronous JavaScript and XML）是一种在网页中异步加载数据的技术，它允许在不刷新整个网页的情况下更新页面的部分内容。**动态加载数据** 指的是通过 AJAX 技术从服务器获取数据，并将这些数据动态地插入到当前网页中。这个过程可以是数据的初次加载或用户交互时额外加载的数据，比如滚动页面加载更多内容、点击按钮加载新的数据等。

## 2.1 AJAX 动态加载数据的关键点
1. **异步加载** ：页面不需要刷新，也不会阻塞用户的其他操作。在数据请求发送后，用户可以继续操作页面，等待数据加载完成。
   
2. **服务器请求** ：AJAX 使用 JavaScript 发起 HTTP 请求（通常是 `GET` 或 `POST` 请求）到服务器，从而获取所需的数据。获取的数据格式通常是 JSON、XML 或 HTML。

3. **动态更新** ：当服务器返回数据后，JavaScript 会处理响应数据，并将其插入页面中的特定位置，更新页面的部分内容，而不会刷新整个网页。

## 2.2 工作流程
1. **用户操作** ：用户的某些操作（如滚动、点击按钮）触发 AJAX 请求。
2. **发送请求** ：通过 JavaScript 使用 `XMLHttpRequest` 或 `fetch` API 发送 HTTP 请求到服务器。
3. **服务器响应** ：服务器接收到请求后处理并返回数据（如 JSON、HTML 片段等）。
4. **动态更新页面** ：前端 JavaScript 解析服务器返回的数据，并将其动态插入页面中，使页面内容更新而不需要重新加载整个网页。

## 2.3 示例
假设一个网页有一个按钮，当用户点击按钮时，通过 AJAX 加载一段新的数据并显示在页面中。以下是一个简单的 AJAX 动态加载例子（使用 `fetch` API）：

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>AJAX 动态加载示例</title>
</head>
<body>
    <h1>AJAX 动态加载示例</h1>
    <div id="content">这里是初始内容。</div>
    <button id="loadDataBtn">加载更多内容</button>

    <script>
        document.getElementById('loadDataBtn').addEventListener('click', function() {
            fetch('https://api.example.com/get-more-data') // 向服务器发送请求，这里的链接就是含有真实数据内容的 url
                .then(response => response.json()) // 解析返回的 JSON 数据
                .then(data => {
                    // 将新数据动态插入页面中
                    document.getElementById('content').innerHTML += `<p>${data.newContent}</p>`;
                })
                .catch(error => console.error('加载失败:', error));
        });
    </script>
</body>
</html>
```