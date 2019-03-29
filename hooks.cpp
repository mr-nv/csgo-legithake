#include "hooks.h"

CVMTManager* clienthook;
CVMTManager* clientmodehook;
CVMTManager* panelhook;

unsigned long text_font;
bool disabledrawing;

void __fastcall hkdFrameStageNotify( void* ecx, void*, ClientFrameStage_t stage )
{
	clienthook->GetOriginal<FrameStageNotifyFn>( 37 )( ecx, stage );

	if( engine->IsInGame( ) && engine->IsConnected( ) && me )
		lagcompensation->FrameStageNotify( stage );
}

bool __fastcall hkdCreateMove( void* ecx, void* edx, float input, CUserCmd * cmd )
{
	auto o = clientmodehook->GetOriginal<CreateMoveFn>( 24 )( ecx, input, cmd );
	if( !cmd || !cmd->command_number ) return o;

	_cmd = cmd;

	me = entitylist->GetClientEntity( engine->GetLocalPlayer( ) );
	if( !me || !_cmd || ( _cmd != cmd ) ) return o;

	if( vars->bunnyhop )
	{
		if( chk( cmd->buttons, IN_JUMP ) && !chk( me->GetFlags( ), FL_ONGROUND ) )
			del( cmd->buttons, IN_JUMP );
	}

#ifdef USE_ENGINE_PREDICTION
	static int oldtick = 0;
	static CUserCmd* oldcmd = nullptr;
	oldtick = ( !oldcmd || oldcmd->hasbeenpredicted ) ? me->GetTickBase( ) : oldtick + 1;
	oldcmd = cmd;

	auto saved_tickbase = me->GetTickBase( );

	me->GetTickBase( ) = oldtick;

	static auto gamemovement = ( void* )( ( ( CreateInterfaceFn )GetProcAddress( GetModuleHandleA( "client_panorama" ), "CreateInterface" ) ) )( "GameMovement001", 0 );
	static auto prediction = ( void* )( ( ( CreateInterfaceFn )GetProcAddress( GetModuleHandleA( "client_panorama" ), "CreateInterface" ) ) )( "VClientPrediction001", 0 );
	static auto movehelper = **( void*** )( PatternScan( GetModuleHandleA( "client_panorama" ), "8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01" ) + 2 );

	auto oldcurtime = 0.f;
	auto oldframetime = 0.f;

	{ // enginepred start
		oldcurtime = globals->curtime;
		oldframetime = globals->frametime;

		globals->curtime = me->GetTickBase( ) * globals->interval_per_tick;
		globals->frametime = globals->interval_per_tick;

		// StartTrackPredictionErrors
		GetVFunc<void( __thiscall* )( void*, CBasePlayer* )>( gamemovement, 3 )( gamemovement, me );

		CMoveData data;
		memset( &data, 0, sizeof( CMoveData ) );

		// SetHost
		GetVFunc<void( __thiscall* )( void*, CBasePlayer* )>( movehelper, 1 )( movehelper, me );

		// SetupMove
		GetVFunc<void( __thiscall* )( void*, CBasePlayer*, CUserCmd*, void*, CMoveData* )>( prediction, 20 )( prediction, me, cmd, movehelper, &data );

		// ProcessMovement
		GetVFunc<void( __thiscall* )( void*, CBasePlayer*, CMoveData* )>( gamemovement, 1 )( gamemovement, me, &data );

		// FinishMove
		GetVFunc<void( __thiscall* )( void*, CBasePlayer*, CUserCmd*, CMoveData* )>( prediction, 21 )( prediction, me, cmd, &data );
	}
#endif

	if( chk( cmd->buttons, IN_ATTACK ) && vars->backtrack )
		lagcompensation->Start( );

#ifdef USE_ENGINE_PREDICTION
	{ // enginepred end
		// FinishTrackPredictionErrors
		GetVFunc<void( __thiscall* )( void*, CBasePlayer* )>( gamemovement, 4 )( gamemovement, me );

		// SetHost
		GetVFunc<void( __thiscall* )( void*, CBasePlayer* )>( movehelper, 1 )( movehelper, nullptr );

		globals->curtime = oldcurtime;
		globals->frametime = oldframetime;
	}

	me->GetTickBase( ) = saved_tickbase;
#endif

	return false;
}

void __fastcall hkdPaintTraverse( void* ecx, void*, unsigned int curpanel, bool repaint, bool force )
{
	panelhook->GetOriginal<PaintTraverseFn>( 41 )( ecx, curpanel, repaint, force );

	static unsigned int panel_id = 0;
	static bool draw = false;
	if( !panel_id )
	{
		if( !strcmp( panel->GetName( curpanel ), "FocusOverlayPanel" ) )
			panel_id = curpanel;
	}

	if( curpanel != panel_id ) return;

	static DWORD old_draw_tick = GetTickCount( ) + 400;
	if( menu::IsKeyDown( VK_NUMPAD4 ) && ( GetTickCount( ) > old_draw_tick ) )
	{
		disabledrawing = !disabledrawing;
		old_draw_tick = GetTickCount( ) + 400;
	}

	draw = !draw;
	if( !draw || disabledrawing ) return;

	if( vars->antiscreen && engine->IsTakingScreenshot( ) ) return;

	static auto f = ( menu::Init( ), false );

	static int sw, sh, sw2, sh2;
	engine->GetScreenSize( sw, sh );
	if( sw != sw2 || sh != sh2 )
	{
		menu::font = 0;
		menu::smallfont = 0;
		sw2 = sw;
		sh2 = sh;
	}

	menu::Draw( );

	if( engine->IsInGame( ) && engine->IsConnected( ) && me )
	{
		if( vars->esp_box || vars->esp_health )
			esp->Draw( );

		if( vars->backtrack && vars->backtrack_draw )
			lagcompensation->Draw( );
	}
}