#include "BlockCondition.h"
#include "GameBuildings.h"
#include "WorldSegment.h"
#include "ContentLoader.h"
#include <iostream>

int getDirectionFromString(const char* strDir)
{
	if (strDir == NULL)
		return INVALID_INDEX;
	if( strcmp(strDir, "None") == 0)
	  return eSimpleSingle;
	if( strcmp(strDir, "North") == 0)
	  return eSimpleN;
	if( strcmp(strDir, "South") == 0)
	  return eSimpleS;
	if( strcmp(strDir, "West") == 0)
	  return eSimpleW;
	if( strcmp(strDir, "East") == 0)
	  return eSimpleE;
	 //these will change when rotation is available
	if( strcmp(strDir, "TopRight") == 0)
	  return eSimpleN;
	if( strcmp(strDir, "BottomLeft") == 0)
	  return eSimpleS;
	if( strcmp(strDir, "TopLeft") == 0)
	  return eSimpleW;
	if( strcmp(strDir, "BottomRight") == 0)
	  return eSimpleE;
	return INVALID_INDEX;	
}

int getBuildingFromString(const char* strType)
{
  for (uint32_t i=0; i<contentLoader.buildingNameStrings.size(); i++){
		if (contentLoader.buildingNameStrings[i].compare(strType) == 0)
		{
			return i;
		}
	}
	return INVALID_INDEX;	
}

NeighbourWallCondition::NeighbourWallCondition(const char* strDir)
	: BlockCondition()
{
	this->value = getDirectionFromString(strDir);
}

bool NeighbourWallCondition::Matches(Block* b)
{

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


PositionIndexCondition::PositionIndexCondition(const char* strValue)
	: BlockCondition()
{
    this->value = atoi( strValue );
}

bool PositionIndexCondition::Matches(Block* b)
{	
    int x = b->x - b->building.info.x1;
    int y = b->y - b->building.info.y1;
    int w = b->building.info.x2 - b->building.info.x1 + 1 ;
    int pos = y * w + x;

    return pos == this->value;
}




MaterialTypeCondition::MaterialTypeCondition(const char* strValue)
	: BlockCondition()
{
	// is there a better way to handle this?
	// seems non-extensible
	value = -1;
	if( strcmp(strValue, "Wood") == 0)
      value = Mat_Wood;
    else if( strcmp(strValue, "Stone") == 0)
      value = Mat_Stone;
    else if( strcmp(strValue, "Metal") == 0)
      value = Mat_Metal;
    else if( strcmp(strValue, "Leather") == 0)
      value = Mat_Leather;
    else if( strcmp(strValue, "Silk") == 0)
      value = Mat_SilkCloth;
    else if( strcmp(strValue, "PlantCloth") == 0)
      value = Mat_PlantCloth;
    else if( strcmp(strValue, "GreenGlass") == 0)
      value = Mat_GreenGlass;
    else if( strcmp(strValue, "ClearGlass") == 0)
      value = Mat_ClearGlass;
    else if( strcmp(strValue, "CrystalGlass") == 0)
      value = Mat_CrystalGlass;
}

bool MaterialTypeCondition::Matches(Block* b)
{
    return b->building.info.material.type == this->value;
}

MaterialIndexCondition::MaterialIndexCondition(const char* strValue)
	: BlockCondition()
{
	this->value = atoi( strValue );
}

bool MaterialIndexCondition::Matches(Block* b)
{
    return b->building.info.material.index == this->value;
}


AnimationFrameCondition::AnimationFrameCondition(const char* strValue)
	: BlockCondition()
{
	this->value = atoi( strValue );
}

bool AnimationFrameCondition::Matches(Block* b)
{
    return this->value == currentAnimationFrame;
}


BuildingOccupancyCondition::BuildingOccupancyCondition(const char* strValue)
	: BlockCondition()
{
    this->value = atoi( strValue );
}

bool BuildingOccupancyCondition::Matches(Block* b)
{
    return b->occ.bits.building == this->value;
}



NeighbourSameBuildingCondition::NeighbourSameBuildingCondition(const char* strDir)
	: BlockCondition()
{
	this->value = getDirectionFromString(strDir);
}

bool NeighbourSameBuildingCondition::Matches(Block* b)
{
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



NeighbourIdenticalCondition::NeighbourIdenticalCondition(const char* strDir)
	: BlockCondition()
{
	this->value = getDirectionFromString(strDir);
}

bool NeighbourIdenticalCondition::Matches(Block* b)
{	
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


NeighbourOfTypeCondition::NeighbourOfTypeCondition(const char* strDir, const char* strType)
	: BlockCondition()
{
	this->direction = getDirectionFromString(strDir);
	this->value = getBuildingFromString(strType);
}

bool NeighbourOfTypeCondition::Matches(Block* b)
{
    bool n = hasBuildingOfID( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eUp ), value );
    bool s = hasBuildingOfID( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eDown ), value );
    bool w = hasBuildingOfID( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eLeft ), value);
    bool e = hasBuildingOfID( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eRight ), value );
   
    if( direction == eSimpleN && n) return true;
    if( direction == eSimpleS && s) return true;
    if( direction == eSimpleW && w) return true;
    if( direction == eSimpleE && e) return true;
    
    if( direction == eSimpleSingle && !n && !s && !w && !e) return true;
    
    return false;
}

