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

    if( strcmp(strType, "NeighbourSameBuilding") == 0){
    BlockCondition cond( Cond_NeighbourSameBuilding );
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
  
  BuildingConfiguration building(strName, (char*) strGameID );

  //for every Tile this building has
  TiXmlElement* elemTile =  elemBuilding->FirstChildElement("Tile");
  while ( elemTile ){
    
    ConditionalSprite tile;
    //Add all the sprites that compose this tile
    TiXmlElement* elemSprite =  elemTile->FirstChildElement("Sprite");
    while( elemSprite ){
      const char* strSheetIndex = elemSprite->Attribute("sheetIndex");
      const char* strOffsetX = elemSprite->Attribute("offsetx");
      const char* strOffsetY = elemSprite->Attribute("offsety");
      int sheetIndex = (strSheetIndex != 0 ? atoi(strSheetIndex) : 0);
      int offsetX    = (strOffsetX    != 0 ? atoi(strOffsetX)    : 0);
      int offsetY    = (strOffsetY    != 0 ? atoi(strOffsetY)    : 0);

      t_SpriteWithOffset sprite = { sheetIndex, offsetX, offsetY };
      tile.sprites.push_back( sprite );
      elemSprite = elemSprite->NextSiblingElement("Sprite");
    }

    //load conditions
    TiXmlElement* elemCondition = elemTile->FirstChildElement("Condition");
    while( elemCondition ){
      parseConditionToSprite( tile, elemCondition->Attribute("type"), elemCondition->Attribute("value") );
      elemCondition = elemCondition->NextSiblingElement("Condition");
    }
    //add copy of sprite to building
    building.sprites.push_back( tile );

    elemTile = elemTile->NextSiblingElement("Tile");
  }

  //add a copy of 'building' to known buildings
  knownBuildings->push_back( building );
  return true;
}


bool LoadBuildingConfiguration( vector<BuildingConfiguration>* knownBuildings ){
  string line;
  ifstream myfile ("buildings/index.txt");
  if (myfile.is_open() == false){
    WriteErr("Unable to load building config index file!\n");
    return false;
  }

  knownBuildings->clear();

  while ( !myfile.eof() )
  {
    char filepath[50] = {0};
    getline (myfile,line);
    
    //some systems don't remove the \r char as a part of the line change:
    if(line.size() > 0 &&  line[line.size() -1 ] == '\r' )
      line.resize(line.size() -1);

    if(line.size() > 0){
      sprintf(filepath, "buildings/%s", line.c_str() );
      bool result = addSingleConfig( filepath, knownBuildings );
      if( !result )
        WriteErr("Unable to load building config %s\n", filepath);
    }
  }
  myfile.close();

  BuildingNamesTranslatedFromGame = false;

  return true;
}

