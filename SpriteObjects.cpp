#include "common.h"
#include "SpriteObjects.h"
#include "GUI.h"
#include "ContentLoader.h"
#include "WorldSegment.h"
#include "SpriteColors.h"
#include "SpriteMaps.h"
#include <cmath>

#include "df/world_raws.h"
#include "df/itemdef_weaponst.h"
#include "df/itemdef_trapcompst.h"
#include "df/itemdef_toyst.h"
#include "df/itemdef_toolst.h"
#include "df/itemdef_instrumentst.h"
#include "df/itemdef_armorst.h"
#include "df/itemdef_ammost.h"
#include "df/itemdef_siegeammost.h"
#include "df/itemdef_shoesst.h"
#include "df/itemdef_shieldst.h"
#include "df/itemdef_helmst.h"
#include "df/itemdef_glovesst.h"
#include "df/itemdef_pantsst.h"
#include "df/itemdef_foodst.h"
#include "df/descriptor_pattern.h"

#define ALL_BORDERS 255

uint8_t dir_to_16(DFHack::TileDirection in)
{
	if(in.north){
		if(in.east){
			if(in.south){
				if(in.west){
					return 15; //NESW
				}
				else{
					return 11; //NES
				}
			}
			else{
				if(in.west){
					return 14; //NEW
				}
				else{
					return 7; //NE
				}
			}
		}
		else{
			if(in.south){
				if(in.west){
					return 13; //NSW
				}
				else{
					return 5; //NS
				}
			}
			else{
				if(in.west){
					return 10; //NW
				}
				else{
					return 1; //N
				}
			}
		}
	}
	else{
		if(in.east){
			if(in.south){
				if(in.west){
					return 12; //ESW
				}
				else{
					return 8; //ES
				}
			}
			else{
				if(in.west){
					return 6; //EW
				}
				else{
					return 2; //E
				}
			}
		}
		else{
			if(in.south){
				if(in.west){
					return 9; //SW
				}
				else{
					return 3; //S
				}
			}
			else{
				if(in.west){
					return 4; //W
				}
				else{
					return 0; //0
				}
			}
		}
	}
	return 0;
}

uint8_t dir_to_4(DFHack::TileDirection in)
{
	uint8_t intermediate = dir_to_16(in);
	switch(intermediate)
	{
	case 0:
	case 2:
	case 5:
	case 8:
	case 12:
	case 15:
		return 0;
	case 3:
	case 6:
	case 9:
	case 13:
		return 1;
	case 4:
	case 10:
	case 14:
		return 2;
	case 1:
	case 7:
	case 11:
		return 3; 
	}
	return 0;
}

DFHack::TileDirection correct_dir_rotation(DFHack::TileDirection in, uint8_t rotation)
{
	uint8_t north,south,east,west;
	north = in.north;
	south = in.south;
	east = in.east;
	west = in.west;
	switch(rotation){
	case 0:
		return in;
	case 1:
		in.north = west;
		in.east = north;
		in.south = east;
		in.west = south;
		break;
	case 2:
		in.north = south;
		in.east = west;
		in.south = north;
		in.west = east;
		break;
	case 3:
		in.north = east;
		in.east = south;
		in.south = west;
		in.west = north;
		break;
	}
	return in;
}

unsigned char get_water_direction( Tile *b )
{
    //Fixme: add the new river ramps, possibly change to a switch statement
	using namespace df::enums::tile_liquid_flow_dir;
	switch(b->flow_direction)
	{
	case none:
		return 0;
	case northwest:
		return 1;
	case north:
		return 2;
	case northeast:
		return 3;
	case east:
		return 4;
	case southeast:
		return 5;
	case south:
		return 6;
	case southwest:
		return 7;
	case west:
		return 8;
	default:
		return 0;
	}
    return 0;
}

unsigned char get_relative_water_direction( Tile *b, uint8_t rotation)
{
    int  dir = get_water_direction(b);
    if(dir == 0) {
        return dir;
    }
    if(rotation == 1) {
        dir += 2;
    }
    if(rotation == 2) {
        dir += 4;
    }
    if(rotation == 3) {
        dir += 6;
    }
    if(dir > 8) {
        dir -= 8;
    }
    return dir;
}

