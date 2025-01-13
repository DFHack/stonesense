#pragma once

#include "tinyxml.h"
#include "MaterialMatcher.h"

typedef struct OverridingMaterial {
    int gameID;
    t_SpriteWithOffset sprite;
} OverridingMaterial;

class TerrainMaterialConfiguration
{
private:
    std::vector<std::pair<c_sprite, int>> defaultSprite;
    int gameID;
    std::vector<std::map<int, std::pair<c_sprite, int>> > overridingMaterials;
public:
    TerrainMaterialConfiguration();
    void updateSprite(auto j, auto sprite, auto x);
    void updateOverridingMaterials(auto j, auto subtypeId, auto sprite, auto x);
    c_sprite& getSprite(auto idx) {
        return defaultSprite[idx].first;
    }
    c_sprite& getOverridingMaterial(auto idx, auto material, auto terrain)
    {
        auto it = this->overridingMaterials[idx].find(material.index);
        if (it != this->overridingMaterials[idx].end()) {
            return (it->second.first);
        }

        if (this->defaultSprite[idx].first.get_sheetindex() != UNCONFIGURED_INDEX) {
            return (this->defaultSprite[idx].first);
        }

        auto it2 = this->overridingMaterials[0].find(material.index);
        if (it2 != this->overridingMaterials[0].end()) {
            return (it2->second.first);
        }

        if (this->defaultSprite[0].first.get_sheetindex() != UNCONFIGURED_INDEX) {
            return (this->defaultSprite[0].first);
        }

        return (terrain->defaultSprite[0].first);
    }
};

class TerrainConfiguration
{
public:
    std::vector<std::unique_ptr<TerrainMaterialConfiguration>> terrainMaterials;
    std::vector<std::pair<c_sprite, int>> defaultSprite;
    TerrainConfiguration();
};

bool addSingleTerrainConfig( TiXmlElement* elemRoot);

void flushTerrainConfig(std::vector<std::unique_ptr<TerrainConfiguration>>& config);
void DumpInorganicMaterialNamesToDisk();
