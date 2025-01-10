#pragma once
#include "common.h"
#include "tinyxml.h"

class ColorMaterialConfiguration
{
public:
    ALLEGRO_COLOR color;
    bool colorSet;
    ColorMaterialConfiguration() :
        color{ al_map_rgb(255,255,255) }, colorSet{ false }
    { };
};


class ColorConfiguration
{
public:
    std::vector<ColorMaterialConfiguration> colorMaterials;
    ALLEGRO_COLOR color;
    bool colorSet;
    ColorConfiguration() :
        color(al_map_rgb(255, 255, 255)), colorSet{ false }
    { };

    ~ColorConfiguration() = default;
};

bool addSingleColorConfig( TiXmlElement* elemRoot);
