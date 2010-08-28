#include "common.h"
#include "SpriteObjects.h"
#include "GUI.h"
#include "ContentLoader.h"
#include "WorldSegment.h"
#include "SpriteColors.h"
#include "SpriteMaps.h"

#define ALL_BORDERS 255
uint8_t getBorders(const char* framestring)
{
	if (framestring == NULL)
		return ALL_BORDERS;
	char aframes=0;
	for (int i=0;i<8;i++)
	{
		if (framestring[i]==0)
			return aframes;
		char temp = framestring[i]-'1';
		if (temp < 0 || temp > 8)
			continue;
		aframes = aframes | (1 << temp);
	}
	return aframes;
}
uint8_t getUnBorders(const char* framestring)
{
	if (framestring == NULL)
		return 0;
	char aframes=0;
	for (int i=0;i<8;i++)
	{
		if (framestring[i]==0)
			return aframes;
		char temp = framestring[i]-'1';
		if (temp < 0 || temp > 8)
			continue;
		aframes = aframes | (1 << temp);
	}
	return aframes;
}

c_sprite::c_sprite(void)
{
	reset();
}
void c_sprite::reset(void)
{
	fileindex = -1;
	sheetindex = 0;
	spritewidth = SPRITEWIDTH;
	spriteheight = SPRITEHEIGHT;
	offset_x = 0;
	offset_y = 0;
	offset_user_x = 0;
	offset_user_y = 0;
	variations = 0;
	animframes = ALL_FRAMES;
	shadecolor = al_map_rgb(255,255,255);
	subsprites;
	snowmin = 0;
	snowmax = -1;
	bloodmin = 0;
	bloodmax = -1;
	needoutline=0;
	defaultsheet=IMGObjectSheet;
	tilelayout=BLOCKTILE;
	shadeBy=ShadeNone;
	isoutline = OUTLINENONE;
	halftile = HALFTILECHOP;

	openborders = ALL_BORDERS;
	wallborders = ALL_BORDERS;
	floorborders = ALL_BORDERS;
	rampborders = ALL_BORDERS;
	notopenborders = 0;
	notwallborders = 0;
	notfloorborders = 0;
	notrampborders = 0;
	randomanimation = 0;
	animate = 1;
	{
		for(int i = 0; i < subsprites.size(); i++)
		{
			subsprites[i].reset();
		}
	}
}

c_sprite::~c_sprite(void)
{
}

void c_sprite::set_by_xml(TiXmlElement *elemSprite, int32_t inFile)
{
	fileindex = inFile;
	set_by_xml(elemSprite);
}

