#include <iostream>
#include <fstream>
#include <string>

#include "common.h"
#include "BuildingConfiguration.h"
#include "BlockCondition.h"
#include "dfhack/library/tinyxml/tinyxml.h"


void parseConditionToSprite(ConditionalSprite& sprite, const char* strType, const char* strValue){
  if( strcmp(strType, "NeighbourWall") == 0){
    BlockCondition cond( Cond_NeighbourWall );
    if( strcmp(strValue, "None") == 0)
      cond.value = eSimpleSingle;
    if( strcmp(strValue, "North") == 0)
      cond.value = eSimpleN;
    if( strcmp(strValue, "South") == 0)
      cond.value = eSimpleS;
    if( strcmp(strValue, "West") == 0)
      cond.value = eSimpleW;
    if( strcmp(strValue, "East") == 0)
      cond.value = eSimpleE;

    sprite.conditions.push_back( cond );
  }

  if( strcmp(strType, "MaterialType") == 0){
    BlockCondition cond( Cond_MaterialType );
    if( strcmp(strValue, "Wood") == 0)
      cond.value = Mat_Wood;
    if( strcmp(strValue, "Stone") == 0)
      cond.value = Mat_Stone;
    if( strcmp(strValue, "Metal") == 0)
      cond.value = Mat_Metal;
    if( strcmp(strValue, "Leather") == 0)
      cond.value = Mat_Leather;
    if( strcmp(strValue, "Slik") == 0)
      cond.value = Mat_SilkCloth;
    if( strcmp(strValue, "PlantCloth") == 0)
      cond.value = Mat_PlantCloth;
    if( strcmp(strValue, "GreenGlass") == 0)
      cond.value = Mat_GreenGlass;
    if( strcmp(strValue, "ClearGlass") == 0)
      cond.value = Mat_ClearGlass;
    if( strcmp(strValue, "CrystalGlass") == 0)
      cond.value = Mat_CrystalGlass;
      

    sprite.conditions.push_back( cond );
  }

  if( strcmp(strType, "PositionIndex") == 0){
    BlockCondition cond( Cond_PositionIndex );
     cond.value = atoi( strValue );

     sprite.conditions.push_back( cond );
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