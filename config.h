#pragma once
#include "main.h"
#include <experimental/filesystem>

class CConfig
{
public:
	void Save( );
	void Load( );
	void SetName( const char* _configname );
private:
	const char* configname = "";
};

extern std::unique_ptr<CConfig> config;