void c_sprite::set_by_xml(TiXmlElement *elemSprite)
{
	const char* sheetIndexStr;
	sheetIndexStr = elemSprite->Attribute("sheetIndex");
	if (sheetIndexStr != NULL && sheetIndexStr[0] != 0)
	{
		sheetindex=atoi(sheetIndexStr);
	}
	const char* spriteStr;
	spriteStr = elemSprite->Attribute("sprite");
	if (spriteStr != NULL && spriteStr[0] != 0)
	{
		sheetindex=atoi(spriteStr);
	}
	const char* indexStr;
	indexStr = elemSprite->Attribute("index");
	if (indexStr != NULL && indexStr[0] != 0)
	{
		sheetindex=atoi(indexStr);
	}
	const char* animoffStr;
	animoffStr = elemSprite->Attribute("random_anim_offset");
	if (animoffStr != NULL && indexStr[0] != 0)
	{
		randomanimation=atoi(animoffStr);
	}
	//load files, if any
	const char* filename = elemSprite->Attribute("file");
	if (filename != NULL && filename[0] != 0)
	{
		fileindex = loadConfigImgFile((char*)filename,elemSprite);
	}

	animframes = getAnimFrames(elemSprite->Attribute("frames"));
	if (animframes == 0)
		animframes = ALL_FRAMES;

	openborders = getBorders(elemSprite->Attribute("needopen"));

	floorborders = getBorders(elemSprite->Attribute("needfloor"));

	wallborders = getBorders(elemSprite->Attribute("needwall"));

	rampborders = getBorders(elemSprite->Attribute("needramp"));

	notopenborders = getUnBorders(elemSprite->Attribute("neednotopen"));

	notfloorborders = getUnBorders(elemSprite->Attribute("neednotfloor"));

	notwallborders = getUnBorders(elemSprite->Attribute("neednotwall"));

	notrampborders = getUnBorders(elemSprite->Attribute("neednotramp"));

	//check for randomised tiles
	const char* spriteVariationsStr = elemSprite->Attribute("variations");
	if (spriteVariationsStr == NULL || spriteVariationsStr[0] == 0)
	{
		variations = 0;
	}
	else 
	{
		variations=atoi(spriteVariationsStr);
	}

	//decide what the sprite should be shaded by.
	const char* spriteVarColorStr = elemSprite->Attribute("color");
	if (spriteVarColorStr == NULL || spriteVarColorStr[0] == 0)
	{
		shadeBy = ShadeNone;
	}
	else
	{
		shadeBy = getShadeType(spriteVarColorStr);
	}

	//some sprites should only be drawn when the tile is chopped in half
	const char* spriteChopStr = elemSprite->Attribute("halftile");
	if (spriteChopStr == NULL || spriteChopStr[0] == 0)
	{
		halftile = HALFTILECHOP;
	}
	else if( strcmp(spriteChopStr, "chop") == 0)
	{
		halftile = HALFTILECHOP;
	}
	else if( strcmp(spriteChopStr, "yes") == 0)
	{
		halftile = HALFTILEYES;
	}
	else if( strcmp(spriteChopStr, "no") == 0)
	{
		halftile = HALFTILENO;
	}
	else if( strcmp(spriteChopStr, "both") == 0)
	{
		halftile = HALFTILEBOTH;
	}

	//some sprites are actually tile borders.
	const char* spriteBorderStr = elemSprite->Attribute("tileborder");
	if (spriteBorderStr == NULL || spriteBorderStr[0] == 0)
	{
		isoutline = OUTLINENONE;
	}
	else if( strcmp(spriteBorderStr, "none") == 0)
	{
		isoutline = OUTLINENONE;
	}
	else if( strcmp(spriteBorderStr, "left") == 0)
	{
		isoutline = OUTLINELEFT;
	}
	else if( strcmp(spriteBorderStr, "right") == 0)
	{
		isoutline = OUTLINERIGHT;
	}
	else if( strcmp(spriteBorderStr, "bottom") == 0)
	{
		isoutline = OUTLINEBOTTOM;
	}

	//do bodyparts
	const char* bodyPartStr = elemSprite->Attribute("bodypart");
	//clear old bodypart string
	memset(bodypart, 0, sizeof(bodypart));
	//copy new, if found
	if (bodyPartStr != NULL && bodyPartStr[0] != 0)
	{
		strcpy(bodypart, bodyPartStr);
	}

	uint8_t red, green, blue, alpha;
	//do custom colors
	const char* spriteRedStr = elemSprite->Attribute("red");
	if (spriteRedStr == NULL || spriteRedStr[0] == 0)
	{
		red = 255;
	}
	else red=atoi(spriteRedStr);
	const char* spriteGreenStr = elemSprite->Attribute("green");
	if (spriteGreenStr == NULL || spriteGreenStr[0] == 0)
	{
		green = 255;
	}
	else green=atoi(spriteGreenStr);
	const char* spriteBlueStr = elemSprite->Attribute("blue");
	if (spriteBlueStr == NULL || spriteBlueStr[0] == 0)
	{
		blue = 255;
	}
	else blue=atoi(spriteBlueStr);
	const char* spriteAlphaStr = elemSprite->Attribute("alpha");
	if (spriteAlphaStr == NULL || spriteAlphaStr[0] == 0)
	{
		alpha = 255;
	}
	else alpha=atoi(spriteAlphaStr);
	shadecolor = al_map_rgba(red, green, blue, alpha);

	//Should the sprite be shown only when there is snow?
	const char* spriteSnowMinStr = elemSprite->Attribute("snow_min");
	if (spriteSnowMinStr == NULL || spriteSnowMinStr[0] == 0)
	{
		snowmin = 0;
	}
	else snowmin=atoi(spriteSnowMinStr);
	const char* spriteSnowMaxStr = elemSprite->Attribute("snow_max");
	if (spriteSnowMaxStr == NULL || spriteSnowMaxStr[0] == 0)
	{
		snowmax = -1;
	}
	else snowmax=atoi(spriteSnowMaxStr);

	//Should the sprite be shown only when there is blood?
	const char* spritebloodMinStr = elemSprite->Attribute("blood_min");
	if (spritebloodMinStr == NULL || spritebloodMinStr[0] == 0)
	{
		bloodmin = 0;
	}
	else bloodmin=atoi(spritebloodMinStr);
	const char* spritebloodMaxStr = elemSprite->Attribute("blood_max");
	if (spritebloodMaxStr == NULL || spritebloodMaxStr[0] == 0)
	{
		bloodmax = -1;
	}
	else bloodmax=atoi(spritebloodMaxStr);

		//Should the sprite be shown only when there is mud?
	const char* spritemudMinStr = elemSprite->Attribute("mud_min");
	if (spritemudMinStr == NULL || spritemudMinStr[0] == 0)
	{
		mudmin = 0;
	}
	else mudmin=atoi(spritemudMinStr);
	const char* spritemudMaxStr = elemSprite->Attribute("mud_max");
	if (spritemudMaxStr == NULL || spritemudMaxStr[0] == 0)
	{
		mudmax = -1;
	}
	else mudmax=atoi(spritemudMaxStr);

	//Add user settable sprite offsets
	const char* strOffsetX = elemSprite->Attribute("offsetx");
	if (strOffsetX == NULL || strOffsetX[0] == 0)
	{
		offset_user_x = 0;
	}
	else offset_user_x=atoi(strOffsetX);
	const char* strOffsetY = elemSprite->Attribute("offsety");
	if (strOffsetY == NULL || strOffsetY[0] == 0)
	{
		offset_user_y = 0;
	}
	else offset_user_y=atoi(strOffsetY);

	//not all tiles work well with an outline
	const char* spriteOutlineStr = elemSprite->Attribute("outline");
	if (spriteOutlineStr != NULL && spriteOutlineStr[0] != 0)
		needoutline=(atoi(spriteOutlineStr) == 1);

	subsprites.clear();
	//add subsprites, if any.
	TiXmlElement* elemSubSprite = elemSprite->FirstChildElement("subsprite");
	for(TiXmlElement* elemSubType = elemSprite->FirstChildElement("subsprite");
		elemSubType;
		elemSubType = elemSubType->NextSiblingElement("subsprite"))
	{
		c_sprite subsprite;
		subsprite.set_size(spritewidth, spriteheight);
		subsprite.set_by_xml(elemSubType, fileindex);
		subsprite.set_offset(offset_x, offset_y);
		subsprites.push_back(subsprite);
	}
}

