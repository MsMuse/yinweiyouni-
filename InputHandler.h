#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include"MIDIMsg.h"
#include<vector>
#include<mutex>

/*
为不同的输入来源实现统一的输入接口
负责捕获用户的输入，并将其转换成MIDIMsg的格式
需要依照MIDIMsg中函数分别实现
*/
class InputHandler
{
public:
    virtual ~InputHandler() = default;
    virtual MIDIMsg handleInput() = 0;
    virtual void getInput(int n,int status)=0;
    std::string name = "";
    std::vector<MIDIMsg> InputList;
};
//在屏幕上点击琴键输入生成MIDIMsg
class MouseInput :public InputHandler
{
public:
    std::string name = "mouse";
    virtual void getInput(int n,int status) override;//音符n，status是开始或结束
    virtual MIDIMsg handleInput() override;
private:
    std::mutex mtx;
};
//敲击电脑键盘输入生成MIDIMsg
class KeyboardInput :public InputHandler
{
public:
    std::string name = "keyboard";
};
//外接MIDIInput输入生成MIDIMsg
class MidiPortInput :public InputHandler
{
public:
    std::string name = "midi";
};
//输入异常类，用于处理异常的输入行为
class InputException :public std::runtime_error
{
public:
    //接受错误消息
    InputException(const std::string& msg) :runtime_error(msg) {}
};
#endif // INPUTHANDLER_H
