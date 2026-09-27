#pragma once

#include "bass/ibass.h"
#include "set"

inline IBASS *BASS = nullptr;

class CAudioStream;

class CSoundSystem
{
    std::set<CAudioStream *> streams;
    BASS_INFO SoundDevice;
    bool initialized;
    int forceDevice;
    bool paused;

  public:
    bool bUseFPAudio;

  public:
    bool Init();
    inline bool Initialized() { return initialized; }
    CSoundSystem() : initialized(false), forceDevice(-1), paused(false), bUseFPAudio(false)
    {
        // TODO: give to user an ability to force a sound device to use (ini-file or cmd-line?)
        // ANDROID: we dont need that ^
    }
    ~CSoundSystem()
    {
        // TRACE("Closing SoundSystem...");
        UnloadAllStreams();
        if (initialized)
        {
            // TRACE("Freeing BASS library");
            // BASS->Free();
            initialized = false;
        }
        // TRACE("SoundSystem closed!");
    }
    CAudioStream *LoadStream(const char *filename, bool in3d = false);
    void PauseStreams();
    void ResumeStreams();
    void UnloadStream(CAudioStream *stream);
    void UnloadAllStreams();
    void Update();
};

inline CSoundSystem *soundsys = new CSoundSystem();