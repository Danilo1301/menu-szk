#include "CAudioStream.h"
#include "CSoundSystem.h"
#include "mod/logger.h"

extern CSoundSystem *soundsys;

CAudioStream::CAudioStream() : streamInternal(0), state(no), OK(false) {}

CAudioStream::CAudioStream(const char *src) : state(no), OK(false)
{
    unsigned flags = BASS_SAMPLE_SOFTWARE;
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
        logger->Error("Loading audiostream failed. Error code: %d\nSource: \"%s\"", BASS->ErrorGetCode(), src);
    }
    else
        OK = true;
}

CAudioStream::~CAudioStream()
{
    if (streamInternal)
        BASS->StreamFree(streamInternal);
}

void CAudioStream::Play()
{
    BASS->ChannelPlay(streamInternal, true);
    state = playing;
    everPlayed = true;
    finished = false;
}

void CAudioStream::Pause(bool change_state)
{
    BASS->ChannelPause(streamInternal);
    if (change_state)
        state = paused;
}

void CAudioStream::Stop()
{
    BASS->ChannelPause(streamInternal);
    BASS->ChannelSetPosition(streamInternal, 0, BASS_POS_BYTE);
    state = paused;
    finished = true;
}

void CAudioStream::Resume()
{
    BASS->ChannelPlay(streamInternal, false);
    state = playing;
}

uint64_t CAudioStream::GetLength()
{
    return (uint64_t)BASS->ChannelBytes2Seconds(streamInternal, BASS->ChannelGetLength(streamInternal, BASS_POS_BYTE));
}

int CAudioStream::GetState()
{
    if (state == stopped)
        return -1;
    switch (BASS->ChannelIsActive(streamInternal))
    {
    case BASS_ACTIVE_STOPPED:
    default:
        return -1;

    case BASS_ACTIVE_PLAYING:
    case BASS_ACTIVE_STALLED:
        return 1;

    case BASS_ACTIVE_PAUSED:
        return 2;
    };
}

float CAudioStream::GetVolume()
{
    float result;
    if (!BASS->ChannelGetAttribute(streamInternal, BASS_ATTRIB_VOL, &result))
        return -1.0f;
    return result;
}

void CAudioStream::SetVolume(float val) { BASS->ChannelSetAttribute(streamInternal, BASS_ATTRIB_VOL, val); }

void CAudioStream::Loop(bool enable)
{
    BASS->ChannelFlags(streamInternal, enable ? BASS_SAMPLE_LOOP : 0, BASS_SAMPLE_LOOP);
}

void CAudioStream::SetMaxDuration(float seconds) { maxDuration = seconds; }

uint64_t CAudioStream::GetInternal() { return streamInternal; }

void CAudioStream::Process()
{
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

    // --- checar maxDuration ---
    if (state == playing && maxDuration > 0.0f)
    {
        double currentSec =
            BASS->ChannelBytes2Seconds(streamInternal, BASS->ChannelGetPosition(streamInternal, BASS_POS_BYTE));

        if (currentSec >= maxDuration)
        {
            Stop(); // se preferir parar de vez
        }
    }
}

void CAudioStream::Set3DPosition(const CVector &)
{
    logger->Error("Unimplemented CAudioStream::Set3DPosition(const CVector&)");
}

void CAudioStream::Set3DPosition(float, float, float)
{
    logger->Error("Unimplemented CAudioStream::Set3DPosition(float,float,float)");
}

void CAudioStream::Link(CPlaceable *) { logger->Error("Unimplemented CAudioStream::Link(CPlaceable*)"); }
