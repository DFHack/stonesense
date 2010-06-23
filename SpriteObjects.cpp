#include "common.h"
#include "SpriteObjects.h"
#include "GUI.h"

c_sprite::c_sprite(void)
{
	fileindex = -1;
	sheetindex = 0;
	spritewidth = SPRITEWIDTH;
	spriteheight = SPRITEHEIGHT;
	offset_x = 0;
	offset_y = -(WALLHEIGHT);
	variations = 0;
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
	if (sheetIndexStr != NULL || sheetIndexStr[0] != 0)
		sheetindex=atoi(sheetIndexStr);

	//check for randomised tiles
	const char* spriteVariationsStr = elemSprite->Attribute("variations");
	if (spriteVariationsStr == NULL || spriteVariationsStr[0] == 0)
	{
		variations = 0;
	}
	else variations=atoi(spriteVariationsStr);
}

/// This is just a very basic sprite drawing routine. all it uses are screen coords
void c_sprite::draw_screen(int x, int y)
{
	int sheetx = sheetindex % SHEET_OBJECTSWIDE;
	int sheety = sheetindex / SHEET_OBJECTSWIDE;
	if(fileindex == -1)
		al_draw_bitmap_region(IMGObjectSheet, sheetx * spritewidth, sheety * spriteheight, spritewidth, spriteheight, x + offset_x, y + offset_y, 0);
	else 
		al_draw_bitmap_region(getImgFile(fileindex), sheetx * spritewidth, sheety * spriteheight, spritewidth, spriteheight, x + offset_x, y + offset_y, 0);
}