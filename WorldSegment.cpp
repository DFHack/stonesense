#include "common.h"
#include "WorldSegment.h"
#include "GUI.h"
#include "ContentLoader.h"
#include "Creatures.h"

ALLEGRO_BITMAP * fog = 0;

Tile* WorldSegment::ResetTile(int32_t x, int32_t y, int32_t z, df::tiletype type)
{
    uint32_t lx = x;
    uint32_t ly = y;
    uint32_t lz = z;
    //make local
    lx -= this->pos.x;
    ly -= this->pos.y;
    lz -= this->pos.z;

    if((int)lx < 0 || lx >= (uint32_t)this->size.x) {
        return 0;
    }
    if((int)ly < 0 || ly >= (uint32_t)this->size.y) {
        return 0;
    }
    if((int)lz < 0 || lz >= (uint32_t)this->size.z) {
        return 0;
    }

    CorrectTileForSegmentRotation( (int32_t&)lx,(int32_t&)ly,(int32_t&)lz );

    uint32_t index = lx + (ly * this->size.x) + ((lz) * this->size.x * this->size.y);
    tiles[index].Reset(this, type);
    tiles[index].x = x;
    tiles[index].y = y;
    tiles[index].z = z;
    return &tiles[index];
}

Tile* WorldSegment::getTile(int32_t x, int32_t y, int32_t z)
{
    uint32_t lx = x;
    uint32_t ly = y;
    uint32_t lz = z;
    //make local
    lx -= this->pos.x;
    ly -= this->pos.y;
    lz -= this->pos.z;

    CorrectTileForSegmentRotation( (int32_t&)lx,(int32_t&)ly,(int32_t&)lz );
    return getTileLocal(lx, ly, lz);
}

Tile* WorldSegment::getTileRelativeTo(uint32_t x, uint32_t y, uint32_t z,  dirRelative direction)
{
    int32_t lx = x;
    int32_t ly = y;
    int32_t lz = z;
    //make local
    lx -= this->pos.x;
    ly -= this->pos.y;
    lz -= this->pos.z;

    CorrectTileForSegmentRotation( (int32_t&)lx,(int32_t&)ly,(int32_t&)lz );
    switch (direction) {
    case eUp:
        ly--;
        break;
    case eDown:
        ly++;
        break;
    case eLeft:
        lx--;
        break;
    case eRight:
        lx++;
        break;
    case eAbove:
        lz++;
        break;
    case eBelow:
        lz--;
        break;
    case eUpLeft:
        ly--;
        lx--;
        break;
    case eUpRight:
        ly--;
        lx++;
        break;
    case eDownLeft:
        ly++;
        lx--;
        break;
    case eDownRight:
        ly++;
        lx++;
        break;
    }
    
    return getTileLocal(lx, ly, lz);
}

Tile* WorldSegment::getTileRelativeTo(uint32_t x, uint32_t y, uint32_t z,  dirRelative direction, int distance)
{
    int32_t lx = x;
    int32_t ly = y;
    int32_t lz = z;
    //make local
    lx -= this->pos.x;
    ly -= this->pos.y;
    lz -= this->pos.z;
    
    CorrectTileForSegmentRotation( (int32_t&)lx,(int32_t&)ly,(int32_t&)lz );
    switch (direction) {
    case eUp:
        ly-= distance;
        break;
    case eDown:
        ly+= distance;
        break;
    case eLeft:
        lx-= distance;
        break;
    case eRight:
        lx+= distance;
        break;
    case eAbove:
        lz+= distance;
        break;
    case eBelow:
        lz-= distance;
        break;
    case eUpLeft:
        ly-= distance;
        lx-= distance;
        break;
    case eUpRight:
        ly-= distance;
        lx+= distance;
        break;
    case eDownLeft:
        ly+= distance;
        lx-= distance;
        break;
    case eDownRight:
        ly+= distance;
        lx+= distance;
        break;
    }

    return getTileLocal(lx, ly, lz);
}

Tile* WorldSegment::getTileLocal(uint32_t x, uint32_t y, uint32_t z)
{
    if((int)x < 0 || x >= (uint32_t)this->size.x) {
        return 0;
    }
    if((int)y < 0 || y >= (uint32_t)this->size.y) {
        return 0;
    }
    if((int)z < 0 || z >= (uint32_t)this->size.z) {
        return 0;
    }

    uint32_t index = x + (y * this->size.x) + ((z) * this->size.x * this->size.y);
    return getTile(index);
}

