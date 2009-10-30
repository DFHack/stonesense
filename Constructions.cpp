#include "common.h"
#include "SpriteMaps.h"
#include "Constructions.h"
#include "WorldSegment.h"


void changeConstructionMaterials(WorldSegment* segment, Block* b, vector<t_construction>* allConstructions){
	//find construction
	int32_t i;
	t_construction* c;
	t_construction* construct = 0;
	i = (uint32_t) allConstructions->size();
  if(i <= 0) return;
  while(--i){
		c = &(*allConstructions)[i];
		if(c->x == b->x && c->y == b->y && c->z == b->z ){
			construct = c;
			break;
		}
	}
	if(!construct) return;
	
	//replace floor, wall and ramp types
	if(b->floorType > 0){
		switch(c->material.type){
			case Mat_Wood:
        if(b->floorType == ID_CNSTFLOOR){
          //set to wooden floor
          b->floorType = ID_WOODFLOOR;
          //if tile beneth has a wooden wall, dont display a floor. hackish. buggish. whatever
          Block* under = segment->getBlock( b->x, b->y, b->z - 1 );
          if(under && under->wallType == ID_WOODWALL)
            b->floorType = 0;
        }
				break;
			case Mat_Stone: 
			//case Trey_Parker:
				break;

      case Mat_Metal:
        b->floorType = ID_METALFLOOR;
		}

	}
  if(b->wallType > 0){
		switch(c->material.type){
			case Mat_Wood:
				if(b->wallType == ID_CNSTFORTIFICATION)
					b->wallType = ID_WOODFORTIFICATION;
				else
					b->wallType = ID_WOODWALL;
				break;
      case Mat_Stone:
        //stones are the default constructed object, so dont change them
        break;
      case Mat_Metal:
        b->wallType = ID_METALWALL;
        break;
		}
	}
  if(b->stairType > 0){
    switch(c->material.type){
		case Mat_Wood:
      if(b->stairType == ID_CNSTR_STAIR_UP)
				b->stairType = ID_WOODFLOOR_STAIR_UP;
      if(b->stairType == ID_CNSTR_STAIR_UPDOWN)
				b->stairType = ID_WOODFLOOR_STAIR_UPDOWN;
      if(b->stairType == ID_CNSTR_STAIR_DOWN)
        b->stairType = ID_WOODFLOOR_STAIR_DOWN;
			break;
		case Mat_Stone: 
		//case Trey_Parker:
			break;
	  }
  }
	
}




bool IDisConstruction(int in){
  switch(in){
		case 495: //constructed pillar    
		case 496: //constructed wall rd2
    case 497: //constructed wall r2d
    case 498: //constructed wall r2u
    case 499: //constructed wall ru2
    case 500: //constructed wall l2u
    case 501: //constructed wall lu2
    case 502: //constructed wall l2d
    case 503: //constructed wall ld2
    case 504: //constructed wall lrud
    case 505: //constructed wall rud
    case 506: //constructed wall lrd
    case 507: //constructed wall lru
    case 508: //constructed wall lud
    case 509: //constructed wall rd
    case 510: //constructed wall ru
    case 511: //constructed wall lu
    case 512: //constructed wall ld
    case 513: //constructed wall ud
    case 514: //constructed wall lr
		
		case 493: //constructed floor detailed

		case 494: //constructed fortification

    case ID_CNSTR_STAIR_UPDOWN:
    case ID_CNSTR_STAIR_DOWN:
    case ID_CNSTR_STAIR_UP:
      return true;
      break;
  }
  return 0;
}