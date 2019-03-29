#pragma once
#include <windows.h>
#include <string>
#include <iostream>
#include <deque>
#include <process.h>
#include <stdint.h>
#include <vector>

extern void* clientmode;

#define	FL_ONGROUND				(1<<0)	// At rest / on the ground
#define FL_DUCKING				(1<<1)	// Player flag -- Player is fully crouched
#define FL_ANIMDUCKING			(1<<2)	// Player flag -- Player is in the process of crouching or uncrouching but could be in transition
// examples:                                   Fully ducked:  FL_DUCKING &  FL_ANIMDUCKING
//           Previously fully ducked, unducking in progress:  FL_DUCKING & !FL_ANIMDUCKING
//                                           Fully unducked: !FL_DUCKING & !FL_ANIMDUCKING
//           Previously fully unducked, ducking in progress: !FL_DUCKING &  FL_ANIMDUCKING
#define	FL_WATERJUMP			(1<<3)	// player jumping out of water
#define FL_ONTRAIN				(1<<4) // Player is _controlling_ a train, so movement commands should be ignored on client during prediction.
#define FL_INRAIN				(1<<5)	// Indicates the entity is standing in rain
#define FL_FROZEN				(1<<6) // Player is frozen for 3rd person camera
#define FL_ATCONTROLS			(1<<7) // Player can't move, but keeps key inputs for controlling another entity
#define	FL_CLIENT				(1<<8)	// Is a player
#define FL_FAKECLIENT			(1<<9)	// Fake client, simulated server side; don't send network messages to them
// NON-PLAYER SPECIFIC (i.e., not used by GameMovement or the client .dll ) -- Can still be applied to players, though
#define	FL_INWATER				(1<<10)	// In water

// NOTE if you move things up, make sure to change this value
#define PLAYER_FLAG_BITS		11

#define	FL_FLY					(1<<11)	// Changes the SV_Movestep() behavior to not need to be on ground
#define	FL_SWIM					(1<<12)	// Changes the SV_Movestep() behavior to not need to be on ground (but stay in water)
#define	FL_CONVEYOR				(1<<13)
#define	FL_NPC					(1<<14)
#define	FL_GODMODE				(1<<15)
#define	FL_NOTARGET				(1<<16)
#define	FL_AIMTARGET			(1<<17)	// set if the crosshair needs to aim onto the entity
#define	FL_PARTIALGROUND		(1<<18)	// not all corners are valid
#define FL_STATICPROP			(1<<19)	// Eetsa static prop!		
#define FL_GRAPHED				(1<<20) // worldgraph has this ent listed as something that blocks a connection
#define FL_GRENADE				(1<<21)
#define FL_STEPMOVEMENT			(1<<22)	// Changes the SV_Movestep() behavior to not do any processing
#define FL_DONTTOUCH			(1<<23)	// Doesn't generate touch functions, generates Untouch() for anything it was touching when this flag was set
#define FL_BASEVELOCITY			(1<<24)	// Base velocity has been applied this frame (used to convert base velocity into momentum)
#define FL_WORLDBRUSH			(1<<25)	// Not moveable/removeable brush entity (really part of the world, but represented as an entity for transparency or something)
#define FL_OBJECT				(1<<26) // Terrible name. This is an object that NPCs should see. Missiles, for example.
#define FL_KILLME				(1<<27)	// This entity is marked for death -- will be freed by game DLL
#define FL_ONFIRE				(1<<28)	// You know...
#define FL_DISSOLVING			(1<<29) // We're dissolving!
#define FL_TRANSRAGDOLL			(1<<30) // In the process of turning into a client side ragdoll.
#define FL_UNBLOCKABLE_BY_PLAYER (1<<31) // pusher that can't be blocked by the player

#define IN_ATTACK		(1 << 0)
#define IN_JUMP			(1 << 1)
#define IN_DUCK			(1 << 2)
#define IN_FORWARD		(1 << 3)
#define IN_BACK			(1 << 4)
#define IN_USE			(1 << 5)
#define IN_CANCEL		(1 << 6)
#define IN_LEFT			(1 << 7)
#define IN_RIGHT		(1 << 8)
#define IN_MOVELEFT		(1 << 9)
#define IN_MOVERIGHT	(1 << 10)
#define IN_ATTACK2		(1 << 11)
#define IN_RUN			(1 << 12)
#define IN_RELOAD		(1 << 13)
#define IN_ALT1			(1 << 14)
#define IN_ALT2			(1 << 15)
#define IN_SCORE		(1 << 16)   // Used by client.dll for when scoreboard is held down
#define IN_SPEED		(1 << 17)	// Player is holding the speed key
#define IN_WALK			(1 << 18)	// Player holding walk key
#define IN_ZOOM			(1 << 19)	// Zoom key for HUD zoom
#define IN_WEAPON1		(1 << 20)	// weapon defines these bits
#define IN_WEAPON2		(1 << 21)	// weapon defines these bits
#define IN_BULLRUSH		(1 << 22)
#define IN_GRENADE1		(1 << 23)	// grenade 1
#define IN_GRENADE2		(1 << 24)	// grenade 2
#define	IN_ATTACK3		(1 << 25)

