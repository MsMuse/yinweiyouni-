# yinweiyouni-
介绍：
一、项目简介
本项目名“音为有你~”，是一款为音乐爱好者提供的基础乐理知识教学以及自行创作演奏的程序。其包含演奏，教学，音色切换，声音效果和自行录制等多项功能。
在设计和制作过程中，我们了解并应用了如下内容：
1、qt creator
本项目的UI界面设计基于6.9.0版本，对6.0及以上版本均有很好的适应性和兼容性
2、编译器
本项目基于c++17设计的qmake项目，文件编码默认为utf-8。平台qt creator是专门为 Qt 应用开发打造的跨平台 IDE，支持 C++（Qt 开发核心语言 ）、QML 等，集代码编辑、编译构建、调试、界面设计（Qt Designer 集成 ）于一体，深度适配 Qt 框架，让开发者能便捷利用 Qt 提供的各类组件、模块（如界面控件、网络、多媒体等 ）开发应用，可配置不同构建套件（Kit ，包含编译器、Qt 版本、调试器等 ）。支持一键构建项目，集成调试器（如 GDB ），方便排查代码逻辑错误。此外，还支持跨平台，开发者在 Windows 上编写的代码，借助对应构建套件，可编译运行在 macOS、Linux 等系统，甚至能适配 Android、iOS 移动端，助力打造跨平台应用。
3、依赖库
本项目依赖于基于vcpkg所下载的fluidsynth：x64-Windows 2.4.6完成声音合成与MIDI交互。
二、使用说明
用户通过下载源码修改一些路径便可直接使用，要修改的部分为：
（1）将piano-interface.pro中INCLUDEPATH中路径改为自己的vcpkg路径\installed\x64-windows\include，LIBS改为-L自己的vcpkg路径\installed\x64-windows\lib。
（2)用户需下载音色库文件GeneralUser-GS.sf2，将OutputHandler.cpp中file_path改为自己GeneralUser-GS.sf2音色文件的路径。
网盘链接：https://disk.pku.edu.cn/link/AA1580C91F752A4BD2A5BD61BF24E69979
文件名：56-演示.mp4
有效期限：永久有效
