#include "common.h"
#include "CreatureConfiguration.h"
#include "Creatures.h"
#include "MapLoading.h"
#include "GUI.h"
#include "ContentLoader.h"

#include "dfhack/library/tinyxml/tinyxml.h"


CreatureConfiguration::CreatureConfiguration(int gameID, int professionID, const char* professionStr, enumCreatureSex sex, enumCreatureSpecialCases special, t_SpriteWithOffset &sprite)
{
  memset(this, 0, sizeof(CreatureConfiguration) );
  this->sprite = sprite;
  this->gameID = gameID;
  this->professionID = professionID;
  this->sex = sex;
  
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
  for(int j=0; (proffStr = dfMemoryInfo.getProfession(j)) != "" ; j++){
    fprintf(fp, "%i:%s\n",j, proffStr.c_str());
  }
  fclose(fp);
}

int translateProfession(const char* currentProf)
{
	if (currentProf == NULL || currentProf[0]==0)
		return INVALID_INDEX;
    uint32_t j;
    string proffStr;
    for(j=0; (proffStr = dfMemoryInfo.getProfession(j)) != "" ; j++)
    {   
      if( proffStr.compare( currentProf ) == 0)
      {
        //assign ID
        return j;
      }
    }
	WriteErr("Unable to match profession '%s' to anything in-game\n", currentProf);
	return INT_MAX; //if it is left at INVALID_INDEX, the condition is ignored entierly.
}

bool addSingleCreatureConfig( TiXmlElement* elemCreature, vector<CreatureConfiguration>* knownCreatures, int basefile ){
  int gameID = lookupIndexedType(elemCreature->Attribute("gameID"),contentLoader.creatureNameStrings);
  if (gameID == INVALID_INDEX)
  	return false;
  const char* sheetIndexStr;
  t_SpriteWithOffset sprite;
  sprite.fileIndex=basefile;
  sprite.x=0;
  sprite.y=0;
  sprite.animFrames=ALL_FRAMES;
  const char* filename = elemCreature->Attribute("file");
	if (filename != NULL && filename[0] != 0)
	{
	  	sprite.fileIndex = loadImgFile((char*)filename);
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
    
    const char* sexstr = elemVariant->Attribute("sex");
    sheetIndexStr = elemVariant->Attribute("sheetIndex");
    enumCreatureSex cresex = eCreatureSex_NA;
    if(sexstr){
      if(strcmp( sexstr, "M" ) == 0) cresex = eCreatureSex_Male;
      if(strcmp( sexstr, "F" ) == 0) cresex = eCreatureSex_Female;
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
    
    //create profession config
    sprite.sheetIndex=atoi(sheetIndexStr);
    CreatureConfiguration cre( gameID, professionID, customStr , cresex, crespec, sprite );
    //add a copy to known creatures
    knownCreatures->push_back( cre );

    elemVariant = elemVariant->NextSiblingElement("variant");
  }

  //create default config
  sheetIndexStr = elemCreature->Attribute("sheetIndex");
  sprite.animFrames = ALL_FRAMES;
  if (sheetIndexStr)
  {
	sprite.sheetIndex = atoi( sheetIndexStr );
    CreatureConfiguration cre( gameID, INVALID_INDEX, NULL, eCreatureSex_NA, eCSC_Any, sprite );
  	//add a copy to known creatures
    knownCreatures->push_back( cre );
  }
  return true;
}

bool addCreaturesConfig( TiXmlElement* elemRoot, vector<CreatureConfiguration>* knownCreatures ){
  int basefile = -1;
  const char* filename = elemRoot->Attribute("file");
  if (filename != NULL && filename[0] != 0)
  {
	basefile = loadImgFile((char*)filename);
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
	