#include "common.h"
#include "WorldSegment.h"
#include "SpriteMaps.h"
#include "GameBuildings.h"
#include "Constructions.h"
#include "BuildingConfiguration.h"
#include "ContentLoader.h"
#include "GUI.h"
#include "MapLoading.h"
#include "MiscUtils.h"
#include "GameConfiguration.h"
#include "StonesenseState.h"

#include "df/buildings_other_id.h"
#include "df/building_wellst.h"
#include "df/buildingitemst.h"
#include "df/item_constructed.h"
#include "df/item_slabst.h"
#include "df/itemimprovement.h"
#include "df/itemimprovement_threadst.h"
#include "df/world.h"

using df::global::world;

dirTypes findWallCloseTo(WorldSegment* segment, Tile* b)
{
    uint32_t x,y,z;
    x = b->x;
    y = b->y;
    z = b->z;
    bool n = hasWall( segment->getTileRelativeTo( x, y, z, eUp) );
    bool s = hasWall( segment->getTileRelativeTo( x, y, z, eDown) );
    bool w = hasWall( segment->getTileRelativeTo( x, y, z, eLeft) );
    bool e = hasWall( segment->getTileRelativeTo( x, y, z, eRight) );

    if(w) {
        return eSimpleW;
    }
    if(n) {
        return eSimpleN;
    }
    if(s) {
        return eSimpleS;
    }
    if(e) {
        return eSimpleE;
    }

    return eSimpleSingle;
}

void ReadBuildings(DFHack::Core& DF, std::vector<Stonesense_Building>* buildingHolder)
{
    auto& ssConfig = stonesenseState.ssConfig;

    if(ssConfig.skipBuildings) {
        return;
    }

    if(!buildingHolder) {
        return;
    }

    df::buildings_other_id types = df::buildings_other_id::IN_PLAY;

    for (size_t i = 0; i < world->buildings.other[types].size(); i++) {
        df::building *bld = world->buildings.other[types][i];
        Stonesense_Building tempbuilding{
            .x1 = uint32_t(bld->x1),
            .y1 = uint32_t(bld->y1),
            .x2 = uint32_t(bld->x2),
            .y2 = uint32_t(bld->y2),
            .z = uint32_t(bld->z),
            .material {.type = bld->mat_type, .index = bld->mat_index },
            .type = bld->getType(),
            .subtype = bld->getSubtype(),
            .custom_type = bld->getCustomType(),
            .origin = bld
        };
        buildingHolder->push_back(tempbuilding);
    }
}

