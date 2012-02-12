#include "common.h"
#include "GUI.h"
#include "SpriteMaps.h"
#include "GameBuildings.h"
#include "Creatures.h"
#include "WorldSegment.h"
#include "BlockFactory.h"
#include "ContentLoader.h"
#include "SpriteColors.h"

ALLEGRO_BITMAP *sprite_miasma = 0;
ALLEGRO_BITMAP *sprite_water = 0;
ALLEGRO_BITMAP *sprite_water2 = 0;
ALLEGRO_BITMAP *sprite_blood = 0;
ALLEGRO_BITMAP *sprite_dust = 0;
ALLEGRO_BITMAP *sprite_magma = 0;
ALLEGRO_BITMAP *sprite_smoke = 0;
ALLEGRO_BITMAP *sprite_dragonfire = 0;
ALLEGRO_BITMAP *sprite_fire = 0;
ALLEGRO_BITMAP *sprite_webing = 0;
ALLEGRO_BITMAP *sprite_boiling = 0;
ALLEGRO_BITMAP *sprite_oceanwave = 0;

int randomCube[RANDOM_CUBE][RANDOM_CUBE][RANDOM_CUBE];

void initRandomCube()
{
	for(int i = 0; i < RANDOM_CUBE; i++)
		for(int j = 0; j < RANDOM_CUBE; j++)
			for(int k = 0; k < RANDOM_CUBE; k++)
				randomCube[i][j][k] = rand();
}

Block::Block(WorldSegment* ownerSegment)
{
	//clear out own memory
	memset(this, 0, sizeof(Block));

	this->ownerSegment = ownerSegment;

	building.info.type = BUILDINGTYPE_NA;
	building.index = -1;
	building.parent = 0;

	this->material.type = INVALID_INDEX;
	this->material.index = INVALID_INDEX;

	wallborders = 0;
	floorborders = 0;
	openborders = 255;
	rampborders = 0;
	upstairborders = 0;
	downstairborders = 0;
	lightborders = 255;
	creature = 0;
	engraving_character = 0;
	visible = true;
}



Block::~Block(void){
	if( creature )
		delete(creature);
}

void* Block::operator new (size_t size){
	return blockFactory.allocateBlock( );
}
void Block::operator delete (void *p){
	blockFactory.deleteBlock( (Block*)p );
}

inline ALLEGRO_BITMAP* imageSheet(t_SpriteWithOffset sprite, ALLEGRO_BITMAP* defaultBmp)
{
	if (sprite.fileIndex == -1)
	{
		return defaultBmp;
	}
	else
	{
		return getImgFile(sprite.fileIndex);
	}
}

inline ALLEGRO_BITMAP* imageSheet(t_subSprite sprite, ALLEGRO_BITMAP* defaultBmp)
{
	if (sprite.fileIndex == -1)
	{
		return defaultBmp;
	}
	else
	{
		return getImgFile(sprite.fileIndex);
	}
}

