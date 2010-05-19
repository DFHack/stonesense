#include "common.h"
#include "CreatureConfiguration.h"
#include "Creatures.h"
#include "MapLoading.h"
#include "GUI.h"
#include "ContentLoader.h"
#include "dfhack/include/DFError.h"

#include "dfhack/depends/tinyxml/tinyxml.h"


CreatureConfiguration::CreatureConfiguration(int professionID, const char* professionStr, uint8_t sex, enumCreatureSpecialCases special, t_SpriteWithOffset &sprite, int shadow)
{
	memset(this, 0, sizeof(CreatureConfiguration) );
	this->sprite = sprite;
	this->professionID = professionID;
	this->sex = sex;
	this->shadow = shadow;
	this->special = special;

	if(professionStr){
		int len = (int) strlen(professionStr);
		if(len > CREATURESTRLENGTH) len = CREATURESTRLENGTH;
		memcpy(this->professionstr, professionStr, len);
		this->professionstr[CREATURESTRLENGTH-1]=0;
	}
	//WriteErr("CC %d %d %d %d %s\n",this->gameID,this->professionID,this->sprite.fileIndex,this->sprite.sheetIndex,(this->professionstr[0]?this->professionstr:"-"));
}

CreatureConfiguration::~CreatureConfiguration(void)
{
}

void DumpCreatureNamesToDisk(){
	/*FILE* fp = fopen("dump.txt", "w");
	if(!fp) return;
	for(uint32_t j=0; j < v_creatureNames.size(); j++){
	fprintf(fp, "%i:%s\n",j, v_creatureNames[j].id);
	}
	fclose(fp);*/
}
void DumpProfessionsToDisk(){
	FILE* fp = fopen("dump.txt", "w");
	if(!fp) return;
	string proffStr;
	for(int j=0; (proffStr = contentLoader.professionStrings[j]) != "" ; j++){
		fprintf(fp, "%i:%s\n",j, proffStr.c_str());
	}
	fclose(fp);
}

int translateProfession(const char* currentProf)
{
	uint32_t j, dfNumJobs;
	string proffStr;

	if (currentProf == NULL || currentProf[0]==0)
		return INVALID_INDEX;

	dfNumJobs = contentLoader.professionStrings.size();
	for(j=0; j < dfNumJobs; j++)
	{   
		proffStr = contentLoader.professionStrings[j];
		if( proffStr.compare( currentProf ) == 0)
		{
			//assign ID
			return j;
		}
	}
	WriteErr("Unable to match profession '%s' to anything in-game\n", currentProf);
	return INT_MAX; //if it is left at INVALID_INDEX, the condition is ignored entierly.
}

void pushCreatureConfig( vector<vector<CreatureConfiguration>*>& knownCreatures, unsigned int gameID, CreatureConfiguration& cre)
{
	if(!config.skipCreatureTypes)
	{
		vector<CreatureConfiguration>* creatureList;
		if (knownCreatures.size() <= gameID)
		{
			//resize using hint from creature name list
			unsigned int newsize = gameID +1;
			if (newsize <= contentLoader.Mats->race.size())
			{
				newsize = contentLoader.Mats->race.size() + 1;
			}
			knownCreatures.resize(newsize);
		}
		creatureList = knownCreatures[gameID];
		if (creatureList == NULL)
		{
			creatureList = new vector<CreatureConfiguration>();
			knownCreatures[gameID]=creatureList;
		}
		creatureList->push_back(cre);
	}
}