// edict->movetype values
enum MoveType_t
{
	MOVETYPE_NONE = 0,	// never moves
	MOVETYPE_ISOMETRIC,			// For players -- in TF2 commander view, etc.
	MOVETYPE_WALK,				// Player only - moving on the ground
	MOVETYPE_STEP,				// gravity, special edge handling -- monsters use this
	MOVETYPE_FLY,				// No gravity, but still collides with stuff
	MOVETYPE_FLYGRAVITY,		// flies through the air + is affected by gravity
	MOVETYPE_VPHYSICS,			// uses VPHYSICS for simulation
	MOVETYPE_PUSH,				// no clip to world, push and crush
	MOVETYPE_NOCLIP,			// No gravity, no collisions, still do velocity/avelocity
	MOVETYPE_LADDER,			// Used by players only when going onto a ladder
	MOVETYPE_OBSERVER,			// Observer movement, depends on player's observer mode
	MOVETYPE_CUSTOM,			// Allows the entity to describe its own physics

	// should always be defined as the last item in the list
	MOVETYPE_LAST = MOVETYPE_CUSTOM,

	MOVETYPE_MAX_BITS = 4
};

template<typename FuncType>
__forceinline static FuncType GetVFunc( void* ppClass, int index )
{
	auto base = *( int** )ppClass;
	return ( FuncType )( base[ index ] );
}

#define VFUNC(_index, _class, _name, _args, ...) \
	_class _name##(__VA_ARGS__) \
	{ \
		return GetVFunc<##_class##(__thiscall*)(void*, __VA_ARGS__)>(this, _index)##_args##; \
	}

typedef void* ( *CreateInterfaceFn )( const char* pName, int* pReturnCode );

class CBasePlayer;

typedef struct player_info_s
{
	int64_t __pad0;
	union
	{
		int64_t xuid;
		struct
		{
			int xuidlow;
			int xuidhigh;
		};
	};
	char name[ 128 ];
	int userid;
	char guid[ 33 ];
	unsigned int friendsid;
	char friendsname[ 128 ];
	bool fakeplayer;
	bool ishltv;
	unsigned int customfiles[ 4 ];
	unsigned char filesdownloaded;
} player_info_t;

class INetChannelInfo
{
public:
	enum
	{
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		TOTAL,			// must be last and is not a real group
	};

	virtual const char* GetName( void ) const = 0;	// get channel name
	virtual const char* GetAddress( void ) const = 0; // get channel IP address as string
	virtual float		GetTime( void ) const = 0;	// current net time
	virtual float		GetTimeConnected( void ) const = 0;	// get connection time in seconds
	virtual int			GetBufferSize( void ) const = 0;	// netchannel packet history size
	virtual int			GetDataRate( void ) const = 0; // send data rate in byte/sec

	virtual bool		IsLoopback( void ) const = 0;	// true if loopback channel
	virtual bool		IsTimingOut( void ) const = 0;	// true if timing out
	virtual bool		IsPlayback( void ) const = 0;	// true if demo playback

	virtual float		GetLatency( int flow ) const = 0;	 // current latency (RTT), more accurate but jittering
	virtual float		GetAvgLatency( int flow ) const = 0; // average packet latency in seconds
	virtual float		GetAvgLoss( int flow ) const = 0;	 // avg packet loss[0..1]
	virtual float		GetAvgChoke( int flow ) const = 0;	 // avg packet choke[0..1]
	virtual float		GetAvgData( int flow ) const = 0;	 // data flow in bytes/sec
	virtual float		GetAvgPackets( int flow ) const = 0; // avg packets/sec
	virtual int			GetTotalData( int flow ) const = 0;	 // total flow in/out in bytes
	virtual int			GetSequenceNr( int flow ) const = 0;	// last send seq number
	virtual bool		IsValidPacket( int flow, int frame_number ) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime( int flow, int frame_number ) const = 0; // time when packet was send
	virtual int			GetPacketBytes( int flow, int frame_number, int group ) const = 0; // group size of this packet
	virtual bool		GetStreamProgress( int flow, int* received, int* total ) const = 0;  // TCP progress if transmitting
	virtual float		GetTimeSinceLastReceived( void ) const = 0;	// get time since last recieved packet in seconds
	virtual	float		GetCommandInterpolationAmount( int flow, int frame_number ) const = 0;
	virtual void		GetPacketResponseLatency( int flow, int frame_number, int* pnLatencyMsecs, int* pnChoke ) const = 0;
	virtual void		GetRemoteFramerate( float* pflFrameTime, float* pflFrameTimeStdDeviation ) const = 0;

	virtual float		GetTimeoutSeconds( ) const = 0;
};

