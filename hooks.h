#pragma once
#include "main.h"

extern CVMTManager* clienthook;
extern CVMTManager* clientmodehook;
extern CVMTManager* panelhook;

extern unsigned long text_font;
extern bool disabledrawing;

typedef void( __thiscall* FrameStageNotifyFn )( void*, ClientFrameStage_t );
typedef bool( __thiscall* CreateMoveFn )( void*, float, CUserCmd* );
typedef void( __thiscall* PaintTraverseFn )( void*, unsigned int, bool, bool );

extern void __fastcall hkdFrameStageNotify( void* ecx, void*, ClientFrameStage_t stage );
extern bool __fastcall hkdCreateMove( void* ecx, void* edx, float input, CUserCmd* cmd );
extern void __fastcall hkdPaintTraverse( void* ecx, void*, unsigned int curpanel, bool repaint, bool force );