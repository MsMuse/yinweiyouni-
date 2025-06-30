#include "MIDIMsg.h"
using namespace std;

MIDIMsg::MIDIMsg() :timestamp(0.0) {}

MIDIMsg::MIDIMsg(const vector<unsigned char>& data_, double timestamp_) :data(data_), timestamp(timestamp_)
{
    msgstatus = data[0];
    channel = static_cast<unsigned int>(msgstatus & 0x0F);
}

const vector<unsigned char>& MIDIMsg::getData() const { return data; }

double MIDIMsg::getTimestamp() const { return timestamp; }

const unsigned char& MIDIMsg::getStatusData() const
{
    if (data.empty())
        throw MIDIException("Empty MIDI Message", data);
    if (msgStatusList.count(msgstatus) == 0)
        throw MIDIException("Invalid MIDI Message", data);
    return msgstatus;
}

unsigned char MIDIMsg::getStatus() const
{
    if (msgStatusList.count(msgstatus & 0xF0) == 0)
        throw MIDIException("Invalid MIDI Message", data);
    return msgstatus & 0xF0;
}

unsigned int MIDIMsg::getChannel() const
{
    if (channel < 0 || channel>15)
        throw invalid_argument("Invalid Channel Number");
    return channel;
}

int MIDIMsg::getNoteNumber() const
{
    if (getStatus() != 0x80 && getStatus() != 0x90)
        throw MIDIException("Invalid Note Message", data);
    return data[1];
}

int MIDIMsg::getVelocity() const
{
    if (getStatus() != 0x80 && getStatus() != 0x90)
        throw MIDIException("Invalid Note Message", data);
    return data[2];
}

int MIDIMsg::getControllerNumber() const
{
    if (getStatus() != 0xB0)
        throw MIDIException("Invalid Controller Message", data);
    return data[1];
}

int MIDIMsg::getControllerValue() const
{
    if (getStatus() != 0xB0)
        throw MIDIException("Invalid Controller Message", data);
    return data[2];
}

int MIDIMsg::getTimbreChange() const
{
    if (getStatus() != 0xC0)
        throw MIDIException("Invalid Program Change Message", data);
    return data[1];
}

int MIDIMsg::getAftertouchForce() const
{
    if (getStatus() != 0xD0)
        throw MIDIException("Invalid Channel Pressure Message", data);
    return data[1];
}

int MIDIMsg::getPitchBend() const
{
    if (getStatus() != 0xE0)
        throw MIDIException("Invalid Pitch Bend Message", data);
    return(data[2] << 7) | data[1];
}

MIDIMsg MIDIMsg::generateNoteOn(int note, int velocity, int channel, double timestamp)
{
    if (note < 0 || note > 127)
        throw invalid_argument("Invalid Note Number");
    if (velocity < 0 || velocity > 127)
        throw invalid_argument("Invalid Note Velocity");
    if (channel < 0 || channel>15)
        throw invalid_argument("Invalid Channel Number");
    vector<unsigned char>data = { static_cast<unsigned char>(0x90 | (channel & 0x0F)) ,static_cast<unsigned char>(note & 0x7F) ,static_cast<unsigned char>(velocity & 0x7F) };
    return MIDIMsg(data, timestamp);
}

MIDIMsg MIDIMsg::generateNoteOff(int note, int velocity, int channel, double timestamp)
{
    if (note < 0 || note > 127)
        throw invalid_argument("Invalid Note Number");
    if (velocity < 0 || velocity > 127)
        throw invalid_argument("Invalid Note Velocity");
    if (channel < 0 || channel>15)
        throw invalid_argument("Invalid Channel Number");
    vector<unsigned char>data = { static_cast<unsigned char>(0x80 | (channel & 0x0F)) ,static_cast<unsigned char>(note & 0x7F) ,static_cast<unsigned char>(velocity & 0x7F) };
    return MIDIMsg(data, timestamp);
}

MIDIMsg MIDIMsg::generateControllerChange(int controller, int value, int channel, double timestamp)
{
    if (controller < 0 || controller > 127)
        throw invalid_argument("Invalid Controller Number");
    if (value < 0 || value > 127)
        throw invalid_argument("Invalid Controller Value");
    if (channel < 0 || channel>15)
        throw invalid_argument("Invalid Channel Number");
    vector<unsigned char>data = { static_cast<unsigned char>(0xB0 | (channel & 0x0F)) ,static_cast<unsigned char>(controller & 0x7F) ,static_cast<unsigned char>(value & 0x7F) };
    return MIDIMsg(data, timestamp);
}

MIDIMsg MIDIMsg::generateTimbreChange(int program, int channel, double timestamp)
{
    if (program < -1 || program > 127)
        throw invalid_argument("Invalid Program Number");
    if (channel < -1 || channel>15)
        throw invalid_argument("Invalid Channel Number");
    vector<unsigned char>data = { static_cast<unsigned char>(0xC0 | (channel & 0x0F)) ,static_cast<unsigned char>(program & 0x7F) };
    return MIDIMsg(data, timestamp);
}

MIDIMsg MIDIMsg::generatePitchBend(int value, int channel, double timestamp)
{
    if (channel < 0 || channel>15)
        throw invalid_argument("Invalid Channel Number");
    if (value < 0 || value > 0x3FFF)
        throw std::invalid_argument("Invalid Pitch Bend Value");
    unsigned char lsb = static_cast<unsigned char>(value & 0x7F);          // 低字节
    unsigned char msb = static_cast<unsigned char>((value >> 7) & 0x7F);   // 高字节
    vector<unsigned char> data = { static_cast<unsigned char>(0xE0 | (channel & 0x0F)), lsb, msb };
    return MIDIMsg(data, timestamp);
}

pair<MIDIMsg, MIDIMsg> MIDIMsg::generateBankChange(int banknum, int channel, double timestamp)
{
    if (banknum < 0)
        throw invalid_argument("Invalid Bank Number");
    unsigned char lsb = static_cast<unsigned char>(banknum & 0x7F);
    unsigned char msb = static_cast<unsigned char>((banknum >> 7) & 0x7F);
    vector<unsigned char>data1 = { static_cast<unsigned char>(0xB0 | (channel & 0x0F)) ,static_cast<unsigned char>(0x00),lsb };
    vector<unsigned char>data2 = { static_cast<unsigned char>(0xB0 | (channel & 0x0F)), static_cast<unsigned char>(0x20),msb };
    return { MIDIMsg(data1,timestamp),MIDIMsg(data2,timestamp) };
}
