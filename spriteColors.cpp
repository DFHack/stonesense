#include "spriteColors.h"
#include "Block.h"
#include "common.h"
#include "GUI.h"
#include "SpriteMaps.h"
#include "GameBuildings.h"
#include "Creatures.h"
#include "WorldSegment.h"
#include "ContentLoader.h"

#include "dfhack/include/DFTypes.h"

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
	t_SpriteWithOffset tempSprite;

	tempSprite.sheetIndex = sprite.sheetIndex;
	tempSprite.fileIndex = sprite.fileIndex;
	tempSprite.shadeColor = sprite.shadeColor;
	tempSprite.shadeBy = sprite.shadeBy;
	strcpy(tempSprite.bodyPart, sprite.bodyPart);

	return getSpriteColor(tempSprite, material, layerMaterial, veinMaterial);
}

ALLEGRO_COLOR getSpriteColor(t_SpriteWithOffset &sprite, t_creature* creature)
{
	Block* b = viewedSegment->getBlock(creature->x, creature->y, creature->z);
	if(sprite.shadeBy == ShadeBodyPart)
	{
		for(unsigned int j = 0; j<b->creature->nbcolors ; j++)
		{
			if(strcmp(contentLoader.Mats->raceEx[creature->race].castes[creature->caste].ColorModifier[j].part, sprite.bodyPart) == 0)
			{
				uint32_t cr_color = contentLoader.Mats->raceEx[creature->race].castes[creature->caste].ColorModifier[j].colorlist[creature->color[j]];
				if(cr_color < contentLoader.Mats->color.size())
				{
					return al_map_rgb_f(
						contentLoader.Mats->color[cr_color].r,
						contentLoader.Mats->color[cr_color].v,
						contentLoader.Mats->color[cr_color].b);
				}
			}
		}
	}
	return getSpriteColor(sprite, b->material, b->layerMaterial, b->veinMaterial);
}

ALLEGRO_COLOR getSpriteColor(t_subSprite &sprite, t_creature* creature)
{
	t_SpriteWithOffset tempSprite;

	tempSprite.sheetIndex = sprite.sheetIndex;
	tempSprite.fileIndex = sprite.fileIndex;
	tempSprite.shadeColor = sprite.shadeColor;
	tempSprite.shadeBy = sprite.shadeBy;
	strcpy(tempSprite.bodyPart, sprite.bodyPart);

	return getSpriteColor(tempSprite, creature);
}
