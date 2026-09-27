#include "C3DAudioStream.h"
#include "CSoundSystem.h"
#include "mod/logger.h"

extern CSoundSystem *soundsys;

C3DAudioStream::C3DAudioStream(const char *src) : CAudioStream(), link(NULL)
{
    unsigned flags = BASS_SAMPLE_3D | BASS_SAMPLE_MONO | BASS_SAMPLE_SOFTWARE;
    if (soundsys->bUseFPAudio)
        flags |= BASS_SAMPLE_FLOAT;

    /*
    if (!(streamInternal = BASS->StreamCreateURL(src, 0, flags, NULL)) && !(streamInternal =
    BASS->StreamCreateFile(false, src, 0, 0, flags)) &&
        !(streamInternal = BASS->StreamCreateFile(false, (sGameRoot + src).c_str(), 0, 0, flags)) &&
        !(streamInternal = BASS->StreamCreateFile(false, (std::string(cleo->GetCleoStorageDir()) + "/" + src).c_str(),
    0, 0, flags)))
    {
    */
    if (!(streamInternal = BASS->StreamCreateFile(false, src, 0, 0, flags)))
    {
        logger->Error("Loading 3D audiostream failed. Error code: %d\nSource: \"%s\"", BASS->ErrorGetCode(), src);
    }
    else
    {
        BASS->ChannelSet3DAttributes(streamInternal, 0, -1.0, -1.0, -1, -1, -1.0);
        OK = true;
    }
}

C3DAudioStream::~C3DAudioStream()
{
    if (streamInternal)
        BASS->StreamFree(streamInternal);
}

void C3DAudioStream::Set3DPosition(const CVector &pos)
{
    position.x = pos.y;
    position.y = pos.z;
    position.z = pos.x;
    link = NULL;
    BASS->ChannelSet3DPosition(streamInternal, &position, NULL, NULL);
}

void C3DAudioStream::Set3DPosition(float x, float y, float z)
{
    position.x = y;
    position.y = z;
    position.z = x;
    link = NULL;
    BASS->ChannelSet3DPosition(streamInternal, &position, NULL, NULL);
}

void C3DAudioStream::Link(CPlaceable *placeable) { link = placeable; }

void C3DAudioStream::Process()
{
    // update playing position of the linked object
    switch (BASS->ChannelIsActive(streamInternal))
    {
    case BASS_ACTIVE_PAUSED:
        state = paused;
        break;

    case BASS_ACTIVE_PLAYING:
    case BASS_ACTIVE_STALLED:
        state = playing;
        break;

    case BASS_ACTIVE_STOPPED:
        state = stopped;
        break;
    }

    if (state == playing)
    {
        if (link)
        {
            // CVector* pVec = nGameLoaded==1 ? link->GetPosVC() : link->GetPosSA();
            CVector *pVec = link->GetPosition();
            position.x = pVec->y;
            position.y = pVec->z;
            position.z = pVec->x;
        }
        BASS->ChannelSet3DPosition(streamInternal, &position, NULL, NULL);
    }
}