class Vector
{
public:
	Vector( )
	{
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
	}
	Vector( float x, float y, float z )
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Vector( const Vector& v )
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
	}
	Vector( const float* v )
	{
		this->x = v[ 0 ];
		this->y = v[ 1 ];
		this->z = v[ 2 ];
	}
	void Set( float x = 0.0f, float y = 0.0f, float z = 0.0f )
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	bool operator!( ) const
	{
		return IsZero( );
	}
	bool operator==( const Vector& v ) const
	{
		return ( this->x == v.x &&
			this->y == v.y &&
			this->z == v.z );
	}
	bool operator!=( const Vector & v ) const
	{
		return ( this->x != v.x ||
			this->y != v.y ||
			this->z != v.z );
	}
	float& operator[ ]( unsigned int i )
	{
		return ( ( float* )this )[ i ];
	}
	float operator[ ]( unsigned int i ) const
	{
		return ( ( float* )this )[ i ];
	}
	Vector operator-( ) const
	{
		return Vector( -x, -y, -z );
	}
	Vector& operator=( const Vector & v )
	{
		//CHECK_VALID(v);
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;

		return *this;
	}
	Vector& operator=( const float* v )
	{
		this->x = v[ 0 ];
		this->y = v[ 1 ];
		this->z = v[ 2 ];

		return *this;
	}
	Vector& operator+=( const Vector & v )
	{
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;

		return *this;
	}
	Vector& operator-=( const Vector & v )
	{
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;

		return *this;
	}
	Vector& operator*=( const Vector & v )
	{
		this->x *= v.x;
		this->y *= v.y;
		this->z *= v.z;

		return *this;
	}
	Vector& operator/=( const Vector & v )
	{
		this->x /= v.x;
		this->y /= v.y;
		this->z /= v.z;

		return *this;
	}
	Vector& operator+=( float v )
	{
		this->x += v;
		this->y += v;
		this->z += v;

		return *this;
	}
	Vector& operator-=( float v )
	{
		this->x -= v;
		this->y -= v;
		this->z -= v;

		return *this;
	}
	Vector& operator*=( float v )
	{
		this->x *= v;
		this->y *= v;
		this->z *= v;

		return *this;
	}
	Vector& operator/=( float v )
	{
		this->x /= v;
		this->y /= v;
		this->z /= v;

		return *this;
	}
	Vector operator+( const Vector & v ) const
	{
		return Vector( this->x + v.x, this->y + v.y, this->z + v.z );
	}
	Vector operator-( const Vector & v ) const
	{
		return Vector( this->x - v.x, this->y - v.y, this->z - v.z );
	}
	Vector operator*( const Vector & v ) const
	{
		return Vector( this->x * v.x, this->y * v.y, this->z * v.z );
	}
	Vector operator/( const Vector & v ) const
	{
		return Vector( this->x / v.x, this->y / v.y, this->z / v.z );
	}
	Vector operator+( float v ) const
	{
		return Vector( this->x + v, this->y + v, this->z + v );
	}
	Vector operator-( float v ) const
	{
		return Vector( this->x - v, this->y - v, this->z - v );
	}
	Vector operator*( float v ) const
	{
		return Vector( this->x * v, this->y * v, this->z * v );
	}
	Vector operator/( float v ) const
	{
		return Vector( this->x / v, this->y / v, this->z / v );
	}
	float Dot( const Vector & v ) const
	{
		return ( this->x * v.x + this->y * v.y + this->z * v.z );
	}
	float Length( ) const
	{
		return sqrt( ( x * x ) + ( y * y ) + ( z * z ) );
	}
	float LengthSqr( ) const
	{
		return ( ( x * x ) + ( y * y ) + ( z * z ) );
	}
	float Length2D( ) const
	{
		return sqrt( ( x * x ) + ( y * y ) );
	}
	float Length2DSqr( ) const
	{
		return ( ( x * x ) + ( y * y ) );
	}
	float DistTo( const Vector & v ) const
	{
		return ( *this - v ).Length( );
	}
	float DistToSqr( const Vector & v ) const
	{
		return ( *this - v ).LengthSqr( );
	}
	void Normalize( )
	{
		while( x < -180.f )
			x += 360.f;
		while( x > 180.f )
			x -= 360.f;

		while( y < -180.f )
			y += 360.f;
		while( y > 180.f )
			y -= 360.f;

		while( z < -180.f )
			z += 360.f;
		while( z > 180.f )
			z -= 360.f;
	}
	void NormalizeInPlace( )
	{
		Normalize( );
	}
	bool IsZero( ) const
	{
		return ( x == 0.0f && y == 0.0f && z == 0.0f );
	}
	bool IsValid( ) const
	{
		return ( !IsZero( ) );
	}
public:
	float		x;
	float		y;
	float		z;
};

__forceinline Vector operator*( float f, const Vector & v )
{
	return v * f;
}

class __declspec( align( 16 ) ) VectorAligned : public Vector
{
public:
	inline VectorAligned( void ) {};
	inline VectorAligned( float X, float Y, float Z )
	{
		Set( X, Y, Z );
	}

	explicit VectorAligned( const Vector& vOther )
	{
		Set( vOther.x, vOther.y, vOther.z );
	}

	VectorAligned& operator=( const Vector& vOther )
	{
		Set( vOther.x, vOther.y, vOther.z );
		return *this;
	}

	VectorAligned& operator=( const VectorAligned& vOther )
	{
		Set( vOther.x, vOther.y, vOther.z );
		return *this;
	}

	float w;
};

class IVEngineClient
{
public:
	VFUNC( 78, INetChannelInfo*, GetNetChannelInfo, ( this ) );
	VFUNC( 26, bool, IsInGame, ( this ) );
	VFUNC( 27, bool, IsConnected, ( this ) );
	VFUNC( 20, int, GetMaxClients, ( this ) );
	VFUNC( 12, int, GetLocalPlayer, ( this ) );
	VFUNC( 8, bool, GetPlayerInfo, ( this, id, info ), int id, player_info_t* info );
	VFUNC( 19, void, SetViewAngles, ( this, angles ), Vector& angles );
	VFUNC( 5, void, GetScreenSize, ( this, width, height ), int& width, int& height );
	VFUNC( 92, bool, IsTakingScreenshot, ( this ) );
};

