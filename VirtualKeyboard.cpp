#include"VirtualKeyboard.h"

using namespace std;

Timbre::Timbre(int channel_, int bank_ , int program_, int lowpitch_ , int highpitch_, int volume_ ) :
    channel(channel_), bank(bank_), program(program_), lowpitch(lowpitch_), highpitch(highpitch_), volume(volume_) {}

VirtualKeyboard::VirtualKeyboard(std::shared_ptr<InputHandler> i, std::shared_ptr<OutputHandler> o) :
    output(NULL), input(NULL), octavetranspose(0), semitonetranspose(0), timbre1(0, 0, 0, 0, 127, 100), timbre2(1), sustainpedal(false),
    reverb_level(0), reverb_room(0.5), chorus_count(0), chorus_level(0), chorus_speed(0.1), chorus_depth(0), split(false),
    split_point(60)
{
    setInput(i);
    setOutput(o);
    //设置好timbre1
    setTimbre1(0, 0);
    std::shared_ptr<SynthOutput> tempPtr = dynamic_pointer_cast<SynthOutput>(output);
    fluid_synth_set_reverb_on(tempPtr->synth,1);
    fluid_synth_set_reverb(tempPtr->synth,reverb_room,0.2,60,0);
    fluid_synth_set_chorus_on(tempPtr->synth,1);
    int result=fluid_synth_set_chorus(tempPtr->synth,chorus_count,chorus_level,chorus_speed,chorus_depth,FLUID_CHORUS_MOD_TRIANGLE );
    if(result==-1)
        throw std::runtime_error("Failed to set chorus parameters");
    fluid_synth_set_gain(tempPtr->synth,1.5);
}

VirtualKeyboard::~VirtualKeyboard()
{

}

void VirtualKeyboard::setInput(std::shared_ptr<InputHandler> i) { input = i; }

void VirtualKeyboard::setOutput(std::shared_ptr<OutputHandler> o) { output = o; }

std::string VirtualKeyboard::getOctave() const
{
    std::string str = to_string(octavetranspose);
    if (octavetranspose > 0)
        str = "+" + str;
    return str;
}

std::string VirtualKeyboard::getSemitone() const
{
    std::string str = to_string(semitonetranspose);
    if (semitonetranspose > 0)
        str = "+" + str;
    return str;
}

void VirtualKeyboard::aOctave()
{
    if (octavetranspose + 1 > 5)
        return;
    octavetranspose++;
}

void VirtualKeyboard::bOctave()
{
    if(octavetranspose-1<-5)
        return;
    octavetranspose--;
}

void VirtualKeyboard::aSemitone()
{
    if (semitonetranspose + 1 > 5)
        return;
    semitonetranspose++;
}

void VirtualKeyboard::bSemitone()
{
    if (semitonetranspose - 1 < -5)
        return;
    semitonetranspose--;
}

void VirtualKeyboard::process()
{
    MIDIMsg msg = input->handleInput();
    unsigned char status;
    unsigned int channel1 = timbre1.channel;
    unsigned int channel2 = timbre2.channel;
    int bank1 = timbre1.bank;
    int bank2 = timbre2.bank;
    try
    {
        status = msg.getStatus();
    }
    catch (const MIDIException& e)
    {
        cerr << e.what() << endl;
    }
    MIDIMsg newmsg;

    int note, velocity, new_note;
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
        try
        {
            velocity = msg.getVelocity();
        }
        catch (const MIDIException& e)
        {
            cerr << e.what() << endl;
        }
        new_note = note + 12 * octavetranspose + semitonetranspose;
        if (bank1 != 14)
        {
            newmsg = MIDIMsg::generateNoteOff(new_note, velocity, channel1);
            if (note_played.count(new_note) != 0)
                note_played.erase(new_note);
            output->send(newmsg);
        }
        if (bank2 != 14)
        {
            newmsg = MIDIMsg::generateNoteOff(new_note, velocity, channel2);
            if (note_played.count(new_note) != 0)
                note_played.erase(new_note);
            output->send(newmsg);
        }
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
        new_note = note + 12 * octavetranspose + semitonetranspose;
        if (bank1 != 14 && new_note >= timbre1.lowpitch && new_note <= timbre1.highpitch)
        {
            newmsg = MIDIMsg::generateNoteOn(new_note, velocity, channel1);
            if (note_played.count(new_note) == 0)
                note_played.insert(new_note);
            output->send(newmsg);
        }
        if (bank2 != 14 && new_note>=timbre2.lowpitch && new_note<=timbre2.highpitch)
        {
            newmsg = MIDIMsg::generateNoteOn(new_note, velocity, channel2);
            if (note_played.count(new_note) == 0)
                note_played.insert(new_note);
            output->send(newmsg);
        }
        break;
    default:
        if (input->name == "midi")
            output->send(msg);
    }
}

