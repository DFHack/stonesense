#pragma once
#include "WorldSegment.h"

void beautifySegment(WorldSegment * segment);

inline bool isTileOnTopOfSegment(WorldSegment* segment, Tile* b)
{
    return int(b->z) == segment->segState.Position.z + segment->segState.Size.z - 2;
}
