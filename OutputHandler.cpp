#include"OutputHandler.h"
#include<iostream>

using namespace std;

string file_path = "C:/Users/Housh/Downloads/GeneralUser_GS_v2.0.2--doc_r4/GeneralUser-GS/GeneralUser-GS.sf2";

SynthOutput::SynthOutput()
{
    settings = new_fluid_settings();
    if (settings==NULL)
        throw OutputException("Fail to create Synth Settings");
    synth = new_fluid_synth(settings);
    if (synth==NULL)
    {
        delete_fluid_settings(settings);
        throw OutputException("Fail to create Synthesizer");
    }
    int sfid = fluid_synth_sfload(synth, file_path.c_str(), 1);//是否需要用户自定义音色文件路径？
    if (sfid == FLUID_FAILED)
    {
        delete_fluid_settings(settings);
        delete_fluid_synth(synth);
        throw OutputException("Fail to load Soundfont");
    }
    adriver = new_fluid_audio_driver(settings, synth);
    if (!adriver)
    {
        delete_fluid_settings(settings);
        delete_fluid_synth(synth);
        throw OutputException("Fail to create Audio Driver");
    }
}

SynthOutput::~SynthOutput()
{
    delete_fluid_audio_driver(adriver);
    delete_fluid_synth(synth);
    delete_fluid_settings(settings);
}

void SynthOutput::send(const MIDIMsg& msg)
{
    unsigned char status;
    unsigned int channel;
    try
    {
        status = msg.getStatus();
    }
    catch (const MIDIException& e)
    {
        cerr << e.what() << endl;
    }
    try
    {
        channel = msg.getChannel();
    }
    catch (const invalid_argument& e)
    {
        cerr << e.what() << endl;
    }
    int note, velocity;
    switch (static_cast<unsigned int>(status))
    {
    case 0x80:
        try
        {
            note = msg.getNoteNumber();
        }
        catch (const MIDIException& e)
        {
            cerr << e.what() << endl;
        }
        fluid_synth_noteoff(synth, channel, note);
        break;
    case 0x90:
        try
        {
            note = msg.getNoteNumber();
        }
        catch (const MIDIException& e)
        {
            cerr << e.what() << endl;
        }
        try
        {
            velocity = msg.getVelocity();
        }
        catch (const MIDIException& e)
        {
            cerr << e.what() << endl;
        }
        fluid_synth_noteon(synth, channel, note, velocity);
        break;
    case 0xB0:
        int control, controlval;
        try
        {
            control = msg.getControllerNumber();
        }
        catch (const MIDIException& e)
        {
            cerr << e.what() << endl;
        }
        try
        {
            controlval = msg.getControllerValue();
        }
        catch (const MIDIException& e)
        {
            cerr << e.what() << endl;
        }
        fluid_synth_cc(synth, channel, control, controlval);
        break;
    case 0xC0:
        int program;
        try
        {
            program = msg.getTimbreChange();
        }
        catch (const MIDIException& e)
        {
            cerr << e.what() << endl;
        }
        fluid_synth_program_change(synth, channel, program);
        break;
    case 0xD0:
        int pressure;
        try
        {
            pressure = msg.getAftertouchForce();
        }
        catch (const MIDIException& e)
        {
            cerr << e.what() << endl;
        }
        fluid_synth_channel_pressure(synth, channel, pressure);
        break;
    case 0xE0:
        int pitchbend;
        try
        {
            pitchbend = msg.getPitchBend();
        }
        catch (const MIDIException& e)
        {
            cerr << e.what() << endl;
        }
        fluid_synth_pitch_bend(synth, channel, pitchbend);
        break;
    default:
        throw OutputException("Invalid Synth Command");
    }
}
