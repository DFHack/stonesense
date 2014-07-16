#pragma once
#include "tinyxml.h"
#include "SpriteObjects.h"

#define CREATURESTRLENGTH 50

enum enumCreatureSpecialCases {
    eCSC_Any,
    eCSC_Normal,
    eCSC_Military,
    eCSC_Zombie,
    eCSC_Skeleton,
    eCSC_Ghost
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
    int caste;

    CreatureConfiguration() {}
    CreatureConfiguration(int professionID, const char* professionStr, uint8_t sex, int caste, enumCreatureSpecialCases special, c_sprite &sprite, int shadow);
    ~CreatureConfiguration(void);
};


bool addCreaturesConfig(TiXmlElement* elemRoot, std::vector<std::vector<CreatureConfiguration>*>& knownCreatures);
