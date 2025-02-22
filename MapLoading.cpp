#include <cmath>

#include "common.h"
#include "MapLoading.h"
#include "GUI.h"
#include "TrackingModes.h"
#include "SegmentProcessing.h"
#include "WorldSegment.h"
#include "SpriteMaps.h"
#include "Constructions.h"
#include "GameBuildings.h"
#include "Creatures.h"
#include "ContentLoader.h"
#include "Config.h"
#include "StonesenseState.h"

#include "df/block_square_event_grassst.h"
#include "df/block_square_event_material_spatterst.h"
#include "df/block_square_event_mineralst.h"
#include "df/construction.h"
#include "df/engraving.h"
#include "df/flow_info.h"
#include "df/inorganic_raw.h"
#include "df/item_constructed.h"
#include "df/item_threadst.h"
#include "df/itemimprovement.h"
#include "df/itemimprovement_threadst.h"
#include "df/map_block.h"
#include "df/map_block_column.h"
#include "df/plant.h"
#include "df/plant_root_tile.h"
#include "df/plant_tree_info.h"
#include "df/plant_tree_tile.h"

#include "allegro5/color.h"

bool connected = 0;
bool threadrunnng = 0;

using std::vector;

//==============================Map Read ==============================//
/*
 * This is the first-stage map reading section.  This deals with reading
 * from the DF map, and storing what is needed for us to draw things.
 */

/**
 * Reads the spatter types and colors from the DF vector 'splatter' at local
 *  position 'lx','ly' into the stonesense Tile 'b'.
 */
void readSpatterToTile(Tile * b, uint32_t lx, uint32_t ly,
    const vector <df::block_square_event_material_spatterst * > & splatter)
{
    auto& ssConfig = stonesenseState.ssConfig;

    b->mudlevel = 0;
    b->snowlevel = 0;
    b->bloodlevel = 0;
    if(ssConfig.bloodcutoff < UCHAR_MAX) {
        long red=0;
        long green=0;
        long blue=0;
        long blood=0;
        long snow=0;
        for(size_t i = 0; i < splatter.size(); i++) {
            if(!splatter[i]->amount[lx][ly]) {
                continue;
            }
            uint8_t level = (uint8_t)splatter[i]->amount[lx][ly];
            ALLEGRO_COLOR tempBlood = al_map_rgb(255,255,255);

            //try to get the blood/snow tint
            DFHack::MaterialInfo mat;
            switch(splatter[i]->mat_type){
            case MUD:
                break;
            case ICE:
                //ice and snow are white (default), water is blue
                if(splatter[i]->mat_state != df::matter_state::Powder
                    && splatter[i]->mat_state != df::matter_state::Solid ) {
                        tempBlood = lookupMaterialColor(splatter[i]->mat_type, -1);
                }
                break;
            case VOMIT:
                tempBlood = lookupMaterialColor(splatter[i]->mat_type, -1);
                break;
            default:
                tempBlood = lookupMaterialColor(splatter[i]->mat_type, splatter[i]->mat_index, al_map_rgb(128,128,128));
            }

            if(splatter[i]->mat_type != MUD) {
                red += (tempBlood.r * 255 * level);
                green += (tempBlood.g * 255 * level);
                blue += (tempBlood.b * 255 * level);
            }

            //use the state information to determine if we should be incrementing the snow or the blood
            auto state = df::matter_state(splatter[i]->mat_state);
            state = splatter[i]->mat_type == VOMIT ? df::matter_state::Paste : state; //change vomit from dust to paste - gross
            switch(state) {
            case df::matter_state::Powder:
            case df::matter_state::Solid:
                snow += level;
                break;
            default:
                if (splatter[i]->mat_type == MUD)
                    b->mudlevel = level;
                else
                    blood += level;
            }
        }
        blood = blood<0 ? 0-blood : blood;
        snow = snow<0 ? 0-snow : snow;
        int total = blood + snow;
        if(blood || snow) {
            if(snow > blood) {
                b->bloodcolor = al_map_rgb(red/total, green/total, blue/total);
            } else {
                b->bloodcolor = al_map_rgba(red/total, green/total, blue/total,
                    (total > ssConfig.bloodcutoff) ? 255 : total*255/ssConfig.bloodcutoff);
            }
            b->snowlevel = snow>UCHAR_MAX ? UCHAR_MAX : snow;
            b->bloodlevel = blood>UCHAR_MAX ? UCHAR_MAX : blood;
        } else {
            b->bloodcolor = al_map_rgba(0,0,0,0);
        }
    } else {
        b->bloodcolor = al_map_rgb(150, 0, 24);
    }
}

/**
 * Converts the tile to a type that indicates what designations are specified.
 * Also converts the material to the "DESIGNATION" material type when appropriate.
 */
