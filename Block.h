#pragma once

#include "common.h"

class WorldSegment;

class Block
{
public:
	Block(WorldSegment* ownerSegment);
	~Block(void);

  WorldSegment* ownerSegment;
  
	uint32_t x, y, z;
	int floorType;
	int wallType;
	int stairType;

  bool depthBorderNorth;
  bool depthBorderWest;

	t_matglossPair ramp;
	t_matglossPair water;

	t_building building;
	int overridingBuildingType;
  bool mirroredBuilding;
	t_matglossPair tree;

	bool IsVisible(){
		return (floorType || wallType) != 0;
	}
	void Draw(BITMAP* target);

private:

};


//find a better place for these
bool hasWall(Block* b);
bool wallShouldNotHaveBorders( int in );