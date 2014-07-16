#pragma once

#include "tinyxml.h"
#include "MaterialMatcher.h"

typedef struct OverridingMaterial {
    int gameID;
    t_SpriteWithOffset sprite;
} OverridingMaterial;

class TerrainMaterialConfiguration
{
public:
    std::vector<std::pair<c_sprite, int>> defaultSprite;
    int gameID;
    std::vector<std::map<int, std::pair<c_sprite, int>> > overridingMaterials;

    TerrainMaterialConfiguration();
    ~TerrainMaterialConfiguration() {}
};

class TerrainConfiguration
{
public:
    std::vector<TerrainMaterialConfiguration*> terrainMaterials;
    std::vector<std::pair<c_sprite, int>> defaultSprite;
    TerrainConfiguration();
    ~TerrainConfiguration();
};

bool addSingleTerrainConfig( TiXmlElement* elemRoot);
//void LoadGroundMaterialConfiguration( );
//void TranslateGroundMaterialNames();

//extern bool GroundMaterialNamesTranslatedFromGame;
//extern vector<GroundMaterialConfiguration*> groundTypes;

void flushTerrainConfig(std::vector<TerrainConfiguration*>& config);
void DumpInorganicMaterialNamesToDisk();