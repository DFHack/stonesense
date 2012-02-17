#include "common.h"
#include "WorldSegment.h"
#include "GUI.h"
#include "ContentLoader.h"


ALLEGRO_BITMAP * fog = 0;

Block* WorldSegment::getBlock(int32_t x, int32_t y, int32_t z){
	if(x < this->x || x >= this->x + this->sizex) return 0;
	if(y < this->y || y >= this->y + this->sizey) return 0;
	if(z < this->z || z >= this->z + this->sizez) return 0;
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

	correctBlockForRotation( (int32_t&)lx,(int32_t&)ly,(int32_t&)lz , rotation);

	uint32_t index = lx + (ly * this->sizex) + ((lz) * this->sizex * this->sizey);
	return blocksAsPointerVolume[index];
}

Block* WorldSegment::getBlockRelativeTo(uint32_t x, uint32_t y, uint32_t z,  dirRelative direction){
	int32_t lx = x;
	int32_t ly = y;
	int32_t lz = z;
	//make local
	lx -= this->x;
	ly -= this->y;
	lz -= this->z;

	correctBlockForRotation( (int32_t&)lx,(int32_t&)ly,(int32_t&)lz, rotation );
	switch (direction){
	case eUp:
		ly--; break;
	case eDown:
		ly++; break;
	case eLeft:
		lx--; break;
	case eRight:
		lx++; break;
	case eAbove:
		lz++; break;
	case eBelow:
		lz--; break;
	case eUpLeft:
		ly--; lx--; break;
	case eUpRight:
		ly--; lx++; break;
	case eDownLeft:
		ly++; lx--; break;
	case eDownRight:
		ly++; lx++; break;
	}

	if((int)lx < 0 || lx >= this->sizex) return 0;
	if((int)ly < 0 || ly >= this->sizey) return 0;
	if((int)lz < 0 || lz >= this->sizez) return 0;

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

	correctBlockForRotation( (int32_t&)lx,(int32_t&)ly,(int32_t&)lz, rotation);
	switch (direction){
	case eUp:
		ly-= distance; break;
	case eDown:
		ly+= distance; break;
	case eLeft:
		lx-= distance; break;
	case eRight:
		lx+= distance; break;
	case eAbove:
		lz+= distance; break;
	case eBelow:
		lz-= distance; break;
	case eUpLeft:
		ly-= distance; lx-= distance; break;
	case eUpRight:
		ly-= distance; lx+= distance; break;
	case eDownLeft:
		ly+= distance; lx-= distance; break;
	case eDownRight:
		ly+= distance; lx+= distance; break;
	}

	if((int)lx < 0 || lx >= this->sizex) return 0;
	if((int)ly < 0 || ly >= this->sizey) return 0;
	if((int)lz < 0 || lz >= this->sizez) return 0;

	uint32_t index = lx + (ly * this->sizex) + ((lz) * this->sizex * this->sizey);
	return blocksAsPointerVolume[index];
}

Block* WorldSegment::getBlockLocal(uint32_t x, uint32_t y, uint32_t z){
	if((int)x < 0 || x >= (uint32_t)this->sizex) return 0;
	if((int)y < 0 || y >= (uint32_t)this->sizey) return 0;
	if((int)z < 0 || z >= (uint32_t)this->sizez) return 0;

	uint32_t index = x + (y * this->sizex) + ((z) * this->sizex * this->sizey);
	return blocksAsPointerVolume[index];
}

Block* WorldSegment::getBlock(uint32_t index){
	if(index<0 || index >= blocks.size() )
		return 0;
	return blocks[index];
}

void correctBlockForRotation(int32_t& x, int32_t& y, int32_t& z, unsigned char rot){
	int32_t oldx = x;
	int32_t oldy = y;
	int w = config.segmentSize.x;
	int h = config.segmentSize.y;

	if(rot == 1){
		x = h - oldy -1;
		y = oldx;
	}
	if(rot == 2){
		x = w - oldx -1;
		y = h - oldy -1;
	}
	if(rot == 3){
		x = oldy;
		y = w - oldx -1;
	}
}

