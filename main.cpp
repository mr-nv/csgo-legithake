#include "main.h"

void* client;
IVEngineClient* engine;
CGlobalVarsBase* globals;
IClientEntityList* entitylist;
void* clientmode;
ICVar* cvar;
IVDebugOverlay* debugoverlay;
ISurface* surface;
IPanel* panel;
IEngineTrace* enginetrace;

CUserCmd* _cmd;
CBasePlayer* me;

unsigned __stdcall Start( void* )
{
	while( !GetModuleHandleA( "serverbrowser" ) ) Sleep( 50 );

	client = ( void* )( ( ( CreateInterfaceFn )GetProcAddress( GetModuleHandleA( "client_panorama" ), "CreateInterface" ) ) )( "VClient018", 0 );
	engine = ( IVEngineClient* )( ( ( CreateInterfaceFn )GetProcAddress( GetModuleHandleA( "engine" ), "CreateInterface" ) ) )( "VEngineClient014", 0 );
	globals = **( CGlobalVarsBase * ** )( ( *( DWORD * * )client )[ 0 ] + 0x1B );
	entitylist = ( IClientEntityList* )( ( ( CreateInterfaceFn )GetProcAddress( GetModuleHandleA( "client_panorama" ), "CreateInterface" ) ) )( "VClientEntityList003", 0 );
	clientmode = *( void** )( PatternScan( GetModuleHandleA( "client_panorama" ), "A1 ? ? ? ? 8B 80 ? ? ? ? 5D" ) + 1 );
	cvar = ( ICVar* )( ( ( CreateInterfaceFn )GetProcAddress( GetModuleHandleA( "vstdlib" ), "CreateInterface" ) ) )( "VEngineCvar007", 0 );
	debugoverlay = ( IVDebugOverlay* )( ( ( CreateInterfaceFn )GetProcAddress( GetModuleHandleA( "engine" ), "CreateInterface" ) ) )( "VDebugOverlay004", 0 );
	surface = ( ISurface* )( ( ( CreateInterfaceFn )GetProcAddress( GetModuleHandleA( "vguimatsurface" ), "CreateInterface" ) ) )( "VGUI_Surface031", 0 );
	panel = ( IPanel* )( ( ( CreateInterfaceFn )GetProcAddress( GetModuleHandleA( "vgui2" ), "CreateInterface" ) ) )( "VGUI_Panel009", 0 );
	enginetrace = ( IEngineTrace* )( ( ( CreateInterfaceFn )GetProcAddress( GetModuleHandleA( "engine" ), "CreateInterface" ) ) )( "EngineTraceClient004", 0 );

	lagcompensation = std::make_unique<CLagCompensation>( );
	esp = std::make_unique<CESP>( );
	vars = new Vars_s( );
	config = std::make_unique<CConfig>( );
	config->SetName( "csgo.ini" );

	// hooking
	clienthook = new CVMTManager( );
	clientmodehook = new CVMTManager( );
	panelhook = new CVMTManager( );

	clienthook->Setup( client );
	clienthook->Hook( 37, hkdFrameStageNotify );

	clientmodehook->Setup( clientmode );
	clientmodehook->Hook( 24, hkdCreateMove );

	panelhook->Setup( panel );
	panelhook->Hook( 41, hkdPaintTraverse );

	return 0;
}

int WINAPI DllMain( HMODULE module, DWORD call, void* reserved )
{
	if( call == DLL_PROCESS_ATTACH )
	{
		DisableThreadLibraryCalls( module );
		CloseHandle( ( HANDLE )_beginthreadex( 0, 0, &Start, 0, 0, 0 ) );
	}
	else if( call == DLL_PROCESS_DETACH )
		_endthreadex( 0 );

	return true;
}