#include "common.h"
#include "SpriteMaps.h"
#include "GroundMaterialConfiguration.h"
#include <set>
#include "dfhack/depends/tinyxml/tinyxml.h"
#include "GUI.h"
#include "ContentLoader.h"

TerrainMaterialConfiguration::TerrainMaterialConfiguration()
{
	defaultSprite.fileIndex=INVALID_INDEX;
	defaultSprite.sheetIndex=UNCONFIGURED_INDEX;
	//dont really care about the rest of the sprite right now.

}

TerrainConfiguration::TerrainConfiguration()
{
	defaultSprite.fileIndex=INVALID_INDEX;
	defaultSprite.sheetIndex=UNCONFIGURED_INDEX;
	//dont really care about the rest of the sprite right now.
}

TerrainConfiguration::~TerrainConfiguration()
{
	uint32_t currentsize = (uint32_t)terrainMaterials.size();
	for (uint32_t i=0;i<currentsize;i++)
	{
		if (terrainMaterials[i] != NULL)
		{
			delete(terrainMaterials[i]);
		}
	}
}

void DumpInorganicMaterialNamesToDisk(){
	FILE* fp = fopen("dump.txt", "w");
	if(!fp) return;
	for(uint32_t j=0; j < contentLoader.inorganicMaterials.size(); j++){
		fprintf(fp, "%i:%s\n",j, contentLoader.inorganicMaterials[j].id);
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
	sprite.numVariations = 0;
	uint8_t red, green, blue;
	//check for randomised tiles
	const char* spriteVariationsStr = elemWallFloorSprite->Attribute("variations");
	if (spriteVariationsStr == NULL || spriteVariationsStr[0] == 0)
	{
		sprite.numVariations = 0;
	}
	else sprite.numVariations=atoi(spriteVariationsStr);

	//  do custom colors
	const char* spriteRedStr = elemWallFloorSprite->Attribute("red");
	if (spriteRedStr == NULL || spriteRedStr[0] == 0)
	{
		red = 255;
	}
	else red=atoi(spriteRedStr);
	const char* spriteGreenStr = elemWallFloorSprite->Attribute("green");
	if (spriteGreenStr == NULL || spriteGreenStr[0] == 0)
	{
		green = 255;
	}
	else green=atoi(spriteGreenStr);
	const char* spriteBlueStr = elemWallFloorSprite->Attribute("blue");
	if (spriteBlueStr == NULL || spriteBlueStr[0] == 0)
	{
		blue = 255;
	}
	else blue=atoi(spriteBlueStr);

	sprite.shadeColor = al_map_rgb(red, green, blue);

	//decide what the sprite should be shaded by.
	const char* spriteColorStr = elemWallFloorSprite->Attribute("color");
	if (spriteColorStr == NULL || spriteColorStr[0] == 0)
	{
		sprite.shadeBy = ShadeNone;
	}
	else
	{
		sprite.shadeBy = ShadeNone;
		if( strcmp(spriteColorStr, "none") == 0)
			sprite.shadeBy = ShadeNone;
		if( strcmp(spriteColorStr, "xml") == 0)
			sprite.shadeBy = ShadeXml;
		if( strcmp(spriteColorStr, "material") == 0)
			sprite.shadeBy = ShadeMat;
		if( strcmp(spriteColorStr, "layer") == 0)
			sprite.shadeBy = ShadeLayer;
		if( strcmp(spriteColorStr, "vein") == 0)
			sprite.shadeBy = ShadeVein;
		if( strcmp(spriteColorStr, "material_fore") == 0)
			sprite.shadeBy = ShadeMatFore;
		if( strcmp(spriteColorStr, "material_back") == 0)
			sprite.shadeBy = ShadeMatBack;
		if( strcmp(spriteColorStr, "layer_fore") == 0)
			sprite.shadeBy = ShadeLayerFore;
		if( strcmp(spriteColorStr, "layer_back") == 0)
			sprite.shadeBy = ShadeLayerBack;
		if( strcmp(spriteColorStr, "vein_fore") == 0)
			sprite.shadeBy = ShadeVeinFore;
		if( strcmp(spriteColorStr, "vein_back") == 0)
			sprite.shadeBy = ShadeVeinBack;
	}


	//not all tiles work well with an outline
	const char* spriteOutlineStr = elemWallFloorSprite->Attribute("outline");
	if (spriteOutlineStr == NULL || spriteOutlineStr[0] == 0)
	{
		sprite.needOutline = true;
	}
	else sprite.needOutline=(atoi(spriteOutlineStr) == 1);
	// check for local file definitions
	const char* filename = elemWallFloorSprite->Attribute("file");
	if (filename != NULL && filename[0] != 0)
	{
		sprite.fileIndex = loadConfigImgFile((char*)filename,elemWallFloorSprite);
	}


	//add subsprites, if any.
	TiXmlElement* elemSubType = elemWallFloorSprite->FirstChildElement("subsprite");
	for(TiXmlElement* elemSubType = elemWallFloorSprite->FirstChildElement("subsprite");
		elemSubType;
		elemSubType = elemSubType->NextSiblingElement("subsprite"))
	{
		const char* subSpriteIndexStr = elemSubType->Attribute("sprite");
		if (subSpriteIndexStr == NULL || subSpriteIndexStr[0] == 0)
		{
			contentError("Invalid Subsprite definition",elemSubType);
			return; //nothing to work with
		}
		// make a base sprite
		t_subSprite subSprite;
		subSprite.sheetIndex=atoi(subSpriteIndexStr);
		subSprite.fileIndex=sprite.fileIndex; //should be the same file as the main sprite by default.

		//do custom colors
		const char* subSpriteRedStr = elemSubType->Attribute("red");
		if (subSpriteRedStr == NULL || subSpriteRedStr[0] == 0)
		{
			red = 255;
		}
		else red=atoi(subSpriteRedStr);
		const char* subSpriteGreenStr = elemSubType->Attribute("green");
		if (subSpriteGreenStr == NULL || subSpriteGreenStr[0] == 0)
		{
			green = 255;
		}
		else green=atoi(subSpriteGreenStr);
		const char* subSpriteBlueStr = elemSubType->Attribute("blue");
		if (subSpriteBlueStr == NULL || subSpriteBlueStr[0] == 0)
		{
			blue = 255;
		}
		else blue=atoi(subSpriteBlueStr);
		subSprite.shadeColor = al_map_rgb(red, green, blue);

		//decide what the sprite should be shaded by.
		const char* subSpriteColorStr = elemSubType->Attribute("color");
		if (subSpriteColorStr == NULL || subSpriteColorStr[0] == 0)
		{
			subSprite.shadeBy = ShadeNone;
		}
		else
		{
			subSprite.shadeBy = ShadeNone;
			if( strcmp(subSpriteColorStr, "none") == 0)
				subSprite.shadeBy = ShadeNone;
			if( strcmp(subSpriteColorStr, "xml") == 0)
				subSprite.shadeBy = ShadeXml;
			if( strcmp(subSpriteColorStr, "material") == 0)
				subSprite.shadeBy = ShadeMat;
			if( strcmp(subSpriteColorStr, "layer") == 0)
				subSprite.shadeBy = ShadeLayer;
			if( strcmp(subSpriteColorStr, "vein") == 0)
				subSprite.shadeBy = ShadeVein;
			if( strcmp(subSpriteColorStr, "material_fore") == 0)
			subSprite.shadeBy = ShadeMatFore;
		if( strcmp(subSpriteColorStr, "material_back") == 0)
			subSprite.shadeBy = ShadeMatBack;
		if( strcmp(subSpriteColorStr, "layer_fore") == 0)
			subSprite.shadeBy = ShadeLayerFore;
		if( strcmp(subSpriteColorStr, "layer_back") == 0)
			subSprite.shadeBy = ShadeLayerBack;
		if( strcmp(subSpriteColorStr, "vein_fore") == 0)
			subSprite.shadeBy = ShadeVeinFore;
		if( strcmp(subSpriteColorStr, "vein_back") == 0)
			subSprite.shadeBy = ShadeVeinBack;
		}

		// check for local file definitions
		const char* subfilename = elemSubType->Attribute("file");
		if (subfilename != NULL && subfilename[0] != 0)
		{
			subSprite.fileIndex = loadConfigImgFile((char*)subfilename,elemWallFloorSprite);
		}
		sprite.subSprites.push_back(subSprite);
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
		if (configTable.size() <= (uint32_t)targetElem)
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
	int elems = (int)lookupKeys.size();
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
			if (tConfig->defaultSprite.sheetIndex == UNCONFIGURED_INDEX)
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
				if (tConfig->terrainMaterials.size() <= (uint32_t)elemIndex)
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
				if (tConfig->terrainMaterials[elemIndex]->defaultSprite.sheetIndex == UNCONFIGURED_INDEX)
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
				if (tConfig->terrainMaterials.size() <= (uint32_t)elemIndex)
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
	uint32_t currentsize = (uint32_t)config.size();
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
