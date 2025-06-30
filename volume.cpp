#include "volume.h"

Volume::Volume()
{

}

void Volume::SetSystemVolume(int value)
{
    HRESULT hr;
    IMMDeviceEnumerator* pDeviceEnumerator=0;
    IMMDevice* pDevice=0;
    IAudioEndpointVolume* pAudioEndpointVolume=0;
    IAudioClient* pAudioClient=0;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),NULL,CLSCTX_ALL,__uuidof(IMMDeviceEnumerator),(void**)&pDeviceEnumerator);

    hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender,eMultimedia,&pDevice);

    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume),CLSCTX_ALL,NULL,(void**)&pAudioEndpointVolume);

    hr = pDevice->Activate(__uuidof(IAudioClient),CLSCTX_ALL,NULL,(void**)&pAudioClient);

    if(value == -1)
        pAudioEndpointVolume->SetMute(FALSE,NULL);
    else if(value == -2)
        pAudioEndpointVolume->SetMute(TRUE,NULL);
    else{
        value = (value > 100) ? 100 : (value < 0) ? 0: value;
        float fVolume;
        fVolume = value/100.0f;
        hr = pAudioEndpointVolume->SetMasterVolumeLevelScalar(fVolume,&GUID_NULL);
    }
    pAudioClient->Release();
    pAudioEndpointVolume->Release();
    pDevice->Release();
    pDeviceEnumerator->Release();
}


int Volume::GetCurrentVolume()
{
    HRESULT hr;
    IMMDeviceEnumerator* pDeviceEnumerator = nullptr;
    IMMDevice* pDevice = nullptr;
    IAudioEndpointVolume* pAudioEndpointVolume = nullptr;
    IAudioClient* pAudioClient = nullptr;

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
                          __uuidof(IMMDeviceEnumerator), (void**)&pDeviceEnumerator);
    if (FAILED(hr)) return 0;

    hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
    if (FAILED(hr)) return 0;

    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pAudioEndpointVolume);
    if (FAILED(hr)) return 0;

    float fVolume = 0.0f;
    hr = pAudioEndpointVolume->GetMasterVolumeLevelScalar(&fVolume);

    // 释放资源
    if (pAudioEndpointVolume) pAudioEndpointVolume->Release();
    if (pDevice) pDevice->Release();
    if (pDeviceEnumerator) pDeviceEnumerator->Release();

    return static_cast<int>(fVolume * 100);  // 转换为百分比
}
