#include "lagcompensation.h"

std::unique_ptr<CLagCompensation> lagcompensation;

void LagRecord_t::Fill( CBasePlayer* entity )
{
	origin = entity->GetVecOrigin( );
	absorigin = entity->GetAbsOrigin( );
	absangles = entity->GetAbsAngles( );
	flags = entity->GetFlags( );
	mins = entity->GetMins( );
	maxs = entity->GetMaxs( );
	velocity = entity->GetVelocity( );
	fallvelocity = entity->GetFallVelocity( );
	simulationtime = entity->GetSimulationTime( );
	eyepos = entity->GetEyePos( );
	viewoffset = entity->GetViewOffset( );
	head = entity->GetBonePosition( 8, simulationtime );

	for( int i = 0; i < entity->GetNumAnimOverlays( ); i++ )
	{
		auto layer = entity->GetAnimOverlay( i );
		anims[ i ].order = layer->m_nOrder;
		anims[ i ].sequence = layer->m_nSequence;
		anims[ i ].weight = layer->m_flWeight;
		anims[ i ].cycle = layer->m_flCycle;
	}

	poseparams = entity->GetPoseParameter( );

	valid = true;
}

void LagRecord_t::Restore( CBasePlayer* entity )
{
	//entity->GetVecOrigin( ) = origin;
	entity->SetAbsOrigin( absorigin );
	entity->SetAbsAngles( absangles );
	entity->GetMins( ) = mins;
	entity->GetMaxs( ) = maxs;
	entity->GetVelocity( ) = velocity;
	entity->GetFallVelocity( ) = fallvelocity;
	entity->GetSimulationTime( ) = simulationtime;
	entity->GetViewOffset( ) = viewoffset;

	for( int i = 0; i < entity->GetNumAnimOverlays( ); i++ )
	{
		auto layer = entity->GetAnimOverlay( i );
		layer->m_nOrder = anims[ i ].order;
		layer->m_nSequence = anims[ i ].sequence;
		layer->m_flWeight = anims[ i ].weight;
		layer->m_flCycle = anims[ i ].cycle;
	}

	entity->GetPoseParameter( ) = poseparams;

	if( chk( flags, FL_ONGROUND ) )
		entity->GetFlags( ) = flags;
}

void CLagCompensation::FrameStageNotify( ClientFrameStage_t stage )
{
	if( stage == FRAME_NET_UPDATE_END )
	{
		for( int i = 1; i <= engine->GetMaxClients( ); i++ )
		{
			auto entity = ( CBasePlayer* )entitylist->GetClientEntity( i );
			if( !entity || entity == me || !entity->IsPlayer( ) || !entity->IsAlive( ) || entity->IsDormant( ) )
			{
				if( records[ i ].size( ) >= 4 )
					records[ i ].clear( );

				continue;
			}

			// disabling interpolation
			auto map = ( DWORD )entity + 0x24;
			for( int i = 0; i < *( int* )( ( DWORD )map + 0x14 ); i++ )
				* ( DWORD* )( *( DWORD* )( map )+i * 0xC ) = 0;

			// removing outdated records
			auto & rec = records[ i ];
			for( auto _rec = rec.begin( ); _rec != rec.end( ); _rec++ )
			{
				if( !IsTickValid( _rec->simulationtime ) )
				{
					records[ i ].erase( _rec );
					if( !records[ i ].empty( ) )
						_rec = records[ i ].begin( );
					else
						break;
				}
			}

			if( records[ i ].size( ) && ( records[ i ].back( ).simulationtime == entity->GetSimulationTime( ) ) ) continue;

			for( int j = 0; j <= vars->backtrack_ticks; j++ )
			{
				if( j >= records[ i ].size( ) ) break;
				records[ i ][ j ].valid = false;
			}

			LagRecord_t record;
			record.Fill( entity );

			records[ i ].emplace_back( record );
		}
	}
}