bool readDesignationsToTile( Tile * b,
                             df::tile_designation des,
                             df::tile_occupancy occ)
{
    using df::game_mode;
    using df::tile_dig_designation;

    if (!df::global::gamemode || *df::global::gamemode == game_mode::ADVENTURE)
        return false; //Adventure mode doesn't use Dwarf mode designations.
    df::tiletype_shape shape = df::tiletype_shape::WALL;
    df::tiletype_material mat = df::tiletype_material::STONE;
    df::tiletype_variant var = df::tiletype_variant::NONE;
    df::tiletype_special spc = df::tiletype_special::NONE;
    DFHack::TileDirection dir;

    if(des.bits.dig != tile_dig_designation::No)
    {
        switch(des.bits.dig)
        {
        case tile_dig_designation::Default:
            if( b->tileShapeBasic() == df::tiletype_shape_basic::Ramp ){
                shape = df::tiletype_shape::FLOOR;
            } else {
                shape = df::tiletype_shape::WALL;
            }
            break;
        case tile_dig_designation::UpDownStair:
            shape = df::tiletype_shape::STAIR_UPDOWN;
            break;
        case tile_dig_designation::Channel:
            shape = df::tiletype_shape::RAMP_TOP;
            break;
        case tile_dig_designation::Ramp:
            shape = df::tiletype_shape::RAMP;
            break;
        case tile_dig_designation::DownStair:
            shape = df::tiletype_shape::STAIR_DOWN;
            break;
        case tile_dig_designation::UpStair:
            shape = df::tiletype_shape::STAIR_UP;
            break;
        default:
            //do nothing
            break;
        }

        b->material.type = DESIGNATION;
        b->material.index = INVALID_INDEX;
        b->tileType = findTileType(shape, mat, var, spc, dir);
        return true;
    }

    //if there is no dig designation, check for smoothing designations
    if(des.bits.smooth != 0)
    {
        if( b->tileShapeBasic() == df::tiletype_shape_basic::Floor ){
            shape = df::tiletype_shape::FLOOR;
        } else {
            shape = df::tiletype_shape::WALL;
        }
        spc = df::tiletype_special::SMOOTH;
    if(des.bits.smooth == 2) {
        // if the tile is being engraved, then fake the engraving flags
        b->engraving_flags.bits.east = 1;
        b->engraving_flags.bits.west = 1;
        b->engraving_flags.bits.north = 1;
        b->engraving_flags.bits.south = 1;
        b->engraving_flags.bits.floor = 1;
        b->engraving_character = '*';
    }
        b->material.type = DESIGNATION;
        b->material.index = INVALID_INDEX;
        b->tileType = findTileType(shape, mat, var, spc, dir);
        return true;
    }

    bool hasTracks = false;
    if(occ.bits.carve_track_north){
        dir.north = 1;
        hasTracks = true;
    } if(occ.bits.carve_track_south){
        dir.south = 1;
        hasTracks = true;
    } if(occ.bits.carve_track_east){
        dir.east = 1;
        hasTracks = true;
    } if(occ.bits.carve_track_west){
        dir.west = 1;
        hasTracks = true;
    }
    if(hasTracks)
    {
        if( b->tileShapeBasic() == df::tiletype_shape_basic::Ramp ){
            shape = df::tiletype_shape::RAMP;
        } else {
            shape = df::tiletype_shape::FLOOR;
        }

        b->material.type = DESIGNATION;
        b->material.index = INVALID_INDEX;
        b->tileType = findTileType(shape, mat, var, spc, dir);
        return true;
    }

    return false;
}

/**
 * Identifies the correct material from the DF vectors, and stores it in the
 * stonesense tile.
 */
