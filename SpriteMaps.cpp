#include "common.h"
#include "SpriteMaps.h"
#include "GroundMaterialConfiguration.h"
#include "ContentLoader.h"
#include "GUI.h"
#include "TileTree.h"
#include "TileTypes.h"

using namespace std;
using namespace DFHack;
using namespace df::enums;

c_sprite *  GetTerrainSpriteMap(int in, t_matglossPair material, vector<std::unique_ptr<TerrainConfiguration>>& configTable, uint16_t form)
{
    // in case we need to return nothing
    static c_sprite* defaultSprite = new c_sprite;
    defaultSprite->reset();
    defaultSprite->set_sheetindex(UNCONFIGURED_INDEX);
    defaultSprite->set_fileindex(INVALID_INDEX);
    defaultSprite->set_needoutline(1);

    int tempform;
    switch (form)
    {
    case item_type::BAR:
        tempform = FORM_BAR;
        break;
    case item_type::BLOCKS:
        tempform = FORM_BLOCK;
        break;
    case item_type::BOULDER:
        tempform = FORM_BOULDER;
        break;
    case item_type::WOOD:
        tempform = FORM_LOG;
        break;
    default:
        return defaultSprite;
    }

    // first check the input is sane
    if( in < 0 || in >= (int)configTable.size() ) {
        return defaultSprite;
    }
    // find a matching terrainConfig
    TerrainConfiguration* terrain = configTable[in].get();
    if (terrain == nullptr) {
        return defaultSprite;
    }
    // find mat config

    auto& terrainMat = terrain->getTerrainMaterials(material.type);
    if (!terrainMat) {
        if(terrain->getDefaultSprite(tempform).get_sheetindex() == UNCONFIGURED_INDEX) {
            return &(terrain->getDefaultSprite(0));
        } else {
            return &(terrain->getDefaultSprite(tempform));
        }
    }

    if(material.index == -1) {
        if (terrainMat->getSprite(tempform).get_sheetindex() == UNCONFIGURED_INDEX) {
            return &(terrainMat->getSprite(0));
        } else {
            return &(terrainMat->getSprite(tempform));
        }
    }
    // return subtype, type default or terrain default as available
    // do map lookup
    return &terrainMat->getOverridingMaterial(tempform, material, terrain);
}

c_sprite * GetFloorSpriteMap(int in, t_matglossPair material, uint16_t form)
{
    return GetTerrainSpriteMap(in, material, contentLoader->terrainFloorConfigs, form);
}

c_sprite * GetTileSpriteMap(int in, t_matglossPair material, uint16_t form)
{
    return GetTerrainSpriteMap(in, material, contentLoader->terrainWallConfigs, form);
}

c_tile_tree * GetTreeVegetation(df::tiletype_shape shape, df::tiletype_special special, int index)
{
    int base_sprite = SPRITEOBJECT_BLUEPRINT;
    vector<std::unique_ptr<VegetationConfiguration>>* graphicSet;
    bool live=true;
    bool grown=true;
    switch(shape) {
    case tiletype_shape::TRUNK_BRANCH:
        if (special == tiletype_special::DEAD) {
            base_sprite = SPRITEOBJECT_TREE_DEAD;
            graphicSet = &(contentLoader->treeConfigs);
            live = false;
        } else {
            base_sprite = SPRITEOBJECT_TREE_OK;
            graphicSet = &(contentLoader->treeConfigs);
        }
        break;
    case tiletype_shape::SAPLING:
        if (special == tiletype_special::DEAD) {
            base_sprite = SPRITEOBJECT_SAPLING_DEAD;
            live = false;
            grown = false;
            graphicSet = &(contentLoader->treeConfigs);
        } else {
            base_sprite = SPRITEOBJECT_SAPLING_OK;
            grown = false;
            graphicSet = &(contentLoader->treeConfigs);
        }
        break;
    case tiletype_shape::SHRUB:
        if (special == tiletype_special::DEAD) {
            base_sprite = SPRITEOBJECT_SHRUB_DEAD;
            live = false;
            graphicSet = &(contentLoader->shrubConfigs);
        } else {
            base_sprite = SPRITEOBJECT_SHRUB_OK;
            graphicSet = &(contentLoader->shrubConfigs);
        }
        break;
    default:
        return nullptr;
    }

    c_tile_tree * configuredTree = getVegetationTree(*graphicSet,index,live,grown);
    if (configuredTree->get_sheetindex() == -1) {
        configuredTree->set_fileindex(-1); // should be set already, but...
        configuredTree->set_sheetindex(base_sprite);
    }
    return configuredTree;
}
