#include "entity.h"

bool CBasePlayer::IsPlayer( )
{
	player_info_t info;
	return ( engine->GetPlayerInfo( GetIndex( ), &info ) );
}

void CBasePlayer::SetAbsOrigin( const Vector& origin )
{
	static auto function = ( void( __thiscall* )( void*, const Vector& ) )PatternScan( GetModuleHandleA( "client_panorama" ), "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8" );
	function( this, origin );
}

void CBasePlayer::SetAbsAngles( const Vector& angles )
{
	static auto function = ( void( __thiscall* )( void*, const Vector& ) )PatternScan( GetModuleHandleA( "client_panorama" ), "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8" );
	function( this, angles );
}