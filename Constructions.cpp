#include "common.h"
#include "SpriteMaps.h"
#include "Constructions.h"
#include "WorldSegment.h"

void changeConstructionMaterials(WorldSegment* segment, vector<df::construction>* allConstructions)
{
    //find construction
    int32_t i;
    Tile* b;
    df::construction* construct = 0;
    i = (uint32_t) allConstructions->size();
    if(i <= 0) {
        return;
    }
    while(--i >= 0) {
        construct = &(*allConstructions)[i];
        b = segment->getTile(construct->pos.x, construct->pos.y, construct->pos.z);
        if( !b ) {
            continue;
        }
        if (b->tileMaterial() != RemoteFortressReader::CONSTRUCTION)
            continue;
        //don't assign invalid material indexes
        //if(construct->mat_idx != -1){
        //on second thought, invalid indices are needed.
        b->material.type = construct->mat_type;
        b->material.index = construct->mat_index;
        //}
        b->consForm = construct->item_type;
    }
}

/**
 * Reads an unbuilt construction's tle type to the given block and converts the 
 * material to "CONSTRUCTION".
 */
bool readConstructionsToTile( Tile* b, const Buildings::t_building* building ) 
{
	if(building->type == df::building_type::Construction) {
		df::tiletype constructedTiletype = df::tiletype::Void;

		switch(building->construction_type)
		{
		case construction_type::Fortification:
			constructedTiletype = df::tiletype::ConstructedFortification;
			break;
		case construction_type::Wall:
			constructedTiletype = df::tiletype::ConstructedPillar;
			break;
		case construction_type::Floor:
			constructedTiletype = df::tiletype::ConstructedFloor;
			break;
		case construction_type::UpStair:
			constructedTiletype = df::tiletype::ConstructedStairU;
			break;
		case construction_type::DownStair:
			constructedTiletype = df::tiletype::ConstructedStairD;
			break;
		case construction_type::UpDownStair:
			constructedTiletype = df::tiletype::ConstructedStairUD;
			break;
		case construction_type::Ramp:
			constructedTiletype = df::tiletype::ConstructedRamp;
			break;
		case construction_type::TrackN:
			constructedTiletype = df::tiletype::ConstructedFloorTrackN;
			break;
		case construction_type::TrackS:
			constructedTiletype = df::tiletype::ConstructedFloorTrackS;
			break;
		case construction_type::TrackE:
			constructedTiletype = df::tiletype::ConstructedFloorTrackE;
			break;
		case construction_type::TrackW:
			constructedTiletype = df::tiletype::ConstructedFloorTrackW;
			break;
		case construction_type::TrackNS:
			constructedTiletype = df::tiletype::ConstructedFloorTrackNS;
			break;
		case construction_type::TrackNE:
			constructedTiletype = df::tiletype::ConstructedFloorTrackNE;
			break;
		case construction_type::TrackNW:
			constructedTiletype = df::tiletype::ConstructedFloorTrackNW;
			break;
		case construction_type::TrackSE:
			constructedTiletype = df::tiletype::ConstructedFloorTrackSE;
			break;
		case construction_type::TrackSW:
			constructedTiletype = df::tiletype::ConstructedFloorTrackSW;
			break;
		case construction_type::TrackEW:
			constructedTiletype = df::tiletype::ConstructedFloorTrackEW;
			break;
		case construction_type::TrackNSE:
			constructedTiletype = df::tiletype::ConstructedFloorTrackNSE;
			break;
		case construction_type::TrackNSW:
			constructedTiletype = df::tiletype::ConstructedFloorTrackNSW;
			break;
		case construction_type::TrackNEW:
			constructedTiletype = df::tiletype::ConstructedFloorTrackNEW;
			break;
		case construction_type::TrackSEW:
			constructedTiletype = df::tiletype::ConstructedFloorTrackSEW;
			break;
		case construction_type::TrackNSEW:
			constructedTiletype = df::tiletype::ConstructedFloorTrackNSEW;
			break;
		case construction_type::TrackRampN:
			constructedTiletype = df::tiletype::ConstructedRampTrackN;
			break;
		case construction_type::TrackRampS:
			constructedTiletype = df::tiletype::ConstructedRampTrackS;
			break;
		case construction_type::TrackRampE:
			constructedTiletype = df::tiletype::ConstructedRampTrackE;
			break;
		case construction_type::TrackRampW:
			constructedTiletype = df::tiletype::ConstructedRampTrackW;
			break;
		case construction_type::TrackRampNS:
			constructedTiletype = df::tiletype::ConstructedRampTrackNS;
			break;
		case construction_type::TrackRampNE:
			constructedTiletype = df::tiletype::ConstructedRampTrackNE;
			break;
		case construction_type::TrackRampNW:
			constructedTiletype = df::tiletype::ConstructedRampTrackNW;
			break;
		case construction_type::TrackRampSE:
			constructedTiletype = df::tiletype::ConstructedRampTrackSE;
			break;
		case construction_type::TrackRampSW:
			constructedTiletype = df::tiletype::ConstructedRampTrackSW;
			break;
		case construction_type::TrackRampEW:
			constructedTiletype = df::tiletype::ConstructedRampTrackEW;
			break;
		case construction_type::TrackRampNSE:
			constructedTiletype = df::tiletype::ConstructedRampTrackNSE;
			break;
		case construction_type::TrackRampNSW:
			constructedTiletype = df::tiletype::ConstructedRampTrackNSW;
			break;
		case construction_type::TrackRampNEW:
			constructedTiletype = df::tiletype::ConstructedRampTrackNEW;
			break;
		case construction_type::TrackRampSEW:
			constructedTiletype = df::tiletype::ConstructedRampTrackSEW;
			break;
		case construction_type::TrackRampNSEW:
			constructedTiletype = df::tiletype::ConstructedRampTrackNSEW;
			break;
		default:
			//do nothing
			break;
		}

		b->material.type = CONSTRUCTION;
		b->material.index = INVALID_INDEX;
		b->tileType = constructedTiletype;
		return true;
	}

	return false;
}
