#pragma once
#include "main.h"

class CESP
{
public:
	bool WorldToScreen( const Vector& in, Vector& out );
	void Draw( );
};

extern std::unique_ptr<CESP> esp;