#include "common.h"
#include "CreatureConfiguration.h"
#include "Creatures.h"
#include "MapLoading.h"

#include "dfhack/library/tinyxml/tinyxml.h"

bool CreatureNamesTranslatedFromGame = false;


CreatureConfiguration::CreatureConfiguration(char* gameIDstr, char* professionStr, enumCreatureSex sex, enumCreatureSpecialCases special, int sheetIndex)
{
  memset(this, 0, sizeof(CreatureConfiguration) );
  this->sheetIndex = sheetIndex;
  this->gameID = INVALID_INDEX;
  this->professionID = INVALID_INDEX;
  this->sex = sex;

  int len = (int) strlen(gameIDstr);
  if(len > CREATURESTRLENGTH) len = CREATURESTRLENGTH;
  memcpy(this->gameIDstr, gameIDstr, len);
  if(professionStr){
    len = (int) strlen(professionStr);
    if(len > CREATURESTRLENGTH) len = CREATURESTRLENGTH;
    memcpy(this->professionstr, professionStr, len);
  }
}

CreatureConfiguration::~CreatureConfiguration(void)
{
}

void DumpCreatureNamesToDisk(){
  FILE* fp = fopen("dump.txt", "w");
  if(!fp) return;
  for(uint32_t j=0; j < v_creatureNames.size(); j++){
    fprintf(fp, "%i:%s\n",j, v_creatureNames[j].id);
  }
  fclose(fp);
}
void DumpProfessionsToDisk(){
  FILE* fp = fopen("dump.txt", "w");
  if(!fp) return;
  string proffStr;
  for(int j=0; (proffStr = dfMemoryInfo.getProfession(j)) != "" ; j++){
    fprintf(fp, "%i:%s\n",j, proffStr.c_str());
  }
  fclose(fp);
}

void TranslateCreatureNames(){
  uint32_t numCreatures = (uint32_t)v_creatureNames.size();
  //uint32_t numProfessions = dfMemoryInfo.P
  //for each config, find it's integer ID
  for(uint32_t i=0; i < creatureTypes.size(); i++){
    char* ptr = creatureTypes[i].gameIDstr;
    uint32_t j;
    for(j=0; j < numCreatures; j++){
      if( strcmp( ptr, v_creatureNames[j].id) == 0){
        //assign ID
        creatureTypes[i].gameID = j; 

        //jump out of ID lookup loop
        break;
      }
    }
    if(j >= v_creatureNames.size())
      WriteErr("Unable to match creature '%s' to anything in-game\n", ptr);
    ptr = creatureTypes[i].professionstr;
    if(strcmp(ptr, "") != 0 ){
      string proffStr;
      for(j=0; (proffStr = dfMemoryInfo.getProfession(j)) != "" ; j++){
        if( proffStr.compare( ptr ) == 0){
          //assign ID
          creatureTypes[i].professionID = j; 

          //jump out of proffessionID lookup loop
          break;
        }
      }
      if(proffStr == ""){
        WriteErr("Unable to match Profession '%s' to anything in-game\n", ptr);
        creatureTypes[i].professionID = INT_MAX; //if it is left at INVALID_INDEX, the condition is ignored entierly.
      }
    }
  }

  CreatureNamesTranslatedFromGame = true;
}


void LoadCreatureConfiguration( vector<CreatureConfiguration>* knownCreatures ){
  char* filename = "Creatures.xml";
  TiXmlDocument doc( filename );
  bool loadOkay = doc.LoadFile();
  TiXmlHandle hDoc(&doc);
  TiXmlElement* elemCreature;
  TiXmlElement* elemProfession;

  knownCreatures->clear();

  elemCreature = hDoc.FirstChildElement("Creature").Element();
  while( elemCreature ){
    const char* name = elemCreature->Attribute("gameID");
    const char* sheetIndexStr;
    
    elemProfession = elemCreature->FirstChildElement("Profession");
    while( elemProfession ){
      const char* professionstr = elemProfession->Attribute("name");
      const char* sexstr = elemProfession->Attribute("sex");
      sheetIndexStr = elemProfession->Attribute("sheetIndex");
      enumCreatureSex cresex = eCreatureSex_NA;
      if(sexstr){
        if(strcmp( sexstr, "M" ) == 0) cresex = eCreatureSex_Male;
        if(strcmp( sexstr, "F" ) == 0) cresex = eCreatureSex_Female;
      }
      const char* specstr = elemProfession->Attribute("special");
      enumCreatureSpecialCases crespec = eCSC_Any;
      if (specstr)
      {
        if(strcmp( specstr, "Normal" ) == 0) crespec = eCSC_Normal;
        if(strcmp( specstr, "Zombie" ) == 0) crespec = eCSC_Zombie;	      
        if(strcmp( specstr, "Skeleton" ) == 0) crespec = eCSC_Skeleton;	      
      }
      //create profession config
      CreatureConfiguration cre( (char*)name, (char*)professionstr, cresex, crespec, atoi(sheetIndexStr) );
      //add a copy to known creatures
      knownCreatures->push_back( cre );

      elemProfession = elemProfession->NextSiblingElement("Profession");
    }

    //create default config
    sheetIndexStr = elemCreature->Attribute("sheetIndex");
    CreatureConfiguration cre( (char*)name, "", eCreatureSex_NA, eCSC_Any, atoi(sheetIndexStr) );
    //add a copy to known creatures
    knownCreatures->push_back( cre );
    
    elemCreature = elemCreature->NextSiblingElement("Creature");
  }

  CreatureNamesTranslatedFromGame = false;
}
