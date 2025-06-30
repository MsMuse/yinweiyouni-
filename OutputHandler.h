#ifndef OUTPUTHANDLER_H
#define OUTPUTHANDLER_H


#include"MIDIMsg.h"
#include"fluidsynth.h"

/*
为不同的输出目标实现统一的输出接口
统一接收MIDIMsg
*/
class OutputHandler
{
public:
    virtual ~OutputHandler() = default;
    virtual void send(const MIDIMsg& msg) = 0;
};
//输出到fluidsynth
class SynthOutput:public OutputHandler
{
    fluid_settings_t* settings = NULL;
    fluid_audio_driver_t* adriver = NULL;
public:
    fluid_synth_t* synth = NULL;
    //构造一个fluidsynth实例及其相关设定
    SynthOutput();
    //清除合成器资源
    ~SynthOutput();
    //向fluidsynth发送MIDI消息
    virtual void send(const MIDIMsg& msg);
};
//输出到外接MIDIOutput
class MidiPortOutput :public OutputHandler
{

};
//输出异常类，用于处理异常的输出行为
class OutputException :public std::runtime_error
{
public:
    OutputException(const std::string& msg) :runtime_error(msg) {}
};
#endif // OUTPUTHANDLER_H
