#include "common.h"
#include <bitset>
#include "block.h"
#include "WorldSegment.h"
#include "GUI.h"

bitset<TILEHEIGHT+WALLHEIGHT+FLOORHEIGHT> base_mask_left;
bitset<TILEHEIGHT+WALLHEIGHT+FLOORHEIGHT> base_mask_right;
bitset<TILEHEIGHT+WALLHEIGHT+FLOORHEIGHT> wall_mask_center;
bitset<TILEHEIGHT+WALLHEIGHT+FLOORHEIGHT> wall_mask_side;
bitset<TILEHEIGHT+WALLHEIGHT+FLOORHEIGHT> floor_mask_center;
bitset<TILEHEIGHT+WALLHEIGHT+FLOORHEIGHT> floor_mask_side;

void init_masks()
{
	base_mask_left.set();
	base_mask_right.set();
	wall_mask_center.set();
	wall_mask_side.set();
	floor_mask_center.set();
	floor_mask_side.set();

	for(int i = 0; i < (TILEHEIGHT / 2 ) ; i++)
	{
		if(i >= wall_mask_side.size())
			break;
		wall_mask_side.set(i, false);
	}
	for(int i = 0; i < (WALLHEIGHT) ; i++)
	{
		if(i >= floor_mask_center.size())
			break;
		floor_mask_center.set(i, false);
	}
	for(int i = 0; i < (WALLHEIGHT + (TILEHEIGHT / 2) ) ; i++)
	{
		if(i >= floor_mask_side.size())
			break;
		floor_mask_side.set(i, false);
	}
	for(int i = TILEHEIGHT+WALLHEIGHT+FLOORHEIGHT - 1; i >= (WALLHEIGHT+FLOORHEIGHT + (TILEHEIGHT / 2 )) ; i--)
	{
		if(i >= floor_mask_side.size())
			break;
		floor_mask_side.set(i, false);
		wall_mask_side.set(i, false);
	}
}



bool is_block_solid(Block * b)
{
	//fixme: glass, etc, needs to return false.
	return true;
}

void mask_center(Block * b, int offset)
{
	if(!b)
		return;
	if(!is_block_solid(b))
		return;
	if(b->wallType)
	{
		if(offset >= 0)
		{
			base_mask_left &= ~(wall_mask_center << offset);
			base_mask_right &= ~(wall_mask_center << offset);
		}
		else
		{
			base_mask_left &= ~(wall_mask_center >> abs(offset));
			base_mask_right &= ~(wall_mask_center >> abs(offset));
		}
	}
	else if(b->floorType)
	{
		if(offset >= 0)
		{
			base_mask_left &= ~(floor_mask_center << offset);
			base_mask_right &= ~(floor_mask_center << offset);
		}
		else
		{
			base_mask_left &= ~(floor_mask_center >> abs(offset));
			base_mask_right &= ~(floor_mask_center >> abs(offset));
		}
	}
}

void mask_left(Block * b, int offset)
{
	if(!b)
		return;
	if(!is_block_solid(b))
		return;
	if(b->wallType)
	{
		if(offset >= 0)
		{
			base_mask_left &= ~(wall_mask_side << offset);
		}
		else
		{
			base_mask_left &= ~(wall_mask_side >> abs(offset));
		}
	}
	else if(b->floorType)
	{
		if(offset >= 0)
		{
			base_mask_left &= ~(floor_mask_center << offset);
		}
		else
		{
			base_mask_left &= ~(floor_mask_center >> abs(offset));
		}
	}
}

void mask_right(Block * b, int offset)
{
	if(!b)
		return;
	if(!is_block_solid(b))
		return;
	if(b->wallType)
	{
		if(offset >= 0)
		{
			base_mask_right &= ~(wall_mask_side << offset);
		}
		else
		{
			base_mask_right &= ~(wall_mask_side >> abs(offset));
		}
	}
	else if(b->floorType)
	{
		if(offset >= 0)
		{
			base_mask_right &= ~(floor_mask_center << offset);
		}
		else
		{
			base_mask_right &= ~(floor_mask_center >> abs(offset));
		}
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
		int stepZ = relZ * (BLOCKHEIGHT) / (TILEHEIGHT + BLOCKHEIGHT);

		for(int relXY = 0; (relXY <= (((TILEHEIGHT + BLOCKHEIGHT) / (TILEHEIGHT))+1)) && !done; relXY++)
		{
			int tempX = baseX + ((relXY + stepZ) * stepX);
			int tempY = baseY + ((relXY + stepZ) * stepY);
			int tempZ = baseZ + relZ;
			//DFConsole->print("Base: %d,%d,%d. Current: %d,%d,%d. Offset: %d. \n", baseX, baseY, baseZ, tempX + stepX, tempY + stepY, tempZ, (((relXY + stepZ) + ((relXY + stepZ)) * TILEHEIGHT / 2) - (relZ * BLOCKHEIGHT)));
			mask_center(segment->getBlock(tempX + stepX, tempY + stepY, tempZ), ((relXY + stepZ) + ((relXY + stepZ)) * TILEHEIGHT / 2) - (relZ * BLOCKHEIGHT));
			if(base_mask_left.none() && base_mask_right.none())
			{
				done = true;
				break;
			}
			//mask_left(segment->getBlockLocal(tempX, tempY+stepY, tempZ), (((relXY + relXY+stepY) * TILEHEIGHT / 2) - (relZ * BLOCKHEIGHT)));
			//if(base_mask_left.none() && base_mask_right.none())
			//{
			//	done = true;
			//	break;
			//}
			//mask_right(segment->getBlockLocal(tempX+stepX, tempY, tempZ), (((relXY+stepX + relXY) * TILEHEIGHT / 2) - (relZ * BLOCKHEIGHT)));
			//if(base_mask_left.none() && base_mask_right.none())
			//{
			//	done = true;
			//	break;
			//}
		}
	}

	if(base_mask_left.none() && base_mask_right.none())
	{
		b->visible = 0;
	}
}