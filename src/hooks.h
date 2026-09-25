#pragma once

#include "pch.h"

struct CSprite2d;

inline uintptr_t* pPedPool;

inline int (*GetPedRef)(void*);
inline int (*OS_ScreenGetWidth)(void);
inline int (*OS_ScreenGetHeight)(void);
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

void DoHooks();