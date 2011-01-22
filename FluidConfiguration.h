#pragma once
#include "common.h"
#include "dfhack/depends/tinyxml/tinyxml.h"
#include "SpriteObjects.h"

class FluidConfiguration
{
public:
	c_sprite sprite;
	bool fluidset;
	FluidConfiguration();
} ;

void parseFluidElement( TiXmlElement* elemFluid, int basefile);
bool addSingleFluidConfig( TiXmlElement* elemRoot);