const std::set<int>& VirtualKeyboard::getNoteList() const
{
    return note_played;
}

string VirtualKeyboard::getSustain() const
{
    if (sustainpedal)
        return "On";
    else
        return "Off";
}

void VirtualKeyboard::changeSustain()
{
    sustainpedal = !sustainpedal;
    unsigned int channel1 = timbre1.channel;
    unsigned int channel2 = timbre2.channel;
    int bank1 = timbre1.bank;
    int bank2 = timbre2.bank;

    if (sustainpedal)
    {
        if (bank1 != 14)
            output->send(MIDIMsg::generateControllerChange(64, 127, channel1));
        if (bank2 != 14)
            output->send(MIDIMsg::generateControllerChange(64, 127, channel2));
    }
    else
    {
        if (bank1 != 14)
            output->send(MIDIMsg::generateControllerChange(64, 0, channel1));
        if (bank2 != 14)
            output->send(MIDIMsg::generateControllerChange(64, 0, channel2));
    }
}

void VirtualKeyboard::setTimbre1(int bank_, int program_)
{
    timbre1.bank = bank_;
    timbre1.program = program_;
    std::pair<MIDIMsg, MIDIMsg> temp = MIDIMsg::generateBankChange(timbre1.bank, timbre1.channel);
    output->send(temp.first);
    output->send(temp.second);
    output->send(MIDIMsg::generateTimbreChange(timbre1.program, timbre1.channel));
}

void VirtualKeyboard::setTimbre2(int bank_, int program_)
{
    timbre2.bank = bank_;
    timbre2.program = program_;
    std::pair<MIDIMsg, MIDIMsg> temp = MIDIMsg::generateBankChange(timbre2.bank, timbre2.channel);
    output->send(temp.first);
    output->send(temp.second);
    output->send(MIDIMsg::generateTimbreChange(timbre2.program, timbre2.channel));
}

void VirtualKeyboard::setReverbLevel(double level)
{
    if(level>=0 && level<=1)
        reverb_level=level;
    else
        return;
    std::shared_ptr<SynthOutput> tempPtr = dynamic_pointer_cast<SynthOutput>(output);
    if(tempPtr)
    {
        fluid_synth_set_reverb_level(tempPtr->synth,reverb_level);
    }
}

void VirtualKeyboard::setReverbRoom(double room)
{
    if(room>=0 && room<=1)
        reverb_room=room;
    else
        return;
    std::shared_ptr<SynthOutput> tempPtr = dynamic_pointer_cast<SynthOutput>(output);
    if(tempPtr)
    {
        fluid_synth_set_reverb_roomsize(tempPtr->synth,reverb_room);
    }
}

void VirtualKeyboard::setChorusNum(int count)
{
    if(count>=1 && count<=60)
        chorus_count=count;
    else
        return;
    std::shared_ptr<SynthOutput> tempPtr = dynamic_pointer_cast<SynthOutput>(output);
    if(tempPtr)
    {
        fluid_synth_set_chorus_nr(tempPtr->synth,chorus_count);
    }
}

