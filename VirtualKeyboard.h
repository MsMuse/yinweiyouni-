#ifndef VIRTUALKEYBOARD_H
#define VIRTUALKEYBOARD_H


#include"MIDIMsg.h"
#include"InputHandler.h"
#include"OutputHandler.h"
#include<iostream>
#include<set>
#include<vector>
#include <QObject>
#include<QStringList>

//音色，用来储存键盘两个叠加音色的参数
struct Timbre
{
    //音色所处通道
    int channel = -1;
    //音色所处库
    int bank = -1;
    //音色在该库中的编号
    int program = -1;
    //音色的最低音高
    int lowpitch = -1;
    //音色的最高音高
    int highpitch = 128;
    //音色的音量
    int volume = 127;
    //构造函数，确定音色的基本参数
    Timbre(int channel_ = -1, int bank_ = 14, int program_ = 0, int lowpitch_ = -1, int highpitch_ = 128, int volume_ = 127);
};
struct Instrument
{
    QString name;
    int bank;
    int program;
    Instrument(std::string name_ = "", int bank_ = -1, int program_ = -1);
};

/*
虚拟键盘类，负责处理将输入输出接口之间的信号传输和实现除了生成MIDI音符消息外的其他音乐效果
*/
class VirtualKeyboard
{
    //输出接口
    std::shared_ptr<OutputHandler> output;
    //八度移调
    int octavetranspose;
    //半音移调
    int semitonetranspose;
    //音色一
    Timbre timbre1;
    //音色二
    Timbre timbre2;
    //延音踏板是否开启
    bool sustainpedal;
    //正在被演奏的音符
    std::set<int> note_played;
    //混响强度
    double reverb_level;
    //混响房间
    double reverb_room;
    //合唱强度
    double chorus_level;
    //合唱数量
    int chorus_count;
    //合唱速率
    double chorus_speed;
    //合唱深度
    double chorus_depth;
public:
    //增加八度移调
    void aOctave();
    //减少八度移调
    void bOctave();
    //增加半音移调
    void aSemitone();
    //减少半音移调
    void bSemitone();
    //输入接口
    std::shared_ptr<InputHandler> input;
    //构造一个默认的虚拟键盘（默认一个grand piano音色）
    VirtualKeyboard(std::shared_ptr<InputHandler> i, std::shared_ptr<OutputHandler> o);
    //释放资源
    ~VirtualKeyboard();
    //设定输入端口
    void setInput(std::shared_ptr<InputHandler> i);
    //设定输出端口
    void setOutput(std::shared_ptr<OutputHandler> o);
    //获取八度移调参数
    std::string getOctave() const;
    //获取半音移调参数
    std::string getSemitone() const;
    //处理输入端口的音符消息
    void process();
    //获取正在被演奏的音符列表，用来在屏幕上显示演奏音符
    const std::set<int>& getNoteList() const;
    //获取延音踏板状态,用来在屏幕上显示延音是否开启
    std::string getSustain() const;
    //改变延音踏板状态
    void changeSustain();
    //设置音色1种类
    void setTimbre1(int bank_, int program_);
    //设置音色2种类
    void setTimbre2(int bank_, int program_);
    //设置混响强度
    void setReverbLevel(double level);
    //设置混响房间
    void setReverbRoom(double room);
    //设置合唱数量
    void setChorusNum(int count);
    //设置合唱强度
    void setChorusLevel(double level);
    //设置合唱速率
    void setChorusSpeed(double speed);
    //设置合唱深度
    void setChorusDepth(double depth);
    //乐器表
    static QVector<Instrument> instruments;
    //设置1号音量
    void setVolume1(int vol);
    //设置2号音量
    void setVolume2(int vol);
    //音名表
    static QStringList noteNames;
    //是否开启音色分割
    bool split;
    //改变音色分割
    void change_split();
    //音色分割点
    int split_point;
    //设置音色分割点
    void set_split(int point);
    //设置弯音轮
    void setPitchbend(int value);
    //设置调制轮
    void setModulation(int value);
    //设置resonance
    void setResonance(int value);
    //设置cutoff
    void setCutoff(int value);
    //设置attack
    void setAttack(int value);
    //设置decay
    void setDecay(int value);
    //设置sustain
    void setSustain(int value);
    //设置release
    void setRelease(int value);
};
//键盘设置的异常类,处理键盘设置中的异常行为
class SettingException :public std::runtime_error
{
public:
    SettingException(const std::string& msg) :runtime_error(msg) {}
};
#endif // VIRTUALKEYBOARD_H
