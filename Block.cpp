#include "Block.h"
#include "common.h"
#include "GUI.h"
#include "SpriteMaps.h"
#include "GameBuildings.h"
#include "Creatures.h"
#include "WorldSegment.h"
#include "BlockFactory.h"
#include "ContentLoader.h"

#include "dfhack/include/DFTypes.h"



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

ALLEGRO_COLOR getSpriteColor(t_SpriteWithOffset &sprite, t_matglossPair material, t_matglossPair layerMaterial, t_matglossPair veinMaterial)
{
	int matFore = lookupMaterialFore(material.type, material.index);
	int matBright = lookupMaterialBright(material.type, material.index);
	int matBack = lookupMaterialBack(material.type, material.index);
	int layerFore = lookupMaterialFore(material.type, layerMaterial.index);
	int layerBright = lookupMaterialBright(material.type, layerMaterial.index);
	int layerBack = lookupMaterialBack(material.type, layerMaterial.index);
	int veinFore = lookupMaterialFore(material.type, veinMaterial.index);
	int veinBright = lookupMaterialBright(material.type, veinMaterial.index);
	int veinBack = lookupMaterialBack(material.type, veinMaterial.index);
	switch(sprite.shadeBy)
	{
	case ShadeNone:
		return al_map_rgb(255, 255, 255);
	case ShadeXml:
		return sprite.shadeColor;
	case ShadeMat:
		return lookupMaterialColor(material.type, material.index);
	case ShadeLayer:
		return lookupMaterialColor(layerMaterial.type, layerMaterial.index);
	case ShadeVein:
		return lookupMaterialColor(veinMaterial.type, veinMaterial.index);
	case ShadeMatFore:
		if(matBright)
		{
			switch (matFore)
			{
			case 0:
				return al_map_rgb(config.colors.dgray_r, config.colors.dgray_g, config.colors.dgray_b);
			case 1:
				return al_map_rgb(config.colors.lblue_r, config.colors.lblue_g, config.colors.lblue_b);
			case 2:
				return al_map_rgb(config.colors.lgreen_r, config.colors.lgreen_g, config.colors.lgreen_b);
			case 3:
				return al_map_rgb(config.colors.lcyan_r, config.colors.lcyan_g, config.colors.lcyan_b);
			case 4:
				return al_map_rgb(config.colors.lred_r, config.colors.lred_g, config.colors.lred_b);
			case 5:
				return al_map_rgb(config.colors.lmagenta_r, config.colors.lmagenta_g, config.colors.lmagenta_b);
			case 6:
				return al_map_rgb(config.colors.yellow_r, config.colors.yellow_g, config.colors.yellow_b);
			case 7:
				return al_map_rgb(config.colors.white_r, config.colors.white_g, config.colors.white_b);
			} ;
		}
		else
		{
			switch (matFore)
			{
			case 0:
				return al_map_rgb(config.colors.black_r, config.colors.black_g, config.colors.black_b);
			case 1:
				return al_map_rgb(config.colors.blue_r, config.colors.blue_g, config.colors.blue_b);
			case 2:
				return al_map_rgb(config.colors.green_r, config.colors.green_g, config.colors.green_b);
			case 3:
				return al_map_rgb(config.colors.cyan_r, config.colors.cyan_g, config.colors.cyan_b);
			case 4:
				return al_map_rgb(config.colors.red_r, config.colors.red_g, config.colors.red_b);
			case 5:
				return al_map_rgb(config.colors.magenta_r, config.colors.magenta_g, config.colors.magenta_b);
			case 6:
				return al_map_rgb(config.colors.brown_r, config.colors.brown_g, config.colors.brown_b);
			case 7:
				return al_map_rgb(config.colors.lgray_r, config.colors.lgray_g, config.colors.lgray_b);
			} ;
		}
	case ShadeMatBack:
		switch (matBack)
		{
		case 0:
			return al_map_rgb(config.colors.black_r, config.colors.black_g, config.colors.black_b);
		case 1:
			return al_map_rgb(config.colors.blue_r, config.colors.blue_g, config.colors.blue_b);
		case 2:
			return al_map_rgb(config.colors.green_r, config.colors.green_g, config.colors.green_b);
		case 3:
			return al_map_rgb(config.colors.cyan_r, config.colors.cyan_g, config.colors.cyan_b);
		case 4:
			return al_map_rgb(config.colors.red_r, config.colors.red_g, config.colors.red_b);
		case 5:
			return al_map_rgb(config.colors.magenta_r, config.colors.magenta_g, config.colors.magenta_b);
		case 6:
			return al_map_rgb(config.colors.brown_r, config.colors.brown_g, config.colors.brown_b);
		case 7:
			return al_map_rgb(config.colors.lgray_r, config.colors.lgray_g, config.colors.lgray_b);
		} ;
	case ShadeLayerFore:
		if(layerBright)
		{
			switch (layerFore)
			{
			case 0:
				return al_map_rgb(config.colors.dgray_r, config.colors.dgray_g, config.colors.dgray_b);
			case 1:
				return al_map_rgb(config.colors.lblue_r, config.colors.lblue_g, config.colors.lblue_b);
			case 2:
				return al_map_rgb(config.colors.lgreen_r, config.colors.lgreen_g, config.colors.lgreen_b);
			case 3:
				return al_map_rgb(config.colors.lcyan_r, config.colors.lcyan_g, config.colors.lcyan_b);
			case 4:
				return al_map_rgb(config.colors.lred_r, config.colors.lred_g, config.colors.lred_b);
			case 5:
				return al_map_rgb(config.colors.lmagenta_r, config.colors.lmagenta_g, config.colors.lmagenta_b);
			case 6:
				return al_map_rgb(config.colors.yellow_r, config.colors.yellow_g, config.colors.yellow_b);
			case 7:
				return al_map_rgb(config.colors.white_r, config.colors.white_g, config.colors.white_b);
			} ;
		}
		else
		{
			switch (layerFore)
			{
			case 0:
				return al_map_rgb(config.colors.black_r, config.colors.black_g, config.colors.black_b);
			case 1:
				return al_map_rgb(config.colors.blue_r, config.colors.blue_g, config.colors.blue_b);
			case 2:
				return al_map_rgb(config.colors.green_r, config.colors.green_g, config.colors.green_b);
			case 3:
				return al_map_rgb(config.colors.cyan_r, config.colors.cyan_g, config.colors.cyan_b);
			case 4:
				return al_map_rgb(config.colors.red_r, config.colors.red_g, config.colors.red_b);
			case 5:
				return al_map_rgb(config.colors.magenta_r, config.colors.magenta_g, config.colors.magenta_b);
			case 6:
				return al_map_rgb(config.colors.brown_r, config.colors.brown_g, config.colors.brown_b);
			case 7:
				return al_map_rgb(config.colors.lgray_r, config.colors.lgray_g, config.colors.lgray_b);
			} ;
		}
	case ShadeLayerBack:
		switch (layerBack)
		{
		case 0:
			return al_map_rgb(config.colors.black_r, config.colors.black_g, config.colors.black_b);
		case 1:
			return al_map_rgb(config.colors.blue_r, config.colors.blue_g, config.colors.blue_b);
		case 2:
			return al_map_rgb(config.colors.green_r, config.colors.green_g, config.colors.green_b);
		case 3:
			return al_map_rgb(config.colors.cyan_r, config.colors.cyan_g, config.colors.cyan_b);
		case 4:
			return al_map_rgb(config.colors.red_r, config.colors.red_g, config.colors.red_b);
		case 5:
			return al_map_rgb(config.colors.magenta_r, config.colors.magenta_g, config.colors.magenta_b);
		case 6:
			return al_map_rgb(config.colors.brown_r, config.colors.brown_g, config.colors.brown_b);
		case 7:
			return al_map_rgb(config.colors.lgray_r, config.colors.lgray_g, config.colors.lgray_b);
		} ;
	case ShadeVeinFore:
		if(veinBright)
		{
			switch (veinFore)
			{
			case 0:
				return al_map_rgb(config.colors.dgray_r, config.colors.dgray_g, config.colors.dgray_b);
			case 1:
				return al_map_rgb(config.colors.lblue_r, config.colors.lblue_g, config.colors.lblue_b);
			case 2:
				return al_map_rgb(config.colors.lgreen_r, config.colors.lgreen_g, config.colors.lgreen_b);
			case 3:
				return al_map_rgb(config.colors.lcyan_r, config.colors.lcyan_g, config.colors.lcyan_b);
			case 4:
				return al_map_rgb(config.colors.lred_r, config.colors.lred_g, config.colors.lred_b);
			case 5:
				return al_map_rgb(config.colors.lmagenta_r, config.colors.lmagenta_g, config.colors.lmagenta_b);
			case 6:
				return al_map_rgb(config.colors.yellow_r, config.colors.yellow_g, config.colors.yellow_b);
			case 7:
				return al_map_rgb(config.colors.white_r, config.colors.white_g, config.colors.white_b);
			} ;
		}
		else
		{
			switch (veinFore)
			{
			case 0:
				return al_map_rgb(config.colors.black_r, config.colors.black_g, config.colors.black_b);
			case 1:
				return al_map_rgb(config.colors.blue_r, config.colors.blue_g, config.colors.blue_b);
			case 2:
				return al_map_rgb(config.colors.green_r, config.colors.green_g, config.colors.green_b);
			case 3:
				return al_map_rgb(config.colors.cyan_r, config.colors.cyan_g, config.colors.cyan_b);
			case 4:
				return al_map_rgb(config.colors.red_r, config.colors.red_g, config.colors.red_b);
			case 5:
				return al_map_rgb(config.colors.magenta_r, config.colors.magenta_g, config.colors.magenta_b);
			case 6:
				return al_map_rgb(config.colors.brown_r, config.colors.brown_g, config.colors.brown_b);
			case 7:
				return al_map_rgb(config.colors.lgray_r, config.colors.lgray_g, config.colors.lgray_b);
			} ;
		}
	case ShadeVeinBack:
		switch (veinBack)
		{
		case 0:
			return al_map_rgb(config.colors.black_r, config.colors.black_g, config.colors.black_b);
		case 1:
			return al_map_rgb(config.colors.blue_r, config.colors.blue_g, config.colors.blue_b);
		case 2:
			return al_map_rgb(config.colors.green_r, config.colors.green_g, config.colors.green_b);
		case 3:
			return al_map_rgb(config.colors.cyan_r, config.colors.cyan_g, config.colors.cyan_b);
		case 4:
			return al_map_rgb(config.colors.red_r, config.colors.red_g, config.colors.red_b);
		case 5:
			return al_map_rgb(config.colors.magenta_r, config.colors.magenta_g, config.colors.magenta_b);
		case 6:
			return al_map_rgb(config.colors.brown_r, config.colors.brown_g, config.colors.brown_b);
		case 7:
			return al_map_rgb(config.colors.lgray_r, config.colors.lgray_g, config.colors.lgray_b);
		} ;
	default:
		return al_map_rgb(255, 255, 255);
	} ;
}

