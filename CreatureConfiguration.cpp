#include "common.h"
#include "CreatureConfiguration.h"
#include "Creatures.h"

#include "dfhack/library/tinyxml/tinyxml.h"

bool CreatureNamesTranslatedFromGame = false;


CreatureConfiguration::CreatureConfiguration(char* gameIDstr, char* professionStr, enumCreatureSex sex, int sheetIndex)
{
  memset(this, 0, sizeof(CreatureConfiguration) );
  this->sheetIndex = sheetIndex;
  this->gameID = INVALID_INDEX;
  this->professionID = 0;
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

void TranslateCreatureNames(){
  uint32_t numCreatures = (uint32_t)v_creatureNames.size();
  //for each config, find it's integer ID
  for(uint32_t i=0; i < creatureTypes.size(); i++){
    char* ptr = creatureTypes[i].gameIDstr;
    uint32_t j;
    for(j=0; j < numCreatures; j++){
      if( strcmp( ptr, v_creatureNames[j].id) == 0){
        //assign ID
        creatureTypes[i].gameID = j; 
        //jump to next creatureType
        break;
      }
    }
    if(j >= v_creatureNames.size())
      WriteErr("Unable to match creature '%s' to anything in-game\n", ptr);
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
    const char* sheetIndexStr = elemCreature->Attribute("sheetIndex");
    
    elemProfession = elemCreature->FirstChildElement("Profession");
    while( elemProfession ){
      const char* professionstr = elemProfession->Attribute("name");
      const char* sexstr = elemProfession->Attribute("sex");
      enumCreatureSex cresex = eCreatureSex_NA;
      if(sexstr){
        if(strcmp( sexstr, "M" ) == 0) cresex = eCreatureSex_Male;
        if(strcmp( sexstr, "F" ) == 0) cresex = eCreatureSex_Female;
      }
      //create profession config
      CreatureConfiguration cre( (char*)name, (char*)professionstr, cresex, atoi(sheetIndexStr) );
      //add a copy to known creatures
      knownCreatures->push_back( cre );

      elemProfession = elemProfession->NextSiblingElement("Profession");
    }
    //create default config
    CreatureConfiguration cre( (char*)name, "", eCreatureSex_NA, atoi(sheetIndexStr) );
    //add a copy to known creatures
    knownCreatures->push_back( cre );
    
    elemCreature = elemCreature->NextSiblingElement("Creature");
  }

  CreatureNamesTranslatedFromGame = false;
}