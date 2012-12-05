#pragma once

#include "tinyxml.h"
#include "common.h"

#define BLOCKPLATE 0
#define RAMPBOTTOMPLATE 1
#define RAMPTOPPLATE 2

#define HALFPLATECHOP 0
#define HALFPLATEYES 1
#define HALFPLATENO 2
#define HALFPLATEBOTH 3

#define OUTLINENONE 0
#define OUTLINELEFT 1
#define OUTLINERIGHT 2
#define OUTLINEBOTTOM 3

#define LIGHTANY 0
#define LIGHTYES 1
#define LIGHTNO 2

enum grass_growth {
    GRASS_GROWTH_ANY,
    GRASS_GROWTH_NORMAL,
    GRASS_GROWTH_DRY,
    GRASS_GROWTH_DEAD
};


class Block;

class c_sprite
{
private:
    int32_t fileindex;
    int32_t sheetindex;
    uint8_t spritewidth;
    uint8_t spriteheight;
    int16_t offset_x;
    int16_t offset_y;
    int16_t offset_user_x;
    int16_t offset_user_y;
    uint8_t variations;
    ShadeBy shadeBy;
    std::vector<c_sprite> subsprites;
    ALLEGRO_COLOR shadecolor;
    ALLEGRO_COLOR namedcolor;
    char bodypart[128];
    char animframes;

    int snowmin;
    int snowmax;
    int bloodmin;
    int bloodmax;
    int mudmin;
    int mudmax;
    int grassmax;
    int grassmin;

    int grasstype;
    char grass_growth;

    unsigned int waterMin : 3;
    unsigned int waterMax : 3;
    unsigned int waterAboveMin : 3;
    unsigned int waterAboveMax : 3;
    unsigned int waterBelowMin : 3;
    unsigned int waterBelowMax : 3;
    unsigned int waterRightMin : 3;
    unsigned int waterRightMax : 3;
    unsigned int waterLeftMin : 3;
    unsigned int waterLeftMax : 3;
    char water_direction;

    bool needoutline;
    bool randomanimation;
    bool bloodsprite;

    unsigned char isoutline : 2;

    unsigned char halftile : 2;

    unsigned char light : 2;

    float spritescale;

    ALLEGRO_BITMAP * defaultsheet;

    uint8_t platelayout;

    uint8_t openborders;
    uint8_t wallborders;
    uint8_t floorborders;
    uint8_t rampborders;
    uint8_t upstairborders;
    uint8_t downstairborders;
    uint8_t lightborders;
    uint8_t darkborders;
    uint8_t notopenborders;
    uint8_t notwallborders;
    uint8_t notfloorborders;
    uint8_t notrampborders;
    uint8_t notupstairborders;
    uint8_t notdownstairborders;

    int itemtype;
    int itemsubtype;
public:
    c_sprite(void);
    ~c_sprite(void);
    //void draw_screen(int x, int y);
    void assemble_world_offset_src(int x, int y, int z, int plateoffset, Block * b, Block* src, bool chop = false);
    inline void c_sprite::assemble_world(int x, int y, int z, Block * b, bool chop=false)
    {
        assemble_world_offset_src(x, y, z, 0, b, b, chop);
    }
    inline void c_sprite::assemble_world_offset(int x, int y, int z, int plateoffset, Block * b, bool chop=false){
        assemble_world_offset_src(x, y, z, plateoffset, b, b, chop);
    }
    void assemble_world_ramp_bottom(int x, int y, int z, bool chop = false);
    void set_by_xml(TiXmlElement* elemSprite, int32_t fileindex);
    void set_by_xml(TiXmlElement* elemSprite);
    int32_t get_sheetindex(void) {
        return sheetindex;
    }
    int32_t get_animframes(void) {
        return animframes;
    }
    char get_fileindex(void) {
        return fileindex;
    }
    void set_sheetindex(int32_t in) {
        sheetindex = in;
    }
    void set_fileindex(int32_t in) {
        fileindex = in;
    }
    void set_animframes(char in) {
        animframes = in;
    }
    void set_size(uint8_t x, uint8_t y);
    void set_offset(int16_t x, int16_t y);
    ALLEGRO_COLOR get_color(void * b);
    ALLEGRO_BITMAP * get_defaultsprite(void) {
        return defaultsheet;
    }
    void set_defaultsheet(ALLEGRO_BITMAP * in) {
        defaultsheet = in;
    }
    void reset();
    void set_plate_layout(uint8_t layout);
    void set_needoutline( bool i ) {
        needoutline = i;
    }
    bool animate;
};