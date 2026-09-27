#pragma once

#include "audio.h"
#include "src/config.h"

inline void PlayChangePage()
{
    auto audioPath = GetMenuAudioPath("menu_change_page.wav");

    Audio::PlayOnce(audioPath);
}

inline void PlaySelect()
{
    auto audioPath = GetMenuAudioPath("menu_select.wav");

    Audio::PlayOnce(audioPath);
}

inline void PlayTestMp3()
{
    auto audioPath = GetMenuAudioPath("creatorshome_digital_click.mp3");

    Audio::PlayOnce(audioPath);
}