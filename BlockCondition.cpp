#include "BlockCondition.h"
#include "GameBuildings.h"
#include "WorldSegment.h"

BlockCondition::BlockCondition(BlockConditionTypes type)
{
  this->type = type;
  this->value = INVALID_INDEX;
}



bool BlockCondition::Matches(Block* b){
  if(type == Cond_PositionIndex){
    int x = b->x - b->building.info.x1;
    int y = b->y - b->building.info.y1;
    int w = b->building.info.x2 - b->building.info.x1 + 1 ;
    int pos = y * w + x;

    return pos == this->value;
  }

  if(type == Cond_MaterialType){
    return b->building.info.material.type == this->value;
  }
  
  if(type == Cond_NeighbourWall){
    //dirTypes closebyWalls = findWallCloseTo(b->ownerSegment,b);    

    bool n = hasWall( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eUp ) );
    bool s = hasWall( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eDown ) );
    bool w = hasWall( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eLeft ) );
    bool e = hasWall( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eRight ) );
    
    if( value == eSimpleN && n) return true;
    if( value == eSimpleS && s) return true;
    if( value == eSimpleW && w) return true;
    if( value == eSimpleE && e) return true;
    
    if( value == eSimpleSingle && !n && !s && !w && !e) return true;

    return false;
  }

  if(type == Cond_NeighbourSameBuilding){
    int blocksBuildingID = b->building.info.type;

    bool n = hasBuildingOfID( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eUp ), blocksBuildingID );
    bool s = hasBuildingOfID( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eDown ), blocksBuildingID );
    bool w = hasBuildingOfID( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eLeft ), blocksBuildingID);
    bool e = hasBuildingOfID( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eRight ), blocksBuildingID );
    
    if( value == eSimpleN && n) return true;
    if( value == eSimpleS && s) return true;
    if( value == eSimpleW && w) return true;
    if( value == eSimpleE && e) return true;
    
    if( value == eSimpleSingle && !n && !s && !w && !e) return true;
    
    return false;
  }
  
  if(type == Cond_NeighbourIdentical){
	   
    int blocksBuildingIndex = b->building.index;
    int blocksBuildingOcc = b->occ.bits.building;

    bool n = hasBuildingIdentity( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eUp ), blocksBuildingIndex, blocksBuildingOcc );
    bool s = hasBuildingIdentity( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eDown ), blocksBuildingIndex, blocksBuildingOcc );
    bool w = hasBuildingIdentity( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eLeft ), blocksBuildingIndex, blocksBuildingOcc );
    bool e = hasBuildingIdentity( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eRight ), blocksBuildingIndex, blocksBuildingOcc );

    if( value == eSimpleN && n) return true;
    if( value == eSimpleS && s) return true;
    if( value == eSimpleW && w) return true;
    if( value == eSimpleE && e) return true;
    
    if( value == eSimpleSingle && !n && !s && !w && !e) return true;
    
    return false;
  }

  if(type == Cond_NeighbourSameIndex){
	   
    int blocksBuildingIndex = b->building.index;

    bool n = hasBuildingOfIndex( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eUp    ), blocksBuildingIndex );
    bool s = hasBuildingOfIndex( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eDown  ), blocksBuildingIndex );
    bool w = hasBuildingOfIndex( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eLeft  ), blocksBuildingIndex );
    bool e = hasBuildingOfIndex( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eRight ), blocksBuildingIndex );

    if( value == eSimpleN && n) return true;
    if( value == eSimpleS && s) return true;
    if( value == eSimpleW && w) return true;
    if( value == eSimpleE && e) return true;
    
    if( value == eSimpleSingle && !n && !s && !w && !e) return true;
    
    return false;
  }
  
    if(type == Cond_BuildingOcc){
	   return b->occ.bits.building == this->value;
    }
    
  return false;
}