//TODO get cavein-sand to work somehow?
void readMaterialToTile(Tile* b, uint32_t lx, uint32_t ly,
    df::map_block* trueBlock,
    const DFHack::t_feature& local,
    const DFHack::t_feature& global,
    const vector <df::block_square_event_mineralst * > & veins,
    vector< vector <int16_t> >* allLayers)
{
    //determine rock/soil type
    int rockIndex = -1;

    //first lookup the default geolayer for the location
    uint32_t tileBiomeIndex = trueBlock->designation[lx][ly].bits.biome;
    uint8_t tileRegionIndex = trueBlock->region_offset[tileBiomeIndex];
    uint32_t tileGeolayerIndex = trueBlock->designation[lx][ly].bits.geolayer_index;
    if(tileRegionIndex < (*allLayers).size()) {
        if(tileGeolayerIndex < (*allLayers).at(tileRegionIndex).size()) {
            rockIndex = (*allLayers).at(tileRegionIndex).at(tileGeolayerIndex);
        }
    }


    bool soilTile = false;//is this tile a match for soil materials?
    bool soilMat = false;//is the material a soil?
    using df::tiletype_material;
    using df::material_flags;
    using df::inorganic_flags;

    soilTile = b->tileMaterial() == tiletype_material::SOIL
        || (b->mudlevel == 0
        && (b->tileMaterial() == tiletype_material::PLANT
        || b->tileMaterial() == tiletype_material::GRASS_LIGHT
        || b->tileMaterial() == tiletype_material::GRASS_DARK
        || b->tileMaterial() == tiletype_material::GRASS_DRY
        || b->tileMaterial() == tiletype_material::GRASS_DEAD));
    if(b->tileMaterial() == tiletype_material::STONE || soilTile) {

        df::inorganic_raw * rawMat = df::inorganic_raw::find(rockIndex);
        if(rawMat) {
            soilMat = rawMat->flags.is_set(inorganic_flags::SOIL_ANY);
            //if the tile is a stone tile but we got a soil material, we need to "dig down" to find it
            while(!soilTile && soilMat) {
                tileGeolayerIndex++;
                if(tileGeolayerIndex < (*allLayers).at(tileRegionIndex).size()) {
                    rockIndex = (*allLayers).at(tileRegionIndex).at(tileGeolayerIndex);
                    rawMat = df::inorganic_raw::find(rockIndex);
                    if(rawMat) {
                        soilMat = rawMat->flags.is_set(inorganic_flags::SOIL_ANY);
                    } else {
                        rockIndex = -1;
                        break;
                    }
                } else {
                    rockIndex = -1;
                    break;
                }
            }
            //if the tile is a soil tile but we got a stone material, we need to "dig up" to find it
            while(soilTile && !soilMat) {
                if(tileGeolayerIndex == 0) {
                    rockIndex = -1;
                    break;
                }
                tileGeolayerIndex--;
                rockIndex = (*allLayers).at(tileRegionIndex).at(tileGeolayerIndex);
                rawMat = df::inorganic_raw::find(rockIndex);
                if(rawMat) {
                    soilMat = rawMat->flags.is_set(inorganic_flags::SOIL_ANY);

                } else {
                    rockIndex = -1;
                    break;
                }
            }
        } else {
            rockIndex = -1;
        }
    }

    b->layerMaterial.type = INORGANIC;
    b->layerMaterial.index = rockIndex;

    //check veins (defaults to layer material)
    b->veinMaterial.type = INORGANIC;
    b->veinMaterial.index = rockIndex;
    for(uint32_t i=0; i<(uint32_t)veins.size(); i++) {
        uint16_t row = veins[i]->tile_bitmask[ly];
        bool set = (row & (1 << lx)) != 0;
        if(set) {
            rockIndex = veins[i]->inorganic_mat;
            b->veinMaterial.type = INORGANIC;
            b->veinMaterial.index = veins[i]->inorganic_mat;
            b->hasVein = 1;
        } else {
            b->veinMaterial.type = INORGANIC;
            b->veinMaterial.index = rockIndex;
        }
    }

    b->material.type = INORGANIC;
    if(soilTile) {
        b->material.index = b->layerMaterial.index;
    } else {
        b->material.index = b->veinMaterial.index;
    }

    //read global/local features
    int16_t idx = trueBlock->global_feature;
    if( idx != -1 && global.type != -1 && global.main_material != -1) {
        if(trueBlock->designation[lx][ly].bits.feature_global) {
            b->layerMaterial.type = global.main_material;
            b->layerMaterial.index = global.sub_material;
            b->material.type = global.main_material;
            b->material.index = global.sub_material;
            b->hasVein = 0;
        }
    }

    //read local features
    idx = trueBlock->local_feature;
    if( idx != -1 && local.type != -1 && local.main_material != -1 ) {
        if(trueBlock->designation[lx][ly].bits.feature_local) {
            b->veinMaterial.type = local.main_material;
            b->veinMaterial.index = local.sub_material;
            b->material.type = local.main_material;
            b->material.index = local.sub_material;
            b->hasVein = 1;
        }
    }

    if(b->tileMaterial() == tiletype_material::LAVA_STONE) {
        b->material.type = INORGANIC;
        b->material.index = stonesenseState.contentLoader->obsidian;
    }
}

std::optional<DFHack::t_matglossPair> getDyeMaterialFromItem(df::item* item)
{
    auto Constructed_Item = virtual_cast<df::item_constructed>(item);
    if (Constructed_Item) {
        using df::improvement_type;
        auto& imp = Constructed_Item->improvements;
        auto it = std::find_if(imp.begin(), imp.end(),
            [&](auto i) { return i && i->getType() == improvement_type::THREAD && virtual_cast<df::itemimprovement_threadst>(i) != nullptr; });
        if (it != imp.end())
        {
            auto impthr = virtual_cast<df::itemimprovement_threadst>(*it);
            return std::optional<DFHack::t_matglossPair>({ .type = impthr->mat_type, .index = impthr->mat_index });
        }
    }
    return std::nullopt;
}