Tile* WorldSegment::getTile(uint32_t index)
{
    if(index<0 || index>=getNumTiles() ) {
        return NULL;
    }
    return tiles[index].IsValid() ? &tiles[index] : NULL;
}

void WorldSegment::CorrectTileForSegmentOffset(int32_t& xin, int32_t& yin, int32_t& zin)
{
    xin -= displayed.x;
    yin -= displayed.y; //DisplayedSegmentY;
    zin -= displayed.z - 1; //need to remove the offset
}

void WorldSegment::CorrectTileForSegmentRotation(int32_t& x, int32_t& y, int32_t& z)
{
    int32_t oldx = x;
    int32_t oldy = y;

    if(rotation == 1) {
        x = size.y - oldy -1;
        y = oldx;
    }
    if(rotation == 2) {
        x = size.x - oldx -1;
        y = size.y - oldy -1;
    }
    if(rotation == 3) {
        x = oldy;
        y = size.x - oldx -1;
    }
}

//void WorldSegment::addTile(Tile* b)
//{
//    uint32_t x = b->x;
//    uint32_t y = b->y;
//    uint32_t z = b->z;
//    //make local
//    x -= this->pos.x;
//    y -= this->pos.y;
//    z -= this->pos.z;
//
//    //rotate
//    CorrectTileForSegmentRotation( (int32_t&)x, (int32_t&)y, (int32_t&)z);
//    uint32_t index = x + (y * this->size.x) + ((z) * this->size.x * this->size.y);
//    tiles[index] = *b;
//}

void WorldSegment::DrawAllTiles()
{
    if(!loaded) {
        return;
    }

    if(ssConfig.fogenable) {
        ALLEGRO_BITMAP* temp = al_get_target_bitmap();
        if(!fog) {
            fog = al_create_bitmap(ssState.ScreenW, ssState.ScreenH);
            al_set_target_bitmap(fog);
            al_clear_to_color(premultiply(ssConfig.fogcol));
            al_set_target_bitmap(temp);
        }
        if(!((al_get_bitmap_width(fog) == ssState.ScreenW) && (al_get_bitmap_height(fog) == ssState.ScreenH))) {
            al_destroy_bitmap(fog);
            fog = al_create_bitmap(ssState.ScreenW, ssState.ScreenH);
            al_set_target_bitmap(fog);
            al_clear_to_color(premultiply(ssConfig.fogcol));
            al_set_target_bitmap(temp);
        }
    }

    if (ssConfig.show_osd) {
        DrawCurrentLevelOutline(true);
    }

    if(todraw.size()>0) {
        al_hold_bitmap_drawing(true);
        for(int i=0; i<todraw.size(); i++) {
            if(i%ssConfig.bitmapHolds==0) {
                al_hold_bitmap_drawing(false);
                al_hold_bitmap_drawing(true);
            }
            switch(todraw[i].type) {
            case TintedScaledBitmap:
                al_draw_tinted_scaled_bitmap(
                    (ALLEGRO_BITMAP*) todraw[i].drawobject,
                    todraw[i].tint,
                    todraw[i].sx,
                    todraw[i].sy,
                    todraw[i].sw,
                    todraw[i].sh,
                    todraw[i].dx,
                    todraw[i].dy,
                    todraw[i].dw,
                    todraw[i].dh,
                    todraw[i].flags );
                break;
            case CreatureText:
                DrawCreatureText(
                    todraw[i].dx,
                    todraw[i].dy,
                    (t_unit*) todraw[i].drawobject );
                break;
            }
        }
    }

    if (ssConfig.show_osd) {
        DrawCurrentLevelOutline(false);
    }

    al_hold_bitmap_drawing(false);
    al_hold_bitmap_drawing(true);
}

/**
 * if direction is -1, returns b-a (positive if a<b)
 * if direction is  1, returns a-b (negative if a<b)
 * all other directions return 0
 */
int32_t compare(int32_t a, int32_t b, int32_t direction){
    switch(direction){
    case -1:
        return b-a;
    case 1:
        return a-b;
    default:
        return 0;
    }
}

