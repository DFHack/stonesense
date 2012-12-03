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
#include "df/itemdef_armorst.h"
#include "df/itemdef_shoesst.h"
#include "df/itemdef_shieldst.h"
#include "df/itemdef_helmst.h"
#include "df/itemdef_glovesst.h"
#include "df/itemdef_pantsst.h"

#define ALL_BORDERS 255

unsigned char get_water_direction( Block *b )
{
    //Fixme: add the new river ramps, possibly change to a switch statement
    int tiletype = b->tileType;
    if(tiletype == 0) {
        return 0;
    }
    if(tiletype == 365) {
        return 2;
    }
    if(tiletype == 366) {
        return 6;
    }
    if(tiletype == 367) {
        return 4;
    }
    if(tiletype == 368) {
        return 8;
    }
    if(tiletype == 369) {
        return 1;
    }
    if(tiletype == 370) {
        return 3;
    }
    if(tiletype == 371) {
        return 7;
    }
    if(tiletype == 372) {
        return 5;
    }
    if(tiletype == 373) {
        return 2;
    }
    if(tiletype == 374) {
        return 6;
    }
    if(tiletype == 375) {
        return 4;
    }
    if(tiletype == 376) {
        return 8;
    }
    if(tiletype == 377) {
        return 1;
    }
    if(tiletype == 378) {
        return 3;
    }
    if(tiletype == 379) {
        return 7;
    }
    if(tiletype == 380) {
        return 5;
    }
    return 0;
}

unsigned char get_relative_water_direction( Block *b )
{
    int  dir = get_water_direction(b);
    if(dir == 0) {
        return dir;
    }
    if(ssState.DisplayedRotation == 1) {
        dir += 2;
    }
    if(ssState.DisplayedRotation == 2) {
        dir += 4;
    }
    if(ssState.DisplayedRotation == 3) {
        dir += 6;
    }
    if(dir > 8) {
        dir -= 8;
    }
    return dir;
}

