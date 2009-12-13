#include "common.h"
#include "SpriteMaps.h"
#include "GroundMaterialConfiguration.h"
#include <set>
#include "dfhack/library/tinyxml/tinyxml.h"
#include "GUI.h"
#include "ContentLoader.h"

TerrainMaterialConfiguration::TerrainMaterialConfiguration()
{
	defaultSprite.fileIndex=INVALID_INDEX;
	defaultSprite.sheetIndex=INVALID_INDEX;
	//dont really care about the rest of the sprite right now.
	
}

TerrainConfiguration::TerrainConfiguration()
{
	defaultSprite.fileIndex=INVALID_INDEX;
	defaultSprite.sheetIndex=INVALID_INDEX;
	//dont really care about the rest of the sprite right now.
}

TerrainConfiguration::~TerrainConfiguration()
{
	uint32_t currentsize=terrainMaterials.size();
	for (uint32_t i=0;i<currentsize;i++)
	{
		if (terrainMaterials[i] != NULL)
		{
			delete(terrainMaterials[i]);
		}
	}
}

void DumpGroundMaterialNamesToDisk(){
  FILE* fp = fopen("dump.txt", "w");
  if(!fp) return;
  for(uint32_t j=0; j < contentLoader.stoneNameStrings.size(); j++){
    fprintf(fp, "%i:%s\n",j, contentLoader.stoneNameStrings[j].id);
  }
  fclose(fp);
}

void parseWallFloorSpriteElement( TiXmlElement* elemWallFloorSprite, vector<TerrainConfiguration*>& configTable ,int basefile)
{
	const char* spriteIndexStr = elemWallFloorSprite->Attribute("sprite");
	if (spriteIndexStr == NULL || spriteIndexStr[0] == 0)
	{
		contentError("Invalid or missing sprite attribute",elemWallFloorSprite);
		return; //nothing to work with
	}
	// make a base sprite
	t_SpriteWithOffset sprite;
	sprite.sheetIndex=atoi(spriteIndexStr);
	sprite.fileIndex=basefile;
	sprite.x=0;
	sprite.y=0;
	sprite.animFrames=ALL_FRAMES; //augh! no animated terrains! please!
	// check for local file definitions
	const char* filename = elemWallFloorSprite->Attribute("file");
	if (filename != NULL && filename[0] != 0)
	{
		sprite.fileIndex = loadConfigImgFile((char*)filename,elemWallFloorSprite);
	}
	
	vector<int> lookupKeys;
	
	// look through terrain elements
	TiXmlElement* elemTerrain = elemWallFloorSprite->FirstChildElement("terrain");
	for(TiXmlElement* elemTerrain = elemWallFloorSprite->FirstChildElement("terrain");
		 elemTerrain;
		 elemTerrain = elemTerrain->NextSiblingElement("terrain"))
	{
		//get a terrain type 
		int targetElem=INVALID_INDEX;
		const char* gameIDstr = elemTerrain->Attribute("value");
		if (gameIDstr == NULL || gameIDstr[0] == 0)
		{
			contentError("Invalid or missing value attribute",elemTerrain);
			continue;
		}
		targetElem = atoi (gameIDstr);
		//add it to the lookup vector
		lookupKeys.push_back(targetElem);
		if (configTable.size() <= targetElem)
		{
			//increase size if needed
			configTable.resize(targetElem+1,NULL);
		}
		if (configTable[targetElem]==NULL)
		{
			// cleaned up in flushTerrainConfig
			configTable[targetElem] = new TerrainConfiguration();
		}
	}
	
	// check we have some terrain types set
	int elems = lookupKeys.size();
	if (elems == 0)
		return; //nothing to link to
	
	// parse material elements
	TiXmlElement* elemMaterial = elemWallFloorSprite->FirstChildElement("material");
	if (elemMaterial == NULL)
	{
		// if none, set default terrain sprites for each terrain type
		for (int i=0 ; i < elems; i++ )
		{
			TerrainConfiguration *tConfig = configTable[lookupKeys[i]];
			// if that was null we have *really* screwed up earlier
			// only update if not by previous configs
			if (tConfig->defaultSprite.sheetIndex == INVALID_INDEX)
			{
				tConfig->defaultSprite = sprite;
			}
		}
	}
	for( ;elemMaterial;elemMaterial = elemMaterial->NextSiblingElement("material"))
	{
		// get material type
		int elemIndex = lookupMaterialType(elemMaterial->Attribute("value"));
		if (elemIndex == INVALID_INDEX)
		{
			contentError("Invalid or missing value attribute",elemMaterial);
			continue;				
		}
		
		// parse subtype elements
		TiXmlElement* elemSubtype = elemMaterial->FirstChildElement("subtype");
		if (elemSubtype == NULL)
		{
			// if none, set material default for each terrain type
			for (int i=0 ; i < elems; i++ )
			{
				TerrainConfiguration *tConfig = configTable[lookupKeys[i]];
				// if that was null we have *really* screwed up earlier
				// create a new TerrainMaterialConfiguration if required
					// make sure we have room for it first
				if (tConfig->terrainMaterials.size() <= elemIndex)
				{
					// dont make a full size vector in advance- most of the time
					// we will only need the first few
					tConfig->terrainMaterials.resize(elemIndex+1,NULL);
				}
				if (tConfig->terrainMaterials[elemIndex] == NULL)
				{
					tConfig->terrainMaterials[elemIndex] = new TerrainMaterialConfiguration();
				}
				// only update if not set by earlier configs
				if (tConfig->terrainMaterials[elemIndex]->defaultSprite.sheetIndex == INVALID_INDEX)
				{
					tConfig->terrainMaterials[elemIndex]->defaultSprite = sprite;
				}
			} 	
		}
		for (;elemSubtype; elemSubtype = elemSubtype ->NextSiblingElement("subtype"))
		{
			// get subtype
			int subtypeId = lookupMaterialIndex(elemIndex,elemSubtype->Attribute("value"));
			if (subtypeId == INVALID_INDEX)
			{
				contentError("Invalid or missing value attribute",elemSubtype);
				continue;				
			}
			
			// set subtype sprite for each terrain type
			for (int i=0 ; i < elems; i++ )
			{
				TerrainConfiguration *tConfig = configTable[lookupKeys[i]];
				//if that was null we have *really* screwed up earlier
				//create a new TerrainMaterialConfiguration if required
					//make sure we have room for it first
				if (tConfig->terrainMaterials.size() <= elemIndex)
				{
					//dont make a full size vector in advance- we wont need it except
					//for those who insist on Soap Fortresses
					tConfig->terrainMaterials.resize(elemIndex+1,NULL);
				}
				if (tConfig->terrainMaterials[elemIndex] == NULL)
				{
					tConfig->terrainMaterials[elemIndex] = new TerrainMaterialConfiguration();
				}
				// add to map (if not already present)
				map<int,t_SpriteWithOffset>::iterator it = tConfig->terrainMaterials[elemIndex]->overridingMaterials.find(subtypeId);
				if (it == tConfig->terrainMaterials[elemIndex]->overridingMaterials.end())
				{
					tConfig->terrainMaterials[elemIndex]->overridingMaterials[subtypeId]=sprite;
				}			
			} 			
		}
	}
}