ALLEGRO_COLOR getSpriteColor(t_subSprite &sprite, t_matglossPair material, t_matglossPair layerMaterial, t_matglossPair veinMaterial)
{
	int matFore = lookupMaterialFore(material.type, material.index);
	int matBright = lookupMaterialBright(material.type, material.index);
	int matBack = lookupMaterialFore(material.type, material.index);
	int layerFore = lookupMaterialFore(layerMaterial.type, layerMaterial.index);
	int layerBright = lookupMaterialBright(layerMaterial.type, layerMaterial.index);
	int layerBack = lookupMaterialFore(layerMaterial.type, layerMaterial.index);
	int veinFore = lookupMaterialFore(veinMaterial.type, veinMaterial.index);
	int veinBright = lookupMaterialBright(veinMaterial.type, veinMaterial.index);
	int veinBack = lookupMaterialFore(veinMaterial.type, veinMaterial.index);
	switch(sprite.shadeBy)
	{
	case ShadeNone:
		return al_map_rgb(255, 255, 255);
	case ShadeXml:
		return sprite.shadeColor;
	case ShadeMat:
		return lookupMaterialColor(material.type, material.index);
	case ShadeLayer:
		return lookupMaterialColor(layerMaterial.type, layerMaterial.index);
	case ShadeVein:
		return lookupMaterialColor(veinMaterial.type, veinMaterial.index);
	case ShadeMatFore:
		if(matBright)
		{
			switch (matFore)
			{
			case 0:
				return al_map_rgb(config.colors.dgray_r, config.colors.dgray_g, config.colors.dgray_b);
			case 1:
				return al_map_rgb(config.colors.lblue_r, config.colors.lblue_g, config.colors.lblue_b);
			case 2:
				return al_map_rgb(config.colors.lgreen_r, config.colors.lgreen_g, config.colors.lgreen_b);
			case 3:
				return al_map_rgb(config.colors.lcyan_r, config.colors.lcyan_g, config.colors.lcyan_b);
			case 4:
				return al_map_rgb(config.colors.lred_r, config.colors.lred_g, config.colors.lred_b);
			case 5:
				return al_map_rgb(config.colors.lmagenta_r, config.colors.lmagenta_g, config.colors.lmagenta_b);
			case 6:
				return al_map_rgb(config.colors.yellow_r, config.colors.yellow_g, config.colors.yellow_b);
			case 7:
				return al_map_rgb(config.colors.white_r, config.colors.white_g, config.colors.white_b);
			} ;
		}
		else
		{
			switch (matFore)
			{
			case 0:
				return al_map_rgb(config.colors.black_r, config.colors.black_g, config.colors.black_b);
			case 1:
				return al_map_rgb(config.colors.blue_r, config.colors.blue_g, config.colors.blue_b);
			case 2:
				return al_map_rgb(config.colors.green_r, config.colors.green_g, config.colors.green_b);
			case 3:
				return al_map_rgb(config.colors.cyan_r, config.colors.cyan_g, config.colors.cyan_b);
			case 4:
				return al_map_rgb(config.colors.red_r, config.colors.red_g, config.colors.red_b);
			case 5:
				return al_map_rgb(config.colors.magenta_r, config.colors.magenta_g, config.colors.magenta_b);
			case 6:
				return al_map_rgb(config.colors.brown_r, config.colors.brown_g, config.colors.brown_b);
			case 7:
				return al_map_rgb(config.colors.lgray_r, config.colors.lgray_g, config.colors.lgray_b);
			} ;
		}
	case ShadeMatBack:
		switch (matBack)
		{
		case 0:
			return al_map_rgb(config.colors.black_r, config.colors.black_g, config.colors.black_b);
		case 1:
			return al_map_rgb(config.colors.blue_r, config.colors.blue_g, config.colors.blue_b);
		case 2:
			return al_map_rgb(config.colors.green_r, config.colors.green_g, config.colors.green_b);
		case 3:
			return al_map_rgb(config.colors.cyan_r, config.colors.cyan_g, config.colors.cyan_b);
		case 4:
			return al_map_rgb(config.colors.red_r, config.colors.red_g, config.colors.red_b);
		case 5:
			return al_map_rgb(config.colors.magenta_r, config.colors.magenta_g, config.colors.magenta_b);
		case 6:
			return al_map_rgb(config.colors.brown_r, config.colors.brown_g, config.colors.brown_b);
		case 7:
			return al_map_rgb(config.colors.lgray_r, config.colors.lgray_g, config.colors.lgray_b);
		} ;
	case ShadeLayerFore:
		if(layerBright)
		{
			switch (layerFore)
			{
			case 0:
				return al_map_rgb(config.colors.dgray_r, config.colors.dgray_g, config.colors.dgray_b);
			case 1:
				return al_map_rgb(config.colors.lblue_r, config.colors.lblue_g, config.colors.lblue_b);
			case 2:
				return al_map_rgb(config.colors.lgreen_r, config.colors.lgreen_g, config.colors.lgreen_b);
			case 3:
				return al_map_rgb(config.colors.lcyan_r, config.colors.lcyan_g, config.colors.lcyan_b);
			case 4:
				return al_map_rgb(config.colors.lred_r, config.colors.lred_g, config.colors.lred_b);
			case 5:
				return al_map_rgb(config.colors.lmagenta_r, config.colors.lmagenta_g, config.colors.lmagenta_b);
			case 6:
				return al_map_rgb(config.colors.yellow_r, config.colors.yellow_g, config.colors.yellow_b);
			case 7:
				return al_map_rgb(config.colors.white_r, config.colors.white_g, config.colors.white_b);
			} ;
		}
		else
		{
			switch (layerFore)
			{
			case 0:
				return al_map_rgb(config.colors.black_r, config.colors.black_g, config.colors.black_b);
			case 1:
				return al_map_rgb(config.colors.blue_r, config.colors.blue_g, config.colors.blue_b);
			case 2:
				return al_map_rgb(config.colors.green_r, config.colors.green_g, config.colors.green_b);
			case 3:
				return al_map_rgb(config.colors.cyan_r, config.colors.cyan_g, config.colors.cyan_b);
			case 4:
				return al_map_rgb(config.colors.red_r, config.colors.red_g, config.colors.red_b);
			case 5:
				return al_map_rgb(config.colors.magenta_r, config.colors.magenta_g, config.colors.magenta_b);
			case 6:
				return al_map_rgb(config.colors.brown_r, config.colors.brown_g, config.colors.brown_b);
			case 7:
				return al_map_rgb(config.colors.lgray_r, config.colors.lgray_g, config.colors.lgray_b);
			} ;
		}
	case ShadeLayerBack:
		switch (layerBack)
		{
		case 0:
			return al_map_rgb(config.colors.black_r, config.colors.black_g, config.colors.black_b);
		case 1:
			return al_map_rgb(config.colors.blue_r, config.colors.blue_g, config.colors.blue_b);
		case 2:
			return al_map_rgb(config.colors.green_r, config.colors.green_g, config.colors.green_b);
		case 3:
			return al_map_rgb(config.colors.cyan_r, config.colors.cyan_g, config.colors.cyan_b);
		case 4:
			return al_map_rgb(config.colors.red_r, config.colors.red_g, config.colors.red_b);
		case 5:
			return al_map_rgb(config.colors.magenta_r, config.colors.magenta_g, config.colors.magenta_b);
		case 6:
			return al_map_rgb(config.colors.brown_r, config.colors.brown_g, config.colors.brown_b);
		case 7:
			return al_map_rgb(config.colors.lgray_r, config.colors.lgray_g, config.colors.lgray_b);
		} ;
	case ShadeVeinFore:
		if(veinBright)
		{
			switch (veinFore)
			{
			case 0:
				return al_map_rgb(config.colors.dgray_r, config.colors.dgray_g, config.colors.dgray_b);
			case 1:
				return al_map_rgb(config.colors.lblue_r, config.colors.lblue_g, config.colors.lblue_b);
			case 2:
				return al_map_rgb(config.colors.lgreen_r, config.colors.lgreen_g, config.colors.lgreen_b);
			case 3:
				return al_map_rgb(config.colors.lcyan_r, config.colors.lcyan_g, config.colors.lcyan_b);
			case 4:
				return al_map_rgb(config.colors.lred_r, config.colors.lred_g, config.colors.lred_b);
			case 5:
				return al_map_rgb(config.colors.lmagenta_r, config.colors.lmagenta_g, config.colors.lmagenta_b);
			case 6:
				return al_map_rgb(config.colors.yellow_r, config.colors.yellow_g, config.colors.yellow_b);
			case 7:
				return al_map_rgb(config.colors.white_r, config.colors.white_g, config.colors.white_b);
			} ;
		}
		else
		{
			switch (veinFore)
			{
			case 0:
				return al_map_rgb(config.colors.black_r, config.colors.black_g, config.colors.black_b);
			case 1:
				return al_map_rgb(config.colors.blue_r, config.colors.blue_g, config.colors.blue_b);
			case 2:
				return al_map_rgb(config.colors.green_r, config.colors.green_g, config.colors.green_b);
			case 3:
				return al_map_rgb(config.colors.cyan_r, config.colors.cyan_g, config.colors.cyan_b);
			case 4:
				return al_map_rgb(config.colors.red_r, config.colors.red_g, config.colors.red_b);
			case 5:
				return al_map_rgb(config.colors.magenta_r, config.colors.magenta_g, config.colors.magenta_b);
			case 6:
				return al_map_rgb(config.colors.brown_r, config.colors.brown_g, config.colors.brown_b);
			case 7:
				return al_map_rgb(config.colors.lgray_r, config.colors.lgray_g, config.colors.lgray_b);
			} ;
		}
	case ShadeVeinBack:
		switch (veinBack)
		{
		case 0:
			return al_map_rgb(config.colors.black_r, config.colors.black_g, config.colors.black_b);
		case 1:
			return al_map_rgb(config.colors.blue_r, config.colors.blue_g, config.colors.blue_b);
		case 2:
			return al_map_rgb(config.colors.green_r, config.colors.green_g, config.colors.green_b);
		case 3:
			return al_map_rgb(config.colors.cyan_r, config.colors.cyan_g, config.colors.cyan_b);
		case 4:
			return al_map_rgb(config.colors.red_r, config.colors.red_g, config.colors.red_b);
		case 5:
			return al_map_rgb(config.colors.magenta_r, config.colors.magenta_g, config.colors.magenta_b);
		case 6:
			return al_map_rgb(config.colors.brown_r, config.colors.brown_g, config.colors.brown_b);
		case 7:
			return al_map_rgb(config.colors.lgray_r, config.colors.lgray_g, config.colors.lgray_b);
		} ;
	default:
		return al_map_rgb(255, 255, 255);
	} ;
}
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

	this->material.type = INVALID_INDEX;
	this->material.index = INVALID_INDEX;
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

