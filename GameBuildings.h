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







//int TileNeighbourhoodType_simple(WorldSegment* segment, Tile* b, bool validationFuctionProc(Tile*) );



int getBuildingSprite(Buildings::t_building &building, bool mirrored);
void ReadBuildingsToSegment(DFHack::Core& DF, WorldSegment* segment);
void MergeBuildingsToSegment(vector<Buildings::t_building>* buildings, WorldSegment* segment);
void loadBuildingSprites( Tile* b);
void ReadBuildings(DFHack::Core& DF, vector<Buildings::t_building>* buildingHolder);
bool TileHasSuspendedBuilding(vector<Buildings::t_building>* buildingList, Tile* b);

dirTypes findWallCloseTo(WorldSegment* segment, Tile* b);


//extern vector<BuildingConfiguration> buildingTypes;
//extern vector <string> v_buildingtypes;