#pragma once

#include "aml-psdk/game_sa/Events.h"
#include "pch.h"

struct CSprite2d;
struct CWidget;

inline uintptr_t* pPedPool;
inline bool* userPaused;
inline bool* codePaused;
inline CCamera* camera;
inline CWidget** m_pWidgets = nullptr;

inline int (*GetPedRef)(void*);
inline void (*CSprite2d_DrawRect)(CRect const& posn, CRGBA const& color);
inline void (*FontSetOrientation)(unsigned char);
inline void (*FontSetColor)(CRGBA*);
inline void (*FontSetBackground)(unsigned char, unsigned char);
inline void (*FontSetWrapx)(float);
inline void (*FontSetStyle)(unsigned char);
inline void (*FontSetScale)(float w, float h);
inline void (*FontSetProportional)(unsigned char);
inline void (*FontSetDropShadowPosition)(char);
inline void (*FontSetEdge)(char);
inline void (*FontSetDropColor)(CRGBA*);
inline void (*FontPrintString)(float, float, unsigned short*);
inline void (*AsciiToGxtChar)(const char* txt, unsigned short* ret);
inline void (*RenderFontBuffer)(void);
inline void (*CSprite2d_DrawSprite)(CSprite2d*, CRect const&, CRGBA const&);
inline bool (*DisplayThisBlip)(int, char);
inline void (*TransformRealWorldPointToRadarSpace)(CVector2D&, CVector2D const&);
inline float (*LimitRadarPoint)(CVector2D&);
inline void (*TransformRadarPointToScreenSpace)(CVector2D&, CVector2D const&);
inline bool (*CSprite_CalcScreenCoors)(RwV3d const& posn, RwV3d* out, float* w, float* h, bool checkMaxVisible, bool checkMinVisible);
inline bool* CTouchInterface_m_bTouchDown;
inline CVector2D* m_vecCachedPos;

inline bool IsGamePaused()
{
    return *userPaused || *codePaused;
};

void DoHooks();