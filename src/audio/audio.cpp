#include "audio.h"

#include "../utils/utils.h"

#include "soundSystem/C3DAudioStream.h"
#include "soundSystem/CAudioStream.h"
#include "soundSystem/CSoundSystem.h"

Audio::Audio(std::string filePath, bool in3D) { LoadFromSource(filePath, in3D); }

Audio::~Audio() {}

void Audio::LoadFromSource(std::string src, bool in3d)
{
    is3D = in3d;

    // menuDebug->AddLine("Loading audio " + src);

    if (!BASS)
    {
        LOGE("~r~Cant load audio. BASS was not loaded!", src.c_str());
        return;
    }

    if (!FileExists(src))
    {
        LOGE("~r~Audio file not founds %s", src.c_str());
        return;
    }

    stream = soundsys->LoadStream(src.c_str(), in3d);

    if (stream)
    {
        LOGI("~g~Audio loaded!");
    }
    else
    {
        LOGE("~r~Could not load audio file: %s", src.c_str());
    }
}

void Audio::DestroyStream()
{
    if (stream == nullptr)
        return;

    soundsys->UnloadStream(stream);
    stream = nullptr;
}

C3DAudioStream *Audio::Get3DStream()
{
    if (!is3D)
        return nullptr;

    return (C3DAudioStream *)stream;
}

void Audio::Play()
{
    if (!Loaded())
        return;

    stream->Play();
}

void Audio::Stop()
{
    if (!Loaded())
        return;

    stream->Stop();
}

void Audio::SetLoop(bool loop)
{
    if (!Loaded())
        return;

    stream->Loop(loop);
}

bool Audio::Finished()
{
    if (!Loaded())
        return true;

    return stream->finished;
}

bool Audio::Loaded()
{
    if (stream == nullptr)
        return false;
    return stream->OK;
}

bool Audio::Is3D() { return is3D; }

void Audio::AttachToCPlaceable(CPlaceable *ptr)
{
    auto stream = Get3DStream();
    if (!stream)
        return;

    auto vehicle = (CPlaceable *)ptr;

    stream->Link(vehicle);
}

void Audio::SetVolume(float volume)
{
    if (stream == nullptr)
        return;
    stream->SetVolume(volume);
}

void Audio::PlayOnce(const std::string &filePath)
{
    static std::unordered_map<std::string, Audio *> cache;

    auto it = cache.find(filePath);

    if (it == cache.end())
    {
        auto audio = new Audio(filePath, false);
        cache[filePath] = audio;
        audio->Play();
        return;
    }

    it->second->Play();
}