void WorldSegment::AssembleBlockTiles(
    int32_t firstX, int32_t firstY, 
    int32_t lastX, int32_t lastY, 
    int32_t incrx, int32_t incry, 
    int32_t blockz){

        incrx = incrx >= 0 ? 1 : -1;
        incry = incry >= 0 ? 1 : -1;

        //now iterate over the tiles in the block
        for(int i=firstX; compare(i, lastX, incrx) < 0; i+=incrx) {
            for(int j=firstY; compare(j, lastY, incry) < 0; j+=incry) {
                //do stuff!
                Tile *b = getTile(i,j,blockz);
                if (b) {
                    b->AssembleTile();
                } 
                //else {
                //    int drawx = i;
                //    int drawy = j;
                //    int drawz = lz+1;

                //    CorrectTileForSegmentOffset( drawx, drawy, drawz);
                //    CorrectTileForSegmentRotation( drawx, drawy, drawz);
                //    pointToScreen((int*)&drawx, (int*)&drawy, drawz);
                //    drawx -= (PLATEWIDTH>>1)*ssConfig.scale;
                //    draw_event d = {TintedScaledBitmap, IMGObjectSheet, al_map_rgb(255,255,255), 0, 0, SPRITEWIDTH, SPRITEHEIGHT, drawx, drawy+FLOORHEIGHT, SPRITEWIDTH, SPRITEHEIGHT, 0};
                //    AssembleSprite(d);
                //}
            }
        }
}

/**
 * Assembles sprites for all tiles in the segment.  
 * The draw order used draws tiles on a per-block basis, so blocks
 * in the back are drawn before blocks in the front.  
 */
void WorldSegment::AssembleAllTiles()
{
    if(!loaded) {
        return;
    }
    
    clock_t starttime = clock();
    int DB1=0;

//---------------------------------------------------------//
    
    //// x,y,z print prices
    //int32_t vsxmax = sizex-1;
    //int32_t vsymax = sizey-1;
    //int32_t vszmax = sizez-1; // grabbing one tile +z more than we should for tile rules
    //for(int32_t vsz=0; vsz < vszmax; vsz++) {
    //    //add the fog to the queue
    //    if(ssConfig.fogenable && fog) {
    //        draw_event d = {TintedScaledBitmap, fog, al_map_rgb(255,255,255), 0, 0, ssState.ScreenW, ssState.ScreenH, 0, 0, ssState.ScreenW, ssState.ScreenH, 0};
    //        AssembleSprite(d);
    //    }
    //    //add the tiles to the queue
    //    for(int32_t vsx=1; vsx < vsxmax; vsx++) {
    //        for(int32_t vsy=1; vsy < vsymax; vsy++) {
    //            Tile *b = getTileLocal(vsx,vsy,vsz);
    //            if (b) {
    //                b->AssembleTile();
    //                DB1++;
    //            }
    //        }
    //    }
    //}

//---------------------------------------------------------//

    //this is the increment by which we shift in either case
    int32_t incrx=2;
    int32_t incry=2;
    correctForRotation(incrx, incry, 4-rotation, 3, 3);
    incrx--;
    incry--;

    //here we set up the variables needed to iterate over the tiles of the block in correct draw-order
    int32_t tilestartx=0;
    int32_t tilestarty=0;
    correctForRotation(tilestartx, tilestarty, 4-rotation, BLOCKEDGESIZE, BLOCKEDGESIZE);
    int32_t tileendx=BLOCKEDGESIZE-1;
    int32_t tileendy=BLOCKEDGESIZE-1;
    correctForRotation(tileendx, tileendy, 4-rotation, BLOCKEDGESIZE, BLOCKEDGESIZE);
    
    //these are used to iterate over the blocks themselves
    int32_t minx, maxx, miny, maxy;
    minx = max<int32_t>(0,pos.x);                        miny = max<int32_t>(0,pos.y);
    maxx = min<int32_t>(pos.x+size.x-1, regionSize.x-1); maxy = min<int32_t>(pos.y+size.y-1, regionSize.y-1); 

    int32_t blockfirstx,blockfirsty,blocklastx,blocklasty;
    switch(rotation){
    case 1:
        blockfirstx = minx/BLOCKEDGESIZE; blockfirsty = maxy/BLOCKEDGESIZE;
        blocklastx = maxx/BLOCKEDGESIZE;  blocklasty = miny/BLOCKEDGESIZE;
        break;
    case 2:
        blockfirstx = maxx/BLOCKEDGESIZE; blockfirsty = maxy/BLOCKEDGESIZE;
        blocklastx = minx/BLOCKEDGESIZE;  blocklasty = miny/BLOCKEDGESIZE;
        break;
    case 3:
        blockfirstx = maxx/BLOCKEDGESIZE; blockfirsty = miny/BLOCKEDGESIZE;
        blocklastx = minx/BLOCKEDGESIZE;  blocklasty = maxy/BLOCKEDGESIZE;
        break;
    default:
        blockfirstx = minx/BLOCKEDGESIZE; blockfirsty = miny/BLOCKEDGESIZE;
        blocklastx = maxx/BLOCKEDGESIZE;  blocklasty = maxy/BLOCKEDGESIZE;
    }

    for(int32_t lz=pos.z; lz < pos.z+size.z-1; lz++) {
        //add the fog
        if(ssConfig.fogenable && fog) {
            draw_event d = {TintedScaledBitmap, fog, al_map_rgb(255,255,255), 0, 0, ssState.ScreenW, ssState.ScreenH, 0, 0, ssState.ScreenW, ssState.ScreenH, 0};
            AssembleSprite(d);
        }
        //figure out what blocks to read
        for(int32_t blockx = blockfirstx; compare(blockx, blocklastx, incrx) <= 0; blockx+=incrx) {
            int32_t firstX = blockx*BLOCKEDGESIZE + tilestartx;
            int32_t lastX = blockx*BLOCKEDGESIZE + tileendx + incrx;
            switch(rotation){
            case 1:
                firstX = max<int32_t>(firstX, pos.x+1);
                lastX = min<int32_t>(lastX, pos.x+size.x-1);
                break;
            case 2:
                firstX = min<int32_t>(firstX, pos.x+size.x-2);
                lastX = max<int32_t>(lastX, pos.x);
                break;
            case 3:
                firstX = min<int32_t>(firstX, pos.x+size.x-2);
                lastX = max<int32_t>(lastX, pos.x);
                break;
            default:
                firstX = max<int32_t>(firstX, pos.x+1);
                lastX = min<int32_t>(lastX, pos.x+size.x-1);
            }
        

            for(int32_t blocky = blockfirsty; compare(blocky, blocklasty, incry) <= 0; blocky+=incry) {
                int32_t firstY = blocky*BLOCKEDGESIZE + tilestarty;
                int32_t lastY = blocky*BLOCKEDGESIZE + tileendy + incry;
                switch(rotation){
                case 1:
                    firstY = min<int32_t>(firstY, pos.y+size.y-2);
                    lastY = max<int32_t>(lastY, pos.y);
                    break;
                case 2:
                    firstY = min<int32_t>(firstY, pos.y+size.y-2);
                    lastY = max<int32_t>(lastY, pos.y);
                    break;
                case 3:
                    firstY = max<int32_t>(firstY, pos.y+1);
                    lastY = min<int32_t>(lastY, pos.y+size.y-1);
                    break;
                default:
                    firstY = max<int32_t>(firstY, pos.y+1);
                    lastY = min<int32_t>(lastY, pos.y+size.y-1);
                }

                DB1++;
                //Now go to that block, and assemble the sprites for it in the order specified.
                AssembleBlockTiles( firstX, firstY, lastX, lastY, incrx, incry, lz);
            }
        }
    }
//---------------------------------------------------------//

    ssTimers.assembly_time = (clock() - starttime)*0.1 + ssTimers.assembly_time*0.9;
    //DebugInt1 = DB1;
}