void WorldSegment::addBlock(Block* b){
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
	correctBlockForRotation( (int32_t&)x, (int32_t&)y, (int32_t&)z, rotation);

	uint32_t index = x + (y * this->sizex) + ((z) * this->sizex * this->sizey);
	//assert( x < sizex && x >=0);
	//assert( y < sizey && y >=0);
	//assert( z < sizez && z >=0);
	//assure not overwriting
	//if(blocksAsPointerVolume[index] != 0)
	//  assert(blocksAsPointerVolume[index] == 0);

	blocksAsPointerVolume[index] = b;
	/*Block* test = getBlock(b->x, b->y, b->z);
	if(test->x != b->x || test->y != b->y || test->z != b->z){
	test = getBlock(b->x, b->y, b->z);
	assert (test == b);
	}*/

}
/*
void WorldSegment::drawAllBlocks(ALLEGRO_BITMAP* target){
uint32_t numblocks = (uint32_t)this->blocks.size();
for(uint32_t i=0; i < numblocks; i++){
viewedSegment->blocks[i]->Draw(target);
}
}*/

void WorldSegment::drawAllBlocks(){
	if(!loaded)
		return;

	// x,y,z print pricess
	ALLEGRO_BITMAP * temp = al_get_target_bitmap();
	int32_t vsxmax = viewedSegment->sizex-1;
	int32_t vsymax = viewedSegment->sizey-1;
	int32_t vszmax = viewedSegment->sizez-1; // grabbing one tile +z more than we should for tile rules
	//al_hold_bitmap_drawing(true);

	for(int32_t vsz=0; vsz < vszmax; vsz++)
	{
		if(config.showRenderStatus)
			SetTitle("Stonesense - Drawing Terrain, Level %d/%d", (vsz+1), vszmax);
		if(config.fogenable && config.foga)
		{
			if(!fog)
			{
				fog = al_create_bitmap(al_get_bitmap_width(temp), al_get_bitmap_height(temp));
				al_set_target_bitmap(fog);
				al_clear_to_color(premultiply(al_map_rgba_f(config.fogr, config.fogg, config.fogb, config.foga)));
				al_set_target_bitmap(temp);
			}
			if(!((al_get_bitmap_width(fog) == al_get_bitmap_width(temp)) && (al_get_bitmap_height(fog) == al_get_bitmap_height(temp))))
			{
				al_destroy_bitmap(fog);
				fog = al_create_bitmap(al_get_bitmap_width(temp), al_get_bitmap_height(temp));
				al_set_target_bitmap(fog);
				al_clear_to_color(al_map_rgba_f(config.fogr*config.foga, config.fogg*config.foga, config.fogb*config.foga, config.foga));
				al_set_target_bitmap(temp);
			}
			al_draw_bitmap(fog, 0, 0, 0);
		}
		if(vsz == vszmax-1)
		{
			if (config.show_osd) DrawCurrentLevelOutline(true);
		}
		if(config.dayNightCycle)
		al_hold_bitmap_drawing(true);
		for(int32_t vsx=1; vsx < vsxmax; vsx++)
		{
			for(int32_t vsy=1; vsy < vsymax; vsy++)
			{
				Block *b = getBlockLocal(vsx,vsy,vsz);
				if (b==NULL || (b->floorType == 0 && b->ramp.type == 0 && b->wallType == 0))
				{
					Block* bLow = getBlockLocal(vsx,vsy,vsz-1);
					if (bLow != NULL)
					{
						bLow->DrawRamptops();
					}
				}
				if (b)
				{
					b->Draw();
					//while(!key[KEY_SPACE]) ;
					//rest(100);
				}
			}
		}
		al_hold_bitmap_drawing(false);
		al_hold_bitmap_drawing(true);
		for(int32_t vsx=1; vsx < vsxmax; vsx++)
		{
			for(int32_t vsy=1; vsy < vsymax; vsy++)
			{
				Block *b = getBlockLocal(vsx,vsy,vsz);
				if (b)
				{
					b->Drawcreaturetext();
					//while(!key[KEY_SPACE]) ;
					//rest(100);
				}
			}
		}
		al_hold_bitmap_drawing(false);
	}
	if(config.showRenderStatus)
		SetTitle("Stonesense");
}

