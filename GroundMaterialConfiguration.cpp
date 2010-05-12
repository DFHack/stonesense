#include "common.h"
#include "SpriteMaps.h"
#include "GroundMaterialConfiguration.h"
#include <set>
#include "dfhack/depends/tinyxml/tinyxml.h"
#include "GUI.h"
#include "ContentLoader.h"

TerrainMaterialConfiguration::TerrainMaterialConfiguration()
{
	overridingMaterials.resize(NUM_FORMS);
	defaultSprite.resize(NUM_FORMS);
	for(int i = 0; i < NUM_FORMS; i++)
	{
		defaultSprite[i].fileIndex=INVALID_INDEX;
		defaultSprite[i].sheetIndex=UNCONFIGURED_INDEX;
	}
	//dont really care about the rest of the sprite right now.

}

TerrainConfiguration::TerrainConfiguration()
{
	defaultSprite.resize(NUM_FORMS);
	for(int i = 0; i < NUM_FORMS; i++)
	{
		defaultSprite[i].fileIndex=INVALID_INDEX;
		defaultSprite[i].sheetIndex=UNCONFIGURED_INDEX;
	}
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
	for(uint32_t j=0; j < contentLoader.Mats->inorganic.size(); j++){
		fprintf(fp, "%i:%s\n",j, contentLoader.Mats->inorganic[j].id);
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
		sprite.shadeBy = getShadeType(spriteColorStr);
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
			subSprite.shadeBy = getShadeType(subSpriteColorStr);
		}

		// check for local file definitions
		const char* subfilename = elemSubType->Attribute("file");
		if (subfilename != NULL && subfilename[0] != 0)
		{
			subSprite.fileIndex = loadConfigImgFile((char*)subfilename,elemSubType);
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

	vector<bool> formToggle;
	formToggle.resize(NUM_FORMS);
	// parse weather tile is for a block, log, etc
	TiXmlElement* elemForm = elemWallFloorSprite->FirstChildElement("form");
	if(elemForm == NULL)
	{
		formToggle[0] = true;
	}
	for( ;elemForm;elemForm = elemForm->NextSiblingElement("form"))
	{
		const char * strForm = elemForm->Attribute("value");

		if( strcmp(strForm, "bar") == 0)
			formToggle[FORM_BAR] = true;
		if( strcmp(strForm, "block") == 0)
			formToggle[FORM_BLOCK] = true;
		if( strcmp(strForm, "boulder") == 0)
			formToggle[FORM_BOULDER] = true;
		if( strcmp(strForm, "log") == 0)
			formToggle[FORM_LOG] = true;
	}
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
			for( int i = 0; i < NUM_FORMS; i++)
			{
				if(formToggle[i])
					if (tConfig->defaultSprite[i].sheetIndex == UNCONFIGURED_INDEX)
					{
						tConfig->defaultSprite[i] = sprite;
					}
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
				for( int i = 0; i < NUM_FORMS; i++)
				{
					if(formToggle[i])
						if (tConfig->terrainMaterials[elemIndex]->defaultSprite[i].sheetIndex == UNCONFIGURED_INDEX)
						{
							tConfig->terrainMaterials[elemIndex]->defaultSprite[i] = sprite;
						}
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
				for( int i = 0; i < NUM_FORMS; i++)
				{
					if(formToggle[i])
					{
						map<int,t_SpriteWithOffset>::iterator it = tConfig->terrainMaterials[elemIndex]->overridingMaterials[i].find(subtypeId);
						if (it == tConfig->terrainMaterials[elemIndex]->overridingMaterials[i].end())
						{
							tConfig->terrainMaterials[elemIndex]->overridingMaterials[i][subtypeId]=sprite;
						}	
					}
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