bool addSingleCreatureConfig( TiXmlElement* elemCreature, vector<vector<CreatureConfiguration>*>& knownCreatures, int basefile ){
	if(config.skipCreatureTypes)
		return false;
	int gameID = lookupIndexedType(elemCreature->Attribute("gameID"),contentLoader.Mats->race);
	if (gameID == INVALID_INDEX)
		return false;
	const char* sheetIndexStr;
	int defaultFile;
	t_SpriteWithOffset sprite;
	sprite.fileIndex=basefile;
	sprite.x=0;
	sprite.y=0;
	sprite.animFrames=ALL_FRAMES;
	int baseShadow = DEFAULT_SHADOW;
	uint8_t red, green, blue;
	const char* shadowStr = elemCreature->Attribute("shadow");
	if (shadowStr != NULL && shadowStr[0] != 0)
	{
		baseShadow = atoi( shadowStr );	  
	}
	if (baseShadow < 0 || baseShadow > MAX_SHADOW)
		baseShadow = DEFAULT_SHADOW;
	const char* filename = elemCreature->Attribute("file");
	if (filename != NULL && filename[0] != 0)
	{
		sprite.fileIndex = defaultFile = loadConfigImgFile((char*)filename,elemCreature);
	}
	TiXmlElement* elemVariant = elemCreature->FirstChildElement("variant");
	while( elemVariant ){
		int professionID = INVALID_INDEX;
		const char* profStr = elemVariant->Attribute("prof");
		if (profStr == NULL || profStr[0] == 0)
		{
			profStr = elemVariant->Attribute("profession");
		}
		professionID = translateProfession(profStr);

		const char* customStr = elemVariant->Attribute("custom");
		if (customStr != NULL && customStr[0] == 0)
		{
			customStr = NULL;
		} 

		if (customStr != NULL)
		{
			WriteErr("custom: %s\n",customStr);	
		}

		const char* filename = elemVariant->Attribute("file");
		if (filename != NULL && filename[0] != 0)
		{
			sprite.fileIndex = loadConfigImgFile((char*)filename,elemCreature);
		}
		else sprite.fileIndex = defaultFile;

		const char* sexstr = elemVariant->Attribute("sex");
		sheetIndexStr = elemVariant->Attribute("sheetIndex");
		uint8_t cresex = 0;
		if(sexstr){
			if(strcmp( sexstr, "M" ) == 0) cresex = 2;
			if(strcmp( sexstr, "F" ) == 0) cresex = 1;
			if(atoi(sexstr)) cresex = atoi(sexstr);
		}
		const char* specstr = elemVariant->Attribute("special");
		enumCreatureSpecialCases crespec = eCSC_Any;
		if (specstr)
		{
			if(strcmp( specstr, "Normal" ) == 0) crespec = eCSC_Normal;
			if(strcmp( specstr, "Zombie" ) == 0) crespec = eCSC_Zombie;	      
			if(strcmp( specstr, "Skeleton" ) == 0) crespec = eCSC_Skeleton;	      
		}
		sprite.animFrames = getAnimFrames(elemVariant->Attribute("frames"));
		if (sprite.animFrames == 0)
			sprite.animFrames = ALL_FRAMES;

		int shadow = baseShadow;
		const char* shadowStr = elemVariant->Attribute("shadow");
		if (shadowStr != NULL && shadowStr[0] != 0)
		{
			shadow = atoi( shadowStr );	  
		}
		if (shadow < 0 || shadow > MAX_SHADOW)
			shadow = baseShadow;

		//decide what the sprite should be shaded by.
		const char* spriteVarColorStr = elemVariant->Attribute("color");
		if (spriteVarColorStr == NULL || spriteVarColorStr[0] == 0)
		{
			sprite.shadeBy = ShadeNone;
		}
		else
		{
			sprite.shadeBy = getShadeType(spriteVarColorStr);
		}

		//do bodyparts
		const char* bodyVarPartStr = elemVariant->Attribute("bodypart");
		//clear old bodypart string
		memset(sprite.bodyPart, 0, sizeof(sprite.bodyPart));
		//copy new, if found
		if (bodyVarPartStr != NULL && bodyVarPartStr[0] != 0)
		{
			strcpy(sprite.bodyPart, bodyVarPartStr);
		}

		//subsprites
		sprite.subSprites.clear();
		TiXmlElement* elemVarSubSprite = elemVariant->FirstChildElement("subsprite");
		while(elemVarSubSprite)
		{
			const char* subVarSpriteIndexStr = elemVarSubSprite->Attribute("sheetIndex");
			if (subVarSpriteIndexStr == NULL || subVarSpriteIndexStr[0] == 0)
			{
				contentError("Invalid Subsprite definition",elemVarSubSprite);
				break; //nothing to work with
			}
			// make a base sprite
			t_subSprite subVarSprite;
			subVarSprite.sheetIndex=atoi(subVarSpriteIndexStr);
			subVarSprite.fileIndex=sprite.fileIndex; //should be the same file as the main sprite by default.

			//do custom colors
			const char* subVarSpriteRedStr = elemVarSubSprite->Attribute("red");
			if (subVarSpriteRedStr == NULL || subVarSpriteRedStr[0] == 0)
			{
				red = 255;
			}
			else red=atoi(subVarSpriteRedStr);
			const char* subVarSpriteGreenStr = elemVarSubSprite->Attribute("green");
			if (subVarSpriteGreenStr == NULL || subVarSpriteGreenStr[0] == 0)
			{
				green = 255;
			}
			else green=atoi(subVarSpriteGreenStr);
			const char* subVarSpriteBlueStr = elemVarSubSprite->Attribute("blue");
			if (subVarSpriteBlueStr == NULL || subVarSpriteBlueStr[0] == 0)
			{
				blue = 255;
			}
			else blue=atoi(subVarSpriteBlueStr);
			subVarSprite.shadeColor = al_map_rgb(red, green, blue);

			//decide what the sprite should be shaded by.
			const char* subVarSpriteColorStr = elemVarSubSprite->Attribute("color");
			if (subVarSpriteColorStr == NULL || subVarSpriteColorStr[0] == 0)
			{
				subVarSprite.shadeBy = ShadeNone;
			}
			else
			{
				subVarSprite.shadeBy = getShadeType(subVarSpriteColorStr);
			}

			//do bodyparts
			const char* subBodyPartStr = elemVarSubSprite->Attribute("bodypart");
			//clear old bodypart string
			memset(subVarSprite.bodyPart, 0, sizeof(sprite.bodyPart));
			//copy new, if found
			if (subBodyPartStr != NULL && subBodyPartStr[0] != 0)
			{
				strcpy(subVarSprite.bodyPart, subBodyPartStr);
			}

			// check for local file definitions
			const char* subfilename = elemVarSubSprite->Attribute("file");
			if (subfilename != NULL && subfilename[0] != 0)
			{
				subVarSprite.fileIndex = loadConfigImgFile((char*)subfilename,elemVarSubSprite);
			}
			sprite.subSprites.push_back(subVarSprite);
			elemVarSubSprite = elemVarSubSprite->NextSiblingElement("subsprite");
		}


		//create profession config
		sprite.sheetIndex=atoi(sheetIndexStr);
		CreatureConfiguration cre( professionID, customStr , cresex, crespec, sprite, shadow);
		//add a copy to known creatures
		pushCreatureConfig(knownCreatures, gameID, cre);
		elemVariant = elemVariant->NextSiblingElement("variant");
	}

	//create default config
	sprite.fileIndex = defaultFile;
	baseShadow;
	sheetIndexStr = elemCreature->Attribute("sheetIndex");
	sprite.animFrames = ALL_FRAMES;
	const char* spriteColorStr = elemCreature->Attribute("color");
	if (spriteColorStr == NULL || spriteColorStr[0] == 0)
	{
		sprite.shadeBy = ShadeNone;
	}
	else
	{
		sprite.shadeBy = getShadeType(spriteColorStr);
	}

	//  do custom colors
	const char* spriteRedStr = elemCreature->Attribute("red");
	if (spriteRedStr == NULL || spriteRedStr[0] == 0)
	{
		red = 255;
	}
	else red=atoi(spriteRedStr);
	const char* spriteGreenStr = elemCreature->Attribute("green");
	if (spriteGreenStr == NULL || spriteGreenStr[0] == 0)
	{
		green = 255;
	}
	else green=atoi(spriteGreenStr);
	const char* spriteBlueStr = elemCreature->Attribute("blue");
	if (spriteBlueStr == NULL || spriteBlueStr[0] == 0)
	{
		blue = 255;
	}
	else blue=atoi(spriteBlueStr);

	sprite.shadeColor = al_map_rgb(red, green, blue);

	//do bodyparts
	const char* bodyPartStr = elemCreature->Attribute("bodypart");
	//clear old bodypart string
	memset(sprite.bodyPart, 0, sizeof(sprite.bodyPart));
	//copy new, if found
	if (bodyPartStr != NULL && bodyPartStr[0] != 0)
	{
		strcpy(sprite.bodyPart, bodyPartStr);
	}
	//subsprites
	sprite.subSprites.clear();
	TiXmlElement* elemSubSprite = elemCreature->FirstChildElement("subsprite");
	while(elemSubSprite)
	{
		const char* subSpriteIndexStr = elemSubSprite->Attribute("sheetIndex");
		if (subSpriteIndexStr == NULL || subSpriteIndexStr[0] == 0)
		{
			contentError("Invalid Subsprite definition",elemSubSprite);
			break; //nothing to work with
		}
		// make a base sprite
		t_subSprite subSprite;
		subSprite.sheetIndex=atoi(subSpriteIndexStr);
		subSprite.fileIndex=sprite.fileIndex; //should be the same file as the main sprite by default.

		//do custom colors
		const char* subSpriteRedStr = elemSubSprite->Attribute("red");
		if (subSpriteRedStr == NULL || subSpriteRedStr[0] == 0)
		{
			red = 255;
		}
		else red=atoi(subSpriteRedStr);
		const char* subSpriteGreenStr = elemSubSprite->Attribute("green");
		if (subSpriteGreenStr == NULL || subSpriteGreenStr[0] == 0)
		{
			green = 255;
		}
		else green=atoi(subSpriteGreenStr);
		const char* subSpriteBlueStr = elemSubSprite->Attribute("blue");
		if (subSpriteBlueStr == NULL || subSpriteBlueStr[0] == 0)
		{
			blue = 255;
		}
		else blue=atoi(subSpriteBlueStr);
		subSprite.shadeColor = al_map_rgb(red, green, blue);

		//decide what the sprite should be shaded by.
		const char* subSpriteColorStr = elemSubSprite->Attribute("color");
		if (subSpriteColorStr == NULL || subSpriteColorStr[0] == 0)
		{
			subSprite.shadeBy = ShadeNone;
		}
		else
		{
			subSprite.shadeBy = getShadeType(subSpriteColorStr);
		}

		//do bodyparts
		const char* subBodyPartStr = elemSubSprite->Attribute("bodypart");
		//clear old bodypart string
		memset(subSprite.bodyPart, 0, sizeof(sprite.bodyPart));
		//copy new, if found
		if (subBodyPartStr != NULL && subBodyPartStr[0] != 0)
		{
			strcpy(subSprite.bodyPart, subBodyPartStr);
		}

		// check for local file definitions
		const char* subfilename = elemSubSprite->Attribute("file");
		if (subfilename != NULL && subfilename[0] != 0)
		{
			subSprite.fileIndex = loadConfigImgFile((char*)subfilename,elemSubSprite);
		}
		sprite.subSprites.push_back(subSprite);
		elemSubSprite = elemSubSprite->NextSiblingElement("subsprite");
	}
	if (sheetIndexStr)
	{
		sprite.sheetIndex = atoi( sheetIndexStr );
		CreatureConfiguration cre( INVALID_INDEX, NULL, eCreatureSex_NA, eCSC_Any, sprite, baseShadow);
		//add a copy to known creatures
		pushCreatureConfig(knownCreatures, gameID, cre);
	}
	return true;
}

bool addCreaturesConfig( TiXmlElement* elemRoot, vector<vector<CreatureConfiguration>*>& knownCreatures ){
	int basefile = -1;
	const char* filename = elemRoot->Attribute("file");
	if (filename != NULL && filename[0] != 0)
	{
		basefile = loadConfigImgFile((char*)filename,elemRoot);
	} 
	TiXmlElement* elemCreature = elemRoot->FirstChildElement("creature");
	if (elemCreature == NULL)
	{
		contentError("No creatures found",elemRoot);
		return false;
	}
	while( elemCreature ){
		addSingleCreatureConfig(elemCreature,knownCreatures,basefile );
		elemCreature = elemCreature->NextSiblingElement("creature");
	}
	return true;
}
