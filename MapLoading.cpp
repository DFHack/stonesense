#include "common.h"
#include "GUI.h"
#include "MapLoading.h"
#include "SegmentProcessing.h"
#include "WorldSegment.h"
#include "SpriteMaps.h"
#include "Constructions.h"
#include "GameBuildings.h"
#include "Creatures.h"
#include "ContentLoader.h"
#include <df/flow_info.h>
#include <df/plant.h>
#include <df/flow_info.h>
#include "df/item_constructed.h"
#include "df/itemimprovement.h"
#include "df/itemimprovement_threadst.h"
#include "df/item_threadst.h"

bool connected = 0;
bool threadrunnng = 0;
segParams parms;

//==============================Map Read ==============================//
/*
 * This is the first-stage map reading section.  This deals with reading 
 * from the DF map, and storing what is needed for us to draw things.  
 */

/**
 * reads one 16x16 map block into stonesense tiles
 * attempts to only read as much information as is necessary to do the tile optimization
 */
void ReadBlockToSegment(DFHack::Core& DF, WorldSegment& segment, 
    int BlockX, int BlockY, int BlockZ,
    uint32_t BoundrySX, uint32_t BoundrySY,
    uint32_t BoundryEX, uint32_t BoundryEY,
    vector< vector <int16_t> >* allLayers)
{
    if(ssConfig.skipMaps) {
        return;
    }
    //boundry check
    int blockDimX, blockDimY, blockDimZ;
    Maps::getSize((unsigned int &)blockDimX, (unsigned int &)blockDimY, (unsigned int &)blockDimZ);
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
    trueBlock = Maps::getBlock(BlockX, BlockY, BlockZ);
    if(!trueBlock) {
        return;
    }        
    //read the map features
    t_feature local, global;
    Maps::ReadFeatures(BlockX,BlockY,BlockZ,&local,&global);
    //read local vein data
    vector <df::block_square_event_mineralst * > veins;
    vector <df::block_square_event_frozen_liquidst * > ices;
    vector <df::block_square_event_material_spatterst * > splatter;
    vector <df::block_square_event_grassst * > grass;
    vector <df::block_square_event_world_constructionst * > worldconstructions;
    Maps::SortBlockEvents(
        trueBlock,
        &veins,
        &ices,
        &splatter,
        &grass,
        &worldconstructions);    
    //parse block
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
            if(isOpenTerrain(trueBlock->tiletype[lx][ly]) 
                && trueBlock->tiletype[lx][ly] != tiletype::RampTop) {
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

            if(!shouldBeIncluded){
                continue;
            }
            Tile * b = segment.ResetTile(gx, gy, BlockZ, trueBlock->tiletype[lx][ly]);
            b->occ = trueBlock->occupancy[lx][ly];
            b->occ.bits.unit = false;//this will be set manually when we read the creatures vector
            b->designation = trueBlock->designation[lx][ly];
            b->fog_of_war = !b->designation.bits.pile;










            //don't read detailed information for blackbox tiles
            if(!ssConfig.show_hidden_tiles
                && ssConfig.shade_hidden_tiles
                && b->designation.bits.hidden
                && !b->designation.bits.flow_size) {
                    continue;
            }

            b->grasslevel = 0;
            b->grassmat = -1;
            //b->grasslevels.clear();
            //b->grassmats.clear();
            for(int i = 0; i < grass.size(); i++) {
                if(grass[i]->amount[lx][ly] > 0 && b->grasslevel == 0) { //b->grasslevel)
                    b->grasslevel = grass[i]->amount[lx][ly];
                    b->grassmat = grass[i]->plant_index;
                    //b->grasslevels.push_back(grass[i].intensity[lx][ly]);
                    //b->grassmats.push_back(grass[i].material);
                }
            }
            
            //do spatters
            b->mudlevel = 0;
            b->snowlevel = 0;
            b->bloodlevel = 0;
            if(ssConfig.bloodcutoff < UINT8_MAX) {
                long red=0;
                long green=0;
                long blue=0;
                long blood=0;
                long snow=0;
                for(int i = 0; i < splatter.size(); i++) {
                    if(!splatter[i]->amount[lx][ly]) {
                        continue;
                    }
                    uint8_t level = (uint8_t)splatter[i]->amount[lx][ly];

                    //try to get the blood/snow tint
                    MaterialInfo mat;
                    switch(splatter[i]->mat_type){
                    case MUD:
                        //red += (152 * level);
                        //green += (118 * level);
                        //blue += (84 *level);
                        break;
                    case ICE:
                        //ice and snow are white, water is blue
                        if(splatter[i]->mat_state == df::matter_state::Powder 
                            || splatter[i]->mat_state == df::matter_state::Solid ) {
                                red += (255 * level);
                                green += (255 * level);
                                blue += (255 *level);
                        } else {
                                red += (150 * level);
                                green += (237 * level);
                                blue += (224 *level);
                        }
                        break;
                    case VOMIT:
                        red += (127 * level);
                        green += (196 * level);
                        blue += (28 *level);
                        break;
                    default:
                        //try to decode the material color, use gray if we fail
                        if(mat.decode(splatter[i]->mat_type, splatter[i]->mat_index)) {
                            red += (contentLoader->Mats->color[mat.material->state_color[splatter[i]->mat_state]].red * level * 255);
                            green += (contentLoader->Mats->color[mat.material->state_color[splatter[i]->mat_state]].green * level * 255);
                            blue += (contentLoader->Mats->color[mat.material->state_color[splatter[i]->mat_state]].blue * level * 255);
                        } else {
                            red += (128 * level);
                            green += (128 * level);
                            blue += (128 *level);
                        }

                    }

                    //use the state innformation to determine if we should be incrementing the snow or the blood
                    int16_t state = splatter[i]->mat_state;
                    state = splatter[i]->mat_type == MUD ? INVALID_INDEX : state;
                    state = splatter[i]->mat_type == VOMIT ? df::matter_state::Paste : state; //change vomit from dust to paste - gross
                    switch(state) {
                    case INVALID_INDEX:
                        b->mudlevel = level;
                    case df::matter_state::Powder:
                    case df::matter_state::Solid:
                        snow += level;
                        break;
                    default:
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
                    b->snowlevel = snow>UINT8_MAX ? UINT8_MAX : snow;
                    b->bloodlevel = blood>UINT8_MAX ? UINT8_MAX : blood;
                } else {
                    b->bloodcolor = al_map_rgba(0,0,0,0);
                }
            } else {
                b->bloodcolor = al_map_rgb(150, 0, 24);
            }

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
                b->material.index = contentLoader->obsidian;
            }
        }








    }

    //add trees and other vegetation
    for(auto iter = trueBlock->plants.begin(); iter != trueBlock->plants.end(); iter++) {
        df::plant * wheat = *iter;
        assert(wheat != NULL);
        Tile* b = segment.getTile( wheat->pos.x, wheat->pos.y, wheat->pos.z);
        if(!b) {
            b = segment.ResetTile(wheat->pos.x, wheat->pos.y, wheat->pos.z, tiletype::OpenSpace);
            if(!b) {
                continue;
            }
        }
        if( b->tileShape() == tiletype_shape::TREE ||
            b->tileShape() == tiletype_shape::SAPLING ||
            b->tileShape() == tiletype_shape::SHRUB) {
                b->tree.type = wheat->flags.whole;
                b->tree.index = wheat->material;
        }
    }

    //add items
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
        b->Item.item.type = found_item->getType(); //itemtype
        b->Item.item.index = found_item->getSubtype(); //item subtype

        b->Item.matt.type = found_item->getActualMaterial();
        b->Item.matt.index = found_item->getActualMaterialIndex();

        if(1) { //found_item->isDyed())
            auto Constructed_Item = virtual_cast<df::item_constructed>(found_item);
            if(Constructed_Item) {
                for(int idex = 0; idex < Constructed_Item->improvements.size(); idex++) {
                    if(!Constructed_Item->improvements[idex]) {
                        continue;
                    }
                    if(Constructed_Item->improvements[idex]->getType() != improvement_type::THREAD) {
                        continue;
                    }
                    auto Improvement_Thread = virtual_cast<df::itemimprovement_threadst>(Constructed_Item->improvements[idex]);
                    if(!Improvement_Thread) {
                        continue;
                    }
                    if (Improvement_Thread->dye.mat_type < 0) {
                        break;
                    }
                    b->Item.dyematt.type = Improvement_Thread->dye.mat_type;
                    b->Item.dyematt.index = Improvement_Thread->dye.mat_index;
                }
            } else if (found_item->getType() == item_type::THREAD) {
                auto Thread_Item = virtual_cast<df::item_threadst>(found_item);
                if(!Thread_Item) {
                    break;
                }
                if (Thread_Item->dye_mat_type < 0) {
                    break;
                }
                b->Item.dyematt.type = Thread_Item->dye_mat_type;
                b->Item.dyematt.index = Thread_Item->dye_mat_index;
            }
        }
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

