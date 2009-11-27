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
  char gameIDstr[CREATURESTRLENGTH];
  int gameID;
  char professionstr[CREATURESTRLENGTH];
  bool customProf;
  int professionID;
  int sheetIndex;
  enumCreatureSpecialCases special;
  enumCreatureSex sex;

  CreatureConfiguration(){}
  CreatureConfiguration(char* gameIDstr, char* professionStr, bool custom, enumCreatureSex sex, enumCreatureSpecialCases, int sheetIndex);
  ~CreatureConfiguration(void);
};


bool addSingleCreatureConfig( TiXmlElement* elemRoot, vector<CreatureConfiguration>* knownCreatures );

void TranslateCreatureNames(vector<CreatureConfiguration>& configs, vector<t_matgloss>& creatureNames );
