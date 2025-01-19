#include "common.h"
#include "SpriteMaps.h"
#include "GroundMaterialConfiguration.h"
#include "ContentLoader.h"
#include "GUI.h"
#include "TileTree.h"
#include "TileTypes.h"
#include "StonesenseState.h"

c_sprite *  GetTerrainSpriteMap(int in, DFHack::t_matglossPair material, std::vector<std::unique_ptr<TerrainConfiguration>>& configTable, uint16_t form)
{
    // in case we need to return nothing
    static c_sprite defaultSprite{};
    defaultSprite.reset();
    defaultSprite.set_sheetindex(UNCONFIGURED_INDEX);
    defaultSprite.set_fileindex(INVALID_INDEX);
    defaultSprite.set_needoutline(1);

    int tempform;
    switch (form)
    {
        using df::item_type;
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
        return &defaultSprite;
    }

    // first check the input is sane
    if( in < 0 || in >= (int)configTable.size() ) {
        return &defaultSprite;
    }
    // find a matching terrainConfig
    TerrainConfiguration* terrain = configTable[in].get();
    if (terrain == nullptr) {
        return &defaultSprite;
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

c_sprite * GetFloorSpriteMap(int in, DFHack::t_matglossPair material, uint16_t form)
{
    return GetTerrainSpriteMap(in, material, stonesenseState.contentLoader->terrainFloorConfigs, form);
}

c_sprite * GetTileSpriteMap(int in, DFHack::t_matglossPair material, uint16_t form)
{
    return GetTerrainSpriteMap(in, material, stonesenseState.contentLoader->terrainWallConfigs, form);
}

c_tile_tree * GetTreeVegetation(df::tiletype_shape shape, df::tiletype_special special, int index)
{
    int base_sprite = SPRITEOBJECT_BLUEPRINT;
    std::vector<std::unique_ptr<VegetationConfiguration>>* graphicSet;
    bool live=true;
    bool grown=true;
    auto& contentLoader = stonesenseState.contentLoader;
    switch(shape) {
        using df::tiletype_shape;
    case tiletype_shape::TRUNK_BRANCH:
        if (special == df::tiletype_special::DEAD) {
            base_sprite = SPRITEOBJECT_TREE_DEAD;
            graphicSet = &(contentLoader->treeConfigs);
            live = false;
        } else {
            base_sprite = SPRITEOBJECT_TREE_OK;
            graphicSet = &(contentLoader->treeConfigs);
        }
        break;
    case tiletype_shape::SAPLING:
        if (special == df::tiletype_special::DEAD) {
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
        if (special == df::tiletype_special::DEAD) {
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