extern IVEngineClient* engine;

class CGlobalVarsBase
{
public:
	float    realtime;
	int      framecount;
	float    absoluteframetime;
	float    absoluteframestarttimestddev;
	float    curtime;
	float    frametime;
	int      maxClients;
	int      tickcount;
	float    interval_per_tick;
	float    interpolation_amount;
	int      simTicksThisFrame;
	int      network_protocol;
	void* pSaveData;
	bool     m_bClient;
	int      nTimestampNetworkingBase;
	int      nTimestampRandomizeWindow;
};

extern CGlobalVarsBase* globals;

class IClientEntityList
{
public:
	VFUNC( 3, CBasePlayer*, GetClientEntity, ( this, index ), int index );
	//VFUNC(4, CBasePlayer*, GetClientEntityFromHandle, (this, handle), void* handle);
};

extern IClientEntityList* entitylist;

enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,			// (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
	// Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END
};

class CUserCmd
{
public:
	virtual ~CUserCmd( ) {}; //Destructor 0
	CUserCmd( )
	{
		Reset( );
	}

	void Reset( )
	{
		command_number = 0;
		tick_count = 0;
		viewangles.Set( );
		forwardmove = 0.0f;
		sidemove = 0.0f;
		upmove = 0.0f;
		buttons = 0;
		impulse = 0;
		weaponselect = 0;
		weaponsubtype = 0;
		random_seed = 0;
		mousedx = 0;
		mousedy = 0;
		headangles.Set( );
		headoffset.Set( );

		hasbeenpredicted = false;
	}

	CUserCmd& operator =( const CUserCmd& src )
	{
		if( this == &src )
			return *this;

		command_number = src.command_number;
		tick_count = src.tick_count;
		viewangles = src.viewangles;
		forwardmove = src.forwardmove;
		sidemove = src.sidemove;
		upmove = src.upmove;
		buttons = src.buttons;
		impulse = src.impulse;
		weaponselect = src.weaponselect;
		weaponsubtype = src.weaponsubtype;
		random_seed = src.random_seed;
		mousedx = src.mousedx;
		mousedy = src.mousedy;

		hasbeenpredicted = src.hasbeenpredicted;
		headangles = src.headangles;
		headoffset = src.headoffset;
		return *this;
	}

	CUserCmd( const CUserCmd & src )
	{
		*this = src;
	}


	int        command_number;
	int        tick_count;
	Vector  viewangles;
	Vector    aimdirection;
	float    forwardmove;
	float    sidemove;
	float    upmove;
	int        buttons;
	BYTE    impulse;
	int        weaponselect;
	int        weaponsubtype;
	int        random_seed;
	short    mousedx;
	short    mousedy;
	bool    hasbeenpredicted;
	Vector  headangles;
	Vector    headoffset;
};

class ConVar
{
public:
	float GetFloat( )
	{
		return GetVFunc<float( __thiscall* )( void* )>( this, 12 )( this );
	}
};

class IAppSystem
{
public:
	virtual ~IAppSystem( )
	{
	}

	virtual void func0( ) = 0;
	virtual void func1( ) = 0;
	virtual void func2( ) = 0;
	virtual void func3( ) = 0;
	virtual void func4( ) = 0;
	virtual void func5( ) = 0;
	virtual void func6( ) = 0;
	virtual void func7( ) = 0;
	virtual void func8( ) = 0;
	virtual void func9( ) = 0;
};

struct CVarDLLIdentifier_t;

class ICVar : public IAppSystem
{
public:
	virtual void			func10( ) = 0;
	virtual void			RegisterConCommand( ConVar* pCommandBase ) = 0;
	virtual void			UnregisterConCommand( ConVar* pCommandBase ) = 0;
	virtual void			func13( ) = 0;
	virtual ConVar* FindVar( const char* var_name ) = 0;
	virtual void			func15( ) = 0;
	virtual void			func16( ) = 0;
	virtual void			func17( ) = 0;
	virtual void			func18( ) = 0;
	virtual void			func19( ) = 0;
	virtual void			func20( ) = 0;
};

extern ICVar* cvar;

class IVDebugOverlay
{
public:
	VFUNC( 13, int, ScreenPosition, ( this, in, out ), const Vector& in, Vector& out );
};

extern IVDebugOverlay* debugoverlay;

enum FontFlags_t
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x1,
	FONTFLAG_UNDERLINE = 0x2,
	FONTFLAG_STRIKEOUT = 0x4,
	FONTFLAG_SYMBOL = 0x8,
	FONTFLAG_ANTIALIAS = 0x10,
	FONTFLAG_GAUSSIANBLUR = 0x20,
	FONTFLAG_ROTARY = 0x40,
	FONTFLAG_DROPSHADOW = 0x80,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};