SS_Item ConvertItem(df::item * found_item, WorldSegment& segment){
    SS_Item Tempitem{};
    Tempitem.item = { .type = found_item->getType(), .index = found_item->getSubtype() };
    Tempitem.matt = { .type = found_item->getActualMaterial(), .index = found_item->getActualMaterialIndex() };
    Tempitem.dyematt = { .type = -1, .index = -1 };

    if (1) { //found_item->isDyed())
        using df::item_type;
        auto dyemat = getDyeMaterialFromItem(found_item);
        if (dyemat)
        {
            Tempitem.dyematt = *dyemat;
        }
        else if (found_item->getType() == item_type::THREAD)
        {
            auto Thread_Item = virtual_cast<df::item_threadst>(found_item);
            if (Thread_Item && Thread_Item->dye_mat_type >= 0)
            {
                Tempitem.dyematt = { .type = Thread_Item->dye_mat_type, .index = Thread_Item->dye_mat_index };
            }
        }
    }
    return Tempitem;
}


/**
* reads one 16x16 map block into stonesense tiles
* attempts to only read as much information as is necessary to do the tile optimization
*/
void readBlockToSegment(DFHack::Core& DF, WorldSegment& segment,
    int BlockX, int BlockY, int BlockZ,
    uint32_t BoundrySX, uint32_t BoundrySY,
    uint32_t BoundryEX, uint32_t BoundryEY,
    vector< vector <int16_t> >* allLayers)
{
    if(stonesenseState.ssConfig.skipMaps) {
        return;
    }
    //boundry check
    int blockDimX, blockDimY, blockDimZ;
    DFHack::Maps::getSize((unsigned int &)blockDimX, (unsigned int &)blockDimY, (unsigned int &)blockDimZ);
    if( BlockX < 0 || BlockX >= blockDimX ||
        BlockY < 0 || BlockY >= blockDimY ||
        BlockZ < 0 || BlockZ >= blockDimZ ) {
            return;
    }

    //make boundries local
    BoundrySX -= BlockX * BLOCKEDGESIZE;
    BoundryEX -= BlockX * BLOCKEDGESIZE;
    BoundrySY -= BlockY * BLOCKEDGESIZE;
    BoundryEY -= BlockY * BLOCKEDGESIZE;

    //read block data
    df::map_block *trueBlock;
    trueBlock = DFHack::Maps::getBlock(BlockX, BlockY, BlockZ);
    if(!trueBlock) {
        return;
    }
    //read the map features
    DFHack::t_feature local, global;
    DFHack::Maps::ReadFeatures(BlockX,BlockY,BlockZ,&local,&global);
    //read local vein data
    vector <df::block_square_event_mineralst * > veins;
    vector <df::block_square_event_frozen_liquidst * > ices;
    vector <df::block_square_event_material_spatterst * > splatter;
    vector <df::block_square_event_grassst * > grass;
    vector <df::block_square_event_world_constructionst * > worldconstructions;
    DFHack::Maps::SortBlockEvents(
        trueBlock,
        &veins,
        &ices,
        &splatter,
        &grass,
        &worldconstructions);
    //parse block

    auto& ssConfig = stonesenseState.ssConfig;

    for(uint32_t ly = BoundrySY; ly <= BoundryEY; ly++) {
        for(uint32_t lx = BoundrySX; lx <= BoundryEX; lx++) {
            uint32_t gx = lx + (BlockX * BLOCKEDGESIZE);
            uint32_t gy = ly + (BlockY * BLOCKEDGESIZE);
            if( !segment.CoordinateInsideSegment( gx, gy, BlockZ) ) {
                continue;
            }

            bool shouldBeIncluded = true;

            //open terrain needs to be included to make blackboxes if
            // we are shading but not showing hidden tiles
            using df::tiletype;
            using df::tiletype_shape;
            if(DFHack::isOpenTerrain(trueBlock->tiletype[lx][ly])
                && trueBlock->tiletype[lx][ly] != tiletype::RampTop
                && DFHack::tileShape(trueBlock->tiletype[lx][ly]) != tiletype_shape::TWIG) {
                    if(!ssConfig.show_hidden_tiles
                        && ssConfig.shade_hidden_tiles
                        && trueBlock->designation[lx][ly].bits.hidden) {
                            shouldBeIncluded = true;
                    } else {
                        shouldBeIncluded = false;
                    }
                    //all other terrain needs including, except for hidden tiles
                    // when we are neither showing nor shading hidden tiles
            } else if(!ssConfig.show_hidden_tiles
                && !ssConfig.shade_hidden_tiles
                && trueBlock->designation[lx][ly].bits.hidden) {
                    shouldBeIncluded = false;
            }

            //add back in any liquid tiles, in case they can be seen from above
            // as well as any hanging buildings
            if(trueBlock->designation[lx][ly].bits.flow_size
                || trueBlock->occupancy[lx][ly].bits.building) {
                    shouldBeIncluded = true;
            }

            //add back in any tiles that are constructions or designations
            if( ssConfig.show_designations
                && containsDesignations(
                    trueBlock->designation[lx][ly],
                    trueBlock->occupancy[lx][ly] ) ) {
                shouldBeIncluded = true;
            }

            if(!shouldBeIncluded){
                continue;
            }

            Tile * b = segment.ResetTile(gx, gy, BlockZ, trueBlock->tiletype[lx][ly]);

            b->occ.bits.unit = false;//this will be set manually when we read the creatures vector
            b->occ = trueBlock->occupancy[lx][ly];
            b->designation = trueBlock->designation[lx][ly];

            //if the tile has designations, read them and nothing else
            if( ssConfig.show_designations
                && readDesignationsToTile(
                    b, trueBlock->designation[lx][ly],
                    trueBlock->occupancy[lx][ly] ) ) {
                        continue;
            }

            //set whether the tile is hidden
            b->fog_of_war = !b->designation.bits.pile;

            //don't read detailed information for blackbox tiles
            if(!ssConfig.show_hidden_tiles
                && ssConfig.shade_hidden_tiles
                && b->designation.bits.hidden
                && !b->designation.bits.flow_size) {
                    continue;
            }

            //read the grasses
            b->grasslevel = 0;
            b->grassmat = -1;
            //b->grasslevels.clear();
            //b->grassmats.clear();
            for(size_t i = 0; i < grass.size(); i++) {
                if(grass[i]->amount[lx][ly] > 0 && b->grasslevel == 0) { //b->grasslevel)
                    b->grasslevel = grass[i]->amount[lx][ly];
                    b->grassmat = grass[i]->plant_index;
                    //b->grasslevels.push_back(grass[i].intensity[lx][ly]);
                    //b->grassmats.push_back(grass[i].material);
                }
            }

            //read the water flows and direction.
            b->flow_direction = trueBlock->liquid_flow[lx][ly].bits.perm_flow_dir;

            //read the tile spatter
            readSpatterToTile(b, lx, ly, splatter);

            //read the tile material
            readMaterialToTile(b, lx, ly, trueBlock, local, global, veins, allLayers);
        }
    }

    ////add trees and other vegetation
    //for(auto iter = trueBlock->plants.begin(); iter != trueBlock->plants.end(); iter++) {
    //    df::plant * wheat = *iter;
    //    assert(wheat != NULL);
    //    Tile* b = segment.getTile( wheat->pos.x, wheat->pos.y, wheat->pos.z);
    //    if(!b) {
    //        b = segment.ResetTile(wheat->pos.x, wheat->pos.y, wheat->pos.z, tiletype::OpenSpace);
    //        if(!b) {
    //            continue;
    //        }
    //    }
    //    if( b->tileShape() == tiletype_shape::TREE ||
    //        b->tileShape() == tiletype_shape::SAPLING ||
    //        b->tileShape() == tiletype_shape::SHRUB) {
    //            b->tree.type = wheat->flags.whole;
    //            b->tree.index = wheat->material;
    //    }
    //}

    //add items
    using df::tiletype;
    for(auto iter = trueBlock->items.begin(); iter != trueBlock->items.end(); iter++) {
        int32_t item_index = *iter;
        df::item * found_item = df::item::find(item_index);
        if(!found_item) {
            continue;
        }
        Tile* b = segment.getTile( found_item->pos.x, found_item->pos.y, found_item->pos.z);
        if(!b) {
            b = segment.ResetTile(found_item->pos.x, found_item->pos.y, found_item->pos.z, tiletype::OpenSpace);
            if(!b) {
                continue;
            }
        }
        b->Item = ConvertItem(found_item, segment);
    }


    //add effects
    for(auto iter = trueBlock->flows.begin(); iter != trueBlock->flows.end(); iter++) {
        df::flow_info * eff = *iter;
        if(eff == NULL || eff->density <= 0) {
            continue;
        }
        Tile* b = segment.getTile( eff->pos.x, eff->pos.y, eff->pos.z);
        if(segment.CoordinateInsideSegment(eff->pos.x, eff->pos.y, eff->pos.z)) {
            if(!b) {
                b = segment.ResetTile(eff->pos.x, eff->pos.y, eff->pos.z, tiletype::OpenSpace);
                if(!b) {
                    continue;
                }
            }
            if(eff->density > b->tileeffect.density
                || b->tileeffect.type == (df::flow_type) INVALID_INDEX) {
                    b->tileeffect.type = eff->type;
                    b->tileeffect.density = eff->density;
                    b->tileeffect.matt.index = eff->mat_index;
                    b->tileeffect.matt.type = eff->mat_type;
            }
        }
    }
}