/// This is just a very basic sprite drawing routine. all it uses are screen coords
void c_sprite::draw_screen(int x, int y)
{
	int sheetx = sheetindex % SHEET_OBJECTSWIDE;
	int sheety = sheetindex / SHEET_OBJECTSWIDE;
	if(fileindex == -1)
		al_draw_bitmap_region(IMGObjectSheet, sheetx * spritewidth, sheety * spriteheight, spritewidth, spriteheight, x + offset_x, y + offset_y, 0);
	else 
		al_draw_bitmap_region(getImgFile(fileindex), sheetx * spritewidth, sheety * spriteheight, spritewidth, spriteheight, x + offset_x, y + (offset_y - WALLHEIGHT), 0);
	if(!subsprites.empty())
	{
		for(int i = 0; i < subsprites.size(); i++)
		{
			subsprites[i].draw_screen(x, y);
		}
	}
}

void c_sprite::draw_world(int x, int y, int z, bool chop)
{
	draw_world_offset(x, y, z, 0, chop);
}


void c_sprite::draw_world_offset(int x, int y, int z, int tileoffset, bool chop)
{
	int rando = randomCube[x%RANDOM_CUBE][y%RANDOM_CUBE][z%RANDOM_CUBE];
	int offsetAnimFrame = (randomanimation?rando:0 + currentAnimationFrame) % MAX_ANIMFRAME;
	if ((animframes & (1 << offsetAnimFrame)) || !animate)
	{
		int randoffset = 0;
		if(variations)
			randoffset = rando%variations;
		Block* b = viewedSegment->getBlock(x, y, z);
		if(((openborders & b->openborders) || (rampborders & b->rampborders) || (wallborders & b->wallborders) || (floorborders & b->floorborders)) && !((notopenborders & b->openborders) || (notrampborders & b->rampborders) || (notwallborders & b->wallborders) || (notfloorborders & b->floorborders)))
		{
			if((snowmin <= b->snowlevel && (snowmax == -1 || snowmax >= b->snowlevel)) && (bloodmin <= b->bloodlevel && (bloodmax == -1 || bloodmax >= b->bloodlevel)) && (mudmin <= b->mudlevel && (mudmax == -1 || mudmax >= b->mudlevel)))
			{
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

				int sheetx, sheety;
				if(tilelayout == BLOCKTILE)
				{
					sheetx = ((sheetindex+tileoffset+randoffset) % SHEET_OBJECTSWIDE) * spritewidth;
					sheety = ((sheetindex+tileoffset+randoffset) / SHEET_OBJECTSWIDE) * spriteheight;
				}
				else if(tilelayout == RAMPBOTTOMTILE)
				{
					sheetx = sheetx = SPRITEWIDTH * b->ramp.index;
					sheety = sheety = ((TILEHEIGHT + FLOORHEIGHT + SPRITEHEIGHT) * (sheetindex+tileoffset+randoffset))+(TILEHEIGHT + FLOORHEIGHT);
				}
				else if(tilelayout == RAMPTOPTILE)
				{
					sheetx = sheetx = SPRITEWIDTH * b->ramp.index;
					sheety = sheety = (TILEHEIGHT + FLOORHEIGHT + SPRITEHEIGHT) * (sheetindex+tileoffset+randoffset);
				}
				else
				{
					sheetx = ((sheetindex+tileoffset+randoffset) % SHEET_OBJECTSWIDE) * spritewidth;
					sheety = ((sheetindex+tileoffset+randoffset) / SHEET_OBJECTSWIDE) * spriteheight;
				}
				if(chop && ( halftile == HALFTILECHOP))
				{
					if(fileindex < 0)
						al_draw_tinted_bitmap_region(defaultsheet, get_color(b), sheetx, sheety+WALL_CUTOFF_HEIGHT, spritewidth, spriteheight-WALL_CUTOFF_HEIGHT, drawx + offset_x + offset_user_x, drawy + offset_user_y + (offset_y - WALLHEIGHT)+WALL_CUTOFF_HEIGHT, 0);
					else 
						al_draw_tinted_bitmap_region(getImgFile(fileindex), get_color(b), sheetx, (sheety)+WALL_CUTOFF_HEIGHT, spritewidth, spriteheight-WALL_CUTOFF_HEIGHT, drawx + offset_x + offset_user_x, drawy + offset_user_y + (offset_y - WALLHEIGHT)+WALL_CUTOFF_HEIGHT, 0);
					//draw cut-off floor thing
					al_draw_bitmap_region(IMGObjectSheet, 
						TILEWIDTH * SPRITEFLOOR_CUTOFF, 0,
						SPRITEWIDTH, SPRITEWIDTH, 
						drawx+offset_x, drawy+offset_y-((SPRITEHEIGHT-WALL_CUTOFF_HEIGHT)/2), 0);
				}
				else if ((chop && (halftile == HALFTILEYES)) || (!chop && (halftile == HALFTILENO)) || (!chop && (halftile == HALFTILECHOP)) || (halftile == HALFTILEBOTH))
				{
					if((isoutline == OUTLINENONE) || ((isoutline == OUTLINERIGHT) && (b->depthBorderNorth)) || ((isoutline == OUTLINELEFT) && (b->depthBorderWest)) || ((isoutline == OUTLINEBOTTOM) && (b->depthBorderDown)))
					{
						if(fileindex < 0)
							al_draw_tinted_bitmap_region(defaultsheet, get_color(b), sheetx, sheety, spritewidth, spriteheight, drawx + offset_x + offset_user_x, drawy + offset_user_y + (offset_y - WALLHEIGHT), 0);
						else 
							al_draw_tinted_bitmap_region(getImgFile(fileindex), get_color(b), sheetx, sheety, spritewidth, spriteheight, drawx + offset_x + offset_user_x, drawy + offset_user_y + (offset_y - WALLHEIGHT), 0);
					}
					if(needoutline)
					{
						//drawy -= (WALLHEIGHT);
						//Northern border
						if(b->depthBorderNorth)
							DrawSpriteFromSheet(281, IMGObjectSheet, al_map_rgb(255,255,255), drawx + offset_x, drawy + offset_y );

						//Western border
						if(b->depthBorderWest)
							DrawSpriteFromSheet(280, IMGObjectSheet, al_map_rgb(255,255,255), drawx + offset_x, drawy + offset_y );

						//drawy += (WALLHEIGHT);
					}
				}
				//draw_textf_border(font, al_map_rgb(255,255,255), drawx, drawy, 0, "%d,%d", fileindex, sheetindex);
			}
		}
	}
	if(!subsprites.empty())
	{
		for(int i = 0; i < subsprites.size(); i++)
		{
			subsprites.at(i).draw_world_offset(x, y, z, tileoffset, chop);
		}
	}
}

