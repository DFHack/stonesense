#pragma once

void changeConstructionMaterials(WorldSegment* segment, vector<df::construction>* allConstructions);
bool readConstructionsToTile( Tile* b, const Buildings::t_building* building );