namespace
{
    void parse_tree(WorldSegment& segment, auto pp, auto info, auto parts, auto depth, bool downward, auto raw)
    {
        for (int zz = 0; zz < depth; zz++)
        {
            // Parse through a single horizontal slice of the tree.
            for (int xx = 0; xx < info->dim_x; xx++)
            {
                for (int yy = 0; yy < info->dim_y; yy++)
                {
                    // Any non-zero value here other than blocked means there's some sort of branch here.
                    // If the block is at or above the plant's base level, we use the body array
                    // otherwise we use the roots.
                    // TODO: verify that the tree bounds intersect the block.
                    auto part = parts[zz][xx + (yy * info->dim_x)];
                    if (part.whole && !(part.bits.blocked))
                    {
                        df::coord pos = pp->pos;
                        pos.x = pos.x - (info->dim_x / 2) + xx;
                        pos.y = pos.y - (info->dim_y / 2) + yy;
                        pos.z = pos.z + downward ? zz : (-1 - zz);
                        if (!segment.CoordinateInsideSegment(pos.x, pos.y, pos.z))
                            continue;
                        Tile* t = segment.getTile(pos.x, pos.y, pos.z);
                        if (!t)
                            t = segment.ResetTile(pos.x, pos.y, pos.z);
                        if (!t)
                            continue;
                        t->tree.type = pp->type;
                        t->tree.index = pp->material;
                        // only update tree_tile if part is a plant_tree_tile
                        if constexpr (std::assignable_from<decltype(t->tree_tile), decltype(part)>)
                        {
                            t->tree_tile = part;
                            if (raw)
                            {
                                t->material.type = raw->material_defs.type[df::plant_material_def::basic_mat];
                                t->material.index = raw->material_defs.idx[df::plant_material_def::basic_mat];
                            }
                        }
                    }
                }
            }
        }
    }
}

