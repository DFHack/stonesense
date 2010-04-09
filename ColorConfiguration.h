#pragma once
#include "common.h"
#include "dfhack/depends/tinyxml/tinyxml.h"
#include <map>

class ColorMaterialConfiguration
{
public:
	ALLEGRO_COLOR color;
	bool colorSet;
	ColorMaterialConfiguration();
} ;


class ColorConfiguration
{
public:
	vector<ColorMaterialConfiguration> colorMaterials;
	ALLEGRO_COLOR color;
	bool colorSet;
	~ColorConfiguration();
} ;

bool addSingleColorConfig( TiXmlElement* elemRoot);

void flushColorConfig(vector<ColorConfiguration>& config);