class ISurface
{
public:
	VFUNC( 15, void, SetDrawColor, ( this, r, g, b, a ), int r, int g, int b, int a );
	VFUNC( 16, void, DrawFilledRect, ( this, x0, y0, x1, y1 ), int x0, int y0, int x1, int y1 );
	VFUNC( 25, void, SetTextColor, ( this, r, g, b, a ), int r, int g, int b, int a );
	VFUNC( 23, void, SetTextFont, ( this, font ), unsigned long font );
	VFUNC( 26, void, SetTextPosition, ( this, x, y ), int x, int y );
	VFUNC( 28, void, DrawPrintText, ( this, text, len, drawtype ), const wchar_t* text, int len, int drawtype );
	VFUNC( 18, void, DrawOutlinedRect, ( this, x0, y0, x1, y1 ), int x0, int y0, int x1, int y1 );
	VFUNC( 71, unsigned long, CreateFont_, ( this ) );
	VFUNC( 79, void, GetTextSize, ( this, font, text, wide, tall ), unsigned long font, const wchar_t* text, int& wide, int& tall );

	void SetFontGlyphSet( unsigned long font, const char* name, int tall, int weight, int blur, int scanlines, int flags )
	{
		GetVFunc<void( __thiscall* )( void*, unsigned long, const char*, int, int, int, int, int, int, int )>( this, 72 )( this, font, name, tall, weight, blur, scanlines, flags, 0, 0 );
	}
};

extern ISurface* surface;

class IPanel
{
public:
	VFUNC( 36, const char*, GetName, ( this, panel ), unsigned int panel );
};

extern IPanel* panel;

#ifdef USE_ENGINE_PREDICTION
class CMoveData
{
public:
	bool    m_bFirstRunOfFunctions : 1;
	bool    m_bGameCodeMovedPlayer : 1;
	int     m_nPlayerHandle;        // edict index on server, client entity handle on client=
	int     m_nImpulseCommand;      // Impulse command issued.
	Vector  m_vecViewAngles;        // Command view angles (local space)
	Vector  m_vecAbsViewAngles;     // Command view angles (world space)
	int     m_nButtons;             // Attack buttons.
	int     m_nOldButtons;          // From host_client->oldbuttons;
	float   m_flForwardMove;
	float   m_flSideMove;
	float   m_flUpMove;
	float   m_flMaxSpeed;
	float   m_flClientMaxSpeed;
	Vector  m_vecVelocity;          // edict::velocity        // Current movement direction.
	Vector  m_vecAngles;            // edict::angles
	Vector  m_vecOldAngles;
	float   m_outStepHeight;        // how much you climbed this move
	Vector  m_outWishVel;           // This is where you tried 
	Vector  m_outJumpVel;           // This is your jump velocity
	Vector  m_vecConstraintCenter;
	float   m_flConstraintRadius;
	float   m_flConstraintWidth;
	float   m_flConstraintSpeedFactor;
	float   m_flUnknown[ 5 ];
	Vector  m_vecAbsOrigin;
};
#endif

class matrix3x4_t
{

public:
	matrix3x4_t( ) {}
	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23 )
	{
		m_flMatVal[ 0 ][ 0 ] = m00; m_flMatVal[ 0 ][ 1 ] = m01; m_flMatVal[ 0 ][ 2 ] = m02; m_flMatVal[ 0 ][ 3 ] = m03;
		m_flMatVal[ 1 ][ 0 ] = m10; m_flMatVal[ 1 ][ 1 ] = m11; m_flMatVal[ 1 ][ 2 ] = m12; m_flMatVal[ 1 ][ 3 ] = m13;
		m_flMatVal[ 2 ][ 0 ] = m20; m_flMatVal[ 2 ][ 1 ] = m21; m_flMatVal[ 2 ][ 2 ] = m22; m_flMatVal[ 2 ][ 3 ] = m23;
	}
	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	void Init( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector& vecOrigin )
	{
		m_flMatVal[ 0 ][ 0 ] = xAxis.x; m_flMatVal[ 0 ][ 1 ] = yAxis.x; m_flMatVal[ 0 ][ 2 ] = zAxis.x; m_flMatVal[ 0 ][ 3 ] = vecOrigin.x;
		m_flMatVal[ 1 ][ 0 ] = xAxis.y; m_flMatVal[ 1 ][ 1 ] = yAxis.y; m_flMatVal[ 1 ][ 2 ] = zAxis.y; m_flMatVal[ 1 ][ 3 ] = vecOrigin.y;
		m_flMatVal[ 2 ][ 0 ] = xAxis.z; m_flMatVal[ 2 ][ 1 ] = yAxis.z; m_flMatVal[ 2 ][ 2 ] = zAxis.z; m_flMatVal[ 2 ][ 3 ] = vecOrigin.z;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	matrix3x4_t( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector& vecOrigin )
	{
		Init( xAxis, yAxis, zAxis, vecOrigin );
	}

	inline void SetOrigin( Vector const& p )
	{
		m_flMatVal[ 0 ][ 3 ] = p.x;
		m_flMatVal[ 1 ][ 3 ] = p.y;
		m_flMatVal[ 2 ][ 3 ] = p.z;
	}

	inline void Invalidate( void )
	{
		for( int i = 0; i < 3; i++ )
		{
			for( int j = 0; j < 4; j++ )
			{
				m_flMatVal[ i ][ j ] = std::numeric_limits<float>::infinity( );;
			}
		}
	}

	Vector GetXAxis( )  const { return at( 0 ); }
	Vector GetYAxis( )  const { return at( 1 ); }
	Vector GetZAxis( )  const { return at( 2 ); }
	Vector GetOrigin( ) const { return at( 3 ); }

	Vector at( int i ) const { return Vector{ m_flMatVal[ 0 ][ i ], m_flMatVal[ 1 ][ i ], m_flMatVal[ 2 ][ i ] }; }

	float* operator[]( int i ) { return m_flMatVal[ i ]; }
	const float* operator[]( int i ) const { return m_flMatVal[ i ]; }
	float* Base( ) { return &m_flMatVal[ 0 ][ 0 ]; }
	const float* Base( ) const { return &m_flMatVal[ 0 ][ 0 ]; }

	float m_flMatVal[ 3 ][ 4 ];
};

