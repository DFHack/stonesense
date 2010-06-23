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
public:
	c_sprite(void);
	~c_sprite(void);
	void draw(int x, int y, int z);
	void draw_screen(int x, int y);
	void set_by_xml(TiXmlElement* elemSprite, int32_t fileindex);
	void set_by_xml(TiXmlElement* elemSprite);
	int32_t get_sheetindex(void){ return sheetindex; }
	int32_t get_fileindex(void){ return fileindex; }
	void set_sheetindex(int32_t in){ sheetindex = in; }
	void set_fileindex(int32_t in){ fileindex = in; }
};
