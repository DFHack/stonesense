#pragma once
#include "tinyxml.h"
#include "SpriteObjects.h"
#include "df/pronoun_type.h"

constexpr auto CREATURESTRLENGTH = 50;

enum enumCreatureSpecialCases {
    eCSC_Any,
    eCSC_Normal,
    eCSC_Military,
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
    df::pronoun_type sex;
    int caste;

    CreatureConfiguration() = delete;
    CreatureConfiguration(int professionID, const char* professionStr, df::pronoun_type sex, int caste, enumCreatureSpecialCases special, c_sprite &sprite, int shadow);
    ~CreatureConfiguration(void);
};


bool addCreaturesConfig(TiXmlElement* elemRoot, std::vector<std::unique_ptr<std::vector<CreatureConfiguration>>>& knownCreatures);
