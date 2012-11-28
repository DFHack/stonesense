#include "common.h"
#include <bitset>
#include "Block.h"
#include "WorldSegment.h"
#include "GUI.h"

#define SCALE_FACTOR 4

#define S_TILE_HEIGHT 4
#define S_FLOOR_HEIGHT 1
#define S_WALL_HEIGHT 4
#define S_BLOCK_HEIGHT (S_FLOOR_HEIGHT+S_WALL_HEIGHT)
#define S_SPRITE_HEIGHT (S_BLOCK_HEIGHT+S_TILE_HEIGHT)

bitset<2*S_SPRITE_HEIGHT> base_mask_left;
bitset<2*S_SPRITE_HEIGHT> base_mask_right;
bitset<2*S_SPRITE_HEIGHT> wall_mask_left;
bitset<2*S_SPRITE_HEIGHT> wall_mask_right;
bitset<2*S_SPRITE_HEIGHT> floor_mask_left;
bitset<2*S_SPRITE_HEIGHT> floor_mask_right;

bool is_block_solid(Block * b)
{
    if(!config.shade_hidden_blocks && !config.show_hidden_blocks && b->designation.bits.hidden) {
        return false;
    }
    if(b->material.type == 3 ||
            b->material.type == 4 ||
            b->material.type == 5 ||
            b->material.type == 6) {
        return false;
    }
    return IDhasOpaqueSides(b->tileType) || IDhasOpaqueFloor(b->tileType);
}

void mask_center(Block * b, int offset)
{
    if(!b) {
        return;
    }
    if(!is_block_solid(b)) {
        return;
    }
    if(IDhasOpaqueSides(b->tileType)) {
        if(offset >= 0) {
            base_mask_left &= ~(wall_mask_left << offset*2);
            base_mask_right &= ~(wall_mask_right << offset*2);
        } else {
            base_mask_left &= ~(wall_mask_left >> -offset*2);
            base_mask_right &= ~(wall_mask_right >> -offset*2);
        }
    } else if(IDhasOpaqueFloor(b->tileType)) {
        if(offset >= 0) {
            base_mask_left &= ~(floor_mask_left << offset*2);
            base_mask_right &= ~(floor_mask_right << offset*2);
        } else {
            base_mask_left &= ~(floor_mask_left >> -offset*2);
            base_mask_right &= ~(floor_mask_right >> -offset*2);
        }
    }
}

void mask_left(Block * b, int offset)
{
    if(!b) {
        return;
    }
    if(!is_block_solid(b)) {
        return;
    }
    if(IDhasOpaqueSides(b->tileType)) {
        if(offset >= 0) {
            base_mask_left &= ~(wall_mask_right << offset*2);
        } else {
            base_mask_left &= ~(wall_mask_right >> -offset*2);
        }
    } else if(IDhasOpaqueFloor(b->tileType)) {
        if(offset >= 0) {
            base_mask_left &= ~(floor_mask_right << offset*2);
        } else {
            base_mask_left &= ~(floor_mask_right >> -offset*2);
        }
    }
}

void mask_right(Block * b, int offset)
{
    if(!b) {
        return;
    }
    if(!is_block_solid(b)) {
        return;
    }
    if(IDhasOpaqueSides(b->tileType)) {
        if(offset >= 0) {
            base_mask_right &= ~(wall_mask_left << offset*2);
        } else {
            base_mask_right &= ~(wall_mask_left >> -offset*2);
        }
    } else if(IDhasOpaqueFloor(b->tileType)) {
        if(offset >= 0) {
            base_mask_right &= ~(floor_mask_left << offset*2);
        } else {
            base_mask_right &= ~(floor_mask_left >> -offset*2);
        }
    }
}

