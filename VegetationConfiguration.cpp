#include "common.h"
#include "CreatureConfiguration.h"
#include "Creatures.h"
#include "MapLoading.h"
#include "GUI.h"
#include "ContentLoader.h"

#include "dfhack/library/tinyxml/tinyxml.h"


VegetationConfiguration::VegetationConfiguration(int gameID, t_SpriteWithOffset &sprite, bool live, bool grown)
{
  memset(this, 0, sizeof(VegetationConfiguration) );
  this->sprite = sprite;
  this->gameID = gameID;
  this->live = live;
  this->grown = grown;
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

bool addSingleVegetationConfig( TiXmlElement* elemRoot,  vector<VegetationConfiguration>* vegetationConfigs, vector<t_matgloss>& plantNames )
{
  const char* sheetIndexStr;
  t_SpriteWithOffset sprite;
  int basefile = -1;
  sprite.fileIndex=basefile;
  sprite.x=0;
  sprite.y=0;
  sprite.animFrames=ALL_FRAMES;
 
  const char* filename = elemRoot->Attribute("file");
	if (filename != NULL && filename[0] != 0)
	{
	  	basefile = loadImgFile((char*)filename);
	}
	
	//kinda round about- looking to needing to shift the lot into the plant elem
	sprite.fileIndex=basefile;
	
  TiXmlElement* elemTree = elemRoot->FirstChildElement("plant");
  while( elemTree ){
  	int gameID = TranslatePlantName(elemTree->Attribute("gameID"),plantNames);
    const char* deadstr = elemTree->Attribute("dead");
    bool dead = (deadstr && deadstr[0]);   
    const char* saplingstr = elemTree->Attribute("sapling");
    bool sapling = (saplingstr && saplingstr[0]);   
    sheetIndexStr = elemTree->Attribute("sheetIndex");
    /* No animated trees.
    	But we may repurpose it later to make a xyz variance?
    sprite.animFrames = getAnimFrames(elemProfession->Attribute("frames"));
	if (sprite.animFrames == 0)
		sprite.animFrames = ALL_FRAMES;*/
    
    //create profession config
    sprite.sheetIndex=atoi(sheetIndexStr);
    VegetationConfiguration vegetationConfiguration(gameID, sprite, !dead, !sapling);
    //add a copy to known creatures
    vegetationConfigs->push_back( vegetationConfiguration );
    elemTree = elemTree->NextSiblingElement("plant");
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
	
t_SpriteWithOffset getVegetationSprite(vector<VegetationConfiguration>& vegetationConfigs,int index,bool live,bool grown)
{
	int vcmax = vegetationConfigs.size();
	for (int i=0;i<vcmax;i++)
	{
		VegetationConfiguration* current = &(vegetationConfigs[i]);
		if (current->gameID != index) continue;
		if (current->live != live) continue;
		if (current->grown != grown) continue;
		return current->sprite;
	}
	t_SpriteWithOffset sprite = {-1,0,0,-1,ALL_FRAMES};
	return sprite;
}

