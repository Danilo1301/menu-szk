#pragma once

#include "aml-psdk/game_sa/entity/Placeable.h"
#include "aml-psdk/gta_base/Vector.h"
#include <cstdint>

#include "bass/ibass.h"

class CAudioStream
{
    friend class CSoundSystem;
    CAudioStream(const CAudioStream &);

  protected:
    uint64_t streamInternal;
    enum eStreamState
    {
        no,
        playing,
        paused,
        stopped,
    } state;
    CAudioStream();

  public:
    bool OK;
    bool destroyOnEnd = false;
    bool everPlayed = false;
    bool finished = false;
    float maxDuration = -1.0f;
    CAudioStream(const char *src);
    virtual ~CAudioStream();
    // actions on streams
    void Play();
    void Pause(bool change_state = true);
    void Stop();
    void Resume();
    uint64_t GetLength();
    int GetState();
    float GetVolume();
    void SetVolume(float val);
    void Loop(bool enable);
    void SetMaxDuration(float seconds);
    uint64_t GetInternal();
    // overloadable actions
    virtual void Set3DPosition(const CVector &pos);
    virtual void Set3DPosition(float x, float y, float z);
    virtual void Link(CPlaceable *placeable = NULL);
    virtual void Process();
};