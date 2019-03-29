#pragma once
#include "sdk.h"
#include <array>
#include <cstdint>

class AnimationLayer
{
public:
	char  pad_0000[ 20 ];
	// These should also be present in the padding, don't see the use for it though
	//float	m_flLayerAnimtime;
	//float	m_flLayerFadeOuttime;
	uint32_t m_nOrder; //0x0014
	uint32_t m_nSequence; //0x0018
	float_t m_flPrevCycle; //0x001C
	float_t m_flWeight; //0x0020
	float_t m_flWeightDeltaRate; //0x0024
	float_t m_flPlaybackRate; //0x0028
	float_t m_flCycle; //0x002C
	void* m_pOwner; //0x0030 // player's thisptr
	char  pad_0038[ 4 ]; //0x0034
}; //Size: 0x0038

class CBasePlayer
{
public:
	VFUNC( 10, Vector&, GetAbsOrigin, ( this ) );
	VFUNC( 11, Vector&, GetAbsAngles, ( this ) );
	void* GetRenderable( )
	{
		return ( void* )( ( DWORD )this + 0x4 );
	}

	void* GetNetworkable( )
	{
		return ( void* )( ( DWORD )this + 0x8 );
	}

	Vector& GetVecOrigin( )
	{
		static auto offset = 0x138;
		return *( Vector* )( ( DWORD )this + offset );
	}

	int& GetFlags( )
	{
		static auto offset = 0x104;
		return *( int* )( ( DWORD )this + offset );
	}

	Vector& GetMins( )
	{
		static auto offset = 0x324;
		return *( Vector* )( ( DWORD )this + offset );
	}

	Vector& GetMaxs( )
	{
		static auto offset = 0x330;
		return *( Vector* )( ( DWORD )this + offset );
	}

	Vector& GetVelocity( )
	{
		static auto offset = 0x114;
		return *( Vector* )( ( DWORD )this + offset );
	}

	Vector& GetBaseVelocity( )
	{
		static auto offset = 0x120;
		return *( Vector* )( ( DWORD )this + offset );
	}

	Vector& GetViewOffset( )
	{
		static auto offset = 0x108;
		return *( Vector* )( ( DWORD )this + offset );
	}

	float& GetFallVelocity( )
	{
		static auto offset = 0x3014;
		return *( float* )( ( DWORD )this + offset );
	}

	float& GetSimulationTime( )
	{
		static auto offset = 0x268;
		return *( float* )( ( DWORD )this + offset );
	}

	int GetNumAnimOverlays( )
	{
		static auto offset = 0x297C;
		return *( int* )( ( DWORD )this + offset );
	}

	AnimationLayer* GetAnimOverlays( )
	{
		static auto offset = 10608;
		return *( AnimationLayer * * )( ( DWORD )this + offset );
	}

	AnimationLayer* GetAnimOverlay( int num )
	{
		if( num < 15 ) return &GetAnimOverlays( )[ num ];

		return nullptr;
	}

	std::array<float, 24>& GetPoseParameter( ) const
	{
		static int _m_flPoseParameter = 0x2774;
		return *( std::array<float, 24>* )( ( uintptr_t )this + _m_flPoseParameter );
	}

	void GetRenderBounds( Vector & mins, Vector & maxs )
	{
		auto renderable = GetRenderable( );
		if( !renderable ) return;

		GetVFunc<void( __thiscall* )( void*, Vector&, Vector& )>( renderable, 17 )( renderable, mins, maxs );
	}

	bool IsDormant( )
	{
		auto network = GetNetworkable( );
		if( !network ) return false;

		return GetVFunc<bool( __thiscall* )( void* )>( network, 9 )( network );
	}

	int GetIndex( )
	{
		auto network = GetNetworkable( );
		if( !network ) return false;

		return GetVFunc<int( __thiscall* )( void* )>( network, 10 )( network );
	}

	bool IsAlive( )
	{
		static auto offset = 0x25F;
		auto lifestate = *( int* )( ( DWORD )this + offset );

		return ( lifestate == 0 );
	}

	int GetTeamNumber( )
	{
		static auto offset = 0xF4;
		return *( int* )( ( DWORD )this + offset );
	}

	Vector GetEyePos( )
	{
		auto origin = GetVecOrigin( );
		auto viewoffset = GetViewOffset( );

		return ( origin + viewoffset );
	}

#ifdef USE_ENGINE_PREDICTION
	int& GetTickBase( )
	{
		static auto offset = 0x342C;
		return *( int* )( ( DWORD )this + offset );
	}
#endif

	int GetHealth( )
	{
		static auto offset = 0x100;

		return *( int* )( ( DWORD )this + offset );
	}

	bool SetupBones( matrix3x4_t * bone, int max, int mask, float curtime )
	{
		auto renderable = ( void* )( ( DWORD )this + 0x4 );
		if( !renderable ) return false;

		return GetVFunc<bool( __thiscall* )( void*, matrix3x4_t*, int, int, float )>( renderable, 13 )( renderable, bone, max, mask, curtime );
	}

	Vector GetBonePosition( int bone, float curtime )
	{
		static Vector nullvec( 0.f, 0.f, 0.f );
		matrix3x4_t matrix[ 128 ];

		if( !SetupBones( matrix, 128, 0x100, curtime ) ) return nullvec; // 0x100 - BONE_USED_BY_HITBOX

		return Vector( matrix[ bone ][ 0 ][ 3 ], matrix[ bone ][ 1 ][ 3 ], matrix[ bone ][ 2 ][ 3 ] );
	}

	bool IsPlayer( );
	void SetAbsOrigin( const Vector & origin );
	void SetAbsAngles( const Vector & angles );
};