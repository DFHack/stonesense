#include "common.h"
#include "WorldSegment.h"
#include "GUI.h"
#include "ContentLoader.h"
#include "Creatures.h"


ALLEGRO_BITMAP * fog = 0;

// currently visible map segment
SegmentWrap map_segment;

Block* WorldSegment::getBlock(int32_t x, int32_t y, int32_t z)
{
    if(x < this->x || x >= this->x + this->sizex) {
        return 0;
    }
    if(y < this->y || y >= this->y + this->sizey) {
        return 0;
    }
    if(z < this->z || z >= this->z + this->sizez) {
        return 0;
    }
    /*for(uint32_t i=0; i<this->blocks.size(); i++){
    Block* b = this->blocks[i];
    if(x == b->x && y == b->y && z == b->z)
    return b;
    }
    return 0;*/

    uint32_t lx = x;
    uint32_t ly = y;
    uint32_t lz = z;
    //make local
    lx -= this->x;
    ly -= this->y;
    lz -= this->z;

    CorrectBlockForSegmentRotation( (int32_t&)lx,(int32_t&)ly,(int32_t&)lz );

    uint32_t index = lx + (ly * this->sizex) + ((lz) * this->sizex * this->sizey);
    return blocksAsPointerVolume[index];
}

Block* WorldSegment::getBlockRelativeTo(uint32_t x, uint32_t y, uint32_t z,  dirRelative direction)
{
    int32_t lx = x;
    int32_t ly = y;
    int32_t lz = z;
    //make local
    lx -= this->x;
    ly -= this->y;
    lz -= this->z;

    CorrectBlockForSegmentRotation( (int32_t&)lx,(int32_t&)ly,(int32_t&)lz );
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

    if((int)lx < 0 || lx >= this->sizex) {
        return 0;
    }
    if((int)ly < 0 || ly >= this->sizey) {
        return 0;
    }
    if((int)lz < 0 || lz >= this->sizez) {
        return 0;
    }

    uint32_t index = lx + (ly * this->sizex) + ((lz) * this->sizex * this->sizey);
    return blocksAsPointerVolume[index];
}

Block* WorldSegment::getBlockRelativeTo(uint32_t x, uint32_t y, uint32_t z,  dirRelative direction, int distance)
{
    int32_t lx = x;
    int32_t ly = y;
    int32_t lz = z;
    //make local
    lx -= this->x;
    ly -= this->y;
    lz -= this->z;

    CorrectBlockForSegmentRotation( (int32_t&)lx,(int32_t&)ly,(int32_t&)lz );
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

    if((int)lx < 0 || lx >= this->sizex) {
        return 0;
    }
    if((int)ly < 0 || ly >= this->sizey) {
        return 0;
    }
    if((int)lz < 0 || lz >= this->sizez) {
        return 0;
    }

    uint32_t index = lx + (ly * this->sizex) + ((lz) * this->sizex * this->sizey);
    return blocksAsPointerVolume[index];
}

Block* WorldSegment::getBlockLocal(uint32_t x, uint32_t y, uint32_t z)
{
    if((int)x < 0 || x >= (uint32_t)this->sizex) {
        return 0;
    }
    if((int)y < 0 || y >= (uint32_t)this->sizey) {
        return 0;
    }
    if((int)z < 0 || z >= (uint32_t)this->sizez) {
        return 0;
    }

    uint32_t index = x + (y * this->sizex) + ((z) * this->sizex * this->sizey);
    return blocksAsPointerVolume[index];
}

Block* WorldSegment::getBlock(uint32_t index)
{
    if(index<0 || index >= blocks.size() ) {
        return 0;
    }
    return blocks[index];
}

void WorldSegment::CorrectBlockForSegmentOffset(int32_t& xin, int32_t& yin, int32_t& zin)
{
    xin -= displayedx;
    yin -= displayedy; //DisplayedSegmentY;
    zin -= displayedz - 1; //need to remove the offset
}

