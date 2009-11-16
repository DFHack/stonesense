#include "common.h"
#include "SpriteMaps.h"
#include "GroundMaterialConfiguration.h"

#include "dfhack/library/tinyxml/tinyxml.h"

bool GroundMaterialNamesTranslatedFromGame = false;

vector<GroundMaterialConfiguration*> groundTypes;
vector<preparseGroundMaterialConfiguration> xmlDefinedGroundTypes;

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

void addConfToIDdsaj (int index, preparseGroundMaterialConfiguration& conf){
  if(groundTypes[index] == NULL){
    //TODO: possible memleak.
    groundTypes[index] = new GroundMaterialConfiguration();
  }
  
  if( conf.overridingMaterials.empty() ){
    groundTypes[index]->defaultSprite = conf.spriteIndex;
    groundTypes[index]->fillerFloorSpriteIndex = conf.fillerFloorSpriteIndex;
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
      groundTypes[index]->overridingMaterials.push_back( ovr );
    }
  }
  
}

void TranslateGroundMaterialNames(){
  //create and entry for every known ground material type
  uint32_t num;
  uint32_t index = 0;

  groundTypes.clear();

  //figure out how many entries to make.
  int highestID = 0;
  num = (uint32_t) xmlDefinedGroundTypes.size();
  while( index < num ){
    for(uint32_t i = 0; i < xmlDefinedGroundTypes[index].wallFloorIDs.size(); i++){
      highestID = max(highestID, xmlDefinedGroundTypes[index].wallFloorIDs[i]);
    }
    index++;
  }

  //create table
  groundTypes.resize( highestID + 1 );

  index = 0;
  num = (int)xmlDefinedGroundTypes.size();
  while( index < num ){
    preparseGroundMaterialConfiguration& conf  = xmlDefinedGroundTypes[index];
    for(uint32_t i=0; i<conf.wallFloorIDs.size(); i++)
      addConfToIDdsaj( conf.wallFloorIDs[i], conf );
    

    index++;
  }

  GroundMaterialNamesTranslatedFromGame = true;
}

void parseWallFloorSpriteElement( TiXmlElement* elemWallFloorSprite ){
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

	xmlDefinedGroundTypes.push_back( newConfig );

}

void LoadGroundMaterialConfiguration(  ){

  char* filename = "GroundMaterials.xml";
  TiXmlDocument doc( filename );
  bool loadOkay = doc.LoadFile();
  TiXmlHandle hDoc(&doc);
  TiXmlElement* elemFloor;
  TiXmlElement* elemWall;
  
  //clear out old config data. TODO: check for memleaks
  xmlDefinedGroundTypes.clear();

  TiXmlElement* elemFloorRoot = hDoc.FirstChildElement("Floors").Element();
  TiXmlElement* elemWallRoot = hDoc.FirstChildElement("Walls").Element();
  if(elemFloorRoot == null){
    WriteErr("Could not find 'Floors' node in xml file\n");
    return;
  }
  if(elemWallRoot == null){
    WriteErr("Could not find 'Walls' node in xml file\n");
    return;
  }

  //parse floors
  elemFloor = elemFloorRoot->FirstChildElement("Floor");
  while( elemFloor ){
    parseWallFloorSpriteElement( elemFloor );
    elemFloor = elemFloor->NextSiblingElement("Floor");
  }
  
  //parse walls
  elemWall = elemWallRoot->FirstChildElement("Wall");
  while( elemWall ){
    parseWallFloorSpriteElement( elemWall );
    elemWall = elemWall->NextSiblingElement("Wall");
  }
  GroundMaterialNamesTranslatedFromGame = false;
}

