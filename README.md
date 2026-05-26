# ZJU-live-better

A collection of useful scripts helping you live better in ZJU.

## 原项目界面

https://github.com/5dbwat4/ZJU-live-better

## 简单介绍
原项目的缺陷是用明文在本地存储密码，较可能泄露（被偷看），现在采用的是终端输入，安全性高一些，缺点就是无法放在线上服务器自动运行。

目前原项目添加了从学在浙大的api直接获取数字点名的数字功能，本项目未同步。未来可能考虑把input.cpp翻译成javasript。

可以通过邮箱redaunt06@gmail.com联系我。

## 配置

运行npm install安装依赖

请确保你已安装g++编译器来编译C++代码

Windows
```powershell
g++ .\input.cpp -o input.exe
.\input.exe
```
Linux / MacOS
```ash
g++ ./input.cpp -o input
./input
```

## 功能列表

### 学在浙大相关 (courses.zju/)

| 功能 | 说明 |
| --- | --- |
| autosign | 自动签到工具，支持多地点配置和钉钉通知 |
| todolist | 生成作业待办事项列表 |
| reliableTodolist | 更可靠的待办事项列表生成 |
| materialDown | 下载课程所有素材 |
| materialMaintainer | 可以基于配置文件增量下载课程素材 |
| materialMaintainer_init | 初始化素材维护配置 |
| quizanswer | 获取学在浙大quiz的答案（互动式测验） |
| watchVideo | 刷课脚本，直接向课程系统发送完成活动的请求 |

### 智云课堂相关 (classroom.zju/)

| 功能 | 说明 |
| --- | --- |
| generateCourseMd | 将智云课堂语音识别&PPT图片生成Markdown文件 |
| getVideoURL | 获取指定课程视频链接 |

### Webplus系统相关 (webplus.zju/)

| 功能 | 说明 |
| --- | --- |
| saveDoc | Webplus系统存档备份工具，保存通知及所有附件（适用于office.ckc.zju.edu.cn、cspo.zju.edu.cn等） |

### 共享工具 (shared/)

| 功能 | 说明 |
| --- | --- |
| cli-entry.js | CLI入口文件 |
| dingtalk-webhook.js | 钉钉Webhook通知工具 |

## 输入程序功能

input.cpp 提供以下增强功能：

- **历史记录支持**：脚本路径、学号、参数均支持历史记录，使用上下箭头键浏览
- **退格键支持**：所有输入字段支持退格键删除
- **跨平台支持**：支持 Windows、Linux 和 macOS
- **安全密码输入**：密码输入时不显示字符（显示*）
- **安全内存擦除**：敏感数据使用后从内存中安全擦除

## 反馈

反馈使用问题可以添加QQ群：1042563780

## 免责声明

本项目仅供学习交流使用，请勿用于任何商业用途，请勿用于任何非法或违规用途。使用本项目前请务必了解并遵守浙江大学相关政策和规定。作者不对因使用本项目而导致的任何后果负责。
