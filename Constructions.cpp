#include "common.h"
#include "SpriteMaps.h"
#include "Constructions.h"
#include "WorldSegment.h"

void changeConstructionMaterials(WorldSegment* segment, vector<df::construction>* allConstructions)
{
    //find construction
    int32_t i;
    Block* b;
    df::construction* construct = 0;
    i = (uint32_t) allConstructions->size();
    if(i <= 0) {
        return;
    }
    while(--i >= 0) {
        construct = &(*allConstructions)[i];
        b = segment->getBlock(construct->pos.x, construct->pos.y, construct->pos.z);
        if( !b ) {
            continue;
        }
        //don't assign invalid material indexes
        //if(construct->mat_idx != -1){
        //on second thought, invalid indices are needed.
        b->material.type = construct->mat_type;
        b->material.index = construct->mat_index;
        //}
        b->consForm = construct->item_type;
    }
}