/*
void WorldSegment::drawAllBlocks(ALLEGRO_BITMAP* target){
// x,y,z print pricess
int32_t vsxmax = viewedSegment->x + viewedSegment->sizex;
int32_t vsymax = viewedSegment->y + viewedSegment->sizey;
int32_t vszmax = viewedSegment->z + viewedSegment->sizez;
for(int32_t vsz=viewedSegment->z; vsz < vszmax; vsz++){
switch (DisplayedRotation * 0){
case 0:
for(int32_t vsx=viewedSegment->x; vsx < vsxmax; vsx++){
for(int32_t vsy=viewedSegment->y; vsy < vsymax; vsy++){
Block *b = getBlock(vsx,vsy,vsz);
if (b)
{
b->Draw(screen);
//while(!key[KEY_SPACE]);
//while(key[KEY_SPACE]);
}
}
}
break;
case 1:
for(int32_t vsx=viewedSegment->x; vsx < vsxmax; vsx++){
for(int32_t vsy=vsymax-1; vsy >= viewedSegment->y; vsy--){
Block *b = getBlock(vsx,vsy,vsz);
if (b)
{
b->Draw(target);

}
}
}
break;
case 2:
for(int32_t vsx=vsxmax-1; vsx >= viewedSegment->x; vsx--){
for(int32_t vsy=vsymax-1; vsy >= viewedSegment->y; vsy--){
Block *b = getBlock(vsx,vsy,vsz);
if (b)
{
b->Draw(target);
}
}
}
break;
case 3:
for(int32_t vsx=vsxmax-1; vsx >= viewedSegment->x; vsx--){
for(int32_t vsy=viewedSegment->y; vsy < vsymax; vsy++){
Block *b = getBlock(vsx,vsy,vsz);
if (b)
{
b->Draw(target);
}
}
}
break;
}
}
}
*/
/*void WorldSegment::drawAllBlocks(ALLEGRO_BITMAP* target){
// x,y,z print pricess
int32_t vsxmax = viewedSegment->x + viewedSegment->sizex;
int32_t vsymax = viewedSegment->y + viewedSegment->sizey;
int32_t vszmax = viewedSegment->z + viewedSegment->sizez;
for(int32_t vsz=viewedSegment->z; vsz < vszmax; vsz++){
for(int32_t vsx=viewedSegment->x; vsx < vsxmax; vsx++){
for(int32_t vsy=viewedSegment->y; vsy < vsymax; vsy++){
Block *b = getBlock(vsx,vsy,vsz);
if (b)
{
b->Draw(target);
}
}
}
}
} */


bool WorldSegment::CoordinateInsideSegment(uint32_t x, uint32_t y, uint32_t z){
	/*if( x < 0 || (int32_t)x >= this->regionSize.x) return false;
	if( y < 0 || (int32_t)y >= this->regionSize.y) return false;
	if( z < 0 || (int32_t)z >= this->regionSize.z) return false;*/
	if( (int32_t)x < this->x || (int32_t)x >= this->x + this->sizex) return false;
	if( (int32_t)y < this->y || (int32_t)y >= this->y + this->sizey) return false;
	if( (int32_t)z < this->z || (int32_t)z >= this->z + this->sizez) return false;
	return true;
}

void WorldSegment::drawPixels()
{
	for(int32_t vsx=0; vsx < this->sizex; vsx++)
		for(int32_t vsy=0; vsy < this->sizey; vsy++)
			for(int32_t vsz=0; vsz < this->sizez; vsz++)
			{
				Block *b = getBlockLocal(vsx,vsy,vsz);
				if (b)
					b->DrawPixel(vsx, (vsz*this->sizey) + vsy);
			}
}