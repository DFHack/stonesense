#pragma once
#include "common.h"
#include "tinyxml.h"
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
    ColorConfiguration();
    ~ColorConfiguration();
} ;

bool addSingleColorConfig( TiXmlElement* elemRoot);

void flushColorConfig(vector<ColorConfiguration>& config);
