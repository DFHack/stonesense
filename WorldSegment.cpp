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

    CorrectTileForSegmentRotation( (int32_t&)lx,(int32_t&)ly,(int32_t&)lz );

    if((int)lx < 0 || lx >= (uint32_t)this->size.x) {
        return 0;
    }
    if((int)ly < 0 || ly >= (uint32_t)this->size.y) {
        return 0;
    }
    if((int)lz < 0 || lz >= (uint32_t)this->size.z) {
        return 0;
    }

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
    yin -= displayed.y; //DisplayedSegment.y;
    zin -= displayed.z - 1; //need to remove the offset
}

void WorldSegment::CorrectTileForSegmentRotation(int32_t& x, int32_t& y, int32_t& z)
{
    int32_t oldx = x;
    int32_t oldy = y;

    if(rotation == 1) {
        x = size.x - oldy -1;
        y = oldx;
    }
    if(rotation == 2) {
        x = size.x - oldx -1;
        y = size.y - oldy -1;
    }
    if(rotation == 3) {
        x = oldy;
        y = size.y - oldx -1;
    }
}

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
    
    // x,y,z print prices
    int32_t vsxmax = size.x-1;
    int32_t vsymax = size.y-1;
    int32_t vszmax = size.z-1; // grabbing one tile +z more than we should for tile rules
    for(int32_t vsz=0; vsz < vszmax; vsz++) {
        //add the fog to the queue
        if(ssConfig.fogenable && fog) {
            draw_event d = {TintedScaledBitmap, fog, al_map_rgb(255,255,255), 0, 0, (float)ssState.ScreenW, (float)ssState.ScreenH, 0, 0, (float)ssState.ScreenW, (float)ssState.ScreenH, 0};
            AssembleSprite(d);
        }
        //add the tiles to the queue
        for(int32_t vsx=1; vsx < vsxmax; vsx++) {
            for(int32_t vsy=1; vsy < vsymax; vsy++) {
                Tile *b = getTileLocal(vsx,vsy,vsz);
                if (b) {
                    b->AssembleTile();
                } 
            }
        }
    }

    ssTimers.assembly_time = (clock() - starttime)*0.1 + ssTimers.assembly_time*0.9;
}


bool WorldSegment::CoordinateInsideSegment(uint32_t x, uint32_t y, uint32_t z)
{
    uint32_t lx = x;
    uint32_t ly = y;
    uint32_t lz = z;
    //make local
    lx -= this->pos.x;
    ly -= this->pos.y;
    lz -= this->pos.z;

    CorrectTileForSegmentRotation( (int32_t&)lx,(int32_t&)ly,(int32_t&)lz );

    if((int)lx < 0 || lx >= (uint32_t)this->size.x) {
        return 0;
    }
    if((int)ly < 0 || ly >= (uint32_t)this->size.y) {
        return 0;
    }
    if((int)lz < 0 || lz >= (uint32_t)this->size.z) {
        return 0;
    }

    return true;
}

bool WorldSegment::CoordinateInteriorSegment(uint32_t x, uint32_t y, uint32_t z, uint32_t shellthick)
{
    uint32_t lx = x;
    uint32_t ly = y;
    uint32_t lz = z;
    //make local
    lx -= this->pos.x;
    ly -= this->pos.y;
    lz -= this->pos.z;

    CorrectTileForSegmentRotation( (int32_t&)lx,(int32_t&)ly,(int32_t&)lz );

    if((int)lx < 0  + shellthick|| lx >= (uint32_t)this->size.x - shellthick) {
        return 0;
    }
    if((int)ly < 0  + shellthick|| ly >= (uint32_t)this->size.y - shellthick) {
        return 0;
    }
    if((int)lz < 0 /*bottom is "interior"*/ || lz >= (uint32_t)this->size.z - shellthick) {
        return 0;
    }
    return true;
}

void WorldSegment::AssembleSprite(draw_event d)
{
    todraw.push_back(d);
}

Buildings::t_building* WorldSegment::AddBuilding(Buildings::t_building building){
    buildings.push_back(building);
    return &buildings[buildings.size()-1];
}