void readBlockColumnToSegment(DFHack::Core& DF, WorldSegment& segment,
    int BlockX, int BlockY)
{
    if (stonesenseState.ssConfig.skipMaps) {
        return;
    }
    //boundry check
    int blockDimX, blockDimY, blockDimZ;
    DFHack::Maps::getSize((unsigned int &)blockDimX, (unsigned int &)blockDimY, (unsigned int &)blockDimZ);
    if (BlockX < 0 || BlockX >= blockDimX ||
        BlockY < 0 || BlockY >= blockDimY) {
        return;
    }

    //read block data
    df::map_block_column *trueColumn;
    trueColumn = DFHack::Maps::getBlockColumn(BlockX, BlockY);
    if (!trueColumn) {
        return;
    }

    for (size_t i = 0; i < trueColumn->plants.size(); i++)
    {
        df::plant * pp = trueColumn->plants[i];
        // A plant without tree_info is single tile
        if (!pp->tree_info)
        {
            if (!segment.CoordinateInsideSegment(pp->pos.x, pp->pos.y, pp->pos.z))
                continue;
            Tile * t = segment.getTile(pp->pos.x, pp->pos.y, pp->pos.z);
            if (!t)
                t = segment.ResetTile(pp->pos.x, pp->pos.y, pp->pos.z);
            if (!t)
                continue;
            t->tree.type = pp->type;
            t->tree.index = pp->material;
            continue;
        }

        // tree_info contains vertical slices of the tree. This ensures there's a slice for our Z-level.
        df::plant_tree_info * info = pp->tree_info;
        if (!segment.RangeInsideSegment(
            pp->pos.x - (pp->tree_info->dim_x / 2),
            pp->pos.y - (pp->tree_info->dim_y / 2),
            pp->pos.z - (pp->tree_info->roots_depth),
            pp->pos.x + (pp->tree_info->dim_x / 2),
            pp->pos.y + (pp->tree_info->dim_y / 2),
            pp->pos.z + pp->tree_info->body_height - 1))
            continue;

        auto raw = df::plant_raw::find(pp->material);

        parse_tree(segment, pp, info, info->body, info->body_height, false, raw);
        parse_tree(segment, pp, info, info->roots, info->roots_depth, true, raw);
    }
}


