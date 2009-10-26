#include "common.h"
#include "WorldSegment.h"
#include "SpriteMaps.h"
#include "GameBuildings.h"
#include "BuildingConfiguration.h"

vector<BuildingConfiguration> buildingTypes;
vector <string> v_buildingtypes;//should be empty for all buildings


void loadSpecialBuildingTypes (WorldSegment* segment, Block* b, uint32_t relativex, uint32_t relativey, uint32_t height);

int BlockNeighbourhoodType_simple(WorldSegment* segment, Block* b, bool validationFuctionProc(Block*) ){
  uint32_t x,y,z;
  x = b->x; y = b->y; z = b->z;

	bool n = validationFuctionProc( segment->getBlock( x, y-1, z) );
	bool s = validationFuctionProc( segment->getBlock( x, y+1, z) );
	bool e = validationFuctionProc( segment->getBlock( x+1, y, z) );
	bool w = validationFuctionProc( segment->getBlock( x-1, y, z) );
	/*bool nw = validationFuctionProc( segment->getBlock(, x-1, y-1, z) );
	bool ne = validationFuctionProc( segment->getBlock(, x+1, y-1, z) );
	bool SW = validationFuctionProc( segment->getBlock(, x-1, y+1, z) );
	bool se = validationFuctionProc( segment->getBlock(, x+1, y+1, z) );*/
  
  if(!n && !s && !w && !e) return eSimpleSingle;
  if( n && !s && !w && !e) return eSimpleN;
  if(!n && !s &&  w && !e) return eSimpleW;
  if(!n &&  s && !w && !e) return eSimpleS;
  if(!n && !s && !w &&  e) return eSimpleE;
  
  if( n &&  s && !w && !e) return eSimpleNnS;
  if(!n && !s &&  w &&  e) return eSimpleWnE;

  if( n && !s &&  w && !e) return eSimpleNnW;
  if(!n &&  s &&  w && !e) return eSimpleSnW;
  if(!n &&  s && !w &&  e) return eSimpleSnE;
  if( n && !s && !w &&  e) return eSimpleNnE;
  
  //....

  return eSimpleSingle;
}

bool blockHasBridge(Block* b){
  if(!b) return 0;
  return b->building.info.type == BUILDINGTYPE_BRIDGE;
}

dirTypes findWallCloseTo(WorldSegment* segment, Block* b){
  uint32_t x,y,z;
  x = b->x; y = b->y; z = b->z;
  bool n = hasWall( segment->getBlock( x, y-1, z) );
	bool s = hasWall( segment->getBlock( x, y+1, z) );
	bool e = hasWall( segment->getBlock( x+1, y, z) );
	bool w = hasWall( segment->getBlock( x-1, y, z) );
  
  if(w) return eSimpleW;
  if(n) return eSimpleN;
  if(s) return eSimpleS;
  if(e) return eSimpleE;
  
  return eSimpleSingle;
}

void ReadBuildings(DFHackAPI& DF, vector<t_building>* buildingHolder){
  if(!buildingHolder) return;

  v_buildingtypes.clear();
	uint32_t numbuildings = DF.InitReadBuildings(v_buildingtypes);
	t_building tempbuilding;

  if( !BuildingNamesTranslatedFromGame )
    TranslateBuildingNames();

	uint32_t index = 0;
	while(index < numbuildings){
		DF.ReadBuilding(index, tempbuilding);
    buildingHolder->push_back(tempbuilding);
    index++;
  }
}


void MergeBuildingsToSegment(vector<t_building>* buildings, WorldSegment* segment){
	t_building tempbuilding;

	uint32_t index = 0;
	for(uint32_t i=0; i < buildings->size(); i++){
    tempbuilding = (*buildings)[i];
		
		int bheight = tempbuilding.y2 - tempbuilding.y1;
		for(uint32_t yy = tempbuilding.y1; yy <= tempbuilding.y2; yy++)
		for(uint32_t xx = tempbuilding.x1; xx <= tempbuilding.x2; xx++){
			Block* b;
      //want hashtable :(
			if( b = segment->getBlock( xx, yy, tempbuilding.z) ){
        //handle special case where zones overlap buildings, and try to replace them
        if(b->building.info.type != BUILDINGTYPE_NA && tempbuilding.type == BUILDINGTYPE_ZONE )
          continue;
        
				b->building.info = tempbuilding;
				//b->building.x1 = b->building.x2 = xx;
				//b->building.y1 = b->building.y2 = yy;
        loadSpecialBuildingTypes(segment, b, xx-tempbuilding.x1, yy-tempbuilding.y1, bheight);
      }
    }
		
		index++;
	}
	
}


void loadSpecialBuildingTypes (WorldSegment* segment, Block* b, uint32_t relativex, uint32_t relativey, uint32_t height){
  uint32_t i,j;
  bool foundBlockBuildingInfo = false;
  for(i = 0; i < buildingTypes.size(); i++){
    BuildingConfiguration& conf = buildingTypes[i];
    if(b->building.info.type != conf.gameID) continue;

    //check all sprites for one that matches all conditions
    for(j = 0; j < conf.sprites.size(); j++){
      if(conf.sprites[j].BlockMatches(b)){
//        t_SpriteWithOffset Sprite;
        b->building.sprites = conf.sprites[j].sprites;
        
        foundBlockBuildingInfo = true;
        //b->overridingBuildingType = conf.sprites[j].spriteIndex;
        
        break;
      }
    }

    //add yellow box, if needed. But only if the building was not found (this way we can have blank slots in buildings)
    if(b->building.sprites.size() == 0 && foundBlockBuildingInfo == false){
      t_SpriteWithOffset unknownBuildingSprite = {SPRITEOBJECT_NA, 0, 0};
      b->building.sprites.push_back( unknownBuildingSprite );
    }
    break;
  }
}

