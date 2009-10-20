#pragma once

#include "common.h"

class DisplaySegment;

class Block
{
public:
	Block(DisplaySegment* ownerSegment);
	~Block(void);

  DisplaySegment* ownerSegment;
  
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


bool hasWall(Block* b);