void MergeBuildingsToSegment(std::vector<Stonesense_Building>* buildings, WorldSegment* segment)
{
    uint32_t numBuildings = (uint32_t)buildings->size();
    for (uint32_t i = 0; i < numBuildings; i++) {
        auto building_ptr = std::make_unique<Stonesense_Building>();
        auto copiedbuilding = building_ptr.get();
        *copiedbuilding = buildings->at(i);
        segment->PushBuilding(std::move(building_ptr));

        //int bheight = tempbuilding.y2 - tempbuilding.y1;
        for (uint32_t yy = copiedbuilding->y1; yy <= copiedbuilding->y2; yy++) {
            for (uint32_t xx = copiedbuilding->x1; xx <= copiedbuilding->x2; xx++) {
                uint32_t z2 = copiedbuilding->z;
                //if it's a well, add the bucket status.
                if (copiedbuilding->type == df::enums::building_type::Well) {
                    auto well_building = virtual_cast<df::building_wellst>(copiedbuilding->origin);
                    if (well_building)
                        z2 = well_building->bucket_z;
                }

                for (uint32_t zz = copiedbuilding->z; zz >= z2; zz--) {
                    if (copiedbuilding->type == df::enums::building_type::Civzone ||
                        copiedbuilding->type == df::enums::building_type::Stockpile ||
                        copiedbuilding->type == df::enums::building_type::FarmPlot) {
                        df::coord2d t;
                        t.x = xx;
                        t.y = yy;
                        if (!DFHack::Buildings::containsTile(copiedbuilding->origin, t))
                            continue;
                    }
                    Tile* b = segment->getTile(xx, yy, zz);
                    if (!b) {
                        b = segment->ResetTile(xx, yy, zz, df::tiletype::OpenSpace);
                        if (!b) {
                            continue;
                        }
                    }
                    //want hashtable :(
                    // still need to test for b, because of ramp/building overlap

                    //handle special case where zones and stockpiles overlap buildings, and try to replace them
                    if (b->building.type != BUILDINGTYPE_NA &&
                        copiedbuilding->type == df::enums::building_type::Civzone) {
                        continue;
                    }
                    if (b->building.type != BUILDINGTYPE_NA &&
                        copiedbuilding->type == df::enums::building_type::Stockpile) {
                        continue;
                    }
                    b->building.type = copiedbuilding->type;
                    b->building.info = copiedbuilding;
                    b->building.special = 0;

                    switch (b->building.type){
                    case df::enums::building_type::Well:
                        // copy down well information
                        if (copiedbuilding->z == z2)
                            b->building.special = 0;
                        else if (zz == copiedbuilding->z)
                            b->building.special = 1;
                        else if (zz == z2)
                            b->building.special = 2;
                        else b->building.special = 3;
                        break;
                    case df::enums::building_type::Slab:
                    {
                        auto Actual_building = virtual_cast<df::building_actual>(b->building.info->origin);
                        if (Actual_building && Actual_building->contained_items.size())
                        {
                            auto slab = virtual_cast<df::item_slabst>(Actual_building->contained_items[0]->item);
                            if (slab)
                                b->building.special = slab->engraving_type;
                        }
                    }
                        break;
                    case df::enums::building_type::Construction:
                        // change tile type to display the construction
                        if (stonesenseState.ssConfig.show_designations) {
                            readConstructionsToTile(b, copiedbuilding);
                            continue;
                        }
                        break;
                    default:
                        break;
                    }

                    //add building components.
                    auto Actual_building = virtual_cast<df::building_actual>(b->building.info->origin);
                    if (Actual_building){
                        for (size_t index = 0; index < Actual_building->contained_items.size(); index++) {
                            if (Actual_building->contained_items[index]->use_mode != 2)
                                break;
                            worn_item item_matt;

                            df::item * item = Actual_building->contained_items[index]->item;

                            if (b->building.type == df::enums::building_type::FarmPlot) {
                                if (item->pos.x == int32_t(xx) && item->pos.y == int32_t(yy) && item->pos.z == int32_t(zz)) {
                                    if (item->getType() == df::enums::item_type::SEEDS) {
                                        b->building.special = 1;
                                    }
                                    else if (item->getType() == df::enums::item_type::PLANT) {
                                        b->building.special = 2;
                                    }
                                }
                                else continue;
                            }

                            item_matt.matt.type = item->getActualMaterial();
                            item_matt.matt.index = item->getActualMaterialIndex();

                            if (item->isDyed()) {
                                auto mat = getDyeMaterialFromItem(item);
                                if (mat)
                                {
                                    item_matt.dyematt = *mat;
                                }
                            }

                            b->building.constructed_mats.push_back(item_matt);
                        }
                    }
                }
            }
        }
    }
}


void loadBuildingSprites ( Tile* b)
{
    bool foundTileBuildingInfo = false;
    if (b == NULL) {
        LogError("Null Tile skipped in loadBuildingSprites\n");
        return;
    }
    auto& contentLoader = stonesenseState.contentLoader;
    BuildingConfiguration* generic = NULL, *specific = NULL, *custom = NULL;
    for(auto iter = stonesenseState.contentLoader->buildingConfigs.begin(); iter < contentLoader->buildingConfigs.end(); iter++) {
        BuildingConfiguration & conf = **iter;
        if(b->building.type == conf.game_type) {
            generic = &conf;
            if(b->building.info && b->building.info->subtype == conf.game_subtype) {
                specific = &conf;
                if(b->building.info->custom_type == conf.game_custom) {
                    custom = &conf;
                }
            }
        }
    }
    BuildingConfiguration * final = custom?custom:(specific?specific:(generic?generic:NULL));
    //check all sprites for one that matches all conditions
    if (final && final->sprites != NULL && final->sprites->copyToTile(b)) {
        foundTileBuildingInfo = true;
    }
    //add yellow box, if needed. But only if the building was not found (this way we can have blank slots in buildings)
    if(b->building.sprites.size() == 0 && foundTileBuildingInfo == false) {
        c_sprite unknownBuildingSprite;
        unknownBuildingSprite.reset();
        b->building.sprites.push_back( unknownBuildingSprite );
    }
}
