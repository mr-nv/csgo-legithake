#pragma once

#define USE_ENGINE_PREDICTION

#pragma warning(disable : 4244 4018)

#include "sdk.h"
#include "entity.h"
#include "vmt.h"
#include "lagcompensation.h"
#include "esp.h"
#include "hooks.h"
#include "menu.h"
#include "config.h"

extern CUserCmd* _cmd;
extern CBasePlayer* me;

#define PI 3.14159265358979323846f
#define DEG2RAD( x ) ( ( float )( x ) * ( float )( ( float )( PI ) / 180.0f ) )
#define RAD2DEG( x ) ( ( float )( x ) * ( float )( 180.0f / ( float )( PI ) ) )
#define square( x ) ( x * x )
#define CHECK_VALID( _v ) 0
#define Assert( _exp ) ((void)0)

#define	add(dst, flag)	((dst) |= (flag))
#define	del(dst, flag)	((dst) &=~(flag))
#define	not(dst, flag)	((dst) & ~(flag))
#define	chk(dst, flag)	((dst) &  (flag))