void init_masks()
{
    //load up some mask files
    int flags = al_get_new_bitmap_flags();
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    ALLEGRO_BITMAP * wall = load_bitmap_withWarning("stonesense/mask_wall.png");
    ALLEGRO_BITMAP * floor = load_bitmap_withWarning("stonesense/mask_floor.png");
    al_set_new_bitmap_flags(flags);

    //copy the mask to the bitsets. Each half is seperate to simplify things.
    for(int y = 0; y < S_SPRITE_HEIGHT; y++) {
        wall_mask_left.set(0 + (y*2), al_get_pixel(wall, 0, y).r > 0.5);
        wall_mask_left.set(1 + (y*2), al_get_pixel(wall, 1, y).r > 0.5);
        wall_mask_right.set(0 + (y*2), al_get_pixel(wall, 2, y).r > 0.5);
        wall_mask_right.set(1 + (y*2), al_get_pixel(wall, 3, y).r > 0.5);
    }
    for(int y = 0; y < S_SPRITE_HEIGHT; y++) {
        floor_mask_left.set(0 + (y*2), al_get_pixel(floor, 0, y).r > 0.5);
        floor_mask_left.set(1 + (y*2), al_get_pixel(floor, 1, y).r > 0.5);
        floor_mask_right.set(0 + (y*2), al_get_pixel(floor, 2, y).r > 0.5);
        floor_mask_right.set(1 + (y*2), al_get_pixel(floor, 3, y).r > 0.5);
    }

    al_destroy_bitmap(wall);
    al_destroy_bitmap(floor);

    base_mask_left.set();
    base_mask_right.set();
    int offset = 4;
    if(offset >= 0) {
        base_mask_left &= ~(wall_mask_left << offset*2);
        base_mask_right &= ~(wall_mask_right << offset*2);
    } else {
        base_mask_left &= ~(wall_mask_left >> -offset*2);
        base_mask_right &= ~(wall_mask_right >> -offset*2);
    }
}

void occlude_block(Block * b)
{
    WorldSegment * segment = b->ownerSegment;
    base_mask_left.set();
    base_mask_right.set();
    int baseX = b->x;
    int baseY = b->y;
    int baseZ = b->z;

    int stepX, stepY;

    switch(b->ownerSegment->rotation) {
    case 0:
        stepX = 1;
        stepY = 1;
        break;
    case 1:
        stepX = 1;
        stepY = -1;
        break;
    case 2:
        stepX = -1;
        stepY = -1;
        break;
    case 3:
        stepX = -1;
        stepY = 1;
        break;
    };

    bool done = 0;
    for(int relZ = 0; !done; relZ++) {
        int stepZ = relZ * S_BLOCK_HEIGHT / S_SPRITE_HEIGHT;
        if(relZ > 0) {
            stepZ--;
        }

        for(int relXY = 0; (relXY <= (((TILEHEIGHT + BLOCKHEIGHT) / (TILEHEIGHT))+1)) && !done ; relXY++) {
            int tempX = baseX + ((relXY + stepZ) * stepX);
            int tempY = baseY + ((relXY + stepZ) * stepY);
            int tempZ = baseZ + relZ;

            bool centerin = segment->CoordinateInteriorSegment(tempX + stepX, tempY + stepY, tempZ, 1);
            if(centerin) {
                Block* center = segment->getBlock(tempX + stepX, tempY + stepY, tempZ);
                mask_center(center, ((relXY+stepZ) * S_TILE_HEIGHT + S_TILE_HEIGHT) - (relZ * S_BLOCK_HEIGHT));
                if(base_mask_left.none() && base_mask_right.none()) {
                    done = true;
                    break;
                }
            }
            bool leftin = segment->CoordinateInteriorSegment(tempX, tempY + stepY, tempZ, 1);
            if(leftin) {
                Block* left = segment->getBlock(tempX, tempY + stepY, tempZ);
                mask_left(left, ((relXY+stepZ) * S_TILE_HEIGHT + S_TILE_HEIGHT/2) - (relZ * S_BLOCK_HEIGHT));
                if(base_mask_left.none() && base_mask_right.none()) {
                    done = true;
                    break;
                }
            }
            bool rightin = segment->CoordinateInteriorSegment(tempX + stepX, tempY, tempZ, 1);
            if(rightin) {
                Block* right = segment->getBlock(tempX + stepX, tempY, tempZ);
                mask_right(right, ((relXY+stepZ) * S_TILE_HEIGHT + S_TILE_HEIGHT/2) - (relZ * S_BLOCK_HEIGHT));
                if(base_mask_left.none() && base_mask_right.none()) {
                    done = true;
                    break;
                }
            }
            if(!leftin || !rightin || !centerin) {
                done = true;
                break;
            }
        }
    }

    if(base_mask_left.none() && base_mask_right.none()) {
        b->visible = 0;
    }
}