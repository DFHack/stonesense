#include "common.h"
#include "CreatureConfiguration.h"
#include "Creatures.h"
#include "MapLoading.h"
#include "GUI.h"
#include "ContentLoader.h"

#include "dfhack/library/tinyxml/tinyxml.h"


VegetationConfiguration::VegetationConfiguration(char* gameID, t_SpriteWithOffset &sprite, bool live)
  memset(this, 0, sizeof(CreatureConfiguration) );
  this->sprite = sprite;
  this->gameID = gameID;
  this->live = live;
}

VegetationConfiguration::~VegetationConfiguration(void)
{
}

int TranslatePlantName(const char* plantName, vector<t_matgloss>& plantNames ){
	if (plantName == NULL)
	{
		return -1;	
	}
  uint32_t numPlants = (uint32_t)plantNames.size();
  for(uint32_t i=0; i < numPlants; i++){
    if (strcmp(plantName,plantNames[i].id) == 0)
    	return i;
 }
 WriteErr("Unable to match plant '%s' to anything in-game\n", plantName);
}

bool addSingleVegetationConfig( TiXmlElement* elemRoot,  vector<VegetationConfiguration>* vegetationConfigs, vector<t_matgloss>& plantNames );
{
  const char* sheetIndexStr;
  t_SpriteWithOffset sprite;
  sprite.fileIndex=basefile;
  sprite.x=0;
  sprite.y=0;
  sprite.animFrames=ALL_FRAMES;
 
  int basefile = -1;
  const char* filename = elemRoot->Attribute("file");
	if (filename != NULL && filename[0] != 0)
	{
	  	basefile = loadImgFile((char*)filename);
	}
	
  TiXmlElement* elemTree = elemCreature->FirstChildElement("tree");
  while( elemTree ){
  	int gameID = TranslatePlantName(elemRoot->Attribute("gameID"),vegetationConfigs);
    const char* deadstr = elemTree->Attribute("dead");
    bool dead = (deadstr && deadstr[0]);   
    /* No animated trees.
    	But we may repurpose it later to make a xyz variance?
    sprite.animFrames = getAnimFrames(elemProfession->Attribute("frames"));
	if (sprite.animFrames == 0)
		sprite.animFrames = ALL_FRAMES;*/
    
    //create profession config
    sprite.sheetIndex=atoi(sheetIndexStr);
    VegetationConfiguration vegCreatureConfiguration(char* gameID, t_SpriteWithOffset &sprite, bool live);
    //add a copy to known creatures
    vegetationConfigs->push_back( cre );
    elemTree = elemTree->NextSiblingElement("tree");
  }

  return true;
}

/*bool addCreaturesConfig( TiXmlElement* elemRoot, vector<CreatureConfiguration>* knownCreatures ){
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
}*/
	