void WorldSegment::CorrectBlockForSegmentRotation(int32_t& x, int32_t& y, int32_t& z)
{
    int32_t oldx = x;
    int32_t oldy = y;
    int w = ssConfig.segmentSize.x;
    int h = ssConfig.segmentSize.y;

    if(rotation == 1) {
        x = h - oldy -1;
        y = oldx;
    }
    if(rotation == 2) {
        x = w - oldx -1;
        y = h - oldy -1;
    }
    if(rotation == 3) {
        x = oldy;
        y = w - oldx -1;
    }
}

void WorldSegment::addBlock(Block* b)
{
    this->blocks.push_back(b);
    //b = &(this->blocks[ blocks.size() - 1]);

    uint32_t x = b->x;
    uint32_t y = b->y;
    uint32_t z = b->z;
    //make local
    x -= this->x;
    y -= this->y;
    z -= this->z;

    //rotate
    CorrectBlockForSegmentRotation( (int32_t&)x, (int32_t&)y, (int32_t&)z);
    uint32_t index = x + (y * this->sizex) + ((z) * this->sizex * this->sizey);
    blocksAsPointerVolume[index] = b;
}

void WorldSegment::DrawAllBlocks()
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

void WorldSegment::AssembleAllBlocks()
{
    if(!loaded) {
        return;
    }

    todraw.clear();
    
    clock_t starttime = clock();

    // x,y,z print prices
    int32_t vsxmax = sizex-1;
    int32_t vsymax = sizey-1;
    int32_t vszmax = sizez-1; // grabbing one tile +z more than we should for tile rules

    for(int32_t vsz=0; vsz < vszmax; vsz++) {
        //add the fog to the queue
        if(ssConfig.fogenable && fog) {
            Draw_Event d = {TintedScaledBitmap, fog, al_map_rgb(255,255,255), 0, 0, ssState.ScreenW, ssState.ScreenH, 0, 0, ssState.ScreenW, ssState.ScreenH, 0};
            AssembleSprite(d);
        }
        //add the blocks to the queue
        for(int32_t vsx=1; vsx < vsxmax; vsx++) {
            for(int32_t vsy=1; vsy < vsymax; vsy++) {
                Block *b = getBlockLocal(vsx,vsy,vsz);
                if (b) {
                    b->AssembleBlock();
                }
            }
        }
    }

    ssTimers.assembly_time = (clock() - starttime)*0.1 + ssTimers.assembly_time*0.9;
}


bool WorldSegment::CoordinateInsideSegment(uint32_t x, uint32_t y, uint32_t z)
{
    if( (int32_t)x < this->x || (int32_t)x >= this->x + this->sizex) {
        return false;
    }
    if( (int32_t)y < this->y || (int32_t)y >= this->y + this->sizey) {
        return false;
    }
    if( (int32_t)z < this->z || (int32_t)z >= this->z + this->sizez) {
        return false;
    }
    return true;
}

bool WorldSegment::CoordinateInteriorSegment(uint32_t x, uint32_t y, uint32_t z, uint32_t shellthick)
{
    if( (int32_t)x < this->x + shellthick || (int32_t)x >= this->x + this->sizex - shellthick) {
        return false;
    }
    if( (int32_t)y < this->y + shellthick || (int32_t)y >= this->y + this->sizey - shellthick) {
        return false;
    }
    if( (int32_t)z < this->z + shellthick || (int32_t)z >= this->z + this->sizez - shellthick) {
        return false;
    }
    return true;
}

//void WorldSegment::drawPixels()
//{
//    for(int32_t vsx=0; vsx < this->sizex; vsx++)
//        for(int32_t vsy=0; vsy < this->sizey; vsy++)
//            for(int32_t vsz=0; vsz < this->sizez; vsz++) {
//                Block *b = getBlockLocal(vsx,vsy,vsz);
//                if (b) {
//                    b->DrawPixel(vsx, (vsz*this->sizey) + vsy);
//                }
//            }
//}

void WorldSegment::AssembleSprite(Draw_Event d)
{
    todraw.push_back(d);
}