void Block::Draw(){
	int sheetOffsetX, sheetOffsetY;
	t_SpriteWithOffset sprite;
	/*if(config.hide_outer_blocks){
	if(x == ownerSegment->x || x == ownerSegment->x + ownerSegment->sizex - 1) return;
	if(y == ownerSegment->y || y == ownerSegment->y + ownerSegment->sizey - 1) return;
	}*/
	int op, src, dst, alpha_op, alpha_src, alpha_dst;
	ALLEGRO_COLOR color;
	al_get_separate_blender(&op, &src, &dst, &alpha_op, &alpha_src, &alpha_dst, &color);
	int32_t drawx = x;
	int32_t drawy = y;
	int32_t drawz = z; //- ownerSegment->sizez + 1;


	correctBlockForSegmetOffset( drawx, drawy, drawz);
	correctBlockForRotation( drawx, drawy, drawz);
	int32_t viewx = drawx;
	int32_t viewy = drawy;
	int32_t viewz = drawz;
	pointToScreen((int*)&drawx, (int*)&drawy, drawz);
	drawx -= TILEWIDTH>>1;

	ALLEGRO_COLOR tileBorderColor = al_map_rgb(85,85,85);
	int rando = randomCube[x%RANDOM_CUBE][y%RANDOM_CUBE][z%RANDOM_CUBE];
	//Draw Floor
	if(floorType > 0 || wallType > 0 || ramp.type > 0 || stairType > 0){

		//If tile has no floor, look for a Filler Floor from it's wall
		if (floorType > 0)
		{
			sprite = GetFloorSpriteMap(floorType, this->material);
		}
		else if (wallType > 0)
		{
			sprite = GetFloorSpriteMap(wallType, this->material);
		}
		else if (ramp.type > 0)
		{
			sprite = GetFloorSpriteMap(ramp.type, this->material);
		}
		else if (stairType > 0)
		{
			sprite = GetFloorSpriteMap(stairType, this->material);
		}

		if(sprite.sheetIndex != INVALID_INDEX)
		{
			int spriteOffset = 0;;
			if(sprite.numVariations)
				spriteOffset = rando % sprite.numVariations;
			al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, getSpriteColor(sprite, this->material, this->layerMaterial, this->veinMaterial));


			//if floor is muddy, override regular floor
			if( mudlevel && water.index == 0)
			{
				sprite.sheetIndex = SPRITEFLOOR_WATERFLOOR;
				sprite.fileIndex = INVALID_INDEX;
				spriteOffset = 0;
				al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, al_map_rgb(255, 255, 255));
			}
			////if floor is snowed down, override  regular floor
			//if( snowlevel )
			//{
			//	sprite.sheetIndex = SPRITEFLOOR_SNOW;
			//	sprite.fileIndex = INVALID_INDEX;
			//	spriteOffset = 0;
			//	al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, al_map_rgb(255, 255, 255));
			//}

			if (sprite.sheetIndex == UNCONFIGURED_INDEX)
			{
				sprite.sheetIndex = SPRITEOBJECT_FLOOR_NA;
				sprite.fileIndex = INVALID_INDEX;
				spriteOffset = 0;
			}

			sheetOffsetX = TILEWIDTH * ((sprite.sheetIndex+spriteOffset) % SHEET_OBJECTSWIDE);
			sheetOffsetY = (TILEHEIGHT + FLOORHEIGHT) * ((sprite.sheetIndex+spriteOffset) / SHEET_OBJECTSWIDE);
			al_draw_bitmap_region(imageSheet(sprite,IMGObjectSheet), sheetOffsetX, sheetOffsetY,  TILEWIDTH, TILEHEIGHT + FLOORHEIGHT, drawx, drawy, 0);
			al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
			if(sprite.sheetIndex != SPRITEFLOOR_WATERFLOOR && sprite.sheetIndex != SPRITEFLOOR_SNOW)
			{
				if(!(sprite.subSprites.empty()))
				{
					for(int i = 0; i < sprite.subSprites.size(); i++)
					{
						sheetOffsetX = TILEWIDTH * ((sprite.subSprites[i].sheetIndex+spriteOffset) % SHEET_OBJECTSWIDE);
						sheetOffsetY = (TILEHEIGHT + FLOORHEIGHT) * ((sprite.subSprites[i].sheetIndex+spriteOffset) / SHEET_OBJECTSWIDE);
						al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, getSpriteColor(sprite.subSprites[i], this->material, this->layerMaterial, this->veinMaterial));
						al_draw_bitmap_region(imageSheet(sprite.subSprites[i],IMGObjectSheet), sheetOffsetX, sheetOffsetY,  TILEWIDTH, TILEHEIGHT + FLOORHEIGHT, drawx, drawy, 0);
						al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
					}
				}
			}
		}

		if(sprite.needOutline)
		{
			drawy += (WALLHEIGHT);
			//Northern border
			if(this->depthBorderNorth)
				DrawSpriteFromSheet(281, IMGObjectSheet, drawx, drawy );

			//Western border
			if(this->depthBorderWest)
				DrawSpriteFromSheet(280, IMGObjectSheet, drawx, drawy );

			drawy -= (WALLHEIGHT);
		}
	}

	//draw surf
	if(eff_oceanwave > 0)
	{
		al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, al_map_rgba(255, 255, 255, (255*eff_oceanwave)/100));
		al_draw_bitmap(sprite_oceanwave, drawx, drawy - (WALLHEIGHT), 0);
		al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
	}
	if(eff_webing > 0)
	{
		al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, al_map_rgba(255, 255, 255, (255*eff_webing)/100));
		al_draw_bitmap(sprite_webing, drawx, drawy - (WALLHEIGHT), 0);
		al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
	}
	//Draw Ramp
	if(ramp.type > 0){
		sprite = GetBlockSpriteMap(ramp.type, material);
		if (sprite.sheetIndex == UNCONFIGURED_INDEX)
		{
			sprite.sheetIndex = 0;
			sprite.fileIndex = INVALID_INDEX;
		}
		if (sprite.sheetIndex != INVALID_INDEX)
		{
			sheetOffsetX = SPRITEWIDTH * ramp.index;
			sheetOffsetY = ((TILEHEIGHT + FLOORHEIGHT + SPRITEHEIGHT) * sprite.sheetIndex)+(TILEHEIGHT + FLOORHEIGHT);
			al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, getSpriteColor(sprite, this->material, this->layerMaterial, this->veinMaterial));
			al_draw_bitmap_region(imageSheet(sprite,IMGRampSheet), sheetOffsetX, sheetOffsetY, SPRITEWIDTH, SPRITEHEIGHT, drawx, drawy - (WALLHEIGHT), 0);
			al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
			if(!(sprite.subSprites.empty()))
			{
				for(int i = 0; i < sprite.subSprites.size(); i++)
				{
					sheetOffsetX = SPRITEWIDTH * ramp.index;
					sheetOffsetY = ((TILEHEIGHT + FLOORHEIGHT + SPRITEHEIGHT) * sprite.subSprites[i].sheetIndex)+(TILEHEIGHT + FLOORHEIGHT);
					al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, getSpriteColor(sprite.subSprites[i], this->material, this->layerMaterial, this->veinMaterial));
					al_draw_bitmap_region(imageSheet(sprite.subSprites[i],IMGObjectSheet), sheetOffsetX, sheetOffsetY, SPRITEWIDTH, SPRITEHEIGHT, drawx, drawy - (WALLHEIGHT), 0);
					al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
				}
			}
		}
	}


	//first part of snow
	if(ramp.type == 0)
	{
		if(snowlevel > 75)
		{
			DrawSpriteFromSheet( 20, IMGObjectSheet, drawx, drawy );
		}
		else if(snowlevel > 50)
		{
			DrawSpriteFromSheet( 21, IMGObjectSheet, drawx, drawy );
		}
		else if(snowlevel > 25)
		{
			DrawSpriteFromSheet( 22, IMGObjectSheet, drawx, drawy );
		}
		else if(snowlevel > 0)
		{
			DrawSpriteFromSheet( 23, IMGObjectSheet, drawx, drawy );
		}
	}


	//vegetation
	if(tree.index > 0 || tree.type > 0){
		sprite =  GetSpriteVegetation( (TileClass) getVegetationType( this->floorType ), tree.index );
		DrawSpriteFromSheet( sprite.sheetIndex, imageSheet(sprite,IMGObjectSheet), drawx, drawy );
	}

	//shadow
	if (shadow > 0)
	{
		DrawSpriteFromSheet( BASE_SHADOW_TILE + shadow - 1, IMGObjectSheet, drawx, (ramp.type > 0)?(drawy - (WALLHEIGHT/2)):drawy );
	}

	//Building
	bool skipBuilding =
		(building.info.type == TranslateBuildingName("building_civzonest", contentLoader.classIdStrings ) && !config.show_stockpiles) ||
		(building.info.type == TranslateBuildingName("building_stockpilest", contentLoader.classIdStrings ) && !config.show_zones);

	if(building.info.type != BUILDINGTYPE_NA && !skipBuilding)
	{
		int spriteNum =  SPRITEOBJECT_NA; //getBuildingSprite(this->building, mirroredBuilding);

		for(uint32_t i=0; i < building.sprites.size(); i++){
			sprite = building.sprites[i];
			if(sprite.numVariations)
				sprite.sheetIndex += rando % sprite.numVariations;
			if (!(sprite.animFrames & (1 << currentAnimationFrame)))
				continue;
			al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, getSpriteColor(sprite, building.info.material, this->layerMaterial, this->veinMaterial));

			DrawSpriteFromSheet(sprite.sheetIndex , imageSheet(sprite,IMGObjectSheet), 
				drawx + building.sprites[i].x,
				drawy + building.sprites[i].y);
			al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
		}
	}




	//Draw Stairs
	if(stairType > 0){
		//down part
		//skipping at the moment?
		//int spriteNum = GetFloorSpriteMap(stairType,material);
		//DrawSpriteFromSheet( spriteNum, target, IMGObjectSheet, drawx, drawy );

		//up part

		bool mirrored = false;
		if(findWallCloseTo(ownerSegment, this) == eSimpleW)
			mirrored = true;
		sprite = GetBlockSpriteMap(stairType, material);
		if(sprite.sheetIndex != INVALID_INDEX && sprite.sheetIndex != UNCONFIGURED_INDEX)
		{
			if (mirrored)
				sprite.sheetIndex += 1;
			al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, getSpriteColor(sprite, this->material, this->layerMaterial, this->veinMaterial));
			DrawSpriteFromSheet( sprite.sheetIndex, imageSheet(sprite,IMGObjectSheet), drawx, drawy );
			al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
			if(!(sprite.subSprites.empty()))
			{
				for(int i = 0; i < sprite.subSprites.size(); i++)
				{
					al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, getSpriteColor(sprite.subSprites[i], this->material, this->layerMaterial, this->veinMaterial));
					DrawSpriteFromSheet( sprite.subSprites[i].sheetIndex, imageSheet(sprite.subSprites[i],IMGObjectSheet), drawx, drawy );
					al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
				}
			}
		}
	}

	//Draw Walls
	if(wallType > 0){
		//draw wall
		sprite =  GetBlockSpriteMap(wallType, material);
		int spriteOffset = 0;
		if(sprite.numVariations)
			spriteOffset = rando % sprite.numVariations;
		if (sprite.sheetIndex == UNCONFIGURED_INDEX)
		{
			sprite.sheetIndex = SPRITEOBJECT_WALL_NA;
			sprite.fileIndex = INVALID_INDEX;
			spriteOffset = 0;
		}
		if (sprite.sheetIndex == INVALID_INDEX)
		{
			//skip   
		}    
		else 
		{
			if( config.truncate_walls && this->z == ownerSegment->z + ownerSegment->sizez -2){
				int sheetx = (sprite.sheetIndex+spriteOffset) % SHEET_OBJECTSWIDE;
				int sheety = (sprite.sheetIndex+spriteOffset) / SHEET_OBJECTSWIDE;
				//draw a tiny bit of wall
				al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, getSpriteColor(sprite, this->material, this->layerMaterial, this->veinMaterial));
				al_draw_bitmap_region(imageSheet(sprite,IMGObjectSheet),
					sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT+WALL_CUTOFF_HEIGHT,
					SPRITEWIDTH, SPRITEHEIGHT-WALL_CUTOFF_HEIGHT, drawx, drawy - (WALLHEIGHT)+WALL_CUTOFF_HEIGHT, 0);
				al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
				if(sprite.subSprites.size() > 0)
				{
					for(int i = 0; i < sprite.subSprites.size(); i++)
					{
						sheetx = (sprite.subSprites[i].sheetIndex+spriteOffset) % SHEET_OBJECTSWIDE;
						sheety = (sprite.subSprites[i].sheetIndex+spriteOffset) / SHEET_OBJECTSWIDE;
						al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, getSpriteColor(sprite.subSprites[i], this->material, this->layerMaterial, this->veinMaterial));
						al_draw_bitmap_region(imageSheet(sprite.subSprites[i],IMGObjectSheet),
							sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT+WALL_CUTOFF_HEIGHT,
							SPRITEWIDTH, SPRITEHEIGHT-WALL_CUTOFF_HEIGHT, drawx, drawy - (WALLHEIGHT)+WALL_CUTOFF_HEIGHT, 0);
						al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
					}
				}
				//draw cut-off floor thing
				al_draw_bitmap_region(IMGObjectSheet, 
					TILEWIDTH * SPRITEFLOOR_CUTOFF, 0,
					SPRITEWIDTH, SPRITEWIDTH, 
					drawx, drawy-(SPRITEHEIGHT-WALL_CUTOFF_HEIGHT)/2, 0);
			}
			else 
			{
				//al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, al_map_rgb(sprite.shadeRed, sprite.shadeGreen, sprite.shadeBlue));
				al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, getSpriteColor(sprite, this->material, this->layerMaterial, this->veinMaterial));
				DrawSpriteFromSheet(sprite.sheetIndex+spriteOffset, imageSheet(sprite,IMGObjectSheet), drawx, drawy );
				al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
				if(sprite.subSprites.size() > 0)
				{
					for(int i = 0; i < sprite.subSprites.size(); i++)
					{
						al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, getSpriteColor(sprite.subSprites[i], this->material, this->layerMaterial, this->veinMaterial));
						DrawSpriteFromSheet(sprite.subSprites[i].sheetIndex+spriteOffset, imageSheet(sprite.subSprites[i],IMGObjectSheet), drawx, drawy );
						al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
					}
				}

				if(sprite.needOutline)
				{
					//drawy -= (WALLHEIGHT);
					//Northern border
					if(this->depthBorderNorth)
						DrawSpriteFromSheet(281, IMGObjectSheet, drawx, drawy );

					//Western border
					if(this->depthBorderWest)
						DrawSpriteFromSheet(280, IMGObjectSheet, drawx, drawy );

					//drawy += (WALLHEIGHT);
				}
			}
		}
	}

	//water
	if(water.index > 0){
		int spriteNum = 0;
		int waterlevel = water.index;

		//if(waterlevel == 7) waterlevel--;

		if(water.type == 0)
			spriteNum = SPRITEOBJECT_WATERLEVEL1 + waterlevel - 1;
		else
			spriteNum = SPRITEOBJECT_WATERLEVEL1_LAVA + waterlevel - 1;
		DrawSpriteFromSheet( spriteNum, IMGObjectSheet, drawx, drawy );
	}

	// creature
	// ensure there is *some* creature according to the map data
	// (no guarantee it is the right one)
	if(creaturePresent)
	{
		DrawCreature(drawx, drawy, creature);
	}

	//second part of snow
	if(snowlevel > 75)
	{
		DrawSpriteFromSheet( 24, IMGObjectSheet, drawx, drawy );
	}
	else if(snowlevel > 50)
	{
		DrawSpriteFromSheet( 25, IMGObjectSheet, drawx, drawy );
	}
	else if(snowlevel > 25)
	{
		DrawSpriteFromSheet( 26, IMGObjectSheet, drawx, drawy );
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
	al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
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
	correctBlockForRotation( drawx, drawy, drawz);
	pointToScreen((int*)&drawx, (int*)&drawy, drawz);
	drawx -= TILEWIDTH>>1;

	// creature
	// ensure there is *some* creature according to the map data
	// (no guarantee it is the right one)
	if(creaturePresent)
	{
		DrawCreatureText(drawx, drawy, creature);
	}

}