void readMapSegment(WorldSegment* segment, GameState inState)
{
    uint32_t index;
    DFHack::Core & DF = DFHack::Core::getInstance();
    clock_t starttime = clock();

    auto& ssState = stonesenseState.ssState;
    //read date
    if(!stonesenseState.ssConfig.skipWorld) {
        auto& contentLoader = stonesenseState.contentLoader;
        contentLoader->currentYear = DFHack::World::ReadCurrentYear();
        contentLoader->currentTick = DFHack::World::ReadCurrentTick();
        contentLoader->currentMonth = (contentLoader->currentTick+9)/33600;
        contentLoader->currentDay = ((contentLoader->currentTick+9)%33600)/1200;
        contentLoader->currentHour = ((contentLoader->currentTick+9)-(((contentLoader->currentMonth*28)+contentLoader->currentDay)*1200))/50;
        contentLoader->currentTickRel = (contentLoader->currentTick+9)-(((((contentLoader->currentMonth*28)+contentLoader->currentDay)*24)+contentLoader->currentHour)*50);
        DFHack::World::ReadGameMode(contentLoader->gameMode);
    }

    if(stonesenseState.ssConfig.skipMaps || !DFHack::Maps::IsValid()) {
        segment->Reset(inState,true);
        return;
    }

    //Read Number of blocks
    uint32_t blockDimX, blockDimY, blockDimZ;
    DFHack::Maps::getSize(blockDimX, blockDimY, blockDimZ);
    //Read position of blocks
    uint32_t regionX, regionY, regionZ;
    DFHack::Maps::getSize(regionX, regionY, regionZ);
    //Store these
    blockDimX *= BLOCKEDGESIZE;
    blockDimY *= BLOCKEDGESIZE;
    ssState.RegionDim.x = blockDimX;
    ssState.RegionDim.y = blockDimY;
    ssState.RegionDim.z = blockDimZ;

    //setup new world segment
    segment->Reset(inState,false);

    //read world wide buildings
    vector<Stonesense_Building> allBuildings;
    if(!stonesenseState.ssConfig.skipBuildings) {
        ReadBuildings(DF, &allBuildings);
    }

    /*if(GroundMaterialNamesTranslatedFromGame == false)
    TranslateGroundMaterialNames();*/

    // read constructions
    vector<df::construction> allConstructions;
    uint32_t numconstructions = 0;

    if(!stonesenseState.ssConfig.skipConstructions) {
        numconstructions = df::global::world->event.constructions.size();
        if (numconstructions) {
            df::construction tempcon;
            index = 0;
            while(index < numconstructions) {
                tempcon = *df::global::world->event.constructions[index];
                if(segment->CoordinateInsideSegment(tempcon.pos.x, tempcon.pos.y, tempcon.pos.z)) {
                    allConstructions.push_back(tempcon);
                }
                index++;
            }
        }
    }

    if(segment->segState.Rotation % 2) {
        int temp = inState.Size.x;
        inState.Size.x = inState.Size.y;
        inState.Size.y = temp;
    }

    //figure out what blocks to read
    int32_t firstTileToReadX = inState.Position.x;
    if (firstTileToReadX < 0) {
        firstTileToReadX = 0;
    }

    // get region geology
    vector< vector <int16_t> > layers;
    vector<df::coord2d> geoidx;
    if (!DFHack::Maps::ReadGeology(&layers, &geoidx)) {
        LogError("Can't get region geology.\n");
    }

    while (firstTileToReadX < inState.Position.x + inState.Size.x) {
        int blockx = firstTileToReadX / BLOCKEDGESIZE;
        int32_t lastTileInBlockX = (blockx + 1) * BLOCKEDGESIZE - 1;
        int32_t lastTileToReadX = std::min<int32_t>(lastTileInBlockX, inState.Position.x + inState.Size.x - 1);

        int32_t firstTileToReadY = inState.Position.y;
        if (firstTileToReadY < 0) {
            firstTileToReadY = 0;
        }

        while (firstTileToReadY < inState.Position.y + inState.Size.y) {
            int blocky = firstTileToReadY / BLOCKEDGESIZE;
            int32_t lastTileInBlockY = (blocky + 1) * BLOCKEDGESIZE - 1;
            int32_t lastTileToReadY = std::min<uint32_t>(lastTileInBlockY, inState.Position.y + inState.Size.y - 1);

            for (int lz = inState.Position.z - inState.Size.z; lz <= inState.Position.z; lz++) {
                //load the tiles from this block to the map segment
                readBlockToSegment(DF, *segment, blockx, blocky, lz,
                    firstTileToReadX, firstTileToReadY,
                    lastTileToReadX, lastTileToReadY, &layers);

            }
            firstTileToReadY = lastTileToReadY + 1;
        }
        firstTileToReadX = lastTileToReadX + 1;
    }
    //figure out what blocks to read
    firstTileToReadX = inState.Position.x;
    if (firstTileToReadX < 0) {
        firstTileToReadX = 0;
    }
    while (firstTileToReadX < inState.Position.x + inState.Size.x) {
        int blockx = (firstTileToReadX / (BLOCKEDGESIZE * 3)) * 3;
        int32_t lastTileInBlockX = ((blockx / 3) + 1) * (BLOCKEDGESIZE * 3) - 1;
        int32_t lastTileToReadX = std::min<int32_t>(lastTileInBlockX, inState.Position.x + inState.Size.x - 1);

        int32_t firstTileToReadY = inState.Position.y;
        if (firstTileToReadY < 0) {
            firstTileToReadY = 0;
        }

        while (firstTileToReadY < inState.Position.y + inState.Size.y) {
            int blocky = (firstTileToReadY / (BLOCKEDGESIZE * 3)) * 3;
            int32_t lastTileInBlockY = ((blocky / 3) + 1) * (BLOCKEDGESIZE * 3) - 1;
            int32_t lastTileToReadY = std::min<uint32_t>(lastTileInBlockY, inState.Position.y + inState.Size.y - 1);

            //read the plants
            if (blockx % 3 == 0 && blocky % 3 == 0)
                readBlockColumnToSegment(DF, *segment, blockx, blocky);

            firstTileToReadY = lastTileToReadY + 1;
        }
        firstTileToReadX = lastTileToReadX + 1;
    }

    //merge buildings with segment
    if(!stonesenseState.ssConfig.skipBuildings) {
        MergeBuildingsToSegment(&allBuildings, segment);
    }

    //translate constructions
    changeConstructionMaterials(segment, &allConstructions);

    uint32_t numengravings = df::global::world->event.engravings.size();
    df::engraving * engraved;
    index = 0;
    Tile * b = 0;
    while(index < numengravings) {
        engraved = df::global::world->event.engravings[index];
        df::coord pos = engraved->pos;
        if(segment->CoordinateInsideSegment(pos.x, pos.y, pos.z)) {
            b = segment->getTile(pos.x, pos.y, pos.z);
            if(!b) {
                continue;
            }
            b->engraving_character = engraved->tile;
            b->engraving_flags = engraved->flags;
            b->engraving_quality = engraved->quality;
        }
        index++;
    }

    //Read Creatures
    if(!stonesenseState.ssConfig.skipCreatures) {
        ReadCreaturesToSegment( DF, segment );
    }

    segment->loaded = 1;
    segment->processed = 0;
    stonesenseState.stoneSenseTimers.read_time.update(clock() - starttime);
}

