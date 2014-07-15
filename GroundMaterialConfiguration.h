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
    vector<c_sprite> defaultSprite;
    int gameID;
    vector<map<int,c_sprite> > overridingMaterials;

    TerrainMaterialConfiguration();
    ~TerrainMaterialConfiguration() {}
};

class TerrainConfiguration
{
public:
    vector<TerrainMaterialConfiguration*> terrainMaterials;
    vector<c_sprite> defaultSprite;
    int priority;
    TerrainConfiguration();
    ~TerrainConfiguration();
};

bool addSingleTerrainConfig( TiXmlElement* elemRoot);
//void LoadGroundMaterialConfiguration( );
//void TranslateGroundMaterialNames();

//extern bool GroundMaterialNamesTranslatedFromGame;
//extern vector<GroundMaterialConfiguration*> groundTypes;

void flushTerrainConfig(vector<TerrainConfiguration*>& config);
void DumpInorganicMaterialNamesToDisk();