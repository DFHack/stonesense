#pragma once

#include "tinyxml.h"
#include "MaterialMatcher.h"

struct OverridingMaterial {
    int gameID;
    t_SpriteWithOffset sprite;
};

class TerrainMaterialConfiguration
{
private:
    std::vector<std::pair<c_sprite, int>> defaultSprite;
    std::vector<std::map<int, std::pair<c_sprite, int>> > overridingMaterials;
public:
    TerrainMaterialConfiguration();
    void updateSprite(int j, c_sprite& sprite, int x);
    void updateOverridingMaterials(auto j, auto subtypeId, auto sprite, auto x);
    c_sprite& getSprite(int idx)
    {
        return defaultSprite[idx].first;
    }
    c_sprite& getOverridingMaterial(int idx, auto material, auto terrain)
    {
        auto it = overridingMaterials[idx].find(material.index);
        if (it != overridingMaterials[idx].end()) {
            return (it->second.first);
        }

        if (defaultSprite[idx].first.get_sheetindex() != UNCONFIGURED_INDEX) {
            return (defaultSprite[idx].first);
        }

        auto it2 = overridingMaterials[0].find(material.index);
        if (it2 != overridingMaterials[0].end()) {
            return (it2->second.first);
        }

        if (defaultSprite[0].first.get_sheetindex() != UNCONFIGURED_INDEX) {
            return (defaultSprite[0].first);
        }

        return (terrain->getDefaultSprite(0));
    }
};

class TerrainConfiguration
{
private:
    std::unordered_map<int, std::unique_ptr<TerrainMaterialConfiguration>> terrainMaterials;
    std::vector<std::pair<c_sprite, int>> defaultSprite;
public:
    TerrainConfiguration();
    void updateSprite(auto j, auto sprite, auto x);
    void expand(auto elemIndex);
    auto& getTerrainMaterials(int idx)
    {
        return terrainMaterials[idx];
    }
    c_sprite& getDefaultSprite(auto idx)
    {
        return defaultSprite[idx].first;
    }

};

bool addSingleTerrainConfig( TiXmlElement* elemRoot);

void flushTerrainConfig(std::vector<std::unique_ptr<TerrainConfiguration>>& config);
void DumpInorganicMaterialNamesToDisk();