//==============================Map Read Main===========================//
/*
 * Here is where the main hub functions dispatch the read thread from,
 *  as well as the read thread's entry point.
 */

void read_segment( void *arg)
{
    if(!DFHack::Maps::IsValid()) {
        return;
    }
    static bool firstLoad = 1;
    stonesenseState.ssConfig.threadstarted = 1;
    WorldSegment* segment = NULL;
    {
        DFHack::CoreSuspender suspend;

        auto& ssState = stonesenseState.ssState;
        //read cursor
        if (stonesenseState.ssConfig.config.follow_DFcursor) {
            ssState.dfCursor = DFHack::Gui::getCursorPos();
            ssState.dfSelection = OptCrd3D { df::global::selection_rect->start_x, df::global::selection_rect->start_y, df::global::selection_rect->start_z };
        }

        if (firstLoad || stonesenseState.ssConfig.config.track_mode != Config::TRACKING_NONE) {
            firstLoad = 0;
            if (stonesenseState.ssConfig.config.track_mode == Config::TRACKING_CENTER) {
                followCurrentDFCenter();
            }
            stonesenseState.ssConfig.zoom = (df::global::gps->viewport_zoom_factor - 64) / 16;
            stonesenseState.ssConfig.recalculateScale();
        }
        segment = stonesenseState.map_segment.getRead();
        readMapSegment(segment, ssState);
        stonesenseState.ssConfig.threadstarted = 0;
    }

    if(segment) {
        beautifySegment(segment);

        //putting these here to increase responsiveness of the UI and to make megashots work
        //segment->segState.Position = ssState.Position;
        //segment->segState.dfCursor = ssState.dfCursor;

        segment->AssembleAllTiles();

        //only need to lock the drawing segment because the reading segment is already locked
        stonesenseState.map_segment.lockDraw();
        stonesenseState.map_segment.swap();
        stonesenseState.map_segment.unlockDraw();
    }
}

static void * threadedSegment(ALLEGRO_THREAD *read_thread, void *arg)
{
    while(!al_get_thread_should_stop(read_thread)) {
        stonesenseState.map_segment.lockRead();
        read_segment(arg);
        stonesenseState.map_segment.unlockRead();
        al_rest(stonesenseState.ssConfig.config.automatic_reload_time/1000.0);
    }
    return 0;
}

void reloadPosition()
{
    if (stonesenseState.timeToReloadConfig) {
        stonesenseState.contentLoader->Load();
        stonesenseState.timeToReloadConfig = false;
    }

    //load segment
    if(stonesenseState.ssConfig.threading_enable) {
        if(!stonesenseState.ssConfig.threadmade) {
            stonesenseState.ssConfig.readThread = al_create_thread(threadedSegment, NULL);
            stonesenseState.ssConfig.threadmade = 1;
        }
    }

    if(stonesenseState.ssConfig.threading_enable) {
        al_start_thread(stonesenseState.ssConfig.readThread);
    } else {
        read_segment(NULL);
    }
}
