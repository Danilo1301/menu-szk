#pragma once

#include "CAudioStream.h"

class C3DAudioStream : public CAudioStream
{
    friend class CSoundSystem;
    C3DAudioStream(const C3DAudioStream &);

  protected:
    CPlaceable *link;
    BASS_3DVECTOR position;

  public:
    C3DAudioStream(const char *src);
    virtual ~C3DAudioStream();
    // overloaded actions
    virtual void Set3DPosition(const CVector &pos);
    virtual void Set3DPosition(float x, float y, float z);
    virtual void Link(CPlaceable *placeable = NULL);
    virtual void Process();
};