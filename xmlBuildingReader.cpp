#include <iostream>
#include <fstream>
#include <string>

#include "common.h"
#include "BuildingConfiguration.h"
#include "dfhack/library/tinyxml/tinyxml.h"


void parseConditionToSprite(ConditionalSprite& sprite, const char* strType, const char* strValue){
  if( strcmp(strType, "NeighbourWall") == 0){
    if( strcmp(strValue, "North") == 0)
      sprite.cNeighbourHasWall = eSimpleN;
    if( strcmp(strValue, "South") == 0)
      sprite.cNeighbourHasWall = eSimpleS;
    if( strcmp(strValue, "West") == 0)
      sprite.cNeighbourHasWall = eSimpleW;
    if( strcmp(strValue, "East") == 0)
      sprite.cNeighbourHasWall = eSimpleE;
  }

  if( strcmp(strType, "MaterialType") == 0){
    if( strcmp(strValue, "Wood") == 0)
      sprite.cMaterialType = Mat_Wood;
    if( strcmp(strValue, "Stone") == 0)
      sprite.cMaterialType = Mat_Stone;
  }

  if( strcmp(strType, "PositionIndex") == 0){
     sprite.cPositionIndex = atoi( strValue );
  }
}





bool addSingleConfig( const char* filename,  vector<BuildingConfiguration>* knownBuildings ){
  TiXmlDocument doc( filename );
  bool loadOkay = doc.LoadFile();
  TiXmlHandle hDoc(&doc);
  TiXmlElement* elemBuilding;

  elemBuilding = hDoc.FirstChildElement("Building").Element();
  if( elemBuilding == 0) 
    return false;
  
  const char* strName = elemBuilding->Attribute("name");
  const char* strGameID = elemBuilding->Attribute("gameID");
  
  BuildingConfiguration building(strName, atoi( strGameID ) );

  //for every Sprite
  TiXmlElement* elemSprite =  elemBuilding->FirstChildElement("Sprite");
  while ( elemSprite ){
    const char* strSheetIndex = elemSprite->Attribute("sheetIndex");  
    ConditionalSprite sprite;
    sprite.spriteIndex = atoi( strSheetIndex );

    //load conditions
    TiXmlElement* elemCondition = elemSprite->FirstChildElement("Condition");
    while( elemCondition ){
      parseConditionToSprite( sprite, elemCondition->Attribute("type"), elemCondition->Attribute("value") );
      elemCondition = elemCondition->NextSiblingElement("Condition");
    }
    //add copy of sprite to building
    building.sprites.push_back( sprite );

    elemSprite = elemSprite->NextSiblingElement("Sprite");
  }

  //add a copy of 'building' to known buildings
  knownBuildings->push_back( building );
  return true;
}


bool LoadBuildingConfiguration( vector<BuildingConfiguration>* knownBuildings ){
  string line;
  ifstream myfile ("buildings/index.txt");
  if (myfile.is_open() == false)
    return false;

  while ( !myfile.eof() )
  {
    char filepath[50] = {0};
    getline (myfile,line);
    sprintf_s(filepath, "buildings/%s", line.c_str() );
    addSingleConfig( filepath, knownBuildings );
  }
  myfile.close();
  return true;
}