#ifndef VOLUME_H
#define VOLUME_H
#include <combaseapi.h>
#include <ole2.h>
#include <QObject>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audioclient.h>
#pragma comment(lib, "ole32.lib")
class Volume
{
public:
    Volume();
    void SetSystemVolume(int);
    int  GetCurrentVolume();
};
#endif // VOLUME_H
