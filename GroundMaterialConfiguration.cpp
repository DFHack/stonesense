#include "common.h"
#include "SpriteMaps.h"
#include "GroundMaterialConfiguration.h"

#include "dfhack/library/tinyxml/tinyxml.h"

//bool GroundMaterialNamesTranslatedFromGame = false;

//vector<GroundMaterialConfiguration*> groundTypes;
//vector<preparseGroundMaterialConfiguration> xmlDefinedGroundTypes;

/*
GroundMaterialConfiguration::GroundMaterialConfiguration(char* gameIDstr, int wallSheetIndex,int floorSheetIndex)
{
  this->wallSheetIndex = wallSheetIndex;
  this->floorSheetIndex = floorSheetIndex;

  int len = (int) strlen(gameIDstr);
  if(len > 100) len = 100;
  memcpy(this->gameIDstr, gameIDstr, len);
  this->gameIDstr[len] = 0;
}*/
GroundMaterialConfiguration::GroundMaterialConfiguration(){
  defaultSprite = SPRITEFLOOR_NA;
}

void DumpGroundMaterialNamesToDisk(){
  FILE* fp = fopen("dump.txt", "w");
  if(!fp) return;
  for(uint32_t j=0; j < v_stonetypes.size(); j++){
    fprintf(fp, "%i:%s\n",j, v_stonetypes[j].id);
  }
  fclose(fp);
}

void addConfToGroundConfigList (vector<GroundMaterialConfiguration*>& configs, int index, preparseGroundMaterialConfiguration& conf){
  if(configs[index] == NULL){
    //TODO: possible memleak.
    configs[index] = new GroundMaterialConfiguration();
  }
  
  if( conf.overridingMaterials.empty() ){
    configs[index]->defaultSprite = conf.spriteIndex;
    configs[index]->fillerFloorSpriteIndex = conf.fillerFloorSpriteIndex;
  }else{
    int num = (int)conf.overridingMaterials.size();
    for(int i=0; i < num; i++){
      OverridingMaterial ovr = {0};
      ovr.spriteIndex = conf.spriteIndex;
      ovr.fillerFloorSpriteIndex = conf.fillerFloorSpriteIndex;
      //find material type
      int matGameId = INVALID_INDEX;
      for(uint32_t j=0; j< v_stonetypes.size(); j++)
        if(conf.overridingMaterials[i].compare(v_stonetypes[j].id) == 0)
          matGameId = j;
      ovr.gameID = matGameId;
      configs[index]->overridingMaterials.push_back( ovr );
    }
  }
  
}

void TranslateGroundMaterialNames(vector<GroundMaterialConfiguration*>& configs, vector<preparseGroundMaterialConfiguration>& rawGroundConfigs){
  //create and entry for every known ground material type
  uint32_t num;
  uint32_t index = 0;

  configs.clear();

  //figure out how many entries to make.
  int highestID = 0;
  num = (uint32_t) rawGroundConfigs.size();
  while( index < num ){
    for(uint32_t i = 0; i < rawGroundConfigs[index].wallFloorIDs.size(); i++){
      highestID = max(highestID, rawGroundConfigs[index].wallFloorIDs[i]);
    }
    index++;
  }

  //create table
  configs.resize( highestID + 1 );

  index = 0;
  num = (int)rawGroundConfigs.size();
  while( index < num ){
    preparseGroundMaterialConfiguration& conf  = rawGroundConfigs[index];
    for(uint32_t i=0; i<conf.wallFloorIDs.size(); i++)
      addConfToGroundConfigList(configs, conf.wallFloorIDs[i], conf );
    index++;
  }

}

void parseWallFloorSpriteElement( TiXmlElement* elemWallFloorSprite, vector<preparseGroundMaterialConfiguration>* rawGroundTypes  ){
  TiXmlElement* elemGameID;
	TiXmlElement* elemMaterial;

  const char* spriteIndexstr = elemWallFloorSprite->Attribute("spriteIndex");
  const char* fillerFloorSpriteIndexstr = elemWallFloorSprite->Attribute("fillerFloorSpriteIndex");
	
  preparseGroundMaterialConfiguration newConfig;
  newConfig.spriteIndex = atoi( spriteIndexstr );
  if(fillerFloorSpriteIndexstr)
    newConfig.fillerFloorSpriteIndex = atoi( fillerFloorSpriteIndexstr );
  else 
    newConfig.fillerFloorSpriteIndex = 0;

	elemGameID = elemWallFloorSprite->FirstChildElement("GameID");
	while( elemGameID ){
		const char* gameIDstr = elemGameID->Attribute("value");
    newConfig.wallFloorIDs.push_back( atoi(gameIDstr) );
    
    elemGameID = elemGameID->NextSiblingElement("GameID");
	}

  elemMaterial = elemWallFloorSprite->FirstChildElement("Material");
  while( elemMaterial ){
	  const char* gameIDstr = elemMaterial->Attribute("name");
    newConfig.overridingMaterials.push_back( *(new string(gameIDstr)) );
    
	  elemMaterial = elemMaterial->NextSiblingElement("Material");
  }

	rawGroundTypes->push_back( newConfig );

}

bool addSingleTerrainConfig( TiXmlElement* elemRoot,  vector<preparseGroundMaterialConfiguration>* rawGroundTypes ){
  string elementType = elemRoot->Value();

  if(elementType.compare( "floors" ) == 0){
    //parse floors
    TiXmlElement* elemFloor = elemRoot->FirstChildElement("Floor");
    while( elemFloor ){
      parseWallFloorSpriteElement( elemFloor, rawGroundTypes );
      elemFloor = elemFloor->NextSiblingElement("Floor");
    }
  }
  if(elementType.compare( "walls" ) == 0){
    //parse walls
    TiXmlElement* elemWall = elemRoot->FirstChildElement("Wall");
    while( elemWall ){
      parseWallFloorSpriteElement( elemWall, rawGroundTypes );
      elemWall = elemWall->NextSiblingElement("Wall");
    }
  }
  return true;
}