int getBloodOffset ( Block *b )
{
    int offset = 0;
    int x = b->x, y = b->y, z = b->z;


    if( b->water.index < 1 && (b->bloodlevel)) {

        // Spatter (should be blood, not blood2) swapped for testing
        if( b->bloodlevel <= ssConfig.poolcutoff ) {
            offset = 7;
        }

        // Smear (should be blood2, not blood) swapped for testing
        else {
            // if there's no block in the respective direction it's false. if there's no blood in that direction it's false too. should also check to see if there's a ramp below, but since blood doesn't flow, that'd look wrong anyway.
            bool _N = ( b->ownerSegment->getBlockRelativeTo( x, y, z, eUp ) != NULL ? (b->ownerSegment->getBlockRelativeTo( x, y, z, eUp )->bloodlevel > ssConfig.poolcutoff) : false ),
                 _S = ( b->ownerSegment->getBlockRelativeTo( x, y, z, eDown ) != NULL ? (b->ownerSegment->getBlockRelativeTo( x, y, z, eDown )->bloodlevel > ssConfig.poolcutoff) : false ),
                 _E = ( b->ownerSegment->getBlockRelativeTo( x, y, z, eRight ) != NULL ? (b->ownerSegment->getBlockRelativeTo( x, y, z, eRight )->bloodlevel > ssConfig.poolcutoff) : false ),
                 _W = ( b->ownerSegment->getBlockRelativeTo( x, y, z, eLeft ) != NULL ? (b->ownerSegment->getBlockRelativeTo( x, y, z, eLeft )->bloodlevel > ssConfig.poolcutoff) : false );

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
    variations = 0;
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
    grass_growth = GRASS_GROWTH_ANY;
    needoutline=0;
    defaultsheet=IMGObjectSheet;
    tilelayout=BLOCKTILE;
    shadeBy=ShadeNone;
    isoutline = OUTLINENONE;
    halftile = HALFTILECHOP;
    water_direction = -1;

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
    {
        for(int i = 0; i < subsprites.size(); i++) {
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
        int scalev=atoi(scaleStr);
        spritescale=std::pow(2.0f,(float)scalev);
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

    //check for randomised tiles
    const char* spriteVariationsStr = elemSprite->Attribute("variations");
    if (spriteVariationsStr == NULL || spriteVariationsStr[0] == 0) {
        variations = 0;
    } else {
        variations=atoi(spriteVariationsStr);
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

    //some sprites should only be drawn when the tile is chopped in half
    const char* spriteChopStr = elemSprite->Attribute("halftile");
    if (spriteChopStr == NULL || spriteChopStr[0] == 0) {
        halftile = HALFTILECHOP;
    } else if( strcmp(spriteChopStr, "chop") == 0) {
        halftile = HALFTILECHOP;
    } else if( strcmp(spriteChopStr, "yes") == 0) {
        halftile = HALFTILEYES;
    } else if( strcmp(spriteChopStr, "no") == 0) {
        halftile = HALFTILENO;
    } else if( strcmp(spriteChopStr, "both") == 0) {
        halftile = HALFTILEBOTH;
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

    //some sprites are actually tile borders.
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
    const char* grass_growth_string = elemSprite->Attribute("grass_state");
    if (grass_growth_string == NULL || grass_growth_string[0] == 0) {
        grass_growth = GRASS_GROWTH_ANY;
    } else if( strcmp(grass_growth_string, "any") == 0) {
        grass_growth = GRASS_GROWTH_ANY;
    } else if( strcmp(grass_growth_string, "green") == 0) {
        grass_growth = GRASS_GROWTH_NORMAL;
    } else if( strcmp(grass_growth_string, "dry") == 0) {
        grass_growth = GRASS_GROWTH_DRY;
    } else if( strcmp(grass_growth_string, "dead") == 0) {
        grass_growth = GRASS_GROWTH_DEAD;
    }

    //do bodyparts
    const char* bodyPartStr = elemSprite->Attribute("bodypart");
    //clear old bodypart string
    memset(bodypart, 0, sizeof(bodypart));
    //copy new, if found
    if (bodyPartStr != NULL && bodyPartStr[0] != 0) {
        strcpy(bodypart, bodyPartStr);
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
        itemsubtype = INVALID_INDEX;
    } else {
        df::world_raws::T_itemdefs &defs = df::global::world->raws.itemdefs;
        switch(itemtype) {
        case item_type::WEAPON:
            itemsubtype = lookupIndexedPonterType(equipsindexstr, defs.weapons);
            break;
        case item_type::ARMOR:
            itemsubtype = lookupIndexedPonterType(equipsindexstr, defs.armor);
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
        case item_type::GLOVES:
            itemsubtype = lookupIndexedPonterType(equipsindexstr, defs.gloves);
            break;
        case item_type::PANTS:
            itemsubtype = lookupIndexedPonterType(equipsindexstr, defs.pants);
            break;
        default:
            itemsubtype = INVALID_INDEX;
        }
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

    //not all tiles work well with an outline
    const char* spriteOutlineStr = elemSprite->Attribute("outline");
    if (spriteOutlineStr != NULL && spriteOutlineStr[0] != 0) {
        needoutline=(atoi(spriteOutlineStr) == 1);
    }

    //get the possible offset for blood bools
    const char* spriteBloodStr = elemSprite->Attribute("blood_sprite");
    if (spriteBloodStr != NULL && spriteBloodStr[0] != 0) {
        bloodsprite=(atoi(spriteBloodStr) == 1);
    }

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

void c_sprite::draw_world(int x, int y, int z, Block * b, bool chop)
{
    draw_world_offset(x, y, z, b, 0, chop);
}


void c_sprite::draw_world_offset(int x, int y, int z, Block * b, int tileoffset, bool chop)
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

    if(sheetindex == 48) {
        int spam = 0;
        spam++;
    }

    if ((animframes & (1 << offsetAnimFrame)) || !animate) {
        //if set by the xml file, a random offset between 0 and 'variations' is added to the sprite.
        int randoffset = 0;
        if(variations) {
            randoffset = rando%variations;
        }

        if(!b) {
            return;
        }
        //if the xml says that this is a blood sprite, and offset is set here for proper pooling. this over-rides the random offset.
        if(bloodsprite) {
            randoffset = getBloodOffset(b);
        }
        if(!((water_direction < 0) || (water_direction == get_relative_water_direction(b)))) {
            goto draw_subsprite;
        }
        if(!( //these are all border conditions. this first section is a list of positive conditions. if at least one of the border conditions is met, the tile can be shown.
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
        if( //This second block consists of negative conditions. if /any/ of these border conditions are met, the tile will not be drawn
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
        //only bother with this tile if it's in the light, or not.
        if(!((light==LIGHTANY) || ((light==LIGHTYES) && b->designation.bits.outside) || ((light==LIGHTNO) && !(b->designation.bits.outside)))) {
            goto draw_subsprite;
        }
        if(!((grasstype == -1) || (grasstype == b->grassmat))) {
            goto draw_subsprite;
        }
        if(!((grass_growth == GRASS_GROWTH_ANY) ||
                ((grass_growth == GRASS_GROWTH_NORMAL) &&
                 ((b->tileMaterial == tiletype_material::GRASS_DARK) ||
                  (b->tileMaterial == tiletype_material::GRASS_LIGHT))) ||
                ((grass_growth == GRASS_GROWTH_DRY) &&
                 (b->tileMaterial == tiletype_material::GRASS_DRY)) ||
                ((grass_growth == GRASS_GROWTH_DEAD) &&
                 (b->tileMaterial == tiletype_material::GRASS_DEAD)))) {
            goto draw_subsprite;
        }

        if(itemtype !=  INVALID_INDEX) { //fixme: we need to store basic types separately and use them.
            if(!b->inv) {
                goto draw_subsprite;
            }
            if(b->inv->item.empty()) {
                goto draw_subsprite;
            }
            if(b->inv->item.size() <= itemtype) {
                goto draw_subsprite;
            }
            if(b->inv->item[itemtype].empty()) {
                goto draw_subsprite;
            }
            if(itemsubtype >= b->inv->item[itemtype].size()) {
                goto draw_subsprite;
            }
            if(b->inv->item[itemtype][itemsubtype].empty()) {
                goto draw_subsprite;
            }
            if(b->inv->item[itemtype][itemsubtype][0].matt.type == INVALID_INDEX) {
                goto draw_subsprite;
            }
        }


        int32_t drawx = x;
        int32_t drawy = y;
        int32_t drawz = z; //- ownerSegment->sizez + 1;

        b->ownerSegment->CorrectBlockForSegmentOffset( drawx, drawy, drawz);
        correctBlockForRotation( drawx, drawy, drawz, b->ownerSegment->rotation);
        int32_t viewx = drawx;
        int32_t viewy = drawy;
        int32_t viewz = drawz;
        pointToScreen((int*)&drawx, (int*)&drawy, drawz);
        drawx -= (TILEWIDTH>>1)*ssConfig.scale;

        if(((drawx + spritewidth*ssConfig.scale) < 0) || (drawx > ssState.ScreenW) || ((drawy + spriteheight*ssConfig.scale) < 0) || (drawy > ssState.ScreenH)) {
            return;
        }

        int sheetx, sheety;
        if(tilelayout == BLOCKTILE) {
            sheetx = ((sheetindex+tileoffset+randoffset) % SHEET_OBJECTSWIDE) * spritewidth;
            sheety = ((sheetindex+tileoffset+randoffset) / SHEET_OBJECTSWIDE) * spriteheight;
        } else if(tilelayout == RAMPBOTTOMTILE) {
            sheetx = SPRITEWIDTH * b->ramp.index;
            sheety = ((TILEHEIGHT + FLOORHEIGHT + SPRITEHEIGHT) * (sheetindex+tileoffset+randoffset))+(TILEHEIGHT + FLOORHEIGHT);
        } else if(tilelayout == RAMPTOPTILE) {
            sheetx = SPRITEWIDTH * b->ramp.index;
            sheety = (TILEHEIGHT + FLOORHEIGHT + SPRITEHEIGHT) * (sheetindex+tileoffset+randoffset);
        } else {
            sheetx = ((sheetindex+tileoffset+randoffset) % SHEET_OBJECTSWIDE) * spritewidth;
            sheety = ((sheetindex+tileoffset+randoffset) / SHEET_OBJECTSWIDE) * spriteheight;
        }
        ALLEGRO_COLOR shade_color = shadeAdventureMode(get_color(b), b->fog_of_war, b->designation.bits.outside);
        if(chop && ( halftile == HALFTILECHOP)) {
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
        } else if ((chop && (halftile == HALFTILEYES)) || (!chop && (halftile == HALFTILENO)) || (!chop && (halftile == HALFTILECHOP)) || (halftile == HALFTILEBOTH)) {
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
            subsprites.at(i).draw_world_offset(x, y, z, b, tileoffset, chop);
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

void c_sprite::set_tile_layout(uint8_t layout)
{
    tilelayout = layout;
    if(!subsprites.empty()) {
        for(int i = 0; i < subsprites.size(); i++) {
            subsprites[i].set_tile_layout(layout);
        }
    }
}

ALLEGRO_COLOR c_sprite::get_color(void* block)
{
    Block * b = (Block *) block;
    uint32_t dayofLife = 0;
    switch(shadeBy) {
    case ShadeNone:
        return al_map_rgb(255, 255, 255);
    case ShadeXml:
        return shadecolor;
    case ShadeMat:
        return lookupMaterialColor(b->material);
    case ShadeNamed:
        return namedcolor;
    case ShadeGrass:
        return lookupMaterialColor(WOOD, b->grassmat);
    case ShadeBuilding:
        return lookupMaterialColor(b->building.info.material);
    case ShadeLayer:
        return lookupMaterialColor(b->layerMaterial);
    case ShadeVein:
        return lookupMaterialColor(b->veinMaterial);
    case ShadeMatFore:
        return ssConfig.colors.getDfColor(lookupMaterialFore(b->material.type, b->material.index), lookupMaterialBright(b->material.type, b->material.index));
    case ShadeMatBack:
        return ssConfig.colors.getDfColor(lookupMaterialBack(b->material.type, b->material.index));
    case ShadeLayerFore:
        return ssConfig.colors.getDfColor(lookupMaterialFore(b->layerMaterial.type, b->layerMaterial.index), lookupMaterialBright(b->layerMaterial.type, b->layerMaterial.index));
    case ShadeLayerBack:
        return ssConfig.colors.getDfColor(lookupMaterialBack(b->layerMaterial.type, b->layerMaterial.index));
    case ShadeVeinFore:
        return ssConfig.colors.getDfColor(lookupMaterialFore(b->veinMaterial.type, b->veinMaterial.index), lookupMaterialBright(b->veinMaterial.type, b->veinMaterial.index));
    case ShadeVeinBack:
        return ssConfig.colors.getDfColor(lookupMaterialBack(b->veinMaterial.type, b->veinMaterial.index));
    case ShadeBodyPart:
        if(b->creaturePresent) {
            dayofLife = b->creature->birth_year*12*28 + b->creature->birth_time/1200;
            if((!ssConfig.skipCreatureTypes) && (!ssConfig.skipCreatureTypesEx) && (!ssConfig.skipDescriptorColors)) {
                t_creaturecaste & caste = contentLoader->Mats->raceEx[b->creature->race].castes[b->creature->caste];
                std::vector<t_colormodifier> & colormods =caste.ColorModifier;
                for(unsigned int j = 0; j<b->creature->nbcolors ; j++) {
                    t_colormodifier & colormod = colormods[j];
                    if(colormods[j].part == bodypart) {
                        if(colormods[j].colorlist.size() > b->creature->color[j]) {
                            uint32_t cr_color = colormod.colorlist.at(b->creature->color[j]);
                            if(cr_color < contentLoader->Mats->color.size()) {
                                if(colormod.startdate > 0) {

                                    if((colormod.startdate <= dayofLife) &&
                                            (colormod.enddate > dayofLife)) {
                                        return al_map_rgb_f(
                                                   contentLoader->Mats->color[cr_color].red,
                                                   contentLoader->Mats->color[cr_color].green,
                                                   contentLoader->Mats->color[cr_color].blue);;
                                    }
                                } else
                                    return al_map_rgb_f(
                                               contentLoader->Mats->color[cr_color].red,
                                               contentLoader->Mats->color[cr_color].green,
                                               contentLoader->Mats->color[cr_color].blue);
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
    case ShadeJob:
        if(b->creaturePresent) {
            return ssConfig.colors.getDfColor(getJobColor(b->creature->profession));
        } else {
            return al_map_rgb(255,255,255);
        }
    case ShadeBlood:
        return b->bloodcolor;
    case ShadeEquip:
        if(itemsubtype >=0) {
            //FIXME: need a way to get a material for generic types.
            //errors here give pink.
            if(itemtype == -1) {
                return al_map_rgb(255, 0, 0);
            }
            if(b->inv->item.size() <= itemtype) {
                return al_map_rgb(0, 255, 0);
            }
            if(b->inv->item[itemtype].size() <= itemsubtype) {
                return al_map_rgb(255, 255, 0);
            }
            if(b->inv->item[itemtype][itemsubtype].empty()) {
                return al_map_rgb(0, 0, 255);
            }
            return lookupMaterialColor(b->inv->item[itemtype][itemsubtype][0].matt, b->inv->item[itemtype][itemsubtype][0].dyematt);
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