bool WorldSegment::CoordinateInsideSegment(uint32_t x, uint32_t y, uint32_t z)
{
    if( (int32_t)x < this->pos.x || (int32_t)x >= this->pos.x + this->size.x) {
        return false;
    }
    if( (int32_t)y < this->pos.y || (int32_t)y >= this->pos.y + this->size.y) {
        return false;
    }
    if( (int32_t)z < this->pos.z || (int32_t)z >= this->pos.z + this->size.z) {
        return false;
    }
    return true;
}

bool WorldSegment::CoordinateInteriorSegment(uint32_t x, uint32_t y, uint32_t z, uint32_t shellthick)
{
    if( (int32_t)x < this->pos.x + shellthick || (int32_t)x >= this->pos.x + this->size.x - shellthick) {
        return false;
    }
    if( (int32_t)y < this->pos.y + shellthick || (int32_t)y >= this->pos.y + this->size.y - shellthick) {
        return false;
    }
    if( (int32_t)z < this->pos.z + shellthick || (int32_t)z >= this->pos.z + this->size.z - shellthick) {
        return false;
    }
    return true;
}

//void WorldSegment::drawPixels()
//{
//    for(int32_t vsx=0; vsx < this->sizex; vsx++)
//        for(int32_t vsy=0; vsy < this->sizey; vsy++)
//            for(int32_t vsz=0; vsz < this->sizez; vsz++) {
//                Tile *b = getTileLocal(vsx,vsy,vsz);
//                if (b) {
//                    b->DrawPixel(vsx, (vsz*this->sizey) + vsy);
//                }
//            }
//}

void WorldSegment::AssembleSprite(draw_event d)
{
    todraw.push_back(d);
}