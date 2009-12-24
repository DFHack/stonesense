#pragma once
#include "dfhack/library/tinyxml/tinyxml.h"


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
  t_SpriteWithOffset sprite;
  int shadow;
  enumCreatureSpecialCases special;
  enumCreatureSex sex;

  CreatureConfiguration(){}
  CreatureConfiguration(int professionID, const char* professionStr, enumCreatureSex sex, enumCreatureSpecialCases special, t_SpriteWithOffset &sprite, int shadow);
  ~CreatureConfiguration(void);
};


bool addCreaturesConfig( TiXmlElement* elemRoot, vector<vector<CreatureConfiguration>*>& knownCreatures );
