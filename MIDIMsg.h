#ifndef MIDIMSG_H
#define MIDIMSG_H

#include<vector>
#include<unordered_map>
#include<string>
#include<stdexcept>

/*
MIDIMsg的目的是为了封装一个MIDI消息类，可以直接对MIDI消息类进行操作而不必深入信号底层，便于操作，
同时便于后续功能可能实现的与其他MIDI模块的泛化

一个一般的MIDI消息结构如下：
    第一个字节表示消息状态和使用通道，第二个或后两个为数据字节

本类主要针对通道消息，鉴于本项目可能使用的消息类型，其消息可分为以下几类（下文中的n表示通道数）：
    一、音符消息：
        又分为Note On和Note Off。Note On的状态字节为0x9n，数据字节分别表示音高和音速（音量）；
        Note Off的状态字节为0x8n，数据字节表示音高和释放速度。
    二、控制器消息：
        状态字节为0xBn，数据字节分别表示为控制器编号和控制器值
    三、音色切换：
        状态字节为0xCn，数据字节表示为音色编号
    四、弯音消息：
        状态字节为0xEn，数据字节分别为高低字节，两字节经运算得到具体弯音数值
*/
class MIDIMsg
{
    std::vector<unsigned char> data;//MIDIMsg的原始数据
    double timestamp;//时间戳，相对于上一个消息的秒数，第一个消息的时间戳是0
    unsigned char msgstatus;//代表消息状态类型的字节
    unsigned int channel;//消息对应的频道
public:
    //空MIDI消息
    MIDIMsg();
    //生成MIDI消息
    MIDIMsg(const std::vector<unsigned char>& data_, double timestamp_);
    //获取原始MIDI消息数据
    const std::vector<unsigned char>& getData()const;
    //获取时间戳
    double getTimestamp()const;
    //获取消息状态数据
    const unsigned char& getStatusData() const;
    //获取消息状态
    unsigned char getStatus() const;
    //获取通道编号
    unsigned int getChannel() const;
    //获取音符编号
    int getNoteNumber() const;
    //获取音符速度（力度）
    int getVelocity() const;
    //获取控制器编号
    int getControllerNumber() const;
    //获取控制器数值
    int getControllerValue() const;
    //获取新音色编号
    int getTimbreChange() const;
    //获取通道触后力度
    int getAftertouchForce() const;
    //获取弯音数值
    int getPitchBend() const;
    //生成音符开启消息
    static MIDIMsg generateNoteOn(int note, int velocity = 100, int channel = 0, double timestamp = 0.0);
    //生成音符关闭消息
    static MIDIMsg generateNoteOff(int note, int velocity = 100, int channel = 0, double timestamp = 0.0);
    //生成控制器变化消息
    static MIDIMsg generateControllerChange(int controller, int value, int channel = 0, double timestamp = 0.0);
    //生成音色变化消息
    static MIDIMsg generateTimbreChange(int program, int channel = 0, double timestamp = 0.0);
    //生成弯音消息
    static MIDIMsg generatePitchBend(int value, int channel = 0, double timestamp = 0.0);
    //生成音色库切换消息（一个pair，一次库切换要两个消息）
    static std::pair<MIDIMsg, MIDIMsg> generateBankChange(int banknum, int channel = 0, double timestamp = 0.0);
};

//MIDIMsg状态类型字节的含义对应
const std::unordered_map<unsigned char, std::string> msgStatusList =
    {
        {0x80, "Note Off"},
        {0x90, "Note On"},
        {0xA0, "Polyphonic Key Pressure (Aftertouch)"},
        {0xB0, "Control Change"},
        {0xC0, "Program Change"},
        {0xD0, "Channel Pressure (Aftertouch)"},
        {0xE0, "Pitch Bend"}
};

//MIDI异常类，用于处理异常的MIDI消息
class MIDIException :public std::runtime_error
{
    std::vector<unsigned char> Msg; // 存储非法消息的内容
public:
    // 接受错误消息和非法信号的内容
    MIDIException(const std::string& message, const std::vector<unsigned char>& midiMsg)
        : std::runtime_error(message), Msg(midiMsg) {}
    // 获取非法信号的内容
    const std::vector<unsigned char>& getMessage() const
    {
        return Msg;
    }
};
#endif // MIDIMSG_H
