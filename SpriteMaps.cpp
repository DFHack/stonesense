#include "common.h"
#include "SpriteMaps.h"
#include "GroundMaterialConfiguration.h"
#include "ContentLoader.h"


t_SpriteWithOffset GetTerrainSpriteMap(int in, t_matglossPair material, vector<TerrainConfiguration*>& configTable)
{
	// in case we need to return nothing
	t_SpriteWithOffset defaultSprite = {-1,0,0,-1,ALL_FRAMES};
	// first check the input is sane
	if( in < 0 || in >= (int)configTable.size() )
	{
		return defaultSprite;
	}
	// find a matching terrainConfig
	TerrainConfiguration* terrain = configTable[in];
	if (terrain == NULL)
	{
		return defaultSprite;
	}
	// check material sanity
	if (material.type<0 || material.type >= terrain->terrainMaterials.size())
	{
		return terrain->defaultSprite;
	}
	// find mat config
	TerrainMaterialConfiguration* terrainMat = terrain->terrainMaterials[material.type];
	if (terrainMat == NULL)
	{
		return terrain->defaultSprite;
	}
	// return subtype, type default or terrain default as available
	// do map lookup
	map<int,t_SpriteWithOffset>::iterator it = terrainMat->overridingMaterials.find(material.index);
	if (it != terrainMat->overridingMaterials.end())
	{
		return it->second;
	}
	if (terrainMat->defaultSprite.sheetIndex != INVALID_INDEX)
	{
		return terrainMat->defaultSprite;
	}
	return terrain->defaultSprite;
}

t_SpriteWithOffset GetFloorSpriteMap(int in, t_matglossPair material){
	return GetTerrainSpriteMap(in, material, contentLoader.terrainFloorConfigs);
}

t_SpriteWithOffset GetBlockSpriteMap(int in, t_matglossPair material){
	return GetTerrainSpriteMap(in, material, contentLoader.terrainBlockConfigs);
}

t_SpriteWithOffset GetSpriteVegetation( TileClass type, int index)
{
	int base_sprite = SPRITEOBJECT_BLUEPRINT;
	vector<VegetationConfiguration>* graphicSet;
	bool live=true;
	bool grown=true;
	
	switch(type)
	{
	case TREE_DEAD:
		base_sprite = SPRITEOBJECT_TREE_DEAD;
		graphicSet = &(contentLoader.treeConfigs);
		live = false;
		break;
	case TREE_OK:
		base_sprite = SPRITEOBJECT_TREE_OK;
		graphicSet = &(contentLoader.treeConfigs);
		break;
	case SAPLING_DEAD:
		base_sprite = SPRITEOBJECT_SAPLING_DEAD;
		live = false;
		grown = false;
		graphicSet = &(contentLoader.treeConfigs);
		break;
	case SAPLING_OK: 
		base_sprite = SPRITEOBJECT_SAPLING_OK;
		grown = false;
		graphicSet = &(contentLoader.treeConfigs);
		break;
	case SHRUB_DEAD:
		base_sprite = SPRITEOBJECT_SHRUB_DEAD;
		live = false;
		graphicSet = &(contentLoader.shrubConfigs);
		break;
	case SHRUB_OK: 
		base_sprite = SPRITEOBJECT_SHRUB_OK;
		graphicSet = &(contentLoader.shrubConfigs);
		break;
	default:
		t_SpriteWithOffset defaultSprite = {SPRITEOBJECT_BLANK,0,0,-1,ALL_FRAMES};
		return defaultSprite;
	}  	
  	
	t_SpriteWithOffset configuredSprite = getVegetationSprite(*graphicSet,index,live,grown);
	if (configuredSprite.sheetIndex == -1)
	{
		configuredSprite.fileIndex = -1; // should be set already, but...
		configuredSprite.sheetIndex = base_sprite;
	}
	return configuredSprite;
}

