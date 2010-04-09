#include "common.h"
#include "SpriteMaps.h"
#include "Constructions.h"
#include "WorldSegment.h"

void changeConstructionMaterials(WorldSegment* segment, vector<t_construction>* allConstructions){
	//find construction
	int32_t i;
  Block* b;
	t_construction* construct = 0;
	i = (uint32_t) allConstructions->size();
  if(i <= 0) return;
  while(--i >= 0){
    construct = &(*allConstructions)[i];	
    b = segment->getBlock(construct->x, construct->y, construct->z);
    if( !b ) continue;
      b->material=construct->material;
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