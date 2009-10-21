#include "BlockCondition.h"
#include "GameBuildings.h"
#include "GUI.h"

BlockCondition::BlockCondition(BlockConditionTypes type)
{
  this->type = type;
  this->value = INVALID_INDEX;
}



bool BlockCondition::Matches(Block* b){
  if(type == Cond_PositionIndex){
    int x = b->x - b->building.x1;
    int y = b->y - b->building.y1;
    int w = b->building.x2 - b->building.x1 + 1 ;
    int pos = y * w + x;

    return pos == this->value;
  }

  if(type == Cond_MaterialType){
    return b->building.material.type == this->value;
  }
  
  if(type == Cond_NeighbourWall){
    dirTypes closebyWalls = findWallCloseTo(b->ownerSegment,b);    

    bool n = hasWall( b->ownerSegment->getBlock( b->x, b->y - 1, b->z ) );
    bool s = hasWall( b->ownerSegment->getBlock( b->x, b->y + 1, b->z ) );
    bool w = hasWall( b->ownerSegment->getBlock( b->x - 1, b->y, b->z ) );
    bool e = hasWall( b->ownerSegment->getBlock( b->x + 1, b->y, b->z ) );
    
    if( value == eSimpleN && n) return true;
    if( value == eSimpleS && s) return true;
    if( value == eSimpleW && w) return true;
    if( value == eSimpleE && e) return true;
    
    if( value == eSimpleSingle && !n && !s && !w && !e) return true;

    return false;
  }

  return false;
}