/*
void loadSpecialBuildingTypes2 (WorldSegment* segment, Block* b, uint32_t relativex, uint32_t relativey, uint32_t height){

  int* buildingInfoArray = 0;
  int arrayIndex;
  int i;

  switch(b->building.type){
  case BUILDINGTYPE_DOOR:
    
    i = findWallCloseTo(segment,b);
    if( i == eSimpleN || i == eSimpleS )
      b->mirroredBuilding = true;
    break;

  case BUILDINGTYPE_TRADEDEPOT:
    buildingInfoArray = layoutTradeDepot;
    break;
  case BUILDINGTYPE_W_STILL:
    //buildingInfoArray = layoutStill;
    break;
  case BUILDINGTYPE_W_CARPENTER:
    //buildingInfoArray = layoutCarpenter;
    break;
  case BUILDINGTYPE_BRIDGE:
    b->overridingBuildingType = SPRITEOBJECT_BRIDGE_C2;
    
    if(b->building.x1 == b->x)
      b->overridingBuildingType = SPRITEOBJECT_BRIDGE_W;
    if(b->building.x2 == b->x)
      b->overridingBuildingType = SPRITEOBJECT_BRIDGE_E;

    if(b->building.y1 == b->y){
      b->overridingBuildingType = SPRITEOBJECT_BRIDGE_N;
      if( b->building.x1 == b->x )
        b->overridingBuildingType = SPRITEOBJECT_BRIDGE_NW;
      if( b->building.x2 == b->x )
        b->overridingBuildingType = SPRITEOBJECT_BRIDGE_NE;
    }
    if(b->building.y2 == b->y){
      b->overridingBuildingType = SPRITEOBJECT_BRIDGE_S;
      if( b->building.x1 == b->x )
        b->overridingBuildingType = SPRITEOBJECT_BRIDGE_SW;
      if( b->building.x2 == b->x )
        b->overridingBuildingType = SPRITEOBJECT_BRIDGE_SE;
    }
    
    if(b->building.y2 == b->building.y1 || b->building.x2 == b->building.x1 )
      b->overridingBuildingType = SPRITEOBJECT_BRIDGE_C;
    break;
  }

  if(buildingInfoArray){
    arrayIndex = relativex + relativey*(height+1);
    b->overridingBuildingType = buildingInfoArray[arrayIndex];
  }
}

*/
/*
int getBuildingSprite(t_building &building, bool mirrored){
  return SPRITEOBJECT_NA;


  switch(building.type){
  case BUILDINGTYPE_DOOR:
    if(building.material.type == Mat_Wood) {
      if(building.material.index == 9)//custom case for Willow doors, just to show it's possible
        return mirrored ? SPRITEOBJECT_DOORWOOD_WILLOW_MIR : SPRITEOBJECT_DOORWOOD_WILLOW;
      return mirrored ? SPRITEOBJECT_DOORWOOD_MIR : SPRITEOBJECT_DOORWOOD;
    }
	  if(building.material.type == Mat_Stone) 
      return mirrored ? SPRITEOBJECT_DOORROCK_MIR : SPRITEOBJECT_DOORROCK;
    break;

  case BUILDINGTYPE_STOCKPILE:
    return SPRITEOBJECT_STOCKPILE;
    break;

  case BUILDINGTYPE_ARMORSTAND:
    return SPRITEOBJECT_ARMORSTAND;
    break;

  case BUILDINGTYPE_BED:
    return SPRITEOBJECT_BED_WOOD;
    break;

  case BUILDINGTYPE_TABLE:
    if(building.material.type == Mat_Wood)
      return SPRITEOBJECT_TABLE_WOOD;
    else
      return SPRITEOBJECT_TABLE_ROCK;
    break;
  case BUILDINGTYPE_CHAIR:
    if(building.material.type == Mat_Wood)
      return SPRITEOBJECT_CHAIR_WOOD;
    else
      return SPRITEOBJECT_CHAIR_ROCK;
    break;
  case BUILDINGTYPE_STATUE:
    return SPRITEOBJECT_STATUE;
    break;
  case BUILDINGTYPE_CABINET:
    if(building.material.type == Mat_Wood)
      return SPRITEOBJECT_CABINET_WOOD;
    else
      return SPRITEOBJECT_CABINET;
    break;
  case BUILDINGTYPE_CONTAINER:
    return SPRITEOBJECT_CHEST;
	case BUILDINGTYPE_FARM:
		return SPRITEOBJECT_FARM;
		break;
  case BUILDINGTYPE_ZONE:
    return SPRITEOBJECT_CURSORZONE;
    break;
  case BUILDINGTYPE_BULEPRINT:
    return SPRITEOBJECT_BLUEPRINT;
    break;
  case BUILDINGTYPE_ROAD:
    return SPRITEOBJECT_BRIDGE_C;
    break;
  }


  return SPRITEOBJECT_NA;
}
*/
/*TODO: this function takes a massive amount of work, looping all buildings for every block*/
bool BlockHasSuspendedBuilding(vector<t_building>* buildingList, Block* b){
  for(uint32_t i=0; i < buildingList->size(); i++){
    t_building* building = &(*buildingList)[i];

    //boundry check
    if(b->z != building->z) continue;
    if(b->x < building->x1  ||   b->x > building->x2) continue;
    if(b->y < building->y1  ||   b->y > building->y2) continue;

    if(building->type == BUILDINGTYPE_BRIDGE){
        return true;
    }
    if(building->type == BUILDINGTYPE_ZONE)
      return true;
  }
  return false;
}