void Block::DrawRamptops(){
	if (ramp.type > 0)
	{
		int op, src, dst, alpha_op, alpha_src, alpha_dst;
		ALLEGRO_COLOR color;
		al_get_separate_blender(&op, &src, &dst, &alpha_op, &alpha_src, &alpha_dst, &color);
		int sheetOffsetX, sheetOffsetY;
		/*if(config.hide_outer_blocks){
		if(x == ownerSegment->x || x == ownerSegment->x + ownerSegment->sizex - 1) return;
		if(y == ownerSegment->y || y == ownerSegment->y + ownerSegment->sizey - 1) return;
		}*/
		int32_t drawx = x;
		int32_t drawy = y;
		int32_t drawz = z+1; //- ownerSegment->sizez + 1;

		correctBlockForSegmetOffset( drawx, drawy, drawz);
		correctBlockForRotation( drawx, drawy, drawz);
		pointToScreen((int*)&drawx, (int*)&drawy, drawz);
		drawx -= TILEWIDTH>>1;

		t_SpriteWithOffset sprite = GetBlockSpriteMap(ramp.type,material);
		if (sprite.sheetIndex == UNCONFIGURED_INDEX)
		{
			sprite.sheetIndex = 0;
			sprite.fileIndex = INVALID_INDEX;
		}
		if (sprite.sheetIndex != INVALID_INDEX)
		{
			sheetOffsetX = SPRITEWIDTH * ramp.index;
			sheetOffsetY = (TILEHEIGHT + FLOORHEIGHT + SPRITEHEIGHT) * sprite.sheetIndex;
			al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, getSpriteColor(sprite, this->material, this->layerMaterial, this->veinMaterial));
			al_draw_bitmap_region(imageSheet(sprite,IMGRampSheet), sheetOffsetX, sheetOffsetY, SPRITEWIDTH, TILEHEIGHT + FLOORHEIGHT, drawx, drawy, 0);
			al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
		}
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