struct Ray_t;
class CGameTrace;
typedef CGameTrace trace_t;

enum class TraceType
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS,
};

class ITraceFilter
{
public:
	virtual bool ShouldHitEntity( CBasePlayer* pEntity, int contentsMask ) = 0;
	virtual TraceType GetTraceType( ) const = 0;
};

// This is the one most normal traces will inherit from
class CTraceFilter : public ITraceFilter
{
public:
	bool ShouldHitEntity( CBasePlayer* pEntityHandle, int /*contentsMask*/ )
	{
		return !( pEntityHandle == pSkip );
	}
	virtual TraceType GetTraceType( ) const
	{
		return TraceType::TRACE_EVERYTHING;
	}
	void* pSkip;
};

class CTraceFilterSkipEntity : public ITraceFilter
{
public:
	CTraceFilterSkipEntity( CBasePlayer* pEntityHandle )
	{
		pSkip = pEntityHandle;
	}

	bool ShouldHitEntity( CBasePlayer* pEntityHandle, int /*contentsMask*/ )
	{
		return !( pEntityHandle == pSkip );
	}
	virtual TraceType GetTraceType( ) const
	{
		return TraceType::TRACE_EVERYTHING;
	}
	void* pSkip;
};

class CPhysCollide;

struct vcollide_t
{
	unsigned short solidCount : 15;
	unsigned short isPacked : 1;
	unsigned short descSize;
	// VPhysicsSolids
	CPhysCollide** solids;
	char* pKeyValues;
	void* pUserData;
};

struct cmodel_t
{
	Vector         mins, maxs;
	Vector         origin;        // for sounds or lights
	int            headnode;
	vcollide_t     vcollisionData;
};

struct csurface_t
{
	const char* name;
	short          surfaceProps;
	unsigned short flags;         // BUGBUG: These are declared per surface, not per material, but this database is per-material now
};

//-----------------------------------------------------------------------------
// A ray...
//-----------------------------------------------------------------------------
struct Ray_t
{
	VectorAligned  m_Start;  // starting point, centered within the extents
	VectorAligned  m_Delta;  // direction + length of the ray
	VectorAligned  m_StartOffset; // Add this to m_Start to Get the actual ray start
	VectorAligned  m_Extents;     // Describes an axis aligned box extruded along a ray
	const matrix3x4_t* m_pWorldAxisTransform;
	bool m_IsRay;  // are the extents zero?
	bool m_IsSwept;     // is delta != 0?

	Ray_t( ) : m_pWorldAxisTransform( NULL ) {}

	void Init( Vector const& start, Vector const& end )
	{
		m_Delta = end - start;

		m_IsSwept = ( m_Delta.LengthSqr( ) != 0 );

		m_Extents.Set( );

		m_pWorldAxisTransform = NULL;
		m_IsRay = true;

		// Offset m_Start to be in the center of the box...
		m_StartOffset.Set( );
		m_Start = start;
	}

	void Init( Vector const& start, Vector const& end, Vector const& mins, Vector const& maxs )
	{
		m_Delta = end - start;

		m_pWorldAxisTransform = NULL;
		m_IsSwept = ( m_Delta.LengthSqr( ) != 0 );

		m_Extents = maxs - mins;
		m_Extents *= 0.5f;
		m_IsRay = ( m_Extents.LengthSqr( ) < 1e-6 );

		// Offset m_Start to be in the center of the box...
		m_StartOffset = maxs + mins;
		m_StartOffset *= 0.5f;
		m_Start = start + m_StartOffset;
		m_StartOffset *= -1.0f;
	}
	Vector InvDelta( ) const
	{
		Vector vecInvDelta;
		for( int iAxis = 0; iAxis < 3; ++iAxis )
		{
			if( m_Delta[ iAxis ] != 0.0f )
			{
				vecInvDelta[ iAxis ] = 1.0f / m_Delta[ iAxis ];
			}
			else
			{
				vecInvDelta[ iAxis ] = FLT_MAX;
			}
		}
		return vecInvDelta;
	}

private:
};

#define   DISPSURF_FLAG_SURFACE           (1<<0)
#define   DISPSURF_FLAG_WALKABLE          (1<<1)
#define   DISPSURF_FLAG_BUILDABLE         (1<<2)
#define   DISPSURF_FLAG_SURFPROP1         (1<<3)
#define   DISPSURF_FLAG_SURFPROP2         (1<<4)

struct cplane_t
{
	Vector normal;
	float dist;
	uint8_t type;   // for fast side tests
	uint8_t signbits;  // signx + (signy<<1) + (signz<<1)
	uint8_t pad[ 2 ];

};

