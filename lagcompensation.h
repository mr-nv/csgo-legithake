#pragma once
#include "main.h"

#define TICK_INTERVAL			(globals->interval_per_tick)

#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )
#define ROUND_TO_TICKS( t )		( TICK_INTERVAL * TIME_TO_TICKS( t ) )
#define TICK_NEVER_THINK		(-1)

struct AnimationRecord_t
{
	int order;
	int sequence;
	float weight;
	float cycle;
};

struct LagRecord_t
{
	void Fill( CBasePlayer* entity );
	void Restore( CBasePlayer* entity );
	Vector origin;
	Vector absorigin;
	Vector vecangles;
	Vector absangles;
	int flags;
	Vector mins;
	Vector maxs;
	Vector velocity;
	float fallvelocity;
	float simulationtime;
	Vector eyepos;
	Vector viewoffset;
	std::array<float, 24> poseparams;
	std::array<AnimationRecord_t, 15> anims;
	bool valid;
	Vector head; // for drawing
};

class CLagCompensation
{
public:
	CLagCompensation( )
	{
		for( int i = 0; i < 64; i++ )
			records[ i ].clear( );
	}
	void FrameStageNotify( ClientFrameStage_t stage );
	void Start( );
	void Draw( );

	bool IsTickValid( float simtime );
	float GetLerpTime( );
private:
	std::deque<LagRecord_t> records[ 64 ];
};

extern std::unique_ptr<CLagCompensation> lagcompensation;