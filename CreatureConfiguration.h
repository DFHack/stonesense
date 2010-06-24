#pragma once
#include "dfhack/depends/tinyxml/tinyxml.h"
#include "SpriteObjects.h"

#define CREATURESTRLENGTH 50

enum enumCreatureSpecialCases{
	eCSC_Any,
	eCSC_Normal,
	eCSC_Zombie,
	eCSC_Skeleton,
};

class CreatureConfiguration
{
public:
	char professionstr[CREATURESTRLENGTH];
	int professionID;
	c_sprite sprite;
	int shadow;
	enumCreatureSpecialCases special;
	uint8_t sex;

	CreatureConfiguration(){}
	CreatureConfiguration(int professionID, const char* professionStr, uint8_t sex, enumCreatureSpecialCases special, c_sprite &sprite, int shadow);
	~CreatureConfiguration(void);
};


bool addCreaturesConfig( TiXmlElement* elemRoot, vector<vector<CreatureConfiguration>*>& knownCreatures );