void VirtualKeyboard::setChorusLevel(double level)
{
    if(level>=0 && level<=100)
        chorus_level=level;
    else
        return;
    std::shared_ptr<SynthOutput> tempPtr = dynamic_pointer_cast<SynthOutput>(output);
    if(tempPtr)
    {
        fluid_synth_set_chorus_level(tempPtr->synth,chorus_level);
    }
}

void VirtualKeyboard::setChorusSpeed(double speed)
{
    if(speed>=0.1 && speed<=5.0)
        chorus_speed=speed;
    else
        return;
    std::shared_ptr<SynthOutput> tempPtr = dynamic_pointer_cast<SynthOutput>(output);
    if(tempPtr)
    {
        fluid_synth_set_chorus_speed(tempPtr->synth,chorus_speed);
    }
}

void VirtualKeyboard::setChorusDepth(double depth)
{
    if(depth>=0 && depth<=20)
        chorus_depth=depth;
    else
        return;
    std::shared_ptr<SynthOutput> tempPtr = dynamic_pointer_cast<SynthOutput>(output);
    if(tempPtr)
    {
        fluid_synth_set_chorus_depth(tempPtr->synth,chorus_depth);
    }
}

void VirtualKeyboard::setVolume1(int vol)
{
    if(vol>=0 && vol<=127)
        timbre1.volume=vol;
    else
        return;
    std::shared_ptr<SynthOutput> tempPtr = dynamic_pointer_cast<SynthOutput>(output);
    if(tempPtr)
    {
        fluid_synth_cc(tempPtr->synth, 0, 7, timbre1.volume);
    }
}

void VirtualKeyboard::setVolume2(int vol)
{
    if(vol>=0 && vol<=127)
        timbre2.volume=vol;
    else
        return;
    std::shared_ptr<SynthOutput> tempPtr = dynamic_pointer_cast<SynthOutput>(output);
    if(tempPtr)
    {
        fluid_synth_cc(tempPtr->synth, 1, 7, timbre2.volume);
    }
}

void VirtualKeyboard::change_split()
{
    if(split)
    {
        split=false;
        timbre1.lowpitch=0;
        timbre2.highpitch=127;
    }
    else
    {
        split=true;
        timbre1.lowpitch=split_point;
        timbre2.highpitch=split_point - 1;
    }
}

void VirtualKeyboard::set_split(int point)
{
    if(point>0 && point<127)
        split_point=point;
    else
        return;
    if(split)
    {
        timbre1.lowpitch=split_point;
        timbre2.highpitch=split_point - 1;
    }
}

void VirtualKeyboard::setPitchbend(int value)
{
    if(value>-8192 && value<8192)
    {
        value+=8192;
        std::shared_ptr<SynthOutput> tempPtr = dynamic_pointer_cast<SynthOutput>(output);
        if(timbre1.bank!=14)
            fluid_synth_pitch_bend(tempPtr->synth,0,value);
        if(timbre2.bank!=14)
            fluid_synth_pitch_bend(tempPtr->synth,1,value);
    }
}

void VirtualKeyboard::setModulation(int value)
{
    if(value>=0 && value<128)
    {
        std::shared_ptr<SynthOutput> tempPtr = dynamic_pointer_cast<SynthOutput>(output);
        if(timbre1.bank!=14)
            fluid_synth_cc(tempPtr->synth,0,1,value);
        if(timbre2.bank!=14)
            fluid_synth_cc(tempPtr->synth,1,1,value);
    }
}

void VirtualKeyboard::setResonance(int value)
{
    if(value>=0 && value<=1000)
    {
        std::shared_ptr<SynthOutput> tempPtr = dynamic_pointer_cast<SynthOutput>(output);
        if(timbre1.bank!=14)
            fluid_synth_set_gen(tempPtr->synth,0,GEN_FILTERQ,value);
        if(timbre2.bank!=14)
            fluid_synth_set_gen(tempPtr->synth,1,GEN_FILTERQ,value);
    }
}

