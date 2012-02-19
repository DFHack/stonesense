#include "common.h"
#include "WorldSegment.h"
#include "SpriteMaps.h"
#include "GameBuildings.h"
#include "BuildingConfiguration.h"
#include "ContentLoader.h"
#include "GUI.h"

//vector<BuildingConfiguration> buildingTypes;
//vector <string> v_buildingtypes;//should be empty for all buildings
/*
int BlockNeighbourhoodType_simple(WorldSegment* segment, Block* b, bool validationFuctionProc(Block*) ){
uint32_t x,y,z;
x = b->x; y = b->y; z = b->z;

bool n = validationFuctionProc( segment->getBlock( x, y-1, z, true) );
bool s = validationFuctionProc( segment->getBlock( x, y+1, z, true) );
bool e = validationFuctionProc( segment->getBlock( x+1, y, z, true) );
bool w = validationFuctionProc( segment->getBlock( x-1, y, z, true) );
//bool nw = validationFuctionProc( segment->getBlock(, x-1, y-1, z) );
//bool ne = validationFuctionProc( segment->getBlock(, x+1, y-1, z) );
//bool SW = validationFuctionProc( segment->getBlock(, x-1, y+1, z) );
//bool se = validationFuctionProc( segment->getBlock(, x+1, y+1, z) );

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
}*/

bool blockHasBridge(Block* b){
	if(!b) return 0;
	return b->building.info.type == df::enums::building_type::Bridge;
}

dirTypes findWallCloseTo(WorldSegment* segment, Block* b){
	uint32_t x,y,z;
	x = b->x; y = b->y; z = b->z;
	bool n = hasWall( segment->getBlockRelativeTo( x, y, z, eUp) );
	bool s = hasWall( segment->getBlockRelativeTo( x, y, z, eDown) );
	bool w = hasWall( segment->getBlockRelativeTo( x, y, z, eLeft) );
	bool e = hasWall( segment->getBlockRelativeTo( x, y, z, eRight) );

	if(w) return eSimpleW;
	if(n) return eSimpleN;
	if(s) return eSimpleS;
	if(e) return eSimpleE;

	return eSimpleSingle;
}

void ReadBuildings(DFHack::Core& DF, vector<Buildings::t_building>* buildingHolder)
{
	if(config.skipBuildings)
		return;
	if(!buildingHolder) return;

	vector<string> dummy;
    uint32_t numbuildings = Buildings::getNumBuildings();
	Buildings::t_building tempbuilding;

	uint32_t index = 0;
	while(index < numbuildings){
		try
		{
			Buildings::Read(index, tempbuilding);
		}
		catch (exception &e)
		{
			WriteErr("DFhack exeption: %s\n", e.what());
			config.skipBuildings = true;
			return;
		}
		buildingHolder->push_back(tempbuilding);
		index++;
	}
}


void MergeBuildingsToSegment(vector<Buildings::t_building>* buildings, WorldSegment* segment){
	Buildings::t_building tempbuilding;
	uint32_t numBuildings = (uint32_t)buildings->size();
	for(uint32_t i=0; i < numBuildings; i++){
		tempbuilding = (*buildings)[i];

		//int bheight = tempbuilding.y2 - tempbuilding.y1;
		for(uint32_t yy = tempbuilding.y1; yy <= tempbuilding.y2; yy++)
			for(uint32_t xx = tempbuilding.x1; xx <= tempbuilding.x2; xx++){
				Block* b;
				bool inside = segment->CoordinateInsideSegment(xx,yy, tempbuilding.z);
				if(inside){
					//want hashtable :(
					// still need to test for b, because of ramp/building overlap
					b = segment->getBlock( xx, yy, tempbuilding.z);

					if(!b){
						//inside segment, but no block to represent it
						b = new Block(segment);
						b->x = xx;
						b->y = yy;
						b->z = tempbuilding.z;
						segment->addBlock( b );
					}

					if( b ){
						//handle special case where zones and stockpiles overlap buildings, and try to replace them
						if(b->building.info.type != BUILDINGTYPE_NA && tempbuilding.type == df::enums::building_type::Civzone )
							continue;
                        if(b->building.info.type != BUILDINGTYPE_NA && tempbuilding.type == df::enums::building_type::Stockpile )
							continue; 
						b->building.index = i;
						b->building.info = tempbuilding;
					}
				}
			}
	}

	//all blocks in the segment now have their building info loaded.
	//now set their sprites
	/*
	for(uint32_t i=0; i < segment->getNumBlocks(); i++){
	Block* b = segment->getBlock( i );
	if( b->building.info.type != BUILDINGTYPE_NA && b->building.info.type != BUILDINGTYPE_BLACKBOX )
	loadBuildingSprites( b );
	}
	*/

}


void loadBuildingSprites ( Block* b)
{
	bool foundBlockBuildingInfo = false;
	if (b == NULL)
	{
		WriteErr("Null Block skipped in loadBuildingSprites\n");
		return;
	}
    BuildingConfiguration* generic = NULL, *specific = NULL, *custom = NULL;
    for(auto iter = contentLoader->buildingConfigs.begin(); iter < contentLoader->buildingConfigs.end(); iter++)
    {
        BuildingConfiguration & conf = *iter;
        if(b->building.info.type == conf.game_type)
        {
            generic = &conf;
            if(b->building.info.subtype == conf.game_subtype)
            {
                specific = &conf;
                if(b->building.info.custom_type == conf.game_custom)
                {
                    custom = &conf;
                }
            }
        }
    }
    BuildingConfiguration * final = custom?custom:(specific?specific:(generic?generic:NULL));
    //check all sprites for one that matches all conditions
    if (final && final->sprites != NULL && final->sprites->copyToBlock(b))
    {
        foundBlockBuildingInfo = true;
    }
	//add yellow box, if needed. But only if the building was not found (this way we can have blank slots in buildings)
	if(b->building.sprites.size() == 0 && foundBlockBuildingInfo == false){
		c_sprite unknownBuildingSprite;
		unknownBuildingSprite.set_defaultsheet(IMGObjectSheet);
		unknownBuildingSprite.set_fileindex(-1);
		unknownBuildingSprite.set_sheetindex(0);
		b->building.sprites.push_back( unknownBuildingSprite );
	}
}

/*TODO: this function takes a massive amount of work, looping all buildings for every block*/
bool BlockHasSuspendedBuilding(vector<Buildings::t_building>* buildingList, Block* b){
	uint32_t num = (uint32_t)buildingList->size();
	for(uint32_t i=0; i < num; i++){
		Buildings::t_building* building = &(*buildingList)[i];

		//boundry check
		if(b->z != building->z) continue;
		if(b->x < building->x1  ||   b->x > building->x2) continue;
		if(b->y < building->y1  ||   b->y > building->y2) continue;

		if(building->type == df::enums::building_type::Bridge)
			return true;
		if(building->type == df::enums::building_type::Civzone)
			return true;
	}
	return false;
}
