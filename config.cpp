#include "config.h"
#include "mINI.h"

std::unique_ptr<CConfig> config;

void CConfig::Save( )
{
	if( !std::experimental::filesystem::exists( configname ) )
	{
		std::ofstream k( configname );
		k.close( );
	}

	mINI::INIFile file( configname );
	mINI::INIStructure ini;
	file.read( ini );

	ini[ "csgo" ].set( {

		{"esp_box", std::to_string( vars->esp_box )},
		{"esp_health", std::to_string( vars->esp_health )},
		{"backtrack", std::to_string( vars->backtrack )},
		{"backtrack_ticks_remove", std::to_string( vars->backtrack_ticks )},
		{"backtrack_draw", std::to_string( vars->backtrack_draw )},
		{"backtrack_draw_r", std::to_string( vars->backtrack_draw_r )},
		{"backtrack_draw_g", std::to_string( vars->backtrack_draw_g )},
		{"backtrack_draw_b", std::to_string( vars->backtrack_draw_b )},
		{"antiscreen", std::to_string( vars->antiscreen )},
		{"bunnyhop", std::to_string( vars->bunnyhop )}

		} );

	file.write( ini );
}

void CConfig::Load( )
{
	auto ToBool = []( std::string str ) -> bool
	{
		return str != "0";
	};

	mINI::INIFile file( configname );
	mINI::INIStructure ini;
	file.read( ini );

	auto csgo = ini.get( "csgo" );

	vars->esp_box = ToBool( csgo.get( "esp_box" ) );
	vars->esp_health = ToBool( csgo.get( "esp_health" ) );
	vars->backtrack = ToBool( csgo.get( "backtrack" ) );
	vars->backtrack_ticks = std::stoi( csgo.get( "backtrack_ticks_remove" ) );
	vars->backtrack_draw = ToBool( csgo.get( "backtrack_draw" ) );
	vars->backtrack_draw_r = std::stoi( csgo.get( "backtrack_draw_r" ) );
	vars->backtrack_draw_g = std::stoi( csgo.get( "backtrack_draw_g" ) );
	vars->backtrack_draw_b = std::stoi( csgo.get( "backtrack_draw_b" ) );
	vars->antiscreen = ToBool( csgo.get( "antiscreen" ) );
	vars->bunnyhop = ToBool( csgo.get( "bunnyhop" ) );
}

void CConfig::SetName( const char* _configname )
{
	configname = _configname;
}