void Block::Draw()
{
	if(!visible)
		return;
	if((material.type == INORGANIC) && (material.index == -1))
	{
		material.index = 0;
	}

	
	if((!(this->designation.bits.hidden) || config.show_hidden_blocks) && config.block_count)
	{
		config.tilecount ++;
	}

	bool defaultSnow = 1;
	int sheetOffsetX, sheetOffsetY;
	t_SpriteWithOffset sprite;
	c_sprite* spriteobject;
	/*if(config.hide_outer_blocks){
	if(x == ownerSegment->x || x == ownerSegment->x + ownerSegment->sizex - 1) return;
	if(y == ownerSegment->y || y == ownerSegment->y + ownerSegment->sizey - 1) return;
	}*/

	drawx = x;
	drawy = y;
	drawz = z; //- ownerSegment->sizez + 1;


	correctBlockForSegmetOffset( drawx, drawy, drawz);
	correctBlockForRotation( drawx, drawy, drawz, ownerSegment->rotation);
	pointToScreen((int*)&drawx, (int*)&drawy, drawz);
	drawx -= (TILEWIDTH>>1)*config.scale;

	if(((drawx + TILEWIDTH*config.scale) < 0) || (drawx > al_get_bitmap_width(al_get_target_bitmap())) || ((drawy + (TILEHEIGHT + FLOORHEIGHT)*config.scale) < 0) || (drawy - WALLHEIGHT*config.scale > al_get_bitmap_height(al_get_target_bitmap())))
		return;

	bool chopThisBlock = 0;

	if(config.truncate_walls == 1) chopThisBlock = 1;
	else if(config.truncate_walls == 2 && obscuringCreature == 1) chopThisBlock = 1;
	else if(config.truncate_walls == 3 && (obscuringCreature == 1 || obscuringBuilding == 1)) chopThisBlock = 1;
	else if(config.truncate_walls == 4 && obscuringBuilding == 1) chopThisBlock = 1;

	if(building.info.type == BUILDINGTYPE_BLACKBOX)
	{
		DrawSpriteFromSheet( SPRITEOBJECT_BLACK, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy+FLOORHEIGHT*config.scale, this);
		DrawSpriteFromSheet( SPRITEOBJECT_BLACK, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this);
		return;
	}


	ALLEGRO_COLOR tileBorderColor = al_map_rgb(85,85,85);
	int rando = randomCube[x%RANDOM_CUBE][y%RANDOM_CUBE][z%RANDOM_CUBE];
	//Draw Floor
	if(floorType > 0 || wallType > 0 || ramp.type > 0 || stairType > 0){

		//If tile has no floor, look for a Filler Floor from it's wall
		if (floorType > 0)
		{
			spriteobject = GetFloorSpriteMap(floorType, this->material, consForm);
		}
		else if (wallType > 0)
		{
			spriteobject = GetFloorSpriteMap(wallType, this->material, consForm);
		}
		else if (ramp.type > 0)
		{
			spriteobject = GetFloorSpriteMap(ramp.type, this->material, consForm);
		}
		else if (stairType > 0)
		{
			spriteobject = GetFloorSpriteMap(stairType, this->material, consForm);
		}

		if(spriteobject->get_sheetindex() != INVALID_INDEX)
		{
			////if floor is muddy, override regular floor
			//if( mudlevel && water.index == 0)
			//{
			//	sprite.sheetIndex = SPRITEFLOOR_WATERFLOOR;
			//	sprite.fileIndex = INVALID_INDEX;
			//}
			////if floor is snowed down, override  regular floor
			//if( snowlevel )
			//{
			//	sprite.sheetIndex = SPRITEFLOOR_SNOW;
			//	sprite.fileIndex = INVALID_INDEX;
			//	spriteOffset = 0;
			//	al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
			//}

			if (spriteobject->get_sheetindex() == UNCONFIGURED_INDEX)
			{
				spriteobject->set_sheetindex(SPRITEOBJECT_FLOOR_NA);
				spriteobject->set_fileindex(INVALID_INDEX);
				spriteobject->set_offset(0, WALLHEIGHT);
				spriteobject->draw_world(x, y, z, this);
			}
			else
				spriteobject->draw_world(x, y, z, this);
		}
	}

	//Floor Engravings
		if((floorType > 0) && engraving_character && engraving_flags.bits.floor)
	{
		DrawSpriteFromSheet( engraving_character, IMGEngFloorSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
	}


	////draw surf
	//if(eff_oceanwave > 0)
	//{
	//	al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color*al_map_rgba(255, 255, 255, (255*eff_oceanwave)/100));
	//	al_draw_tinted_bitmap(sprite_oceanwave, drawx, drawy - (WALLHEIGHT), 0);
	//	al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
	//}
	//if(eff_webing > 0)
	//{
	//	al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color*al_map_rgba(255, 255, 255, (255*eff_webing)/100));
	//	al_draw_tinted_bitmap(sprite_webing, drawx, drawy - (WALLHEIGHT), 0);
	//	al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
	//}

	//Draw Ramp
	if(ramp.type > 0){
		spriteobject = GetBlockSpriteMap(ramp.type, material, consForm);
		if (spriteobject->get_sheetindex() == UNCONFIGURED_INDEX)
		{
			spriteobject->set_sheetindex(0);
			spriteobject->set_fileindex(INVALID_INDEX);
			spriteobject->set_defaultsheet(IMGRampSheet);
		}
		if (spriteobject->get_sheetindex() != INVALID_INDEX)
		{
			spriteobject->set_size(SPRITEWIDTH, SPRITEHEIGHT);
			spriteobject->set_tile_layout(RAMPBOTTOMTILE);
			spriteobject->draw_world(x, y, z, this, (chopThisBlock && this->z == ownerSegment->z + ownerSegment->sizez -2));
		}
		spriteobject->set_tile_layout(BLOCKTILE);
	}


	drawFloorBlood ( this, drawx, drawy );
	//first part of snow
	if(ramp.type == 0 && wallType == 0 && stairType == 0 && defaultSnow)
	{
		if(snowlevel > 75)
		{
			DrawSpriteFromSheet( 20, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this);
		}
		else if(snowlevel > 50)
		{
			DrawSpriteFromSheet( 21, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this );
		}
		else if(snowlevel > 25)
		{
			DrawSpriteFromSheet( 22, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this );
		}
		else if(snowlevel > 0)
		{
			DrawSpriteFromSheet( 23, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this );
		}
	}


	////vegetation
	//if(tree.index > 0 || tree.type > 0){
	//	c_sprite * vegetationsprite = 0;
	//	vegetationsprite = GetSpriteVegetation( (TileClass) getVegetationType( this->floorType ), tree.index );
	//	if(vegetationsprite)
	//		vegetationsprite->draw_world(x, y, z);
	//}



	//shadow
	if (shadow > 0)
	{
		DrawSpriteFromSheet( BASE_SHADOW_TILE + shadow - 1, IMGObjectSheet, al_map_rgb(255,255,255), drawx, (ramp.type > 0)?(drawy - ((WALLHEIGHT/2)*config.scale)):drawy , this);
	}

	//Building
	bool skipBuilding =
		(building.info.type == contentLoader->civzoneNum && !config.show_stockpiles) ||
		(building.info.type == contentLoader->stockpileNum && !config.show_zones);

	if(building.info.type != BUILDINGTYPE_NA && !skipBuilding)
	{
		for(uint32_t i=0; i < building.sprites.size(); i++)
		{
			spriteobject = &building.sprites[i];
			if(building.parent)
				spriteobject->draw_world(x, y, z, building.parent);
			else
				spriteobject->draw_world(x, y, z, this);
		}
	}

	//Draw Stairs
	if(stairType > 0)
	{
		bool mirrored = false;
		if(findWallCloseTo(ownerSegment, this) == eSimpleW)
			mirrored = true;

		//down part
		spriteobject = GetFloorSpriteMap(stairType, material, consForm);
		if(spriteobject->get_sheetindex() != INVALID_INDEX && spriteobject->get_sheetindex() != UNCONFIGURED_INDEX)
		{
			if (mirrored)
				spriteobject->draw_world_offset(x, y, z, this, 1);
			else
				spriteobject->draw_world(x, y, z, this);
		}

		//up part
		spriteobject = GetBlockSpriteMap(stairType, material, consForm);
		if(spriteobject->get_sheetindex() != INVALID_INDEX && spriteobject->get_sheetindex() != UNCONFIGURED_INDEX)
		{
			if (mirrored)
				spriteobject->draw_world_offset(x, y, z, this, 1);
			else
				spriteobject->draw_world(x, y, z, this);
		}
	}

	if(wallType > 0)
	{
		//draw wall
		spriteobject =  GetBlockSpriteMap(wallType, material, consForm);
		int spriteOffset = 0;
		if (spriteobject->get_sheetindex() == UNCONFIGURED_INDEX)
		{
			spriteobject->set_sheetindex(SPRITEOBJECT_WALL_NA);
			spriteobject->set_fileindex(INVALID_INDEX);
				spriteobject->set_tile_layout(BLOCKTILE);
				spriteobject->set_defaultsheet(IMGObjectSheet);
		}
		if (spriteobject->get_sheetindex() == INVALID_INDEX)
		{
			//skip   
		}    
		else 
		{
			spriteobject->draw_world(x, y, z, this, (chopThisBlock && this->z == ownerSegment->z + ownerSegment->sizez -2));
		}
	}

	//Wall Engravings
	if((wallType > 0) && engraving_character)
	{
		if(ownerSegment->rotation == 0)
		{
            if(engraving_flags.bits.east)
				DrawSpriteFromSheet( engraving_character, IMGEngRightSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            if(engraving_flags.bits.south)
				DrawSpriteFromSheet( engraving_character, IMGEngLeftSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
		}
		if(ownerSegment->rotation == 1)
		{
            if(engraving_flags.bits.north)
				DrawSpriteFromSheet( engraving_character, IMGEngRightSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            if(engraving_flags.bits.east)
				DrawSpriteFromSheet( engraving_character, IMGEngLeftSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
		}
		if(ownerSegment->rotation == 2)
		{
            if(engraving_flags.bits.west)
				DrawSpriteFromSheet( engraving_character, IMGEngRightSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            if(engraving_flags.bits.north)
				DrawSpriteFromSheet( engraving_character, IMGEngLeftSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
		}
		if(ownerSegment->rotation == 3)
		{
            if(engraving_flags.bits.south)
				DrawSpriteFromSheet( engraving_character, IMGEngRightSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            if(engraving_flags.bits.west)
				DrawSpriteFromSheet( engraving_character, IMGEngLeftSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
		}
	}


	if(water.index > 0)
	{
		//if(waterlevel == 7) waterlevel--;
		if(water.type == 0)
		{
			contentLoader->water[water.index-1].sprite.draw_world(x, y, z, this, (chopThisBlock && this->z == ownerSegment->z + ownerSegment->sizez -2));
		}
		else
		{
			contentLoader->lava[water.index-1].sprite.draw_world(x, y, z, this, (chopThisBlock && this->z == ownerSegment->z + ownerSegment->sizez -2));
		}
	}

	// creature
	// ensure there is *some* creature according to the map data
	// (no guarantee it is the right one)
	if(creaturePresent && (config.show_hidden_blocks || !designation.bits.hidden))
	{
		DrawCreature(drawx, drawy, creature, this);
	}

	//second part of snow
	if(wallType == 0 && stairType == 0 && defaultSnow)
	{
		if(snowlevel > 75)
		{
			DrawSpriteFromSheet( 24, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this );
		}
		else if(snowlevel > 50)
		{
			DrawSpriteFromSheet( 25, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this );
		}
		else if(snowlevel > 25)
		{
			DrawSpriteFromSheet( 26, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this );
		}
	}

	//if(eff_miasma > 0)
	//{
	//	draw_particle_cloud(eff_miasma, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_miasma);
	//}
	//if(eff_water > 0)
	//{
	//	draw_particle_cloud(eff_water, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_water);
	//}
	//if(eff_water2 > 0)
	//{
	//	draw_particle_cloud(eff_water2, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_water2);
	//}
	//if(eff_blood > 0)
	//{
	//	draw_particle_cloud(eff_blood, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_blood);
	//}
	////if(eff_magma > 0)
	////{
	////	al_set_separate_blender(op, ALLEGRO_ONE, ALLEGRO_ONE, alpha_op, ALLEGRO_ONE, ALLEGRO_ONE, al_map_rgba(255, 255, 255, (255*eff_magma/100)));
	////	DrawSpriteFromSheet( 185, IMGObjectSheet, drawx, drawy );
	////	al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
	////}
	//if(eff_magma > 0)
	//{
	//	draw_particle_cloud(eff_magma, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_magma);
	//}
	//if(eff_dust > 0)
	//{
	//	draw_particle_cloud(eff_dust, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_dust);
	//}
	//if(eff_smoke > 0)
	//{
	//	draw_particle_cloud(eff_smoke, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_smoke);
	//}
	//if(eff_dragonfire > 0)
	//{
	//	draw_particle_cloud(eff_smoke, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_smoke);
	//}
	//if(eff_fire > 0)
	//{
	//	draw_particle_cloud(eff_smoke, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_smoke);
	//}
	//if(eff_boiling > 0)
	//{
	//	draw_particle_cloud(eff_smoke, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_smoke);
	//}
}

void Block::Drawcreaturetext(){
	int sheetOffsetX, sheetOffsetY;
	t_SpriteWithOffset sprite;
	/*if(config.hide_outer_blocks){
	if(x == ownerSegment->x || x == ownerSegment->x + ownerSegment->sizex - 1) return;
	if(y == ownerSegment->y || y == ownerSegment->y + ownerSegment->sizey - 1) return;
	}*/

	int32_t drawx = x;
	int32_t drawy = y;
	int32_t drawz = z; //- ownerSegment->sizez + 1;

	correctBlockForSegmetOffset( drawx, drawy, drawz);
	correctBlockForRotation( drawx, drawy, drawz, ownerSegment->rotation);
	pointToScreen((int*)&drawx, (int*)&drawy, drawz);
	drawx -= TILEWIDTH>>1;

	// creature
	// ensure there is *some* creature according to the map data
	// (no guarantee it is the right one)
	if(creaturePresent && (config.show_hidden_blocks || !designation.bits.hidden))
	{
		DrawCreatureText(drawx, drawy, creature);
	}

}

void Block::DrawRamptops()
{
	if(!visible)
		return;
	if (ramp.type > 0)
	{

		bool chopThisBlock = 0;

		if(config.truncate_walls == 1) chopThisBlock = 1;
		else if(config.truncate_walls == 2 && obscuringCreature == 1) chopThisBlock = 1;
		else if(config.truncate_walls == 3 && (obscuringCreature == 1 || obscuringBuilding == 1)) chopThisBlock = 1;
		else if(config.truncate_walls == 4 && obscuringBuilding == 1) chopThisBlock = 1;
		//Draw Ramp
		c_sprite * spriteobject = GetBlockSpriteMap(ramp.type,material, consForm);
		if (spriteobject->get_sheetindex() == UNCONFIGURED_INDEX)
		{
			spriteobject->set_sheetindex(0);
			spriteobject->set_fileindex(INVALID_INDEX);
		}
		if (spriteobject->get_sheetindex() != INVALID_INDEX)
		{
			spriteobject->set_size(SPRITEWIDTH, TILEHEIGHT);
			spriteobject->set_offset(0, -(FLOORHEIGHT));
			spriteobject->set_tile_layout(RAMPTOPTILE);
			spriteobject->set_defaultsheet(IMGRampSheet);
			spriteobject->draw_world(x, y, z, this, (chopThisBlock && this->z == ownerSegment->z + ownerSegment->sizez -2));
			spriteobject->set_offset(0, 0);
		}
		spriteobject->set_tile_layout(BLOCKTILE);
	}
}

void Block::DrawPixel(int drawx, int drawy)
{
	ALLEGRO_COLOR temp;
	if(floorType > 0 || wallType > 0 || ramp.type > 0 || stairType > 0)
	{
		al_put_pixel(drawx, drawy, lookupMaterialColor(this->material.type, this->material.index));
	}
	if(this->water.index)
	{
		if(this->water.type == 0) //water
			al_draw_pixel(drawx, drawy, al_map_rgba_f(0.6f, 0.85f, 0.92f, (float)water.index / 7.0f));
		else
			al_draw_pixel(drawx, drawy, al_map_rgba_f(1.0f, 0.5f, 0.15f, (float)water.index / 7.0f));
	}
	//Grass
	if(grasslevel > 0 && (
		(tileTypeTable[floorType].material == GRASS) || 
		(tileTypeTable[floorType].material == GRASS2) ||
		(tileTypeTable[floorType].material == GRASS_DEAD) ||
		(tileTypeTable[floorType].material == GRASS_DRY)))
	{
		temp = lookupMaterialColor(WOOD, grassmat);
		al_draw_pixel(drawx, drawy, al_map_rgba_f(temp.r,temp.g, temp.b, (float)grasslevel/100.0f));
	}
}

bool hasWall(Block* b){
	if(!b) return false;
	return b->wallType > 0;
}

bool hasBuildingOfID(Block* b, int ID){
	if(!b) return false;
	return b->building.info.type == ID;
}

bool hasBuildingIdentity(Block* b, uint32_t index, int buildingOcc){
	if(!b) return false;
	if (!(b->building.index == index)) return false;
	return b->occ.bits.building == buildingOcc;
}

bool hasBuildingOfIndex(Block* b, uint32_t index){
	if(!b) return false;
	return b->building.index == index;
}

bool wallShouldNotHaveBorders( int in ){
	switch( in ){
	case 65: //stone fortification
	case 436: //minstone fortification
	case 326: //lavastone fortification
	case 327: //featstone fortification
	case 494: //constructed fortification
		return true;
		break;
	};
	return false;
}

void createEffectSprites()
{
	sprite_miasma		= CreateSpriteFromSheet( 180, IMGObjectSheet);
	sprite_water		= CreateSpriteFromSheet( 181, IMGObjectSheet);
	sprite_water2		= CreateSpriteFromSheet( 182, IMGObjectSheet);
	sprite_blood		= CreateSpriteFromSheet( 183, IMGObjectSheet);
	sprite_dust			= CreateSpriteFromSheet( 184, IMGObjectSheet);
	sprite_magma		= CreateSpriteFromSheet( 185, IMGObjectSheet);
	sprite_smoke		= CreateSpriteFromSheet( 186, IMGObjectSheet);
	sprite_dragonfire	= CreateSpriteFromSheet( 187, IMGObjectSheet);
	sprite_fire			= CreateSpriteFromSheet( 188, IMGObjectSheet);
	sprite_webing		= CreateSpriteFromSheet( 189, IMGObjectSheet);
	sprite_boiling		= CreateSpriteFromSheet( 190, IMGObjectSheet);
	sprite_oceanwave	= CreateSpriteFromSheet( 191, IMGObjectSheet);
}

void destroyEffectSprites()
{
	al_destroy_bitmap(sprite_miasma);
	al_destroy_bitmap(sprite_water);
	al_destroy_bitmap(sprite_water2);
	al_destroy_bitmap(sprite_blood);
	al_destroy_bitmap(sprite_dust);
	al_destroy_bitmap(sprite_magma);
	al_destroy_bitmap(sprite_smoke);
	al_destroy_bitmap(sprite_dragonfire);
	al_destroy_bitmap(sprite_fire);
	al_destroy_bitmap(sprite_webing);
	al_destroy_bitmap(sprite_boiling);
	al_destroy_bitmap(sprite_oceanwave);
}


void drawFloorBlood ( Block *b, int32_t drawx, int32_t drawy )
{
	t_occupancy occ = b->occ;
	t_SpriteWithOffset sprite;
	int x = b->x, y = b->y, z = b->z;


	if( b->water.index < 1 && (b->bloodlevel))
	{
		sprite.fileIndex = INVALID_INDEX;

		// Spatter (should be blood, not blood2) swapped for testing
		if( b->bloodlevel <= config.poolcutoff )
			sprite.sheetIndex = 7;

		// Smear (should be blood2, not blood) swapped for testing
		else
		{
			// if there's no block in the respective direction it's false. if there's no blood in that direction it's false too. should also check to see if there's a ramp below, but since blood doesn't flow, that'd look wrong anyway.
			bool _N = ( b->ownerSegment->getBlockRelativeTo( x, y, z, eUp ) != NULL ? (b->ownerSegment->getBlockRelativeTo( x, y, z, eUp )->bloodlevel > config.poolcutoff) : false ),
				_S = ( b->ownerSegment->getBlockRelativeTo( x, y, z, eDown ) != NULL ? (b->ownerSegment->getBlockRelativeTo( x, y, z, eDown )->bloodlevel > config.poolcutoff) : false ),
				_E = ( b->ownerSegment->getBlockRelativeTo( x, y, z, eRight ) != NULL ? (b->ownerSegment->getBlockRelativeTo( x, y, z, eRight )->bloodlevel > config.poolcutoff) : false ),
				_W = ( b->ownerSegment->getBlockRelativeTo( x, y, z, eLeft ) != NULL ? (b->ownerSegment->getBlockRelativeTo( x, y, z, eLeft )->bloodlevel > config.poolcutoff) : false );

			// do rules-based puddling
			if( _N || _S || _E || _W )
			{
				if( _E )
				{
					if( _N && _S )
						sprite.sheetIndex = 5;
					else if( _S )
						sprite.sheetIndex = 3;
					else if( _W )
						sprite.sheetIndex = 1;
					else
						sprite.sheetIndex = 6;
				}
				else if( _W )
				{
					if( _S && _N)
						sprite.sheetIndex = 5;
					else if( _S )
						sprite.sheetIndex = 2;
					else
						sprite.sheetIndex = 0;
				}
				else if ( _N )
					sprite.sheetIndex = 4;
				else
					sprite.sheetIndex = 2;
			}
			else
				sprite.sheetIndex = 8;
		}

		int sheetOffsetX = TILEWIDTH * (sprite.sheetIndex % SHEET_OBJECTSWIDE),
			sheetOffsetY = 0;

		al_draw_tinted_bitmap_region( IMGBloodSheet, premultiply(b->bloodcolor), sheetOffsetX, sheetOffsetY, TILEWIDTH, TILEHEIGHT+FLOORHEIGHT, drawx, drawy, 0);
		al_draw_bitmap_region( IMGBloodSheet, sheetOffsetX, sheetOffsetY+TILEHEIGHT+FLOORHEIGHT, TILEWIDTH, TILEHEIGHT+FLOORHEIGHT, drawx, drawy, 0);
	}
}