void CLagCompensation::Start( )
{
	auto CalcAngle = []( Vector src, Vector dst )
	{
		Vector ret( 0.f, 0.f, 0.f );
		auto delta = src - dst;
		auto hyp = delta.Length2DSqr( ), root = sqrt( hyp );

		ret.x = RAD2DEG( atan( delta.z / root ) );
		ret.y = RAD2DEG( atan( delta.y / delta.x ) );
		ret.z = 0.0f;

		if( delta.x >= 0.0f ) ret.y += 180.f;

		ret.Normalize( );

		return ret;
	};

	auto angles = _cmd->viewangles;

	float fov = FLT_MAX;
	int index = -1;
	CBasePlayer * _entity = nullptr;
	int closestrecord = -1;

	static auto mp_teammates_are_enemies = cvar->FindVar( "mp_teammates_are_enemies" );

	for( int i = 1; i <= engine->GetMaxClients( ); i++ )
	{
		auto entity = ( CBasePlayer* )entitylist->GetClientEntity( i );

		if( !entity || entity == me || !entity->IsPlayer( ) || !entity->IsAlive( ) || entity->IsDormant( ) ) continue;
		if( !mp_teammates_are_enemies->GetFloat( ) && entity->GetTeamNumber( ) == me->GetTeamNumber( ) ) continue;
		if( records[ i ].empty( ) ) continue;

		if( vars->backtrack_visible )
		{
			Ray_t ray;
			CGameTrace gametrace;
			CTraceFilter filter;
			filter.pSkip = me;

			ray.Init( me->GetEyePos( ), entity->GetEyePos( ) );
			enginetrace->TraceRay( ray, MASK_SHOT | CONTENTS_GRATE, &filter, &gametrace );

			if( gametrace.entity != entity || gametrace.fraction < 0.97f ) continue;
		}

		auto delta = CalcAngle( me->GetEyePos( ), entity->GetEyePos( ) ) - angles;
		delta.Normalize( );
		delta[ 2 ] = 0.f;

		auto _fov = sqrtf( powf( delta.x, 2.f ) + powf( delta.y, 2.f ) );

		if( _fov < fov )
		{
			fov = _fov;
			index = i;
			_entity = entity;
		}
	}
	if( index == -1 || !_entity ) return;

	fov = FLT_MAX;

	for( int i = 0; i < records[ index ].size( ); i++ )
	{
		auto record = records[ index ][ i ];
		if( !record.valid ) continue;
		if( !IsTickValid( record.simulationtime ) ) continue;

		auto delta = CalcAngle( me->GetEyePos( ), record.eyepos ) - angles;
		delta.Normalize( );
		delta[ 2 ] = 0.f;

		auto _fov = sqrtf( powf( delta.x, 2.f ) + powf( delta.y, 2.f ) );
		if( _fov < fov )
		{
			fov = _fov;
			closestrecord = i;
		}
	}
	if( closestrecord == -1 ) return;

	records[ index ][ closestrecord ].Restore( _entity );
	_cmd->tick_count = ( IsTickValid( records[ index ][ closestrecord ].simulationtime ) ) ? TIME_TO_TICKS( records[ index ][ closestrecord ].simulationtime + GetLerpTime( ) ) : TIME_TO_TICKS( _entity->GetSimulationTime( ) + GetLerpTime( ) );
}

void CLagCompensation::Draw( )
{
	if( !engine->IsInGame( ) || !engine->IsConnected( ) || !me || !me->IsAlive( ) ) return;

	static auto mp_teammates_are_enemies = cvar->FindVar( "mp_teammates_are_enemies" );

	for( int i = 1; i <= engine->GetMaxClients( ); i++ )
	{
		auto entity = ( CBasePlayer* )entitylist->GetClientEntity( i );
		if( !entity || entity == me || !entity->IsPlayer( ) || !entity->IsAlive( ) || entity->IsDormant( ) ) continue;
		if( !mp_teammates_are_enemies->GetFloat( ) && entity->GetTeamNumber( ) == me->GetTeamNumber( ) ) continue;
		if( records[ i ].empty( ) || records[ i ].size( ) < 4 ) continue;

		for( int j = 0; j < records[ i ].size( ); j++ )
		{
			if( !records[ i ][ j ].valid ) continue;
			Vector screen;
			if( !esp->WorldToScreen( records[ i ][ j ].head, screen ) ) continue;

			if( vars->backtrack_draw_r == 255 && vars->backtrack_draw_g == 255 && vars->backtrack_draw_b == 255 )
				surface->SetDrawColor( rand( ) % 255, rand( ) % 255, rand( ) % 255, 255 );
			else
				surface->SetDrawColor( vars->backtrack_draw_r, vars->backtrack_draw_g, vars->backtrack_draw_b, 255 );

			surface->DrawFilledRect( screen.x - 2, screen.y - 2, screen.x + 2, screen.y + 2 );
		}
	}
}

bool CLagCompensation::IsTickValid( float simtime )
{
	static auto sv_maxunlag = cvar->FindVar( "sv_maxunlag" );
	auto net = engine->GetNetChannelInfo( );
	if( !sv_maxunlag || !net ) return false;

	auto delta = std::clamp( net->GetLatency( 0 ) + GetLerpTime( ), 0.f, sv_maxunlag->GetFloat( ) ) - ( globals->curtime - simtime );
	return ( fabsf( delta ) < 0.2f );
}

float CLagCompensation::GetLerpTime( )
{
	static auto cl_updaterate = cvar->FindVar( "cl_updaterate" );
	static auto sv_minupdaterate = cvar->FindVar( "sv_minupdaterate" );
	static auto sv_maxupdaterate = cvar->FindVar( "sv_maxupdaterate" );
	static auto cl_interp_ratio = cvar->FindVar( "cl_interp_ratio" );
	static auto cl_interp = cvar->FindVar( "cl_interp" );
	static auto sv_client_min_interp_ratio = cvar->FindVar( "sv_client_min_interp_ratio" );
	static auto sv_client_max_interp_ratio = cvar->FindVar( "sv_client_max_interp_ratio" );

	auto ratio = cl_interp_ratio->GetFloat( );
	if( !ratio )
		ratio = 1.f;

	if( sv_client_min_interp_ratio && sv_client_max_interp_ratio && sv_client_min_interp_ratio->GetFloat( ) != 1.f )
		ratio = std::clamp( ratio, sv_client_min_interp_ratio->GetFloat( ), sv_client_max_interp_ratio->GetFloat( ) );

	return max( cl_interp->GetFloat( ), ( ratio / ( ( sv_minupdaterate && sv_maxupdaterate ) ? sv_maxupdaterate->GetFloat( ) : cl_updaterate->GetFloat( ) ) ) );
}