void readMapSegment(WorldSegment* segment, int x, int y, int z, int sizex, int sizey, int sizez)
{
    uint32_t index;
    DFHack::Core & DF = Core::getInstance();
    clock_t starttime = clock();

    //read date
    if(!ssConfig.skipWorld) {
        contentLoader->currentYear = World::ReadCurrentYear();
        contentLoader->currentTick = World::ReadCurrentTick();
        contentLoader->currentMonth = (contentLoader->currentTick+9)/33600;
        contentLoader->currentDay = ((contentLoader->currentTick+9)%33600)/1200;
        contentLoader->currentHour = ((contentLoader->currentTick+9)-(((contentLoader->currentMonth*28)+contentLoader->currentDay)*1200))/50;
        contentLoader->currentTickRel = (contentLoader->currentTick+9)-(((((contentLoader->currentMonth*28)+contentLoader->currentDay)*24)+contentLoader->currentHour)*50);
        World::ReadGameMode(contentLoader->gameMode);
    }

    if(ssConfig.skipMaps || !Maps::IsValid()) {
        segment->Reset(x,y,z + 1,sizex,sizey,sizez + 1,true);
        return;
    }

    //Read Number of blocks
    uint32_t blockDimX, blockDimY, blockDimZ;
    Maps::getSize(blockDimX, blockDimY, blockDimZ);
    //Read position of blocks
    uint32_t regionX, regionY, regionZ;
    Maps::getSize(regionX, regionY, regionZ);
    //Store these
    blockDimX *= BLOCKEDGESIZE;
    blockDimY *= BLOCKEDGESIZE;
    regionX *= BLOCKEDGESIZE;
    regionY *= BLOCKEDGESIZE;
    ssConfig.blockDimX = blockDimX;
    ssConfig.blockDimY = blockDimY;
    ssConfig.blockDimZ = blockDimZ;

    //setup new world segment
    segment->Reset(x,y,z,sizex,sizey,sizez,false);
    segment->regionSize.x = blockDimX;
    segment->regionSize.y = blockDimY;
    segment->regionSize.z = blockDimZ;
    segment->regionPos.x = regionX;
    segment->regionPos.y = regionY;
    segment->regionPos.z = regionZ;
    segment->rotation = ssState.DisplayedRotation;

    //read world wide buildings
    vector<Buildings::t_building> allBuildings;
    if(!ssConfig.skipBuildings) {
        ReadBuildings(DF, &allBuildings);
    }

    /*if(GroundMaterialNamesTranslatedFromGame == false)
    TranslateGroundMaterialNames();*/

    //read cursor
    Gui::getCursorCoords(ssConfig.dfCursorX, ssConfig.dfCursorY, ssConfig.dfCursorZ);

    // read constructions
    vector<df::construction> allConstructions;
    uint32_t numconstructions = 0;

    if(!ssConfig.skipConstructions) {
        numconstructions = Constructions::getCount();
        if (numconstructions) {
            df::construction tempcon;
            index = 0;
            while(index < numconstructions) {
                tempcon = *Constructions::getConstruction(index);
                if(segment->CoordinateInsideSegment(tempcon.pos.x, tempcon.pos.y, tempcon.pos.z)) {
                    allConstructions.push_back(tempcon);
                }
                index++;
            }
        }
    }

    //figure out what blocks to read
    int32_t firstTileToReadX = x;
    if( firstTileToReadX < 0 ) {
        firstTileToReadX = 0;
    }

    // get region geology
    vector< vector <int16_t> > layers;
    vector<df::coord2d> geoidx;
    if(!Maps::ReadGeology( &layers, &geoidx )) {
        LogError("Can't get region geology.\n");
    }
    
    while(firstTileToReadX < x + sizex) {
        int blockx = firstTileToReadX / BLOCKEDGESIZE;
        int32_t lastTileInBlockX = (blockx+1) * BLOCKEDGESIZE - 1;
        int32_t lastTileToReadX = min<int32_t>(lastTileInBlockX, x+sizex-1);

        int32_t firstTileToReadY = y;
        if( firstTileToReadY < 0 ) {
            firstTileToReadY = 0;
        }

        while(firstTileToReadY < y + sizey) {
            int blocky = firstTileToReadY / BLOCKEDGESIZE;
            int32_t lastTileInBlockY = (blocky+1) * BLOCKEDGESIZE - 1;
            int32_t lastTileToReadY = min<uint32_t>(lastTileInBlockY, y+sizey-1);

            for(int lz=z-sizez; lz <= z; lz++) {
                //load the tiles from this block to the map segment
                ReadBlockToSegment(DF, *segment, blockx, blocky, lz,
                    firstTileToReadX, firstTileToReadY, 
                    lastTileToReadX, lastTileToReadY, &layers);

            }
            firstTileToReadY = lastTileToReadY + 1;
        }
        firstTileToReadX = lastTileToReadX + 1;
    }


    //merge buildings with segment
    if(!ssConfig.skipBuildings) {
        MergeBuildingsToSegment(&allBuildings, segment);
    }

    //translate constructions
    changeConstructionMaterials(segment, &allConstructions);

    uint32_t numengravings = Engravings::getCount();
    df::engraving * engraved;
    index = 0;
    Tile * b = 0;
    while(index < numengravings) {
        engraved = Engravings::getEngraving(index);
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
    if(!ssConfig.skipCreatures) {
        ReadCreaturesToSegment( DF, segment );
    }

    segment->loaded = 1;
    segment->processed = 0;
    ssTimers.read_time = (clock() - starttime)*0.1 + ssTimers.read_time*0.9;
}

//==================================Misc================================//
/*
 * TODO: move this stuff to GUI or main or something - really doe not belong 
 *  here even a little.
 */

void FollowCurrentDFWindow()
{
    int32_t newviewx;
    int32_t newviewy;
    int32_t viewsizex;
    int32_t viewsizey;
    int32_t newviewz;
    int32_t mapx, mapy, mapz;
    // we take the rectangle you'd get if you scrolled the DF view closely around
    // map edges with a pen pierced through the center,
    // compute the scaling factor between this rectangle and the map bounds and then scale
    // the coords with this scaling factor
    /**
    +---+
    |W+-++----------+
    +-+-+---------+ |
    | |         | |
    | | inner   | |
    | |   rect. | |
    | |         | |
    | |         | |--- map boundary
    | +---------+ |
    +-------------+  W - corrected view
    */
    Maps::getSize((uint32_t &)mapx, (uint32_t &)mapy, (uint32_t &)mapz);
    mapx *= 16;
    mapy *= 16;

    Gui::getWindowSize(viewsizex,viewsizey);
    float scalex = float (mapx) / float (mapx - viewsizex);
    float scaley = float (mapy) / float (mapy - viewsizey);

    Gui::getViewCoords(newviewx,newviewy,newviewz);
    newviewx = newviewx + (viewsizex / 2) - mapx / 2;
    newviewy = newviewy + (viewsizey / 2) - mapy / 2;

    parms.x = float (newviewx) * scalex - (ssConfig.segmentSize.x / 2) + ssConfig.viewXoffset + mapx / 2;
    parms.y = float (newviewy) * scaley - (ssConfig.segmentSize.y / 2) + ssConfig.viewYoffset + mapy / 2;
    parms.z = newviewz + ssConfig.viewZoffset + 1;
}

void FollowCurrentDFCenter()
{
    int32_t newviewx;
    int32_t newviewy;
    int32_t viewsizex;
    int32_t viewsizey;
    int32_t newviewz;
    Gui::getWindowSize(viewsizex,viewsizey);
    Gui::getViewCoords(newviewx,newviewy,newviewz);
    int screenx, screeny, screenz;
    ScreenToPoint(ssState.ScreenW/2, ssState.ScreenH/2, screenx, screeny, screenz);
    parms.x = newviewx + (viewsizex/2) - screenx + ssConfig.viewXoffset;
    parms.y = newviewy + (viewsizey/2) - screeny + ssConfig.viewYoffset;
    parms.z = newviewz + ssConfig.viewZoffset + 1;
}

//==============================Map Read Main===========================//
/*
 * Here is where the main hub functions dispatch the read thread from, 
 *  as well as the read thread's entry point.  
 */

void read_segment( void *arg)
{
    if(!Maps::IsValid()) {
        return;
    }
    static bool firstLoad = 1;
    ssConfig.threadstarted = 1;
    WorldSegment* segment = NULL;
    {
        CoreSuspender suspend;
        if (firstLoad || ssConfig.follow_DFscreen) {
            firstLoad = 0;
            if (ssConfig.track_center) {
                FollowCurrentDFCenter();
            } else {
                FollowCurrentDFWindow();
            }
        }
        segment = map_segment.getRead();
        readMapSegment(segment, parms.x, parms.y, parms.z,parms.sizex, parms.sizey, parms.sizez);
        ssConfig.threadstarted = 0;
    }

    if(segment) {
        beautifySegment(segment);

        //putting these here to increase responsiveness of the UI and to make megashots work
        segment->displayed.x = ssState.DisplayedSegmentX;
        segment->displayed.y = ssState.DisplayedSegmentY;
        segment->displayed.z = ssState.DisplayedSegmentZ;

        segment->AssembleAllTiles();

        //only need to lock the drawing segment because the reading segment is already locked
        map_segment.lockDraw();
        map_segment.swap();
        map_segment.unlockDraw();
    }
}

static void * threadedSegment(ALLEGRO_THREAD *read_thread, void *arg)
{
    while(!al_get_thread_should_stop(read_thread)) {
        map_segment.lockRead();
        read_segment(arg);
        map_segment.unlockRead();
        al_rest(ssConfig.automatic_reload_time/1000.0);
    }
    return 0;
}

void reloadDisplayedSegment()
{
    //create handle to dfHack API
    static bool firstLoad = 1;

    if (timeToReloadConfig) {
        parms.thread_connect = 0;
        contentLoader->Load();
        timeToReloadConfig = false;
    }

    if (firstLoad || ssConfig.follow_DFscreen) {
        ssState.DisplayedSegmentX = parms.x;
        ssState.DisplayedSegmentY = parms.y;
        ssState.DisplayedSegmentZ = parms.z;
    }

    int segmentHeight = ssConfig.single_layer_view ? 2 : ssConfig.segmentSize.z;
    //load segment
    if(ssConfig.threading_enable) {
        if(!ssConfig.threadmade) {
            ssConfig.readThread = al_create_thread(threadedSegment, NULL);
            ssConfig.threadmade = 1;
        }
    }

    parms.x = ssState.DisplayedSegmentX;
    parms.y = ssState.DisplayedSegmentY;
    parms.z = ssState.DisplayedSegmentZ;
    parms.sizex = ssConfig.segmentSize.x;
    parms.sizey = ssConfig.segmentSize.y;
    parms.sizez = segmentHeight;

    if(ssConfig.threading_enable) {
        al_start_thread(ssConfig.readThread);
    } else {
        read_segment(NULL);
    }

    firstLoad = 0;
}
