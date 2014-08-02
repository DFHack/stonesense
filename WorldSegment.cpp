#include "common.h"
#include "WorldSegment.h"
#include "GUI.h"
#include "ContentLoader.h"
#include "Creatures.h"

const GameState SegmentWrap::zeroState = 
{
	{0,0,0},0,{0,0,0},{0,0,0},{0,0,0},0,0
};

ALLEGRO_BITMAP * fog = 0;

void WorldSegment::CorrectTileForSegmentOffset(int32_t& xin, int32_t& yin, int32_t& zin)
{
    xin -= segState.Position.x;
    yin -= segState.Position.y; //Position.y;
    zin -= segState.Position.z - 1; //need to remove the offset
}

void WorldSegment::CorrectTileForSegmentRotation(int32_t& x, int32_t& y, int32_t& z)
{
    int32_t oldx = x;
    int32_t oldy = y;

    if(segState.Rotation == 1) {
        x = segState.Size.x - oldy -1;
        y = oldx;
    }
    if(segState.Rotation == 2) {
        x = segState.Size.x - oldx -1;
        y = segState.Size.y - oldy -1;
    }
    if(segState.Rotation == 3) {
        x = oldy;
        y = segState.Size.y - oldx -1;
    }
}

//Converts a set of world coordinates into local coordinates, taking into account view rotation.
bool WorldSegment::ConvertToSegmentLocal(int32_t & x, int32_t & y, int32_t & z)
{
    int32_t lx = x;
    int32_t ly = y;
    int32_t lz = z;

    lx -= segState.Position.x;
    ly -= segState.Position.y;
    lz -= segState.Position.z;

    CorrectTileForSegmentRotation(lx, ly, lz);

    //even if we return false, we still might want to use the new coords, maybe.
    x = lx;
    y = ly;
    z = lz;

    if(lx < 0 || lx >= segState.Size.x) {
        return false;
    }
    if(ly < 0 || ly >= segState.Size.y) {
        return false;
    }
    if(lz < 0 || lz >= segState.Size.z) {
        return false;
    }

	return true;
}

uint32_t WorldSegment::ConvertLocalToIndex(int32_t x, int32_t y, int32_t z)
{
	return (uint32_t) (x + (y + (z*segState.Size.y))*segState.Size.x );
}

//Returns a blank tile at the specified world coordinates.
Tile* WorldSegment::ResetTile(int32_t x, int32_t y, int32_t z, df::tiletype type)
{
    int32_t lx = x;
    int32_t ly = y;
    int32_t lz = z;
    
	if(!ConvertToSegmentLocal(lx, ly, lz)){
		return NULL;
	}

    uint32_t index = ConvertLocalToIndex(lx, ly, lz);

	Tile* tptr = &(tiles[index]);

	Tile::InvalidateAndDestroy(tptr);
	Tile::CleanCreateAndValidate(tptr,this,type);
    tptr->x = x;
    tptr->y = y;
    tptr->z = z;
    return tptr;
}

//Returns an existing tile at the specified world coordinates.
Tile* WorldSegment::getTile(int32_t x, int32_t y, int32_t z)
{
    int32_t lx = x;
    int32_t ly = y;
    int32_t lz = z;
    
	if(!ConvertToSegmentLocal(lx, ly, lz)){
		return NULL;
	}

    uint32_t index = ConvertLocalToIndex(lx, ly, lz);

    return getTile(index);
}

