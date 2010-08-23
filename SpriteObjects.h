#pragma once

#include "dfhack/depends/tinyxml/tinyxml.h"
#include "common.h"

#define BLOCKTILE 0
#define RAMPBOTTOMTILE 1
#define RAMPTOPTILE 2

#define HALFTILECHOP 0
#define HALFTILEYES 1
#define HALFTILENO 2
#define HALFTILEBOTH 3

#define OUTLINENONE 0
#define OUTLINELEFT 1
#define OUTLINERIGHT 2
#define OUTLINEBOTTOM 3
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
	vector<c_sprite> subsprites;
	ALLEGRO_COLOR shadecolor;
	char bodypart[128];
	char animframes;

	int snowmin;
	int snowmax;
	int bloodmin;
	int bloodmax;

	bool needoutline;
	bool randomanimation;

	char isoutline;

	char halftile;

	ALLEGRO_BITMAP * defaultsheet;

	uint8_t tilelayout;

	uint8_t openborders;
	uint8_t wallborders;
	uint8_t floorborders;
	uint8_t rampborders;
	uint8_t notopenborders;
	uint8_t notwallborders;
	uint8_t notfloorborders;
	uint8_t notrampborders;
public:
	c_sprite(void);
	~c_sprite(void);
	void draw_screen(int x, int y);
	void draw_world(int x, int y, int z, bool chop = false);
	void draw_world_offset(int x, int y, int z, int offset, bool chop = false);
	void draw_world_ramp_bottom(int x, int y, int z, bool chop = false);
	void set_by_xml(TiXmlElement* elemSprite, int32_t fileindex);
	void set_by_xml(TiXmlElement* elemSprite);
	int32_t get_sheetindex(void){ return sheetindex; }
	int32_t get_animframes(void){ return animframes; }
	char get_fileindex(void){ return fileindex; }
	void set_sheetindex(int32_t in){ sheetindex = in; }
	void set_fileindex(int32_t in){	fileindex = in;	}
	void set_animframes(char in){ animframes = in; }
	void set_size(uint8_t x, uint8_t y);
	void set_offset(int16_t x, int16_t y);
	ALLEGRO_COLOR get_color(void * b);
	ALLEGRO_BITMAP * get_defaultsprite(void){ return defaultsheet; }
	void set_defaultsheet(ALLEGRO_BITMAP * in){ defaultsheet = in; }
	void reset();
	void set_tile_layout(uint8_t layout);
	bool animate;
};