void VirtualKeyboard::setCutoff(int value)
{
    if(value>=20 && value<=8000)
    {
        std::shared_ptr<SynthOutput> tempPtr = dynamic_pointer_cast<SynthOutput>(output);
        if(timbre1.bank!=14)
            fluid_synth_set_gen(tempPtr->synth,0,GEN_FILTERFC,value);
        if(timbre2.bank!=14)
            fluid_synth_set_gen(tempPtr->synth,1,GEN_FILTERFC,value);
    }
}

void VirtualKeyboard::setAttack(int value)
{
    if(value>=0 && value<=1000)
    {
        std::shared_ptr<SynthOutput> tempPtr = dynamic_pointer_cast<SynthOutput>(output);
        if(timbre1.bank!=14)
            fluid_synth_set_gen(tempPtr->synth,0,GEN_VOLENVATTACK,value);
        if(timbre2.bank!=14)
            fluid_synth_set_gen(tempPtr->synth,1,GEN_VOLENVATTACK,value);
    }
}

void VirtualKeyboard::setDecay(int value)
{
    if(value>=0 && value<=1000)
    {
        std::shared_ptr<SynthOutput> tempPtr = dynamic_pointer_cast<SynthOutput>(output);
        if(timbre1.bank!=14)
            fluid_synth_set_gen(tempPtr->synth,0,GEN_VOLENVDECAY,value);
        if(timbre2.bank!=14)
            fluid_synth_set_gen(tempPtr->synth,1,GEN_VOLENVDECAY,value);
    }
}

void VirtualKeyboard::setSustain(int value)
{
    if(value>=0 && value<=1000)
    {
        std::shared_ptr<SynthOutput> tempPtr = dynamic_pointer_cast<SynthOutput>(output);
        if(timbre1.bank!=14)
            fluid_synth_set_gen(tempPtr->synth,0,GEN_VOLENVSUSTAIN,value);
        if(timbre2.bank!=14)
            fluid_synth_set_gen(tempPtr->synth,1,GEN_VOLENVSUSTAIN,value);
    }
}

void VirtualKeyboard::setRelease(int value)
{
    if(value>=0 && value<=1000)
    {
        std::shared_ptr<SynthOutput> tempPtr = dynamic_pointer_cast<SynthOutput>(output);
        if(timbre1.bank!=14)
            fluid_synth_set_gen(tempPtr->synth,0,GEN_VOLENVRELEASE,value);
        if(timbre2.bank!=14)
            fluid_synth_set_gen(tempPtr->synth,1,GEN_VOLENVRELEASE,value);
    }
}

Instrument::Instrument(std::string name_, int bank_, int program_):name(QString::fromStdString(name_)),bank(bank_),program(program_){}

