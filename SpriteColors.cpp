#include "SpriteColors.h"
#include "Block.h"
#include "common.h"
#include "GUI.h"
#include "SpriteMaps.h"
#include "GameBuildings.h"
#include "Creatures.h"
#include "WorldSegment.h"
#include "ContentLoader.h"

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
		return getDfColor(matFore, matBright);
	case ShadeMatBack:
		return getDfColor(matBack);
	case ShadeLayerFore:
		return getDfColor(layerFore, layerBright);
	case ShadeLayerBack:
		return getDfColor(layerBack);
	case ShadeVeinFore:
		return getDfColor(veinFore, veinBright);
	case ShadeVeinBack:
		return getDfColor(veinBack);
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
	uint32_t dayofLife = creature->birth_year*12*28 + creature->birth_time/1200;
	Block* b = viewedSegment->getBlock(creature->x, creature->y, creature->z);
	ALLEGRO_COLOR output;
	if(sprite.shadeBy == ShadeBodyPart)
	{
		if((!config.skipCreatureTypes) && (!config.skipCreatureTypesEx) && (!config.skipDescriptorColors))
		{
		for(unsigned int j = 0; j<b->creature->nbcolors ; j++)
		{
			if(strcmp(contentLoader.Mats->raceEx[creature->race].castes[creature->caste].ColorModifier[j].part, sprite.bodyPart) == 0)
			{
				uint32_t cr_color = contentLoader.Mats->raceEx[creature->race].castes[creature->caste].ColorModifier[j].colorlist[creature->color[j]];
					if(cr_color < contentLoader.Mats->color.size())
					{
						if(contentLoader.Mats->raceEx[creature->race].castes[creature->caste].ColorModifier[j].startdate > 0)
						{

							if((contentLoader.Mats->raceEx[creature->race].castes[creature->caste].ColorModifier[j].startdate <= dayofLife) &&
								(contentLoader.Mats->raceEx[creature->race].castes[creature->caste].ColorModifier[j].enddate > dayofLife))
							{
								output = al_map_rgb_f(
									contentLoader.Mats->color[cr_color].red,
									contentLoader.Mats->color[cr_color].green,
									contentLoader.Mats->color[cr_color].blue);
								return output;
							}
						}
						else
							output = al_map_rgb_f(
							contentLoader.Mats->color[cr_color].red,
							contentLoader.Mats->color[cr_color].green,
							contentLoader.Mats->color[cr_color].blue);
					}
			}
		}
		return output;
		}
		else return al_map_rgb(255,255,255);
	}
	else if(sprite.shadeBy == ShadeJob)
	{
		return getDfColor(getJobColor(creature->profession));
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
ALLEGRO_COLOR getDfColor(int color)
{
	switch (color)
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
	case 8:
		return al_map_rgb(config.colors.dgray_r, config.colors.dgray_g, config.colors.dgray_b);
	case 9:
		return al_map_rgb(config.colors.lblue_r, config.colors.lblue_g, config.colors.lblue_b);
	case 10:
		return al_map_rgb(config.colors.lgreen_r, config.colors.lgreen_g, config.colors.lgreen_b);
	case 11:
		return al_map_rgb(config.colors.lcyan_r, config.colors.lcyan_g, config.colors.lcyan_b);
	case 12:
		return al_map_rgb(config.colors.lred_r, config.colors.lred_g, config.colors.lred_b);
	case 13:
		return al_map_rgb(config.colors.lmagenta_r, config.colors.lmagenta_g, config.colors.lmagenta_b);
	case 14:
		return al_map_rgb(config.colors.yellow_r, config.colors.yellow_g, config.colors.yellow_b);
	case 15:
		return al_map_rgb(config.colors.white_r, config.colors.white_g, config.colors.white_b);
	}
	return al_map_rgb(255,255,255);
}

ALLEGRO_COLOR getDfColor(int color, int bright)
{
	return getDfColor(color+(bright*8));
}
int getJobColor(unsigned char job)
{
	switch (job)
	{
	case 0:
		return 7;
	case 1:
	case 2:
	case 3:
	case 4:
		return 14;
	case 5:
	case 6:
	case 7:
		return 15;
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
		return 2;
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
		return 8;
	case 20:
	case 21:
	case 22:
		return 10;
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
		return 9;
	case 32:
	case 33:
	case 34:
	case 35:
		return 1;
	case 36:
	case 37:
	case 38:
	case 39:
	case 40:
	case 41:
	case 42:
	case 43:
	case 44:
	case 45:
	case 46:
	case 47:
	case 48:
	case 49:
	case 50:
	case 51:
		return 6;
	case 52:
	case 53:
	case 54:
	case 55:
	case 56:
		return 12;
	case 57:
	case 58:
	case 59:
	case 60:
	case 61:
	case 62:
	case 63:
	case 64:
	case 65:
	case 66:
	case 67:
		return 5;
	case 68:
		return 5;
	case 69:
		return 13;
	case 70:
		return 6;
	case 71:
		return 14;
	case 72:
		return 2;
	case 73:
		return 10;
	case 74:
		return 7;
	case 75:
		return 15;
	case 76:
		return 1;
	case 77:
		return 9;
	case 78:
		return 3;
	case 79:
		return 11;
	case 80:
		return 4;
	case 81:
		return 12;
	case 82:
		return 6;
	case 83:
		return 14;
	case 84:
		return 2;
	case 85:
		return 10;
	case 86:
		return 2;
	case 87:
		return 10;
	case 88:
	case 89:
		return 8;
	case 90:
	case 91:
	case 92:
	case 93:
	case 94:
	case 95:
		return 3;
	case 96:
	case 97:
		return 4;
	case 98:
		return 3;
	}
	return 3;
}