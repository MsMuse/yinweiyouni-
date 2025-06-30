#include"InputHandler.h"

using namespace std;

MIDIMsg MouseInput::handleInput()
{
    std::lock_guard<std::mutex> lock(mtx);
    if (InputList.empty()) {
        throw std::runtime_error("InputList is empty");
    }
    MIDIMsg temp = InputList[0];
    InputList.erase(InputList.begin());
    return temp;
}

void MouseInput::getInput(int n, int status)
{
    std::lock_guard<std::mutex> lock(mtx);
    if (status == 0)
    {
        InputList.insert(InputList.begin(), MIDIMsg::generateNoteOn(n));
    }
    else
    {
        InputList.insert(InputList.begin(), MIDIMsg::generateNoteOff(n));
    }
}