QVector<Instrument> VirtualKeyboard::instruments=
    {
        {"None", 14, 0},
        {"Grand Piano", 0, 0},
        {"Bright Grand Piano", 0, 1},
        {"Electric Grand Piano", 0, 2},
        {"Honky-Tonk Piano", 0, 3},
        {"Tine Electric Piano", 0, 4},
        {"FM Electric Piano", 0, 5},
        {"Harpsichord", 0, 6},
        {"Clavinet", 0, 7},
        {"Celeste", 0, 8},
        {"Glockenspiel", 0, 9},
        {"Music Box", 0, 10},
        {"Vibraphone", 0, 11},
        {"Marimba", 0, 12},
        {"Xylophone", 0, 13},
        {"Tubular Bells", 0, 14},
        {"Dulcimer", 0, 15},
        {"Tonewheel Organ", 0, 16},
        {"Percussive Organ", 0, 17},
        {"Rock Organ", 0, 18},
        {"Pipe Organ", 0, 19},
        {"Reed Organ", 0, 20},
        {"Accordion", 0, 21},
        {"Harmonica", 0, 22},
        {"Bandeon", 0, 23},
        {"Nylon Guitar", 0, 24},
        {"Steel Guitar", 0, 25},
        {"Jazz Guitar", 0, 26},
        {"Clean Guitar", 0, 27},
        {"Muted Guitar", 0, 28},
        {"Overdrive Guitar", 0, 29},
        {"Distortion Guitar", 0, 30},
        {"Guitar Harmonics", 0, 31},
        {"Acoustic Bass", 0, 32},
        {"Finger Bass", 0, 33},
        {"Pick Bass", 0, 34},
        {"Fretless Bass", 0, 35},
        {"Slap Bass 1", 0, 36},
        {"Slap Bass 2", 0, 37},
        {"Synth Bass 1", 0, 38},
        {"Synth Bass 2", 0, 39},
        {"Violin", 0, 40},
        {"Viola", 0, 41},
        {"Cello", 0, 42},
        {"Double Bass", 0, 43},
        {"Tremolo Strings", 0, 44},
        {"Pizzicato Strings", 0, 45},
        {"Orchestral Harp", 0, 46},
        {"Timpani", 0, 47},
        {"Fast Strings", 0, 48},
        {"Slow Strings", 0, 49},
        {"Synth Strings 1", 0, 50},
        {"Synth Strings 2", 0, 51},
        {"Concert Choir", 0, 52},
        {"Voice Oohs", 0, 53},
        {"Synth Voice", 0, 54},
        {"Orchestra Hit", 0, 55},
        {"Trumpet", 0, 56},
        {"Trombone", 0, 57},
        {"Tuba", 0, 58},
        {"Muted Trumpet", 0, 59},
        {"French Horns", 0, 60},
        {"Brass Section", 0, 61},
        {"Synth Brass 1", 0, 62},
        {"Synth Brass 2", 0, 63},
        {"Soprano Sax", 0, 64},
        {"Alto Sax", 0, 65},
        {"Tenor Sax", 0, 66},
        {"Baritone Sax", 0, 67},
        {"Oboe", 0, 68},
        {"English Horn", 0, 69},
        {"Bassoon", 0, 70},
        {"Clarinet", 0, 71},
        {"Piccolo", 0, 72},
        {"Flute", 0, 73},
        {"Recorder", 0, 74},
        {"Pan Flute", 0, 75},
        {"Bottle Blow", 0, 76},
        {"Shakuhachi", 0, 77},
        {"Whistle", 0, 78},
        {"Ocarina", 0, 79},
        {"Square Lead", 0, 80},
        {"Saw Lead", 0, 81},
        {"Synth Calliope", 0, 82},
        {"Chiffer Lead", 0, 83},
        {"Charang", 0, 84},
        {"Solo Vox", 0, 85},
        {"5th Saw Wave", 0, 86},
        {"Bass & Lead", 0, 87},
        {"Fantasia", 0, 88},
        {"Warm Pad", 0, 89},
        {"Polysynth", 0, 90},
        {"Space Voice", 0, 91},
        {"Bowed Glass", 0, 92},
        {"Metal Pad", 0, 93},
        {"Halo Pad", 0, 94},
        {"Sweep Pad", 0, 95},
        {"Ice Rain", 0, 96},
        {"Soundtrack", 0, 97},
        {"Crystal", 0, 98},
        {"Atmosphere", 0, 99},
        {"Brightness", 0, 100},
        {"Goblin", 0, 101},
        {"Echo Drops", 0, 102},
        {"Star Theme", 0, 103},
        {"Sitar", 0, 104},
        {"Banjo", 0, 105},
        {"Shamisen", 0, 106},
        {"Koto", 0, 107},
        {"Kalimba", 0, 108},
        {"Bagpipes", 0, 109},
        {"Fiddle", 0, 110},
        {"Shenai", 0, 111},
        {"Tinker Bell", 0, 112},
        {"Agogo", 0, 113},
        {"Steel Drums", 0, 114},
        {"Wood Block", 0, 115},
        {"Talko Drum", 0, 116},
        {"Melodic Tom", 0, 117},
        {"Synth Drum", 0, 118},
        {"Reverse Cymbal", 0, 119},
        {"Fret Noise", 0, 120},
        {"Breath Noise", 0, 121},
        {"Seashore", 0, 122},
        {"Birds", 0, 123},
        {"lephone 1", 0, 124},
        {"Helicopter", 0, 125},
        {"Applause", 0, 126},
        {"Gun Shot", 0, 127},
        {"Synth Bass 101", 1, 38},
        {"Tremolo Strings Mono", 1, 44},
        {"Fast Strings Mono", 1, 48},
        {"Slow Strings Mono", 1, 49},
        {"Concert Choir Mono", 1, 52},
        {"Trumpet 2", 1, 56},
        {"Trombone 2", 1, 57},
        {"Solio French Horn", 1, 60},
        {"Brass Section Mono", 1, 61},
        {"Square Wave", 1, 80},
        {"Saw Wave", 1, 81},
        {"Synth Mallet", 1, 98},
        {"Cut Noise", 1, 120},
        {"Fl. Key Click", 1, 121},
        {"Rain", 1, 122},
        {"Dog", 1, 123},
        {"lephone 2", 1, 124},
        {"Car-Engine", 1, 125},
        {"Laughing", 1, 126},
        {"Machine Gun", 1, 127},
        {"Echo Pan", 2, 102},
        {"String Slap", 2, 120},
        {"Thunder", 2, 122},
        {"Horse Gallop", 2, 123},
        {"Door Creaking", 2, 124},
        {"Car-Stop", 2, 125},
        {"Scream", 2, 126},
        {"Lasergun", 2, 127},
        {"Wind", 3, 122},
        {"Bird 2", 3, 123},
        {"Door", 3, 124},
        {"Car-Pass", 3, 125},
        {"Punch", 3, 126},
        {"Explosion", 3, 127},
        {"Stream", 4, 122},
        {"Scratch", 4, 124},
        {"Car-Crash", 4, 125},
        {"Heart Beat", 4, 126},
        {"Bubbles", 5, 122},
        {"Windchime", 5, 124},
        {"Siren", 5, 125},
        {"Footsteps", 5, 126},
        {"Train", 6, 125},
        {"Jet Plane", 7, 125},
        {"Chorused Tine EP", 8, 4},
        {"Chorused FM EP", 8, 5},
        {"Coupled Harpsichord", 8, 6},
        {"Church Bells", 8, 14},
        {"Detuned Twv. Organ", 8, 16},
        {"Detuned Perc. Organ", 8, 17},
        {"Pipe Organ 2", 8, 19},
        {"Italian Accordion", 8, 21},
        {"Ukulele", 8, 24},
        {"12-String Guitar", 8, 25},
        {"Hawaiian Guitar", 8, 26},
        {"Chorused Clean Gt.", 8, 27},
        {"Funk Guitar", 8, 28},
        {"Feedback Guitar", 8, 30},
        {"Guitar Feedback", 8, 31},
        {"Acid Bass", 8, 38},
        {"Beef FM Bass", 8, 39},
        {"Orchestra Pad", 8, 48},
        {"Synth Strings 3", 8, 50},
        {"Brass Section 2", 8, 61},
        {"Synth Brass 3", 8, 62},
        {"Synth Brass 4", 8, 63},
        {"Sine Wave", 8, 80},
        {"Doctor Solo", 8, 81},
        {"Taisho Koto", 8, 107},
        {"Castanets", 8, 115},
        {"Concert Bass Drum", 8, 116},
        {"Melodic Tom 2", 8, 117},
        {"808 Tom", 8, 118},
        {"Starship", 8, 125},
        {"Carillon", 9, 14},
        {"Burst Noise", 9, 125},
        {"Piano & Str.-Fade", 11, 0},
        {"Piano & Str.-Sus", 11, 1},
        {"Tine & FM EPs", 11, 4},
        {"Piano & FM EP", 11, 5},
        {"Harpsichord noVel", 11, 6},
        {"Tinkling Bells", 11, 8},
        {"Vibraphone No Trem.", 11, 11},
        {"Bell Tower", 11, 14},
        {"Tonewheel Org noVel", 11, 16},
        {"Percussive Org noVel", 11, 17},
        {"Rock Organ noVel", 11, 18},
        {"Pipe Organ noVel", 11, 19},
        {"Red Organ noVel", 11, 20},
        {"Wah Guitar (CC21)", 11, 29},
        {"Techno Bass", 11, 38},
        {"Pulse Bass", 11, 39},
        {"Velo Strings", 11, 49},
        {"Synth Strings 4", 11, 50},
        {"Synth Strings 5", 11, 51},
        {"Brass Section 3", 11, 61},
        {"Whistlin'", 11, 78},
        {"Sawtooth Stab", 11, 81},
        {"Harpsi Pad", 11, 88},
        {"Solar Wind", 11, 89},
        {"Mystery Pad", 11, 96},
        {"Synth Chime", 11, 98},
        {"Bright Saw Stack", 11, 100},
        {"Cymbal Crash", 11, 119},
        {"Filter Snap", 11, 121},
        {"Howling Winds", 11, 122},
        {"Interference", 11, 127},
        {"Bell Piano", 12, 0},
        {"Bell Tine EP", 12, 4},
        {"Coupled Harpsi noVel", 6, 12},
        {"Christmas Bells", 12, 10},
        {"Detun Invl Org noVel", 12, 16},
        {"Detun Perc Org noVel", 12, 17},
        {"Pipe Organ 2 noVel", 12, 19},
        {"Clean Guitar 2", 12, 27},
        {"Mean Saw Bass", 12, 38},
        {"Full Orchestra", 12, 48},
        {"Velo Strings Mono", 12, 49},
        {"Square Lead 2", 12, 80},
        {"Saw Lead 2", 12, 81},
        {"Fantasia 2", 12, 88},
        {"Solar Wind 2", 12, 89},
        {"Tambourine", 12, 119},
        {"White Noise Wave", 12, 122},
        {"Shooting Star", 12, 127},
        {"Woodwind Choir", 13, 48},
        {"Square Lead 3", 13, 80},
        {"Saw Lead 3", 13, 81},
        {"Night Vision", 13, 88},
        {"Mandolin", 16, 25},
        {"Tin Whistle", 24, 75},
        {"Tin Whistle Nm", 25, 75},
        {"Tin Whistle Or", 26, 75},
        {"Standard 1 Kit", 120, 0},
        {"Standard 2 Kit", 120, 1},
        {"Standard 3 Kit", 120, 2},
        {"Room Kit", 120, 8},
        {"Power Kit", 120, 16},
        {"Electronic Kit", 120, 24},
        {"808/909 Kit", 120, 25},
        {"Dance Kit", 120, 26},
        {"Jazz Kit", 120, 32},
        {"Brush Kit", 120, 40},
        {"Orchestral Kit", 120, 48},
        {"SFX Kit", 120, 56},
        {"CM-64/32L Kit", 120, 127},
        {"Standard 1", 128, 0},
        {"Standard 2", 128, 1},
        {"Standard 3", 128, 2},
        {"Room", 128, 8},
        {"Power", 128, 16},
        {"Electronic", 128, 24},
        {"808/909", 128, 25},
        {"Dance", 128, 26},
        {"Jazz", 128, 32},
        {"Brush", 128, 40},
        {"Orchestral", 128, 48},
        {"SEX", 128, 56},
        {"CM-64/32L", 128, 127}
};

QStringList VirtualKeyboard::noteNames = {
    "C-1", "C#-1", "D-1", "D#-1", "E-1", "F-1", "F#-1", "G-1", "G#-1", "A-1", "A#-1", "B-1",
    "C0", "C#0", "D0", "D#0", "E0", "F0", "F#0", "G0", "G#0", "A0", "A#0", "B0",
    "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1",
    "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
    "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",
    "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",
    "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
    "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6",
    "C7", "C#7", "D7", "D#7", "E7", "F7", "F#7", "G7", "G#7", "A7", "A#7", "B7",
    "C8", "C#8", "D8", "D#8", "E8", "F8", "F#8", "G8", "G#8", "A8", "A#8", "B8",
    "C9", "C#9", "D9", "D#9", "E9", "F9", "F#9", "G9", "G#9", "A9", "A#9", "B9"
};