void c_sprite::set_size(uint8_t x, uint8_t y)
{
	spritewidth = x; spriteheight = y;
	if(!subsprites.empty())
	{
		for(int i = 0; i < subsprites.size(); i++)
		{
			subsprites[i].set_size(spritewidth, spriteheight);
		}
	}
}

void c_sprite::set_offset(int16_t offx, int16_t offy)
{
	offset_x = offx;
	offset_y = offy;
	if(!subsprites.empty())
	{
		for(int i = 0; i < subsprites.size(); i++)
		{
			subsprites[i].set_offset(offset_x, offset_y);
		}
	}
}

void c_sprite::set_tile_layout(uint8_t layout)
{
	tilelayout = layout;
	if(!subsprites.empty())
	{
		for(int i = 0; i < subsprites.size(); i++)
		{
			subsprites[i].set_tile_layout(layout);
		}
	}
}

ALLEGRO_COLOR c_sprite::get_color(void* block)
{
	Block * b = (Block *) block;
	uint32_t dayofLife = 0;
	switch(shadeBy)
	{
	case ShadeNone:
		return al_map_rgb(255, 255, 255);
	case ShadeXml:
		return shadecolor;
	case ShadeMat:
		return lookupMaterialColor(b->material.type, b->material.index);
	case ShadeBuilding:
		return lookupMaterialColor(b->building.info.material.type, b->building.info.material.index);
	case ShadeLayer:
		return lookupMaterialColor(b->layerMaterial.type, b->layerMaterial.index);
	case ShadeVein:
		return lookupMaterialColor(b->veinMaterial.type, b->veinMaterial.index);
	case ShadeMatFore:
		return getDfColor(lookupMaterialFore(b->material.type, b->material.index), lookupMaterialBright(b->material.type, b->material.index));
	case ShadeMatBack:
		return getDfColor(lookupMaterialBack(b->material.type, b->material.index));
	case ShadeLayerFore:
		return getDfColor(lookupMaterialFore(b->layerMaterial.type, b->layerMaterial.index), lookupMaterialBright(b->layerMaterial.type, b->layerMaterial.index));
	case ShadeLayerBack:
		return getDfColor(lookupMaterialBack(b->layerMaterial.type, b->layerMaterial.index));
	case ShadeVeinFore:
		return getDfColor(lookupMaterialFore(b->veinMaterial.type, b->veinMaterial.index), lookupMaterialBright(b->veinMaterial.type, b->veinMaterial.index));
	case ShadeVeinBack:
		return getDfColor(lookupMaterialBack(b->veinMaterial.type, b->veinMaterial.index));
	case ShadeBodyPart:
		if(b->creaturePresent)
		{
			dayofLife = b->creature->birth_year*12*28 + b->creature->birth_time/1200;
			if((!config.skipCreatureTypes) && (!config.skipCreatureTypesEx) && (!config.skipDescriptorColors))
			{
				for(unsigned int j = 0; j<b->creature->nbcolors ; j++)
				{
					if(strcmp(contentLoader.Mats->raceEx[b->creature->race].castes[b->creature->caste].ColorModifier[j].part, bodypart) == 0)
					{
						if(contentLoader.Mats->raceEx.at(b->creature->race).castes.at(b->creature->caste).ColorModifier.at(j).colorlist.size() > b->creature->color[j])
						{
							uint32_t cr_color = contentLoader.Mats->raceEx.at(b->creature->race).castes.at(b->creature->caste).ColorModifier.at(j).colorlist.at(b->creature->color[j]);
							if(cr_color < contentLoader.Mats->color.size())
							{
								if(contentLoader.Mats->raceEx[b->creature->race].castes[b->creature->caste].ColorModifier[j].startdate > 0)
								{

									if((contentLoader.Mats->raceEx[b->creature->race].castes[b->creature->caste].ColorModifier[j].startdate <= dayofLife) &&
										(contentLoader.Mats->raceEx[b->creature->race].castes[b->creature->caste].ColorModifier[j].enddate > dayofLife))
									{
										return al_map_rgb_f(
											contentLoader.Mats->color[cr_color].r,
											contentLoader.Mats->color[cr_color].v,
											contentLoader.Mats->color[cr_color].b);;
									}
								}
								else
									return al_map_rgb_f(
									contentLoader.Mats->color[cr_color].r,
									contentLoader.Mats->color[cr_color].v,
									contentLoader.Mats->color[cr_color].b);
							}
						}
					}
				}
			}
			else return al_map_rgb(255,255,255);
		}
		else return al_map_rgb(255,255,255);
	case ShadeJob:
		if(b->creaturePresent)
		{
			return getDfColor(getJobColor(b->creature->profession));
		}
		else return al_map_rgb(255,255,255);
	case ShadeBlood:
		return b->bloodcolor;
	default:
		return al_map_rgb(255, 255, 255);
	} ;
	return al_map_rgb(255, 255, 255);
}