NeighbourSameTypeCondition::NeighbourSameTypeCondition(const char* strDir)
	: BlockCondition()
{
	this->direction = getDirectionFromString(strDir);
}

bool NeighbourSameTypeCondition::Matches(Block* b)
{	
	int value = b->building.info.type;
	
    bool n = hasBuildingOfID( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eUp ), value );
    bool s = hasBuildingOfID( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eDown ), value );
    bool w = hasBuildingOfID( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eLeft ), value);
    bool e = hasBuildingOfID( b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eRight ), value );
    
    if( direction == eSimpleN && n) return true;
    if( direction == eSimpleS && s) return true;
    if( direction == eSimpleW && w) return true;
    if( direction == eSimpleE && e) return true;
    
    if( direction == eSimpleSingle && !n && !s && !w && !e) return true;
    
    return false;
}

AndConditionalNode::~AndConditionalNode(void)
{
	uint32_t max = (int)children.size();
	for(uint32_t i=0; i<max; i++)
	{
		delete(children[i]);
	}
}

bool AndConditionalNode::Matches(Block* b)
{	
  uint32_t max = (int)children.size();
	for(uint32_t i=0; i<max; i++)
	{
		if (!children[i]->Matches( b ))
			return false;
	}
	return true;
}
bool AndConditionalNode::addCondition(BlockCondition* cond)
{
	children.push_back(cond);
	return true;
}

OrConditionalNode::~OrConditionalNode(void)
{
	uint32_t max = (int)children.size();
	for(uint32_t i=0; i<max; i++)
	{
		delete(children[i]);
	}
}

bool OrConditionalNode::Matches(Block* b)
{	
	uint32_t max = (int)children.size();
	for(uint32_t i=0; i<max; i++)
	{
		if (children[i]->Matches( b ))
			return true;
	}
	return false;
}
bool OrConditionalNode::addCondition(BlockCondition* cond)
{
	children.push_back(cond);
	return true;
}

bool AlwaysCondition::Matches(Block* b)
{
	return true;
}
bool NeverCondition::Matches(Block* b)
{
	return false;
}

NotConditionalNode::NotConditionalNode(void)
{
	childcond = NULL;
}

NotConditionalNode::~NotConditionalNode(void)
{
	delete(childcond);
}

bool NotConditionalNode::Matches(Block* b)
{	
	if (childcond == NULL)
		return true;
	return !childcond->Matches( b );
}
bool NotConditionalNode::addCondition(BlockCondition* cond)
{
	if (childcond != NULL)
	{
		WriteErr("Too many condition elements for NotConditionalNode\n");
		return false;
	}
	childcond = cond;
	return true;
}

bool HaveFloorCondition::Matches(Block* b)
{	
	return (b->floorType > 0);
}

FluidBelowCondition::FluidBelowCondition(const char* strValue)
	: BlockCondition()
{
    this->value = atoi( strValue );
}

bool FluidBelowCondition::Matches(Block* b)
{	
	Block* bLow = b->ownerSegment->getBlockRelativeTo( b->x, b->y, b->z, eBelow );
	if (!bLow)
		return false;
	return (bLow->water.index >= this->value);
}
