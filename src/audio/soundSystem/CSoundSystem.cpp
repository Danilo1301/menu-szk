#include "CSoundSystem.h"
#include "../../hooks.h"
#include "C3DAudioStream.h"
#include "aml-psdk/game_sa/entity/Placeable.h"
#include "mod/logger.h"

BASS_3DVECTOR pos(0, 0, 0);
BASS_3DVECTOR vel(0, 0, 0);
BASS_3DVECTOR front(0, -1.0, 0);
BASS_3DVECTOR top(0, 0, 1.0);

BASS_3DVECTOR bass_tmp(0.0f, 0.0f, 0.0f);
BASS_3DVECTOR bass_tmp2(0.0f, 0.0f, 0.0f);
BASS_3DVECTOR bass_tmp3(0.0f, 0.0f, 0.0f);

bool CSoundSystem::Init()
{
    // default: BASS->Set3DFactors(1.0f, 0.3f, 1.0f)
    // https://managedbass.github.io/api/ManagedBass.Bass.html#ManagedBass_Bass_Set3DFactors_System_Single_System_Single_System_Single_

    if (BASS->Set3DFactors(1.0f, 2.0f, 1.0f) && BASS->Set3DPosition(&pos, &vel, &front, &top))
    {
        logger->Info("Initializing SoundSystem...");

        // Can we use floating-point (HQ) audio streams?
        uint32_t floatable; // floating-point channel support? 0 = no, else yes
        if ((floatable = BASS->StreamCreate(44100, 1, BASS_SAMPLE_FLOAT, NULL, NULL)))
        {
            logger->Info("Floating-point audio is supported!");
            bUseFPAudio = true;
            BASS->StreamFree(floatable);
        }
        else
        {

            logger->Info("Floating-point audio is not supported!");
        }

        initialized = true;
        BASS->Apply3D();
        return true;
    }
    logger->Error("Could not initialize SoundSys");
    return false;
}

CAudioStream *CSoundSystem::LoadStream(const char *filename, bool in3d)
{
    CAudioStream *result = in3d ? new C3DAudioStream(filename) : new CAudioStream(filename);
    if (result->OK)
    {
        streams.insert(result);
        return result;
    }
    delete result;
    return NULL;
}

void CSoundSystem::UnloadStream(CAudioStream *stream)
{
    if (streams.erase(stream))
        delete stream;
    else
        logger->Error("Unloading of stream that is not in a list of loaded streams");
}

void CSoundSystem::UnloadAllStreams()
{
    std::for_each(streams.begin(), streams.end(), [](CAudioStream *stream) { delete stream; });
    streams.clear();
}

void CSoundSystem::ResumeStreams()
{
    paused = false;
    std::for_each(streams.begin(), streams.end(),
        [](CAudioStream *stream)
        {
            if (stream->state == CAudioStream::playing)
                stream->Resume();
        });
}

void CSoundSystem::PauseStreams()
{
    paused = true;
    std::for_each(streams.begin(), streams.end(),
        [](CAudioStream *stream)
        {
            if (stream->state == CAudioStream::playing)
                stream->Pause(false);
        });
}

void CSoundSystem::Update()
{
    if (*userPaused || *codePaused) // covers menu pausing, no disc in drive pausing (KILL MAN: disc on a phone), etc.
    {
        if (!paused)
            PauseStreams();
    }
    else
    {
        if (paused)
            ResumeStreams();

        auto cameraPlaceable = (CPlaceable *)camera;

        // CMatrix* pMatrix = nGameLoaded == 1 ? camera->GetCamMatVC() : camera->GetMatSA();
        CMatrix *pMatrix = cameraPlaceable->GetMatrix();
        CVector *pVec = &pMatrix->pos;

        bass_tmp = {pVec->y, pVec->z, pVec->x};
        bass_tmp2 = {pMatrix->at.y, pMatrix->at.z, pMatrix->at.x};
        bass_tmp3 = {pMatrix->up.y, pMatrix->up.z, pMatrix->up.x};
        BASS->Set3DPosition(&bass_tmp, nullptr, pMatrix ? &bass_tmp2 : nullptr, pMatrix ? &bass_tmp3 : nullptr);

        // process all streams
        for (auto it = streams.begin(); it != streams.end();)
        {
            CAudioStream *stream = *it;
            stream->Process(); // atualiza estado

            if (stream->GetState() == -1 && stream->everPlayed && !stream->finished)
            {
                stream->finished = true;
            }

            ++it;
        }

        // apply above changes
        BASS->Apply3D();
    }
}
