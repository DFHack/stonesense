#pragma once
#include "WorldSegment.h"

void beautifySegment(WorldSegment * segment);

inline bool isTileOnTopOfSegment(WorldSegment* segment, Tile* b)
{
    return b->z == segment->pos.z + segment->size.z - 2;
}
