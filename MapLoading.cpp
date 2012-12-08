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

void ReadBlockToSegment(DFHack::Core& DF, WorldSegment& segment, int BlockX, int BlockY, int BlockZ,
    uint32_t BoundrySX, uint32_t BoundrySY,
    uint32_t BoundryEX, uint32_t BoundryEY
    //uint16_t Flags/*not in use*/,
    //vector<Buildings::t_building>* allBuildings,
    //vector<df::construction>* allConstructions,
    )
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
            if(trueBlock->designation[lx][ly].bits.flow_size) {
                shouldBeIncluded = true;
            }

            if(!shouldBeIncluded){
                continue;
            }

            Tile * b = segment.ResetTile(gx, gy, BlockZ, tiletype::OpenSpace);

            b->occ = trueBlock->occupancy[lx][ly];
            b->occ.bits.unit = false;//this will be set manually when we read the creatures vector
            b->designation = trueBlock->designation[lx][ly];
            //read tiletype
            b->tileType = trueBlock->tiletype[lx][ly];
            b->fog_of_war = !b->designation.bits.pile;
        }
    }

    //add trees and other vegetation
    for(auto iter = trueBlock->plants.begin(); iter != trueBlock->plants.end(); iter++) {
        df::plant * wheat = *iter;
        assert(wheat != NULL);
        Tile* b = segment.getTile( wheat->pos.x, wheat->pos.y, wheat->pos.z);
        if(!b) {
            continue;
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
            continue;
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
                continue;
            }
            b->haseffect = true;
            switch(eff->type) {
            case flow_type::Steam: {
                b->Eff_Steam.density += eff->density;
                b->Eff_Steam.matt.index = eff->mat_index;
                b->Eff_Steam.matt.type = eff->mat_type;
                break;
                                   }
            case flow_type::Mist: {
                b->Eff_Mist.density += eff->density;
                b->Eff_Mist.matt.index = eff->mat_index;
                b->Eff_Mist.matt.type = eff->mat_type;
                break;
                                  }
            case flow_type::MaterialDust: {
                b->Eff_MaterialDust.density += eff->density;
                b->Eff_MaterialDust.matt.index = eff->mat_index;
                b->Eff_MaterialDust.matt.type = eff->mat_type;
                break;
                                          }
            case flow_type::MagmaMist: {
                b->Eff_MagmaMist.density += eff->density;
                b->Eff_MagmaMist.matt.index = eff->mat_index;
                b->Eff_MagmaMist.matt.type = eff->mat_type;
                break;
                                       }
            case flow_type::Smoke: {
                b->Eff_Smoke.density += eff->density;
                b->Eff_Smoke.matt.index = eff->mat_index;
                b->Eff_Smoke.matt.type = eff->mat_type;
                break;
                                   }
            case flow_type::Dragonfire: {
                b->Eff_Dragonfire.density += eff->density;
                b->Eff_Dragonfire.matt.index = eff->mat_index;
                b->Eff_Dragonfire.matt.type = eff->mat_type;
                break;
                                        }
            case flow_type::Fire: {
                b->Eff_Fire.density += eff->density;
                b->Eff_Fire.matt.index = eff->mat_index;
                b->Eff_Fire.matt.type = eff->mat_type;
                break;
                                  }
            case flow_type::Web: {
                b->Eff_Web.density += eff->density;
                b->Eff_Web.matt.index = eff->mat_index;
                b->Eff_Web.matt.type = eff->mat_type;
                break;
                                 }
            case flow_type::MaterialGas: {
                b->Eff_MaterialGas.density += eff->density;
                b->Eff_MaterialGas.matt.index = eff->mat_index;
                b->Eff_MaterialGas.matt.type = eff->mat_type;
                break;
                                         }
            case flow_type::MaterialVapor: {
                b->Eff_MaterialVapor.density += eff->density;
                b->Eff_MaterialVapor.matt.index = eff->mat_index;
                b->Eff_MaterialVapor.matt.type = eff->mat_type;
                break;
                                           }
            case flow_type::OceanWave: {
                b->Eff_OceanWave.density += eff->density;
                b->Eff_OceanWave.matt.index = eff->mat_index;
                b->Eff_OceanWave.matt.type = eff->mat_type;
                break;
                                       }
            case flow_type::SeaFoam: {
                b->Eff_SeaFoam.density += eff->density;
                b->Eff_SeaFoam.matt.index = eff->mat_index;
                b->Eff_SeaFoam.matt.type = eff->mat_type;
                break;
                                     }
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
                    lastTileToReadX, lastTileToReadY);

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

        //suspend DF to do the detailed read/draw
        {
            CoreSuspender suspend;
            segment->AssembleAllTiles();
        }

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