Tile* WorldSegment::getTileRelativeTo(int32_t x, int32_t y, int32_t z,  dirRelative direction)
{
    int32_t lx = x;
    int32_t ly = y;
    int32_t lz = z;
    
	if(!ConvertToSegmentLocal(lx, ly, lz)){
		return NULL;
	}

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

Tile* WorldSegment::getTileRelativeTo(int32_t x, int32_t y, int32_t z,  dirRelative direction, int distance)
{
    int32_t lx = x;
    int32_t ly = y;
    int32_t lz = z;
    
	if(!ConvertToSegmentLocal(lx, ly, lz)){
		return NULL;
	}

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

Tile* WorldSegment::getTileLocal(int32_t x, int32_t y, int32_t z)
{
    if(x < 0 || x >= segState.Size.x) {
        return 0;
    }
    if(y < 0 || y >= segState.Size.y) {
        return 0;
    }
    if(z < 0 || z >= segState.Size.z) {
        return 0;
    }
	
    uint32_t index = ConvertLocalToIndex(x, y, z);

    return getTile(index);
}

Tile* WorldSegment::getTile(uint32_t index)
{
    if(index < 0 || index>=getNumTiles() ) {
        return NULL;
    }
    return tiles[index].IsValid() ? &(tiles[index]) : NULL;
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
                    (SS_Unit*) todraw[i].drawobject );
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
    int32_t vsxmax = segState.Size.x-1;
    int32_t vsymax = segState.Size.y-1;
    int32_t vszmax = segState.Size.z-1; // grabbing one tile +z more than we should for tile rules
    for(int32_t vsz=0; vsz < vszmax; vsz++) {
        //add the fog to the queue
        if(ssConfig.fogenable && fog) {
            draw_event d = {
				TintedScaledBitmap, 
                fog, 
                al_map_rgb(255,255,255), 
                0, 
                0, 
                (float)ssState.ScreenW, 
                (float)ssState.ScreenH, 
                0, 
                0, 
                (float)ssState.ScreenW, 
                (float)ssState.ScreenH, 
                0};
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


bool WorldSegment::CoordinateInsideSegment(int32_t x, int32_t y, int32_t z)
{
    int32_t lx = x;
    int32_t ly = y;
    int32_t lz = z;
    
	if(!ConvertToSegmentLocal(lx, ly, lz)){
		return NULL;
	}

    if(lx < 0 || lx >= segState.Size.x) {
        return 0;
    }
    if(ly < 0 || ly >= segState.Size.y) {
        return 0;
    }
    if(lz < 0 || lz >= segState.Size.z) {
        return 0;
    }

    return true;
}

bool WorldSegment::RangeInsideSegment(int32_t min_x, int32_t min_y, int32_t min_z, int32_t max_x, int32_t max_y, int32_t max_z)
{
    int32_t lnx = min_x;
    int32_t lny = min_y;
    int32_t lnz = min_z;
    int32_t lxx = max_x;
    int32_t lxy = max_y;
    int32_t lxz = max_z;

    ConvertToSegmentLocal(lnx, lny, lnz);
    ConvertToSegmentLocal(lxx, lxy, lxz);


    if (lxx < 0 || lnx >= segState.Size.x) {
        return 0;
    }
    if (lxy < 0 || lny >= segState.Size.y) {
        return 0;
    }
    if (lxz < 0 || lnz >= segState.Size.z) {
        return 0;
    }

    return true;
}

bool WorldSegment::CoordinateInteriorSegment(int32_t x, int32_t y, int32_t z, uint32_t shellthick)
{
    int32_t lx = x;
    int32_t ly = y;
    int32_t lz = z;
    
	if(!ConvertToSegmentLocal(lx, ly, lz)){
		return NULL;
	}

    if(lx < 0  + shellthick|| lx >= segState.Size.x - shellthick) {
        return 0;
    }
    if(ly < 0  + shellthick|| ly >= segState.Size.y - shellthick) {
        return 0;
    }
    if(lz < 0 /*bottom is "interior"*/ || lz >= segState.Size.z - shellthick) {
        return 0;
    }
    return true;
}

void WorldSegment::AssembleSprite(draw_event d)
{
    todraw.push_back(d);
}

void WorldSegment::PushBuilding( Buildings::t_building * building)
{
    buildings.push_back(building);
}

void WorldSegment::ClearBuildings()
{
    for(int i=0; i<buildings.size(); i++){
        delete(buildings[i]);
        buildings[i] = NULL;
    }
    buildings.clear();
}

void WorldSegment::PushUnit( SS_Unit * unit)
{
    units.push_back(unit);
}

void WorldSegment::ClearUnits()
{
    for(int i=0; i<units.size(); i++){
        if(units[i]){
            delete(units[i]->inv);
            delete(units[i]);
            units[i] = NULL;
        }
    }
    units.clear();
}