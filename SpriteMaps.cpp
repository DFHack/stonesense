#include "common.h"
#include "SpriteMaps.h"
#include "GroundMaterialConfiguration.h"
#include "ContentLoader.h"
#include "GUI.h"
#include "TileTree.h"
#include "TileTypes.h"

c_sprite *  GetTerrainSpriteMap(int in, t_matglossPair material, vector<TerrainConfiguration*>& configTable, uint16_t form)
{
    int tempform;
    if(form == item_type::BAR) {
        tempform = FORM_BAR;
    }
    if(form == item_type::BLOCKS) {
        tempform = FORM_BLOCK;
    }
    if(form == item_type::BOULDER) {
        tempform = FORM_BOULDER;
    }
    if(form == item_type::WOOD) {
        tempform = FORM_LOG;
    }
    // in case we need to return nothing
    static c_sprite * defaultSprite = new c_sprite;
    defaultSprite->reset();
    defaultSprite->set_sheetindex(UNCONFIGURED_INDEX);
    defaultSprite->set_fileindex(INVALID_INDEX);
    defaultSprite->set_needoutline(1);
    // first check the input is sane
    if( in < 0 || in >= (int)configTable.size() ) {
        return defaultSprite;
    }
    // find a matching terrainConfig
    TerrainConfiguration* terrain = configTable[in];
    if (terrain == NULL) {
        return defaultSprite;
    }
    // check material sanity
    if (material.type<0 || material.type >= (int16_t)terrain->terrainMaterials.size()) {
        if(terrain->defaultSprite[tempform].first.get_sheetindex() == UNCONFIGURED_INDEX) {
            return &(terrain->defaultSprite[0].first);
        } else {
            return &(terrain->defaultSprite[tempform].first);
        }
    }
    // find mat config
    TerrainMaterialConfiguration* terrainMat = terrain->terrainMaterials[material.type];
    if (terrainMat == NULL) {
        if (terrain->defaultSprite[tempform].first.get_sheetindex() == UNCONFIGURED_INDEX) {
            return &(terrain->defaultSprite[0].first);
        } else {
            return &(terrain->defaultSprite[tempform].first);
        }
    }
    if(material.index == -1) {
        if (terrainMat->defaultSprite[tempform].first.get_sheetindex() == UNCONFIGURED_INDEX) {
            return &(terrainMat->defaultSprite[0].first);
        } else {
            return &(terrainMat->defaultSprite[tempform].first);
        }
    }
    // return subtype, type default or terrain default as available
    // do map lookup
    map<int,pair<c_sprite, int>>::iterator it = terrainMat->overridingMaterials[tempform].find(material.index);
    if (it != terrainMat->overridingMaterials[tempform].end()) {
        return &(it->second.first);
    }
    if (terrainMat->defaultSprite[tempform].first.get_sheetindex() != UNCONFIGURED_INDEX) {
        return &(terrainMat->defaultSprite[tempform].first);
    }
    it = terrainMat->overridingMaterials[0].find(material.index);
    if (it != terrainMat->overridingMaterials[0].end()) {
        return &(it->second.first);
    }
    if (terrainMat->defaultSprite[0].first.get_sheetindex() != UNCONFIGURED_INDEX) {
        return &(terrainMat->defaultSprite[0].first);
    }
    return &(terrain->defaultSprite[0].first);
}

c_sprite * GetFloorSpriteMap(int in, t_matglossPair material, uint16_t form)
{
    return GetTerrainSpriteMap(in, material, contentLoader->terrainFloorConfigs, form);
}

c_sprite * GetTileSpriteMap(int in, t_matglossPair material, uint16_t form)
{
    return GetTerrainSpriteMap(in, material, contentLoader->terrainWallConfigs, form);
}

c_tile_tree * GetTreeVegetation(RemoteFortressReader::TiletypeShape shape, RemoteFortressReader::TiletypeSpecial special, int index)
{
    int base_sprite = SPRITEOBJECT_BLUEPRINT;
    vector<VegetationConfiguration>* graphicSet;
    bool live=true;
    bool grown=true;
    switch(shape) {
    case RemoteFortressReader::TiletypeShape::TREE_SHAPE:
        if (special == RemoteFortressReader::TiletypeSpecial::DEAD) {
            base_sprite = SPRITEOBJECT_TREE_DEAD;
            graphicSet = &(contentLoader->treeConfigs);
            live = false;
        } else {
            base_sprite = SPRITEOBJECT_TREE_OK;
            graphicSet = &(contentLoader->treeConfigs);
        }
        break;
    case RemoteFortressReader::TiletypeShape::SAPLING:
        if (special == RemoteFortressReader::TiletypeSpecial::DEAD) {
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
    case RemoteFortressReader::TiletypeShape::SHRUB:
        if (special == RemoteFortressReader::TiletypeSpecial::DEAD) {
            base_sprite = SPRITEOBJECT_SHRUB_DEAD;
            live = false;
            graphicSet = &(contentLoader->shrubConfigs);
        } else {
            base_sprite = SPRITEOBJECT_SHRUB_OK;
            graphicSet = &(contentLoader->shrubConfigs);
        }
        break;
    default:
        return null;
    }

    c_tile_tree * configuredTree = getVegetationTree(*graphicSet,index,live,grown);
    if (configuredTree->get_sheetindex() == -1) {
        configuredTree->set_fileindex(-1); // should be set already, but...
        configuredTree->set_sheetindex(base_sprite);
    }
    return configuredTree;
}