class CBaseTrace
{
public:
	bool IsDispSurface( void ) { return ( ( dispFlags & DISPSURF_FLAG_SURFACE ) != 0 ); }
	bool IsDispSurfaceWalkable( void ) { return ( ( dispFlags & DISPSURF_FLAG_WALKABLE ) != 0 ); }
	bool IsDispSurfaceBuildable( void ) { return ( ( dispFlags & DISPSURF_FLAG_BUILDABLE ) != 0 ); }
	bool IsDispSurfaceProp1( void ) { return ( ( dispFlags & DISPSURF_FLAG_SURFPROP1 ) != 0 ); }
	bool IsDispSurfaceProp2( void ) { return ( ( dispFlags & DISPSURF_FLAG_SURFPROP2 ) != 0 ); }

public:

	// these members are aligned!!
	Vector         startpos;            // start position
	Vector         endpos;              // final position
	cplane_t       plane;               // surface normal at impact

	float          fraction;            // time completed, 1.0 = didn't hit anything

	int            contents;            // contents on other side of surface hit
	unsigned short dispFlags;           // displacement flags for marking surfaces with data

	bool           allsolid;            // if true, plane is not valid
	bool           startsolid;          // if true, the initial point was in a solid area

	CBaseTrace( ) {}

};

class CGameTrace : public CBaseTrace
{
public:
	bool DidHitWorld( ) const;
	bool DidHitNonWorldEntity( ) const;
	int GetEntityIndex( ) const;
	bool DidHit( ) const;
	bool IsVisible( ) const;

public:

	float               fractionleftsolid;  // time we left a solid, only valid if we started in solid
	csurface_t          surface;            // surface hit (impact surface)
	int                 hitgroup;           // 0 == generic, non-zero is specific body part
	short               physicsbone;        // physics bone hit by trace in studio
	unsigned short      worldSurfaceIndex;  // Index of the msurface2_t, if applicable
	CBasePlayer* entity;
	int                 hitbox;                       // box hit by trace in studio

	CGameTrace( ) {}

private:
	// No copy constructors allowed
	CGameTrace( const CGameTrace& other ) :
		fractionleftsolid( other.fractionleftsolid ),
		surface( other.surface ),
		hitgroup( other.hitgroup ),
		physicsbone( other.physicsbone ),
		worldSurfaceIndex( other.worldSurfaceIndex ),
		entity( other.entity ),
		hitbox( other.hitbox )
	{
		startpos = other.startpos;
		endpos = other.endpos;
		plane = other.plane;
		fraction = other.fraction;
		contents = other.contents;
		dispFlags = other.dispFlags;
		allsolid = other.allsolid;
		startsolid = other.startsolid;
	}

	/*CGameTrace& CGameTrace::operator=(const CGameTrace& other)
	{
		startpos = other.startpos;
		endpos = other.endpos;
		plane = other.plane;
		fraction = other.fraction;
		contents = other.contents;
		dispFlags = other.dispFlags;
		allsolid = other.allsolid;
		startsolid = other.startsolid;
		fractionleftsolid = other.fractionleftsolid;
		surface = other.surface;
		hitgroup = other.hitgroup;
		physicsbone = other.physicsbone;
		worldSurfaceIndex = other.worldSurfaceIndex;
		entity = other.entity;
		hitbox = other.hitbox;
		return *this;
	}*/
};

inline bool CGameTrace::DidHit( ) const
{
	return fraction < 1 || allsolid || startsolid;
}

inline bool CGameTrace::IsVisible( ) const
{
	return fraction > 0.97f;
}

class IEngineTrace
{
public:
	VFUNC( 5, void, TraceRay, ( this, ray, mask, filter, trace ), const Ray_t& ray, unsigned int mask, ITraceFilter* filter, CGameTrace* trace );
};

extern IEngineTrace* enginetrace;

// contents flags are seperate bits
// a given brush can contribute multiple content bits
// multiple brushes can be in a single leaf

// these definitions also need to be in q_shared.h!

// lower bits are stronger, and will eat weaker brushes completely
#define	CONTENTS_EMPTY			0		// No contents

#define	CONTENTS_SOLID			0x1		// an eye is never valid in a solid
#define	CONTENTS_WINDOW			0x2		// translucent, but not watery (glass)
#define	CONTENTS_AUX			0x4
#define	CONTENTS_GRATE			0x8		// alpha-tested "grate" textures.  Bullets/sight pass through, but solids don't
#define	CONTENTS_SLIME			0x10
#define	CONTENTS_WATER			0x20
#define	CONTENTS_BLOCKLOS		0x40	// block AI line of sight
#define CONTENTS_OPAQUE			0x80	// things that cannot be seen through (may be non-solid though)
#define	LAST_VISIBLE_CONTENTS	0x80

#define ALL_VISIBLE_CONTENTS (LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1))

#define CONTENTS_TESTFOGVOLUME	0x100
#define CONTENTS_UNUSED			0x200	

// unused 
// NOTE: If it's visible, grab from the top + update LAST_VISIBLE_CONTENTS
// if not visible, then grab from the bottom.
#define CONTENTS_UNUSED6		0x400

#define CONTENTS_TEAM1			0x800	// per team contents used to differentiate collisions 
#define CONTENTS_TEAM2			0x1000	// between players and objects on different teams