int getBloodOffset ( Tile *b )
{
    int offset = 0;
    int x = b->x, y = b->y, z = b->z;


    if( b->designation.bits.flow_size < 1 && (b->bloodlevel)) {

        // Spatter (should be blood, not blood2) swapped for testing
        if( b->bloodlevel < ssConfig.poolcutoff ) {
            offset = 7;
        }

        // Smear (should be blood2, not blood) swapped for testing
        else {
            // if there's no tile in the respective direction it's false. if there's no blood in that direction it's false too. should also check to see if there's a ramp below, but since blood doesn't flow, that'd look wrong anyway.
            bool _N = ( b->ownerSegment->getTileRelativeTo( x, y, z, eUp ) != NULL ? (b->ownerSegment->getTileRelativeTo( x, y, z, eUp )->bloodlevel > ssConfig.poolcutoff) : false ),
                 _S = ( b->ownerSegment->getTileRelativeTo( x, y, z, eDown ) != NULL ? (b->ownerSegment->getTileRelativeTo( x, y, z, eDown )->bloodlevel > ssConfig.poolcutoff) : false ),
                 _E = ( b->ownerSegment->getTileRelativeTo( x, y, z, eRight ) != NULL ? (b->ownerSegment->getTileRelativeTo( x, y, z, eRight )->bloodlevel > ssConfig.poolcutoff) : false ),
                 _W = ( b->ownerSegment->getTileRelativeTo( x, y, z, eLeft ) != NULL ? (b->ownerSegment->getTileRelativeTo( x, y, z, eLeft )->bloodlevel > ssConfig.poolcutoff) : false );

            // do rules-based puddling
            if( _N || _S || _E || _W ) {
                if( _E ) {
                    if( _N && _S ) {
                        offset = 5;
                    } else if( _S ) {
                        offset = 3;
                    } else if( _W ) {
                        offset = 1;
                    } else {
                        offset = 6;
                    }
                } else if( _W ) {
                    if( _S && _N) {
                        offset = 5;
                    } else if( _S ) {
                        offset = 2;
                    } else {
                        offset = 0;
                    }
                } else if ( _N ) {
                    offset = 4;
                } else {
                    offset = 2;
                }
            } else {
                offset = 8;
            }
        }
    }
    return offset;
}
uint8_t getBorders(const char* framestring)
{
    if (framestring == NULL) {
        return ALL_BORDERS;
    }
    char aframes=0;
    for (int i=0; i<8; i++) {
        if (framestring[i]==0) {
            return aframes;
        }
        char temp = framestring[i]-'1';
        if (temp < 0 || temp > 8) {
            continue;
        }
        aframes = aframes | (1 << temp);
    }
    return aframes;
}
uint8_t getUnBorders(const char* framestring)
{
    if (framestring == NULL) {
        return 0;
    }
    char aframes=0;
    for (int i=0; i<8; i++) {
        if (framestring[i]==0) {
            return aframes;
        }
        char temp = framestring[i]-'1';
        if (temp < 0 || temp > 8) {
            continue;
        }
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
	offsettype = NONE;
    offsetcode = 0;
    animframes = ALL_FRAMES;
    shadecolor = al_map_rgb(255,255,255);
    subsprites;
    snowmin = 0;
    snowmax = -1;
    bloodmin = 0;
    bloodmax = -1;
    mudmin = 0;
    mudmax = -1;
    grassmin = 0;
    grassmax = -1;
    grasstype = -1;
    grassgrowth = GRASS_GROWTH_ANY;
    needoutline=0;
    defaultsheet=IMGObjectSheet;
    platelayout=TILEPLATE;
    shadeBy=ShadeNone;
    isoutline = OUTLINENONE;
    halftile = HALFPLATECHOP;
    water_direction = -1;

    hairtype = hairtypes_invalid;
    hairmin = 0;
    hairmax = -1;
    hairstyle = hairstyles_invalid;

    openborders = ALL_BORDERS;
    wallborders = ALL_BORDERS;
    floorborders = ALL_BORDERS;
    rampborders = ALL_BORDERS;
    upstairborders = ALL_BORDERS;
    downstairborders = ALL_BORDERS;
    lightborders = ALL_BORDERS;
    darkborders =  0;
    notopenborders = 0;
    notwallborders = 0;
    notfloorborders = 0;
    notrampborders = 0;
    notupstairborders = 0;
    notdownstairborders = 0;
    randomanimation = 0;
    animate = 1;
    bloodsprite = 0;
    spritescale=1.0f;
    itemtype= -1;
    itemsubtype=-1;
    light=LIGHTANY;
	pattern_index=0;
    {
        for(int i = 0; i < subsprites.size(); i++) {
            subsprites[i].reset();
        }
    }
}

c_sprite::~c_sprite(void)
{
}

void c_sprite::set_by_xml(TiXmlElement *elemSprite, int32_t inFile, int creatureID, int casteID)
{
    fileindex = inFile;
    set_by_xml(elemSprite);
	
	//do bodyparts
	const char* bodyPartStr = elemSprite->Attribute("bodypart");
	//copy new, if found
	if (bodyPartStr != NULL && bodyPartStr[0] != 0) {
		t_creaturecaste & caste = contentLoader->Mats->raceEx[creatureID].castes[(casteID==INVALID_INDEX) ? 0 : casteID];
		std::vector<t_colormodifier> & colormods = caste.ColorModifier;
		for(int32_t j = 0; j<colormods.size() ; j++) {
			if(colormods[j].part == bodyPartStr) {
				caste_bodypart_index = j;
				return;
			}
		}
		LogError("Failed loading bodypart '%s' of creature '%s' with caste '%s' from xml.", 
			bodyPartStr, contentLoader->Mats->raceEx[creatureID].id.c_str(), caste.id.c_str());
	}

    subsprites.clear();
    //add subsprites, if any.
    TiXmlElement* elemSubSprite = elemSprite->FirstChildElement("subsprite");
    for(TiXmlElement* elemSubType = elemSprite->FirstChildElement("subsprite");
            elemSubType;
            elemSubType = elemSubType->NextSiblingElement("subsprite")) {
        c_sprite subsprite;
        subsprite.set_size(spritewidth, spriteheight);
        subsprite.set_by_xml(elemSubType, fileindex, creatureID, casteID);
        subsprite.set_offset(offset_x, offset_y);
        subsprites.push_back(subsprite);
    }
}

void c_sprite::set_by_xml(TiXmlElement *elemSprite, int32_t inFile)
{
    fileindex = inFile;
    set_by_xml(elemSprite);

    subsprites.clear();
    //add subsprites, if any.
    TiXmlElement* elemSubSprite = elemSprite->FirstChildElement("subsprite");
    for(TiXmlElement* elemSubType = elemSprite->FirstChildElement("subsprite");
            elemSubType;
            elemSubType = elemSubType->NextSiblingElement("subsprite")) {
        c_sprite subsprite;
        subsprite.set_size(spritewidth, spriteheight);
        subsprite.set_by_xml(elemSubType, fileindex);
        subsprite.set_offset(offset_x, offset_y);
        subsprites.push_back(subsprite);
    }
}

void c_sprite::set_by_xml(TiXmlElement *elemSprite)
{
    const char* sheetIndexStr;
    sheetIndexStr = elemSprite->Attribute("sheetIndex");
    if (sheetIndexStr != NULL && sheetIndexStr[0] != 0) {
        sheetindex=atoi(sheetIndexStr);
    }
    const char* spriteStr;
    spriteStr = elemSprite->Attribute("sprite");
    if (spriteStr != NULL && spriteStr[0] != 0) {
        sheetindex=atoi(spriteStr);
    }
    const char* indexStr;
    indexStr = elemSprite->Attribute("index");
    if (indexStr != NULL && indexStr[0] != 0) {
        sheetindex=atoi(indexStr);
    }
    const char* animoffStr;
    animoffStr = elemSprite->Attribute("random_anim_offset");
    if (animoffStr != NULL && animoffStr[0] != 0) {
        randomanimation=atoi(animoffStr);
    }
    const char* scaleStr;
    scaleStr = elemSprite->Attribute("zoom");
    if (scaleStr != NULL && scaleStr[0] != 0) {
        float scalev=atof(scaleStr);
        spritescale=std::pow(2.0f,scalev);
    }
    //load files, if any
    const char* filename = elemSprite->Attribute("file");
    if (filename != NULL && filename[0] != 0) {
        fileindex = loadConfigImgFile((char*)filename,elemSprite);
        if(fileindex == -1) {
            return;
        }
    }

    animframes = getAnimFrames(elemSprite->Attribute("frames"));
    if (animframes == 0) {
        animframes = ALL_FRAMES;
    }

    openborders = getBorders(elemSprite->Attribute("border_open_OR"));

    floorborders = getBorders(elemSprite->Attribute("border_floor_OR"));

    wallborders = getBorders(elemSprite->Attribute("border_wall_OR"));

    rampborders = getBorders(elemSprite->Attribute("border_ramp_OR"));

    upstairborders = getBorders(elemSprite->Attribute("border_upstair_OR"));

    downstairborders = getBorders(elemSprite->Attribute("border_downstair_OR"));

    darkborders = getUnBorders(elemSprite->Attribute("border_dark_OR"));

    lightborders = getBorders(elemSprite->Attribute("border_light_OR"));

    notopenborders = getUnBorders(elemSprite->Attribute("border_open_NOR"));

    notfloorborders = getUnBorders(elemSprite->Attribute("border_floor_NOR"));

    notwallborders = getUnBorders(elemSprite->Attribute("border_wall_NOR"));

    notrampborders = getUnBorders(elemSprite->Attribute("border_ramp_NOR"));

    notupstairborders = getUnBorders(elemSprite->Attribute("border_upstair_NOR"));

    notdownstairborders = getUnBorders(elemSprite->Attribute("border_downstair_NOR"));

    //check for randomised plates
    const char* spriteVariationsStr = elemSprite->Attribute("variations");
    if (spriteVariationsStr != NULL && spriteVariationsStr[0] != 0) {
		offsettype = VARIATIONS;
        offsetcode=atoi(spriteVariationsStr);
    } 
	
    //check for rotationally-dependant plates
    const char* spriteRotationStr = elemSprite->Attribute("rotation");
    if (spriteRotationStr != NULL && spriteRotationStr[0] != 0) {
		offsettype = ROTATION;
        offsetcode=atoi(spriteRotationStr);
    } 

	//check for plate patterns
    const char* spriteVariationTypeStr = elemSprite->Attribute("variationtype");
    if (spriteVariationTypeStr != NULL && spriteVariationTypeStr[0] != 0) {
		if(strcmp(spriteVariationTypeStr, "four") == 0)
			offsettype = FOUR;
		else if(strcmp(spriteVariationTypeStr, "sixteen") == 0)
			offsettype = SIXTEEN;
		else if(strcmp(spriteVariationTypeStr, "random") == 0)
			offsettype = VARIATIONS;
		else if(strcmp(spriteVariationTypeStr, "animation") == 0)
			offsettype = ANIMATION;
		else if(strcmp(spriteVariationTypeStr, "rotation") == 0)
			offsettype = ROTATION;
    } 

    const char* waterDirStr = elemSprite->Attribute("water_direction");
    if (waterDirStr == NULL || waterDirStr[0] == 0) {
        water_direction = -1;
    } else {
        water_direction=atoi(waterDirStr);
    }

    //decide what the sprite should be shaded by.
    const char* spriteVarColorStr = elemSprite->Attribute("color");
    if (spriteVarColorStr == NULL || spriteVarColorStr[0] == 0) {
        shadeBy = ShadeNone;
    } else {
        shadeBy = getShadeType(spriteVarColorStr);
    }

    //some sprites should only be drawn when the plate is chopped in half
    const char* spriteChopStr = elemSprite->Attribute("halftile");
    if (spriteChopStr == NULL || spriteChopStr[0] == 0) {
        halftile = HALFPLATECHOP;
    } else if( strcmp(spriteChopStr, "chop") == 0) {
        halftile = HALFPLATECHOP;
    } else if( strcmp(spriteChopStr, "yes") == 0) {
        halftile = HALFPLATEYES;
    } else if( strcmp(spriteChopStr, "no") == 0) {
        halftile = HALFPLATENO;
    } else if( strcmp(spriteChopStr, "both") == 0) {
        halftile = HALFPLATEBOTH;
    }

    //hidden in the shadows
    const char* spriteShadowStr = elemSprite->Attribute("dark");
    if (spriteShadowStr == NULL || spriteShadowStr[0] == 0) {
        light = LIGHTANY;
    } else if( strcmp(spriteShadowStr, "yes") == 0) {
        light = LIGHTNO;
    } else if( strcmp(spriteShadowStr, "no") == 0) {
        light = LIGHTYES;
    } else if( strcmp(spriteShadowStr, "both") == 0) {
        light = LIGHTANY;
    }

    //some sprites are actually plate borders.
    const char* spriteBorderStr = elemSprite->Attribute("tileborder");
    if (spriteBorderStr == NULL || spriteBorderStr[0] == 0) {
        isoutline = OUTLINENONE;
    } else if( strcmp(spriteBorderStr, "none") == 0) {
        isoutline = OUTLINENONE;
    } else if( strcmp(spriteBorderStr, "left") == 0) {
        isoutline = OUTLINELEFT;
    } else if( strcmp(spriteBorderStr, "right") == 0) {
        isoutline = OUTLINERIGHT;
    } else if( strcmp(spriteBorderStr, "bottom") == 0) {
        isoutline = OUTLINEBOTTOM;
    }

    //Grass states
    const char* GRASS_GROWTH_string = elemSprite->Attribute("grass_state");
    if (GRASS_GROWTH_string == NULL || GRASS_GROWTH_string[0] == 0) {
        grassgrowth = GRASS_GROWTH_ANY;
    } else if( strcmp(GRASS_GROWTH_string, "any") == 0) {
        grassgrowth = GRASS_GROWTH_ANY;
    } else if( strcmp(GRASS_GROWTH_string, "green") == 0) {
        grassgrowth = GRASS_GROWTH_NORMAL;
    } else if( strcmp(GRASS_GROWTH_string, "dry") == 0) {
        grassgrowth = GRASS_GROWTH_DRY;
    } else if( strcmp(GRASS_GROWTH_string, "dead") == 0) {
        grassgrowth = GRASS_GROWTH_DEAD;
    }


    //Hairtypes
    const char* HAIR_TYPE_string = elemSprite->Attribute("hair_type");
    if (HAIR_TYPE_string == NULL || HAIR_TYPE_string[0] == 0) {
        hairtype = hairtypes_invalid;
    } else if( strcmp(HAIR_TYPE_string, "any") == 0) {
        hairtype = hairtypes_invalid;
    } else if( strcmp(HAIR_TYPE_string, "hair") == 0) {
        hairtype = HAIR;
    } else if( strcmp(HAIR_TYPE_string, "beard") == 0) {
        hairtype = BEARD;
    } else if( strcmp(HAIR_TYPE_string, "moustache") == 0) {
        hairtype = MOUSTACHE;
    } else if( strcmp(HAIR_TYPE_string, "sideburns") == 0) {
        hairtype = SIDEBURNS;
    }

    //Hairstyles
    const char* HAIR_STYLE_string = elemSprite->Attribute("hair_style");
    if (HAIR_STYLE_string == NULL || HAIR_STYLE_string[0] == 0) {
        hairstyle = hairstyles_invalid;
    } else if( strcmp(HAIR_STYLE_string, "any") == 0) {
        hairstyle = hairstyles_invalid;
    } else if( strcmp(HAIR_STYLE_string, "combed") == 0) {
        hairstyle = NEATLY_COMBED;
    } else if( strcmp(HAIR_STYLE_string, "braid") == 0) {
        hairstyle = BRAIDED;
    } else if( strcmp(HAIR_STYLE_string, "two_braid") == 0) {
        hairstyle = DOUBLE_BRAID;
    } else if( strcmp(HAIR_STYLE_string, "ponytails") == 0) {
        hairstyle = PONY_TAILS;
    } else if( strcmp(HAIR_STYLE_string, "unkempt") == 0) {
        hairstyle = CLEAN_SHAVEN;
    }

    const char* HAIR_MIN_string = elemSprite->Attribute("hair_min");
    if (HAIR_MIN_string == NULL || HAIR_MIN_string[0] == 0) {
        hairmin = 0;
    } else {
        hairmin=atoi(HAIR_MIN_string);
    }

    const char* HAIR_MAX_string = elemSprite->Attribute("hair_max");
    if (HAIR_MAX_string == NULL || HAIR_MAX_string[0] == 0) {
        hairmax = -1;
    } else {
        hairmax=atoi(HAIR_MAX_string);
    }

	//picking different patterns isn't really useful, but finding the colors within them is.
	const char* spritePatternIndexStr = elemSprite->Attribute("pattern_index");
	if (spritePatternIndexStr == NULL || spritePatternIndexStr[0] == 0) {
		//if it's a building sprite, the default pattern index needs to be -1. otherwise 0 is fine.
		if(shadeBy == ShadeBuilding)
			pattern_index = -1;
		else pattern_index = 0;
	} else {
		pattern_index=atoi(spritePatternIndexStr);
	}

    uint8_t red, green, blue, alpha;
    //do custom colors
    const char* spriteRedStr = elemSprite->Attribute("red");
    if (spriteRedStr == NULL || spriteRedStr[0] == 0) {
        red = 255;
    } else {
        red=atoi(spriteRedStr);
    }
    const char* spriteGreenStr = elemSprite->Attribute("green");
    if (spriteGreenStr == NULL || spriteGreenStr[0] == 0) {
        green = 255;
    } else {
        green=atoi(spriteGreenStr);
    }
    const char* spriteBlueStr = elemSprite->Attribute("blue");
    if (spriteBlueStr == NULL || spriteBlueStr[0] == 0) {
        blue = 255;
    } else {
        blue=atoi(spriteBlueStr);
    }
    const char* spriteAlphaStr = elemSprite->Attribute("alpha");
    if (spriteAlphaStr == NULL || spriteAlphaStr[0] == 0) {
        alpha = 255;
    } else {
        alpha=atoi(spriteAlphaStr);
    }
    shadecolor = al_map_rgba(red, green, blue, alpha);

    //do references to predefined named colors 
    const char* namedColorStr = elemSprite->Attribute("color_name");
    if (namedColorStr == NULL || namedColorStr[0] == 0) {
        namedcolor=al_map_rgb(255, 255, 255);
    } else {
        int colorindex = lookupIndexedType(namedColorStr, contentLoader->Mats->color);
        t_descriptor_color col = contentLoader->Mats->color[colorindex];
        namedcolor = al_map_rgb_f( col.red, col.green, col.blue);
    }

    //Should the sprite be shown only when there is snow?
    const char* spriteSnowMinStr = elemSprite->Attribute("snow_min");
    if (spriteSnowMinStr == NULL || spriteSnowMinStr[0] == 0) {
        snowmin = 0;
    } else {
        snowmin=atoi(spriteSnowMinStr);
    }
    const char* spriteSnowMaxStr = elemSprite->Attribute("snow_max");
    if (spriteSnowMaxStr == NULL || spriteSnowMaxStr[0] == 0) {
        snowmax = -1;
    } else {
        snowmax=atoi(spriteSnowMaxStr);
    }

    //Should the sprite be shown only when there is grass?
    const char* spriteGrassMinStr = elemSprite->Attribute("grass_min");
    if (spriteGrassMinStr == NULL || spriteGrassMinStr[0] == 0) {
        grassmin = 0;
    } else {
        grassmin=atoi(spriteGrassMinStr);
    }
    const char* spriteGrassMaxStr = elemSprite->Attribute("grass_max");
    if (spriteGrassMaxStr == NULL || spriteGrassMaxStr[0] == 0) {
        grassmax = -1;
    } else {
        grassmax=atoi(spriteGrassMaxStr);
    }

    //does the sprite match a particular grass type?
    const char* idstr = elemSprite->Attribute("grass_type");
    if (idstr == NULL || idstr[0] == 0) {
        grasstype = INVALID_INDEX;
    } else {
        grasstype = lookupIndexedType(idstr,contentLoader->organic);
    }

    //find the item type
    const char* equiptypestr = elemSprite->Attribute("equipment_class");
    if (equiptypestr == NULL || equiptypestr[0] == 0) {
        itemtype = INVALID_INDEX;
    } else {
        df::item_type index;
        if (find_enum_item(&index, equiptypestr)) {
            itemtype = (int)index;
        }
    }

    const char* equipsindexstr = elemSprite->Attribute("equipment_name");
    if (equipsindexstr == NULL || equipsindexstr[0] == 0) {
        itemsubtype = 0;
    } else if(equipsindexstr == "NONE") {
        itemsubtype = INVALID_INDEX;
    } else {
        df::world_raws::T_itemdefs &defs = df::global::world->raws.itemdefs;
        switch(itemtype) {
		case item_type::WEAPON:
			itemsubtype = lookupIndexedPonterType(equipsindexstr, defs.weapons);
			break;
		case item_type::TRAPCOMP:
			itemsubtype = lookupIndexedPonterType(equipsindexstr, defs.trapcomps);
			break;
		case item_type::TOY:
			itemsubtype = lookupIndexedPonterType(equipsindexstr, defs.toys);
			break;
		case item_type::TOOL:
			itemsubtype = lookupIndexedPonterType(equipsindexstr, defs.tools);
			break;
		case item_type::INSTRUMENT:
			itemsubtype = lookupIndexedPonterType(equipsindexstr, defs.instruments);
			break;
		case item_type::ARMOR:
			itemsubtype = lookupIndexedPonterType(equipsindexstr, defs.armor);
			break;
		case item_type::AMMO:
			itemsubtype = lookupIndexedPonterType(equipsindexstr, defs.ammo);
			break;
		case item_type::SIEGEAMMO:
			itemsubtype = lookupIndexedPonterType(equipsindexstr, defs.siege_ammo);
			break;
		case item_type::GLOVES:
			itemsubtype = lookupIndexedPonterType(equipsindexstr, defs.gloves);
			break;
		case item_type::SHOES:
			itemsubtype = lookupIndexedPonterType(equipsindexstr, defs.shoes);
			break;
		case item_type::SHIELD:
			itemsubtype = lookupIndexedPonterType(equipsindexstr, defs.shields);
			break;
		case item_type::HELM:
			itemsubtype = lookupIndexedPonterType(equipsindexstr, defs.helms);
			break;
		case item_type::PANTS:
			itemsubtype = lookupIndexedPonterType(equipsindexstr, defs.pants);
			break;
		case item_type::FOOD:
			itemsubtype = lookupIndexedPonterType(equipsindexstr, defs.food);
			break;
        default:
            itemsubtype = 0;
        }
    }

    //shoud the prite be drawn only if the equipment has a specific material?
    const char* equipmatstr = elemSprite->Attribute("equipment_material");
    if (equipmatstr == NULL || equipmatstr[0] == 0) {
        itemmat = INVALID_INDEX;
    } else {
        itemmat = lookupMaterialType(equipmatstr);
    }



    //Should the sprite be shown only when there is blood?
    const char* spritebloodMinStr = elemSprite->Attribute("blood_min");
    if (spritebloodMinStr == NULL || spritebloodMinStr[0] == 0) {
        bloodmin = 0;
    } else {
        bloodmin=atoi(spritebloodMinStr);
    }
    const char* spritebloodMaxStr = elemSprite->Attribute("blood_max");
    if (spritebloodMaxStr == NULL || spritebloodMaxStr[0] == 0) {
        bloodmax = -1;
    } else {
        bloodmax=atoi(spritebloodMaxStr);
    }

    //Should the sprite be shown only when there is mud?
    const char* spritemudMinStr = elemSprite->Attribute("mud_min");
    if (spritemudMinStr == NULL || spritemudMinStr[0] == 0) {
        mudmin = 0;
    } else {
        mudmin=atoi(spritemudMinStr);
    }
    const char* spritemudMaxStr = elemSprite->Attribute("mud_max");
    if (spritemudMaxStr == NULL || spritemudMaxStr[0] == 0) {
        mudmax = -1;
    } else {
        mudmax=atoi(spritemudMaxStr);
    }

    //Add user settable sprite offsets
    const char* strOffsetX = elemSprite->Attribute("offsetx");
    if (strOffsetX == NULL || strOffsetX[0] == 0) {
        offset_user_x = 0;
    } else {
        offset_user_x=atoi(strOffsetX);
    }
    const char* strOffsetY = elemSprite->Attribute("offsety");
    if (strOffsetY == NULL || strOffsetY[0] == 0) {
        offset_user_y = 0;
    } else {
        offset_user_y=atoi(strOffsetY);
    }

    //not all plates work well with an outline
    const char* spriteOutlineStr = elemSprite->Attribute("outline");
    if (spriteOutlineStr != NULL && spriteOutlineStr[0] != 0) {
        needoutline=(atoi(spriteOutlineStr) == 1);
    }

    //get the possible offset for blood bools
    const char* spriteBloodStr = elemSprite->Attribute("blood_sprite");
    if (spriteBloodStr != NULL && spriteBloodStr[0] != 0) {
        bloodsprite=(atoi(spriteBloodStr) == 1);
    }
}

/// This is just a very basic sprite drawing routine. all it uses are screen coords
//void c_sprite::draw_screen(int x, int y)
//{
//    int sheetx = sheetindex % SHEET_OBJECTSWIDE;
//    int sheety = sheetindex / SHEET_OBJECTSWIDE;
//    if(fileindex == -1) {
//
//        al_draw_bitmap_region(IMGObjectSheet, sheetx * spritewidth * spritescale, sheety * spriteheight * spritescale, spritewidth * spritescale, spriteheight * spritescale, x + offset_x, y + offset_y, 0);
//    } else {
//
//        al_draw_bitmap_region(getImgFile(fileindex), sheetx * spritewidth * spritescale, sheety * spriteheight * spritescale, spritewidth * spritescale, spriteheight * spritescale, x + offset_x, y + (offset_y - WALLHEIGHT * spritescale), 0);
//    }
//    if(!subsprites.empty()) {
//        for(int i = 0; i < subsprites.size(); i++) {
//            subsprites[i].draw_screen(x, y);
//        }
//    }
//}

void c_sprite::assemble_world_offset(int x, int y, int z, int plateoffset, Tile * b, bool chop)
{
    if(defaultsheet == 0) {
        defaultsheet = IMGObjectSheet;
    }
    //sprites can be offset by a random amount, both animationwise, and just variationwise.
    //the base offset is set here.
    int rando = randomCube[(b->x)%RANDOM_CUBE][(b->y)%RANDOM_CUBE][(b->z)%RANDOM_CUBE];
    //and the random offset of the animation frame is set here, provided the sprite iis set to use random animation frames.
    int offsetAnimFrame = ((randomanimation?rando:0) + currentAnimationFrame) % MAX_ANIMFRAME;
    //the following stuff is only bothered with if the animation frames say it should be drawn. this can be over-ridden
    // by setting animate to 0

    if ((animframes & (1 << offsetAnimFrame)) || !animate) {
        int spriteoffset;
		switch(offsettype){
		case ROTATION:
		    spriteoffset = (b->ownerSegment->segState.Rotation + offsetcode) % 4;
            break;
		case VARIATIONS:
            spriteoffset = rando%offsetcode;
			break;
		case ANIMATION:
			spriteoffset = ((randomanimation?rando:0) + currentAnimationFrame) % offsetcode;
			break;
		case SIXTEEN:
			spriteoffset = dir_to_16(correct_dir_rotation(tileDirection(b->tileType), (offsetcode + ssState.Rotation) %4));
			break;
		case FOUR:
			spriteoffset = dir_to_4(correct_dir_rotation(tileDirection(b->tileType), (offsetcode + ssState.Rotation) %4));
			break;
		default:
		    spriteoffset = 0;
		}

        if(!b) {
            return;
        }
        //if the xml says that this is a blood sprite, and offset is set here for proper pooling. this over-rides the random offset.
        if(bloodsprite) {
            spriteoffset = getBloodOffset(b);
        }
        if(!((water_direction < 0) || (water_direction == get_relative_water_direction(b, ssState.Rotation)))) {
            goto draw_subsprite;
        }
        if(!( //these are all border conditions. this first section is a list of positive conditions. if at least one of the border conditions is met, the plate can be shown.
                    (openborders & b->openborders) ||
                    (upstairborders & b->upstairborders) ||
                    (downstairborders & b->downstairborders) ||
                    (rampborders & b->rampborders) ||
                    (wallborders & b->wallborders) ||
                    (floorborders & b->floorborders) ||
                    (lightborders & b->lightborders)
                )) {
            goto draw_subsprite;
        }
        if( //This second tile consists of negative conditions. if /any/ of these border conditions are met, the plate will not be drawn
            (notopenborders & b->openborders) ||
            (notupstairborders & b->upstairborders) ||
            (notdownstairborders & b->downstairborders) ||
            (notrampborders & b->rampborders) ||
            (notwallborders & b->wallborders) ||
            (notfloorborders & b->floorborders) ||
            (darkborders & b->lightborders)
        ) {
            goto draw_subsprite;
        }
        int foo = 0;
        if(!(snowmin <= b->snowlevel &&	(snowmax == -1 || snowmax >= b->snowlevel))) {
            goto draw_subsprite;
        }
        if (!(bloodmin <= b->bloodlevel && (bloodmax == -1 || bloodmax >= b->bloodlevel))) {
            goto draw_subsprite;
        }
        if(!(mudmin <= b->mudlevel && (mudmax == -1 || mudmax >= b->mudlevel))) {
            goto draw_subsprite;
        }
        if(!(grassmin <= b->grasslevel && (grassmax == -1 || grassmax >= b->grasslevel))) {
            goto draw_subsprite;
        }
        //only bother with this plate if it's in the light, or not.
        if(!((light==LIGHTANY) || ((light==LIGHTYES) && b->designation.bits.outside) || ((light==LIGHTNO) && !(b->designation.bits.outside)))) {
            goto draw_subsprite;
        }
        if(!((grasstype == -1) || (grasstype == b->grassmat))) {
            goto draw_subsprite;
        }
        if(!((grassgrowth == GRASS_GROWTH_ANY) ||
                ((grassgrowth == GRASS_GROWTH_NORMAL) &&
                 ((b->tileMaterial() == tiletype_material::GRASS_DARK) ||
                  (b->tileMaterial() == tiletype_material::GRASS_LIGHT))) ||
                ((grassgrowth == GRASS_GROWTH_DRY) &&
                 (b->tileMaterial() == tiletype_material::GRASS_DRY)) ||
                ((grassgrowth == GRASS_GROWTH_DEAD) &&
                 (b->tileMaterial() == tiletype_material::GRASS_DEAD)))) {
            goto draw_subsprite;
        }

        if(itemtype !=  INVALID_INDEX) {
            if(!b->creature) {
                goto draw_subsprite;
            }
			if(itemsubtype >= 0) {
			
				if(!b->creature->inv) {
					goto draw_subsprite;
				}
				if(b->creature->inv->item.empty()) {
					goto draw_subsprite;
				}
				if(b->creature->inv->item.size() <= itemtype) {
					goto draw_subsprite;
				}
				if(b->creature->inv->item[itemtype].empty()) {
					goto draw_subsprite;
				}
				if(itemsubtype >= b->creature->inv->item[itemtype].size()) {
					goto draw_subsprite;
				}
				if(pattern_index >= b->creature->inv->item[itemtype][itemsubtype].size()) {
					goto draw_subsprite;
				}
				if(b->creature->inv->item[itemtype][itemsubtype][pattern_index].matt.type == INVALID_INDEX) {
					goto draw_subsprite;
				}
				if(itemmat != INVALID_INDEX) {
					if(b->creature->inv->item[itemtype][itemsubtype][pattern_index].matt.type != itemmat) {
						goto draw_subsprite;
					}
				}
				
			} else {

			if(b->creature->inv
				&& ! (b->creature->inv->item.empty())
				&& ! (b->creature->inv->item.size() <= itemtype) 
				&& ! (b->creature->inv->item[itemtype].empty())) {
					goto draw_subsprite;
				}
			}
        }

        if(hairtype != hairtypes_invalid) {
            if(hairmin > b->creature->hairlength[hairtype]) {
                goto draw_subsprite;
            }
            if((hairmax >= 0) && (hairmax < b->creature->hairlength[hairtype])) {
                goto draw_subsprite;
            }
            if(hairstyle != hairstyles_invalid && hairstyle !=b->creature->hairstyle[hairtype]) {
                goto draw_subsprite;
            }
        }


        int32_t drawx = x;
        int32_t drawy = y;
        int32_t drawz = z; //- ownerSegment->sizez + 1;

        b->ownerSegment->CorrectTileForSegmentOffset( drawx, drawy, drawz );
        b->ownerSegment->CorrectTileForSegmentRotation( drawx, drawy, drawz );
        int32_t viewx = drawx;
        int32_t viewy = drawy;
        int32_t viewz = drawz;
        pointToScreen((int*)&drawx, (int*)&drawy, drawz);
        drawx -= (TILEWIDTH>>1)*ssConfig.scale;

        if(((drawx + spritewidth*ssConfig.scale) < 0) || (drawx > ssState.ScreenW) || ((drawy + spriteheight*ssConfig.scale) < 0) || (drawy > ssState.ScreenH)) {
            return;
        }

        int sheetx, sheety;
        if(platelayout == TILEPLATE) {
            sheetx = ((sheetindex+plateoffset+spriteoffset) % SHEET_OBJECTSWIDE) * spritewidth;
            sheety = ((sheetindex+plateoffset+spriteoffset) / SHEET_OBJECTSWIDE) * spriteheight;
        } else if(platelayout == RAMPBOTTOMPLATE) {
            sheetx = SPRITEWIDTH * b->rampindex;
            sheety = ((TILETOPHEIGHT + FLOORHEIGHT + SPRITEHEIGHT) * (sheetindex+plateoffset+spriteoffset))+(TILETOPHEIGHT + FLOORHEIGHT);
        } else if(platelayout == RAMPTOPPLATE) {
            sheetx = SPRITEWIDTH * b->rampindex;
            sheety = (TILETOPHEIGHT + FLOORHEIGHT + SPRITEHEIGHT) * (sheetindex+plateoffset+spriteoffset);
        } else {
            sheetx = ((sheetindex+plateoffset+spriteoffset) % SHEET_OBJECTSWIDE) * spritewidth;
            sheety = ((sheetindex+plateoffset+spriteoffset) / SHEET_OBJECTSWIDE) * spriteheight;
        }
        ALLEGRO_COLOR shade_color = shadeAdventureMode(get_color(b), b->fog_of_war, b->designation.bits.outside);
        if(chop && ( halftile == HALFPLATECHOP)) {
            if(fileindex < 0) {
                if(shade_color.a > 0.001f)
                    b->AssembleSprite(
                        defaultsheet, premultiply(shade_color),
                        sheetx * spritescale,
                        (sheety+WALL_CUTOFF_HEIGHT) * spritescale,
                        spritewidth * spritescale,
                        (spriteheight-WALL_CUTOFF_HEIGHT) * spritescale,
                        drawx + (offset_x + offset_user_x)*ssConfig.scale,
                        drawy + (offset_user_y + (offset_y - WALLHEIGHT)+WALL_CUTOFF_HEIGHT)*ssConfig.scale,
                        spritewidth*ssConfig.scale,
                        (spriteheight-WALL_CUTOFF_HEIGHT)*ssConfig.scale,
                        0);
            } else {

                if(shade_color.a > 0.001f)
                    b->AssembleSprite(
                        getImgFile(fileindex),
                        premultiply(shade_color),
                        sheetx * spritescale,
                        (sheety+WALL_CUTOFF_HEIGHT) * spritescale,
                        spritewidth * spritescale,
                        (spriteheight-WALL_CUTOFF_HEIGHT) * spritescale,
                        drawx + (offset_x + offset_user_x)*ssConfig.scale,
                        drawy + (offset_user_y + (offset_y - WALLHEIGHT)+WALL_CUTOFF_HEIGHT)*ssConfig.scale,
                        spritewidth*ssConfig.scale,
                        (spriteheight-WALL_CUTOFF_HEIGHT)*ssConfig.scale,
                        0);
            }

            if(shade_color.a > 0.001f) {
                b->AssembleSprite(
                    IMGObjectSheet,
                    al_map_rgb(255,255,255),
                    TILEWIDTH * SPRITEFLOOR_CUTOFF,
                    0,
                    SPRITEWIDTH, 
                    SPRITEWIDTH,
                    drawx+offset_x, 
                    (drawy+offset_y-(((SPRITEHEIGHT-WALL_CUTOFF_HEIGHT)/2)*ssConfig.scale)),
                    SPRITEWIDTH*ssConfig.scale, SPRITEWIDTH*ssConfig.scale, 
                    0);
            }
        } else if ((chop && (halftile == HALFPLATEYES)) || (!chop && (halftile == HALFPLATENO)) || (!chop && (halftile == HALFPLATECHOP)) || (halftile == HALFPLATEBOTH)) {
            if((isoutline == OUTLINENONE) || ((isoutline == OUTLINERIGHT) && (b->depthBorderNorth)) || ((isoutline == OUTLINELEFT) && (b->depthBorderWest)) || ((isoutline == OUTLINEBOTTOM) && (b->depthBorderDown))) {
                if(fileindex < 0) {
                    if(shade_color.a > 0.001f)
                        b->AssembleSprite(
                            defaultsheet, premultiply(shade_color),
                            sheetx * spritescale,
                            sheety * spritescale,
                            spritewidth * spritescale,
                            spriteheight * spritescale,
                            drawx + (offset_x + offset_user_x)*ssConfig.scale,
                            drawy + (offset_user_y + (offset_y - WALLHEIGHT))*ssConfig.scale,
                            spritewidth*ssConfig.scale,
                            spriteheight*ssConfig.scale,
                            0);
                } else {

                    if(shade_color.a > 0.001f)
                        b->AssembleSprite(
                            getImgFile(fileindex),
                            premultiply(shade_color),
                            sheetx * spritescale,
                            sheety * spritescale,
                            spritewidth * spritescale,
                            spriteheight * spritescale,
                            drawx + (offset_x + offset_user_x)*ssConfig.scale,
                            drawy + (offset_user_y + (offset_y - WALLHEIGHT))*ssConfig.scale,
                            spritewidth*ssConfig.scale,
                            spriteheight*ssConfig.scale,
                            0);
                }
            }
            if(needoutline) {
                //drawy -= (WALLHEIGHT);
                //Northern border
                if(b->depthBorderNorth) {
                    int sheetx = 281 % SHEET_OBJECTSWIDE;
                    int sheety = 281 / SHEET_OBJECTSWIDE;
                    b->AssembleSprite(
                        IMGObjectSheet,
                        al_map_rgb(255,255,255),
                        sheetx * SPRITEWIDTH,
                        sheety * SPRITEHEIGHT,
                        SPRITEWIDTH,
                        SPRITEHEIGHT,
                        drawx + (offset_x)*ssConfig.scale,
                        drawy + (offset_y)*ssConfig.scale - (WALLHEIGHT)*ssConfig.scale,
                        SPRITEWIDTH*ssConfig.scale,
                        SPRITEHEIGHT*ssConfig.scale,
                        0);
                }

                //Western border
                if(b->depthBorderWest) {
                    int sheetx = 280 % SHEET_OBJECTSWIDE;
                    int sheety = 280 / SHEET_OBJECTSWIDE;
                    b->AssembleSprite(
                        IMGObjectSheet,
                        al_map_rgb(255,255,255),
                        sheetx * SPRITEWIDTH,
                        sheety * SPRITEHEIGHT,
                        SPRITEWIDTH,
                        SPRITEHEIGHT,
                        drawx + (offset_x)*ssConfig.scale,
                        drawy + (offset_y)*ssConfig.scale - (WALLHEIGHT)*ssConfig.scale,
                        SPRITEWIDTH*ssConfig.scale,
                        SPRITEHEIGHT*ssConfig.scale,
                        0);
                }

                //drawy += (WALLHEIGHT);
            }
        }
    }
draw_subsprite:
    if(!subsprites.empty()) {
        for(int i = 0; i < subsprites.size(); i++) {
            subsprites.at(i).assemble_world_offset(x, y, z, plateoffset, b, chop);
        }
    }
}

void c_sprite::set_size(uint8_t x, uint8_t y)
{
    spritewidth = x;
    spriteheight = y;
    if(!subsprites.empty()) {
        for(int i = 0; i < subsprites.size(); i++) {
            subsprites[i].set_size(spritewidth, spriteheight);
        }
    }
}

void c_sprite::set_offset(int16_t offx, int16_t offy)
{
    offset_x = offx;
    offset_y = offy;
    if(!subsprites.empty()) {
        for(int i = 0; i < subsprites.size(); i++) {
            subsprites[i].set_offset(offset_x, offset_y);
        }
    }
}

void c_sprite::set_plate_layout(uint8_t layout)
{
    platelayout = layout;
    if(!subsprites.empty()) {
        for(int i = 0; i < subsprites.size(); i++) {
            subsprites[i].set_plate_layout(layout);
        }
    }
}

ALLEGRO_COLOR c_sprite::get_color(void* tile)
{
    Tile * b = (Tile *) tile;
    uint32_t dayofLife = 0;
    switch(shadeBy) {
    case ShadeNone:
        return al_map_rgb(255, 255, 255);
    case ShadeXml:
        return shadecolor;
    case ShadeWood:
    {
                      DFHack::t_matglossPair mat = b->material;
                      if (mat.type == 419) //this is a dirty hack, but it works, at least in vanilla.
                          mat.type = 420;
                      return lookupMaterialColor(mat);
    }
    case ShadeMat:
        return lookupMaterialColor(b->material);
    case ShadeGrowth:
        return growthColor;
    case ShadeNamed:
        return namedcolor;
    case ShadeGrass:
        return lookupMaterialColor(WOOD, b->grassmat);
	case ShadeBuilding:
		if(b->building.constructed_mats.size() == 0)
			return (b->building.info ? lookupMaterialColor(b->building.info->material) : al_map_rgb(255, 255, 255));
		if(pattern_index == -1)
			return(lookupMaterialColor(b->building.constructed_mats[b->building.constructed_mats.size()-1].matt, b->building.constructed_mats[b->building.constructed_mats.size()-1].dyematt));
		return(lookupMaterialColor(b->building.constructed_mats[pattern_index%b->building.constructed_mats.size()].matt, b->building.constructed_mats[pattern_index%b->building.constructed_mats.size()].dyematt));
	case ShadeLayer:
        return lookupMaterialColor(b->layerMaterial);
    case ShadeVein:
        return lookupMaterialColor(b->veinMaterial);
    case ShadeMatFore:
		return ssConfig.colors.getDfColor(lookupMaterialFore(b->material.type, b->material.index), lookupMaterialBright(b->material.type, b->material.index), ssConfig.useDfColors);
    case ShadeMatBack:
		return ssConfig.colors.getDfColor(lookupMaterialBack(b->material.type, b->material.index), ssConfig.useDfColors);
    case ShadeLayerFore:
		return ssConfig.colors.getDfColor(lookupMaterialFore(b->layerMaterial.type, b->layerMaterial.index), lookupMaterialBright(b->layerMaterial.type, b->layerMaterial.index), ssConfig.useDfColors);
    case ShadeLayerBack:
		return ssConfig.colors.getDfColor(lookupMaterialBack(b->layerMaterial.type, b->layerMaterial.index), ssConfig.useDfColors);
    case ShadeVeinFore:
		return ssConfig.colors.getDfColor(lookupMaterialFore(b->veinMaterial.type, b->veinMaterial.index), lookupMaterialBright(b->veinMaterial.type, b->veinMaterial.index), ssConfig.useDfColors);
    case ShadeVeinBack:
		return ssConfig.colors.getDfColor(lookupMaterialBack(b->veinMaterial.type, b->veinMaterial.index), ssConfig.useDfColors);
    case ShadeBodyPart:
        if(b->occ.bits.unit && b->creature) {
			dayofLife = b->creature->birth_year*12*28 + b->creature->birth_time/1200;
			if((!ssConfig.skipCreatureTypes) && (!ssConfig.skipCreatureTypesEx) && (!ssConfig.skipDescriptorColors)) {
				t_creaturecaste & caste = contentLoader->Mats->raceEx[b->creature->race].castes[b->creature->caste];
				std::vector<t_colormodifier> & colormods =caste.ColorModifier;
				if(caste_bodypart_index != INVALID_INDEX && caste_bodypart_index < colormods.size()){
					t_colormodifier & colormod = colormods[caste_bodypart_index];
					if(colormod.colorlist.size() > b->creature->color[caste_bodypart_index]) {
						uint32_t cr_color = colormod.colorlist.at(b->creature->color[caste_bodypart_index]);
						if(cr_color < df::global::world->raws.language.patterns.size()) {
							uint16_t actual_color = df::global::world->raws.language.patterns[cr_color]->colors[pattern_index%df::global::world->raws.language.patterns[cr_color]->colors.size()];
							if(actual_color < contentLoader->Mats->color.size()){
								if(colormod.startdate > 0) {
									if((colormod.startdate <= dayofLife) &&
										(colormod.enddate > dayofLife)) {
											return al_map_rgb_f(
												contentLoader->Mats->color[actual_color].red,
												contentLoader->Mats->color[actual_color].green,
												contentLoader->Mats->color[actual_color].blue);;
									}
								} else
									return al_map_rgb_f(
									contentLoader->Mats->color[actual_color].red,
									contentLoader->Mats->color[actual_color].green,
									contentLoader->Mats->color[actual_color].blue);
							}
						}
					}
				}
			} else {
                return al_map_rgb(255,255,255);
            }
        } else {
            return al_map_rgb(255,255,255);
        }
    case ShadeBlood:
        return b->bloodcolor;
    case ShadeEquip:
		if(ssConfig.show_creature_professions != 3) {
			if(itemsubtype >=0) {
				//FIXME: need a way to get a material for generic types.
				//errors here give pink.
				if(itemtype == -1) {
					return al_map_rgb(255, 0, 0);
				}
				if(b->creature->inv->item.size() <= itemtype) {
					return al_map_rgb(0, 255, 0);
				}
				if(b->creature->inv->item[itemtype].size() <= itemsubtype) {
					return al_map_rgb(255, 255, 0);
				}
				if(b->creature->inv->item[itemtype][itemsubtype].empty()) {
					return al_map_rgb(0, 0, 255);
				}
				return lookupMaterialColor(b->creature->inv->item[itemtype][itemsubtype][pattern_index].matt, b->creature->inv->item[itemtype][itemsubtype][pattern_index].dyematt);
			} else {
				return al_map_rgb(255,255,255);
			}
		}
    case ShadeJob:
        if(b->occ.bits.unit && b->creature) {
			return ssConfig.colors.getDfColor(Units::getProfessionColor(b->creature->origin), ssConfig.useDfColors);
        } else {
            return al_map_rgb(255,255,255);
        }
    case ShadeItem:
        return lookupMaterialColor(b->Item.matt, b->Item.dyematt);
    default:
        return al_map_rgb(255, 255, 255);
    } ;
    return al_map_rgb(255, 255, 255);
}

void c_sprite::set_growthColor(ALLEGRO_COLOR color)
{
    growthColor = color;
    for (int i = 0; i < subsprites.size(); i++)
    {
        subsprites[i].set_growthColor(color);
    }
}
