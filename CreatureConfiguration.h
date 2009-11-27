#pragma once


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

  CreatureConfiguration(char* gameIDstr, char* professionStr, bool custom, enumCreatureSex sex, enumCreatureSpecialCases, int sheetIndex);
  ~CreatureConfiguration(void);
};





void TranslateCreatureNames();

void LoadCreatureConfiguration( vector<CreatureConfiguration>* knownCreatures );

extern bool CreatureNamesTranslatedFromGame;
