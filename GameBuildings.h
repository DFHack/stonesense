#pragma once

#include "common.h"
#include "BuildingConfiguration.h"


enum BuildingTypes{
  BUILDINGTYPE_NA = -1,
  BUILDINGTYPE_STOCKPILE = 0,
  BUILDINGTYPE_ZONE = 1,
  BUILDINGTYPE_BULEPRINT = 2,
	BUILDINGTYPE_SUPPORT = 3,
  BUILDINGTYPE_ARMORSTAND = 4,
  BUILDINGTYPE_BED = 5,
  BUILDINGTYPE_CHAIR = 6,
  BUILDINGTYPE_DOOR = 8,
  BUILDINGTYPE_CABINET = 15,
  BUILDINGTYPE_CONTAINER = 16,
  BUILDINGTYPE_W_CARPENTER = 19,
  BUILDINGTYPE_W_MASON = 21,
	BUILDINGTYPE_W_STILL = 34,
	BUILDINGTYPE_FARM = 42,
  BUILDINGTYPE_STATUE = 44,
  BUILDINGTYPE_TABLE = 45,
  BUILDINGTYPE_ROAD = 46,
  BUILDINGTYPE_BRIDGE = 47,
  BUILDINGTYPE_TRADEDEPOT = 62,
  BUILDINGTYPE_PUMP = 71,
  BUILDINGTYPE_BLACKBOX = 999,
};







//int BlockNeighbourhoodType_simple(WorldSegment* segment, Block* b, bool validationFuctionProc(Block*) );



int getBuildingSprite(t_building &building, bool mirrored);
void ReadBuildingsToSegment(API& DF, WorldSegment* segment);
void MergeBuildingsToSegment(vector<t_building>* buildings, WorldSegment* segment);
void loadBuildingSprites( Block* b );
void ReadBuildings(API& DF, vector<t_building>* buildingHolder);
bool BlockHasSuspendedBuilding(vector<t_building>* buildingList, Block* b);

dirTypes findWallCloseTo(WorldSegment* segment, Block* b);


//extern vector<BuildingConfiguration> buildingTypes;
//extern vector <string> v_buildingtypes;