#include "esp.h"

std::unique_ptr<CESP> esp;

bool CESP::WorldToScreen( const Vector& in, Vector& out )
{
	return ( debugoverlay->ScreenPosition( in, out ) != 1 );
}

void CESP::Draw( )
{
	int screenwidth, screenheight;
	engine->GetScreenSize( screenwidth, screenheight );

	for( int i = 0; i <= engine->GetMaxClients( ); i++ )
	{
		auto entity = entitylist->GetClientEntity( i );
		if( !entity || entity == me || !entity->IsPlayer( ) || !entity->IsAlive( ) || entity->IsDormant( ) || entity->GetTeamNumber( ) == me->GetTeamNumber( ) ) continue;

		Vector min, max, pos = entity->GetAbsOrigin( ), minscreen, maxscreen, posscreen;
		entity->GetRenderBounds( min, max );
		min += pos;
		max += pos;

		if( !WorldToScreen( min, minscreen ) || !WorldToScreen( max, maxscreen ) || !WorldToScreen( pos, posscreen ) ) continue;

		auto h = ( int )( abs( maxscreen.y - minscreen.y ) );
		auto w = ( int )( h * 0.60f );
		auto x = ( int )( posscreen.x - w / 2 );
		auto y = ( int )( posscreen.y - h );

		if( vars->esp_box )
		{
			// outline
			surface->SetDrawColor( 0, 0, 0, 255 );
			surface->DrawOutlinedRect( x - 1, y - 1, ( x + w ) + 1, ( y + h ) + 1 );
			surface->DrawOutlinedRect( x + 1, y + 1, ( x + w ) - 1, ( y + h ) - 1 );

			if( entity->GetTeamNumber( ) == 2 ) // T
				surface->SetDrawColor( 255, 0, 0, 255 );
			else // CT
				surface->SetDrawColor( 39, 28, 199, 255 );

			surface->DrawOutlinedRect( x, y, w + x, y + h );
		}

		if( vars->esp_health )
		{
			auto health = entity->GetHealth( );
			auto health_x = x - 4;
			auto health_y = y;
			auto health_h = std::clamp( health * h / 100, 0, h );

			// outline
			surface->SetDrawColor( 0, 0, 0, 255 );
			surface->DrawFilledRect( health_x - 1, ( health_y + h - health_h ) - 1, health_x + 2, ( ( health_y + h - health_h ) + health_h ) + 1 );
			surface->DrawFilledRect( health_x + 1, ( health_y + h - health_h ) + 1, health_x - 1, ( ( health_y + h - health_h ) + health_h ) - 1 );

			surface->SetDrawColor( 255 - ( int )( health * 2.55f ), ( int )( health * 2.55f ), 0, 255 );
			surface->DrawFilledRect( health_x, health_y + h - health_h, health_x + 1, ( health_y + h - health_h ) + health_h );

			// text
			if( health < 100 )
				menu::Text( health_x + 1, ( health_y + h - health_h ), 255, 255, 255, 255, 1, false, "%i", health );
		}
	}
}