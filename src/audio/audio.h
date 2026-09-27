#pragma once

#include "../pch.h"

#include "menu/menu.h"
#include <string>

class CAudioStream;
class C3DAudioStream;
class CPlaceable;

class Audio : public IAudio
{
  private:
    bool is3D = false;

  public:
    CAudioStream *stream = nullptr;

    Audio(std::string filePath, bool in3D);
    ~Audio() override;

    void LoadFromSource(std::string src, bool in3d);
    void DestroyStream();
    C3DAudioStream *Get3DStream();

    //

    void Play() override;
    void Stop() override;
    void SetLoop(bool loop) override;
    bool Finished() override;
    bool Loaded() override;
    bool Is3D() override;
    void AttachToCPlaceable(CPlaceable *ptr) override;
    void SetVolume(float volume) override;

    static void PlayOnce(const std::string &filePath);
};