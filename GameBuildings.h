#pragma once

#include "common.h"
#include "BuildingConfiguration.h"

/*
 * extension to building_type enum from DFHack
 */
enum BuildingTypes {
    BUILDINGTYPE_NA = -1,
    BUILDINGTYPE_TREE = 998,
    BUILDINGTYPE_BLACKBOX = 999,
};







//int BlockNeighbourhoodType_simple(WorldSegment* segment, Block* b, bool validationFuctionProc(Block*) );



int getBuildingSprite(Buildings::t_building &building, bool mirrored);
void ReadBuildingsToSegment(DFHack::Core& DF, WorldSegment* segment);
void MergeBuildingsToSegment(vector<Buildings::t_building>* buildings, WorldSegment* segment);
void loadBuildingSprites( Block* b);
void ReadBuildings(DFHack::Core& DF, vector<Buildings::t_building>* buildingHolder);
bool BlockHasSuspendedBuilding(vector<Buildings::t_building>* buildingList, Block* b);

dirTypes findWallCloseTo(WorldSegment* segment, Block* b);


//extern vector<BuildingConfiguration> buildingTypes;
//extern vector <string> v_buildingtypes;