// ignore CONTENTS_OPAQUE on surfaces that have SURF_NODRAW
#define CONTENTS_IGNORE_NODRAW_OPAQUE	0x2000

// hits entities which are MOVETYPE_PUSH (doors, plats, etc.)
#define CONTENTS_MOVEABLE		0x4000

// remaining contents are non-visible, and don't eat brushes
#define	CONTENTS_AREAPORTAL		0x8000

#define	CONTENTS_PLAYERCLIP		0x10000
#define	CONTENTS_MONSTERCLIP	0x20000

// currents can be added to any other contents, and may be mixed
#define	CONTENTS_CURRENT_0		0x40000
#define	CONTENTS_CURRENT_90		0x80000
#define	CONTENTS_CURRENT_180	0x100000
#define	CONTENTS_CURRENT_270	0x200000
#define	CONTENTS_CURRENT_UP		0x400000
#define	CONTENTS_CURRENT_DOWN	0x800000

#define	CONTENTS_ORIGIN			0x1000000	// removed before bsping an entity

#define	CONTENTS_MONSTER		0x2000000	// should never be on a brush, only in game
#define	CONTENTS_DEBRIS			0x4000000
#define	CONTENTS_DETAIL			0x8000000	// brushes to be added after vis leafs
#define	CONTENTS_TRANSLUCENT	0x10000000	// auto set if any surface has trans
#define	CONTENTS_LADDER			0x20000000
#define CONTENTS_HITBOX			0x40000000	// use accurate hitboxes on trace


// NOTE: These are stored in a short in the engine now.  Don't use more than 16 bits
#define	SURF_LIGHT		0x0001		// value will hold the light strength
#define	SURF_SKY2D		0x0002		// don't draw, indicates we should skylight + draw 2d sky but not draw the 3D skybox
#define	SURF_SKY		0x0004		// don't draw, but add to skybox
#define	SURF_WARP		0x0008		// turbulent water warp
#define	SURF_TRANS		0x0010
#define SURF_NOPORTAL	0x0020	// the surface can not have a portal placed on it
#define	SURF_TRIGGER	0x0040	// FIXME: This is an xbox hack to work around elimination of trigger surfaces, which breaks occluders
#define	SURF_NODRAW		0x0080	// don't bother referencing the texture

#define	SURF_HINT		0x0100	// make a primary bsp splitter

#define	SURF_SKIP		0x0200	// completely ignore, allowing non-closed brushes
#define SURF_NOLIGHT	0x0400	// Don't calculate light
#define SURF_BUMPLIGHT	0x0800	// calculate three lightmaps for the surface for bumpmapping
#define SURF_NOSHADOWS	0x1000	// Don't receive shadows
#define SURF_NODECALS	0x2000	// Don't receive decals
#define SURF_NOCHOP		0x4000	// Don't subdivide patches on this surface 
#define SURF_HITBOX		0x8000	// surface is part of a hitbox



// -----------------------------------------------------
// spatial content masks - used for spatial queries (traceline,etc.)
// -----------------------------------------------------
#define	MASK_ALL					(0xFFFFFFFF)
// everything that is normally solid
#define	MASK_SOLID					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// everything that blocks player movement
#define	MASK_PLAYERSOLID			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// blocks npc movement
#define	MASK_NPCSOLID				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// water physics in these contents
#define	MASK_WATER					(CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME)
// everything that blocks lighting
#define	MASK_OPAQUE					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE)
// everything that blocks lighting, but with monsters added.
#define MASK_OPAQUE_AND_NPCS		(MASK_OPAQUE|CONTENTS_MONSTER)
// everything that blocks line of sight for AI
#define MASK_BLOCKLOS				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS)
// everything that blocks line of sight for AI plus NPCs
#define MASK_BLOCKLOS_AND_NPCS		(MASK_BLOCKLOS|CONTENTS_MONSTER)
// everything that blocks line of sight for players
#define	MASK_VISIBLE					(MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE)
// everything that blocks line of sight for players, but with monsters added.
#define MASK_VISIBLE_AND_NPCS		(MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE)
// bullets see these as solid
#define	MASK_SHOT					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)
// non-raycasted weapons see this as solid (includes grates)
#define MASK_SHOT_HULL				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
// hits solids (not grates) and passes through everything else
#define MASK_SHOT_PORTAL			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER)
// everything normally solid, except monsters (world+brush only)
#define MASK_SOLID_BRUSHONLY		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
// everything normally solid for player movement, except monsters (world+brush only)
#define MASK_PLAYERSOLID_BRUSHONLY	(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE)
// everything normally solid for npc movement, except monsters (world+brush only)
#define MASK_NPCSOLID_BRUSHONLY		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
// just the world, used for route rebuilding
#define MASK_NPCWORLDSTATIC			(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
// These are things that can split areaportals
#define MASK_SPLITAREAPORTAL		(CONTENTS_WATER|CONTENTS_SLIME)

// UNDONE: This is untested, any moving water
#define MASK_CURRENT				(CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)

// everything that blocks corpse movement
// UNDONE: Not used yet / may be deleted
#define	MASK_DEADSOLID				(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_GRATE)

// FUNCS //
std::uint8_t * PatternScan( void* module, const char* signature );