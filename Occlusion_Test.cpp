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
	if(b->tileMaterial == tiletype_shape::RAMP_TOP)
		return false;
	if(!config.shade_hidden_blocks && !config.show_hidden_blocks && b->designation.bits.hidden)
		return false;
	//fixme: glass, etc, needs to return false.
	if(
		b->material.type == 3 ||
		b->material.type == 4 ||
		b->material.type == 5 ||
		b->material.type == 6)
		return false;
	if(b->tileMaterial == tiletype_shape::BROOK_TOP)
		return false;
	return true;
}

void mask_center(Block * b, int offset)
{
	if(!b)
		return;
	if(!is_block_solid(b))
		return;
	if(b->tileShapeBasic==tiletype_shape_basic::Wall && b->tileMaterial != tiletype_shape::BROOK_BED)
	{
		if(offset >= 0)
		{
			base_mask_left &= ~(wall_mask_left << offset*2);
			base_mask_right &= ~(wall_mask_right << offset*2);
		}
		else
		{
			base_mask_left &= ~(wall_mask_left >> -offset*2);
			base_mask_right &= ~(wall_mask_right >> -offset*2);
		}
	}
	else if(
		b->tileShapeBasic==tiletype_shape_basic::Floor || 
		b->tileShapeBasic==tiletype_shape_basic::Ramp || 
		b->tileMaterial != tiletype_shape::BROOK_BED
		)
	{
		if(offset >= 0)
		{
			base_mask_left &= ~(floor_mask_left << offset*2);
			base_mask_right &= ~(floor_mask_right << offset*2);
		}
		else
		{
			base_mask_left &= ~(floor_mask_left >> -offset*2);
			base_mask_right &= ~(floor_mask_right >> -offset*2);
		}
	}
}

void mask_left(Block * b, int offset)
{
	if(!b)
		return;
	if(!is_block_solid(b))
		return;
	if(b->tileShapeBasic==tiletype_shape_basic::Wall && b->tileMaterial != tiletype_shape::BROOK_BED)
	{
		if(offset >= 0)
		{
			base_mask_left &= ~(wall_mask_right << offset*2);
		}
		else
		{
			base_mask_left &= ~(wall_mask_right >> -offset*2);
		}
	}
	else if(b->tileShapeBasic==tiletype_shape_basic::Floor || b->tileShapeBasic==tiletype_shape_basic::Ramp || b->tileMaterial != tiletype_shape::BROOK_BED)
	{
		if(offset >= 0)
		{
			base_mask_left &= ~(floor_mask_right << offset*2);
		}
		else
		{
			base_mask_left &= ~(floor_mask_right >> -offset*2);
		}
	}
}

void mask_right(Block * b, int offset)
{
	if(!b)
		return;
	if(!is_block_solid(b))
		return;
	if(b->tileShapeBasic==tiletype_shape_basic::Wall && b->tileMaterial != tiletype_shape::BROOK_BED)
	{
		if(offset >= 0)
		{
			base_mask_right &= ~(wall_mask_left << offset*2);
		}
		else
		{
			base_mask_right &= ~(wall_mask_left >> -offset*2);
		}
	}
	else if(b->tileShapeBasic==tiletype_shape_basic::Floor || b->tileShapeBasic==tiletype_shape_basic::Ramp || b->tileMaterial != tiletype_shape::BROOK_BED)
	{
		if(offset >= 0)
		{
			base_mask_right &= ~(floor_mask_left << offset*2);
		}
		else
		{
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
	for(int y = 0; y < S_SPRITE_HEIGHT; y++)
	{
		wall_mask_left.set(0 + (y*2), al_get_pixel(wall, 0, y).r > 0.5);
		wall_mask_left.set(1 + (y*2), al_get_pixel(wall, 1, y).r > 0.5);
		wall_mask_right.set(0 + (y*2), al_get_pixel(wall, 2, y).r > 0.5);
		wall_mask_right.set(1 + (y*2), al_get_pixel(wall, 3, y).r > 0.5);
	}
	for(int y = 0; y < S_SPRITE_HEIGHT; y++)
	{
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
	if(offset >= 0)
	{
		base_mask_left &= ~(wall_mask_left << offset*2);
		base_mask_right &= ~(wall_mask_right << offset*2);
	}
	else
	{
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

	int distX = b->ownerSegment->x + b->ownerSegment->sizex - b->x - 1;
	int distY = b->ownerSegment->y + b->ownerSegment->sizey - b->y - 1;
	int distZ = b->ownerSegment->z + b->ownerSegment->sizez - b->z - 1;
	
	if(distX == 1 || distY == 1 || distZ == 1)
		return;

	int stepX, stepY;

	switch(b->ownerSegment->rotation){
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
	for(int relZ = 0; (relZ < distZ) && !done; relZ++)
	{
		int stepZ = relZ * S_BLOCK_HEIGHT / S_SPRITE_HEIGHT;
		if(relZ > 0)
			stepZ--;

		for(int relXY = 0; (relXY <= (((TILEHEIGHT + BLOCKHEIGHT) / (TILEHEIGHT))+1)) && !done && relXY <= distX && relXY <= distY; relXY++)
		{
			int tempX = baseX + ((relXY + stepZ) * stepX);
			int tempY = baseY + ((relXY + stepZ) * stepY);
			int tempZ = baseZ + relZ;
			//DFConsole->print("Base: %d,%d,%d. Current: %d,%d,%d. Offset: %d. \n", baseX, baseY, baseZ, tempX + stepX, tempY + stepY, tempZ, (((relXY + stepZ) + ((relXY + stepZ)) * TILEHEIGHT / 2) - (relZ * BLOCKHEIGHT)));
			mask_center(segment->getBlock(tempX + stepX, tempY + stepY, tempZ), ((relXY+stepZ) * S_TILE_HEIGHT + S_TILE_HEIGHT) - (relZ * S_BLOCK_HEIGHT));
			if(base_mask_left.none() && base_mask_right.none())
			{
				done = true;
				break;
			}
			mask_left(segment->getBlock(tempX, tempY + stepY, tempZ), ((relXY+stepZ) * S_TILE_HEIGHT + S_TILE_HEIGHT/2) - (relZ * S_BLOCK_HEIGHT));
			if(base_mask_left.none() && base_mask_right.none())
			{
				done = true;
				break;
			}
			mask_right(segment->getBlock(tempX + stepX, tempY, tempZ), ((relXY+stepZ) * S_TILE_HEIGHT + S_TILE_HEIGHT/2) - (relZ * S_BLOCK_HEIGHT));
			if(base_mask_left.none() && base_mask_right.none())
			{
				done = true;
				break;
			}
		}
	}

	if(base_mask_left.none() && base_mask_right.none())
	{
		b->visible = 0;
	}
}