bool addSingleTerrainConfig( TiXmlElement* elemRoot){
	int basefile = INVALID_INDEX;
  const char* filename = elemRoot->Attribute("file");
	if (filename != NULL && filename[0] != 0)
	{
	  	basefile = loadConfigImgFile((char*)filename,elemRoot);
	}
	
  string elementType = elemRoot->Value();
  if(elementType.compare( "floors" ) == 0){
    //parse floors
    TiXmlElement* elemFloor = elemRoot->FirstChildElement("floor");
    while( elemFloor ){
      parseWallFloorSpriteElement( elemFloor, contentLoader.terrainFloorConfigs, basefile );
      elemFloor = elemFloor->NextSiblingElement("floor");
    }
  }
  if(elementType.compare( "blocks" ) == 0){
    //parse walls
    TiXmlElement* elemWall = elemRoot->FirstChildElement("block");
    while( elemWall ){
      parseWallFloorSpriteElement( elemWall, contentLoader.terrainBlockConfigs, basefile );
      elemWall = elemWall->NextSiblingElement("block");
    }
  }
  return true;
}

void flushTerrainConfig(vector<TerrainConfiguration*>& config)
{
	uint32_t currentsize=config.size();
	for (uint32_t i=0;i<currentsize;i++)
	{
		if (config[i] != NULL)
		{
			delete(config[i]);
		}
	}
	
	config.clear();
	if (currentsize < MAX_BASE_TERRAIN + FAKE_TERRAIN_COUNT)
		currentsize = MAX_BASE_TERRAIN + FAKE_TERRAIN_COUNT;
	config.resize(currentsize,NULL);
}
