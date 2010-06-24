#pragma once

#include "dfhack/depends/tinyxml/tinyxml.h"

class c_sprite
{
private:
	int32_t fileindex;
	int32_t sheetindex;
	uint8_t spritewidth;
	uint8_t spriteheight;
	int16_t offset_x;
	int16_t offset_y;
	uint8_t variations;
	ShadeBy shadeBy;
	vector<c_sprite *> subsprites;
	ALLEGRO_COLOR shadecolor;
	char bodypart[128];
	char animframes;

	int snowmin;
	int snowmax;
	int bloodmin;
	int bloodmax;

	bool needoutline;
public:
	c_sprite(void);
	~c_sprite(void);
	void draw_screen(int x, int y);
	void draw_world(int x, int y, int z, bool chop = false);
	void set_by_xml(TiXmlElement* elemSprite, int32_t fileindex);
	void set_by_xml(TiXmlElement* elemSprite);
	int32_t get_sheetindex(void){ return sheetindex; }
	int32_t get_animframes(void){ return animframes; }
	char get_fileindex(void){ return fileindex; }
	void set_sheetindex(int32_t in){ sheetindex = in; }
	void set_fileindex(int32_t in){ fileindex = in; }
	void set_animframes(char in){ animframes = in; }
	void set_size(uint8_t x, uint8_t y){ spritewidth = x; spriteheight = y; }
	void set_offset(uint8_t x, uint8_t y){ offset_x = x; offset_y = y; }
	ALLEGRO_COLOR get_color(Block * b);
	void reset();
};
