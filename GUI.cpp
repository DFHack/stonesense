#include <assert.h>
#include <vector>

using namespace std;


#include "common.h"
#include "Block.h"
#include "GUI.h"
#include "WorldSegment.h"
#include "SpriteMaps.h"
#include "MapLoading.h"
#include "GameBuildings.h"
#include "Creatures.h"
#include "ContentLoader.h"
#include "BlockFactory.h"
#include "Block.h"

#define color_segmentoutline al_map_rgb(0,0,0)

extern ALLEGRO_FONT *font;

WorldSegment* viewedSegment;
WorldSegment* altSegment;
int DisplayedSegmentX;
int DisplayedSegmentY;
int DisplayedSegmentZ;
int DisplayedRotation = 0;
int MiniMapTopLeftX = 0;
int MiniMapTopLeftY = 0;
int MiniMapBottomRightX = 0;
int MiniMapBottomRightY = 0;
int MiniMapSegmentWidth =0;
int MiniMapSegmentHeight =0;
double oneBlockInPixels = 0;

ALLEGRO_BITMAP* IMGObjectSheet;
ALLEGRO_BITMAP* IMGCreatureSheet; 
ALLEGRO_BITMAP* IMGRampSheet; 
ALLEGRO_BITMAP* IMGStatusSheet; 
ALLEGRO_BITMAP* IMGBloodSheet; 
ALLEGRO_BITMAP* IMGEngFloorSheet;
ALLEGRO_BITMAP* IMGEngLeftSheet;
ALLEGRO_BITMAP* IMGEngRightSheet;

ALLEGRO_BITMAP* buffer = 0;
ALLEGRO_BITMAP* bigFile = 0;
vector<ALLEGRO_BITMAP*> IMGCache;
vector<ALLEGRO_BITMAP*> IMGFilelist;
vector<string*> IMGFilenames;
GLhandleARB tinter;
GLhandleARB tinter_shader;
Crd3D debugCursor;

ALLEGRO_COLOR premultiply(ALLEGRO_COLOR input)
{
	ALLEGRO_COLOR out;
	out.a = input.a;
	out.r = input.r * input.a;
	out.g = input.g * input.a;
	out.b = input.b * input.a;
	return out;
}

void draw_diamond(float x, float y, ALLEGRO_COLOR color)
{
	al_draw_filled_triangle(x, y, x+4, y+4, x-4, y+4, color);
	al_draw_filled_triangle(x+4, y+4, x, y+8, x-4, y+4, color);
}

void draw_borders(float x, float y, uint8_t borders)
{
	if(borders & 1)
		draw_diamond(x, y, al_map_rgb(255,255,255));
	else
		draw_diamond(x, y, al_map_rgb(0,0,0));

	if(borders & 2)
		draw_diamond(x+4, y+4, al_map_rgb(255,255,255));
	else
		draw_diamond(x+4, y+4, al_map_rgb(0,0,0));

	if(borders & 4)
		draw_diamond(x+8, y+8, al_map_rgb(255,255,255));
	else
		draw_diamond(x+8, y+8, al_map_rgb(0,0,0));

	if(borders & 8)
		draw_diamond(x+4, y+12, al_map_rgb(255,255,255));
	else
		draw_diamond(x+4, y+12, al_map_rgb(0,0,0));

	if(borders & 16)
		draw_diamond(x, y+16, al_map_rgb(255,255,255));
	else
		draw_diamond(x, y+16, al_map_rgb(0,0,0));

	if(borders & 32)
		draw_diamond(x-4, y+12, al_map_rgb(255,255,255));
	else
		draw_diamond(x-4, y+12, al_map_rgb(0,0,0));

	if(borders & 64)
		draw_diamond(x-8, y+8, al_map_rgb(255,255,255));
	else
		draw_diamond(x-8, y+8, al_map_rgb(0,0,0));

	if(borders & 128)
		draw_diamond(x-4, y+4, al_map_rgb(255,255,255));
	else
		draw_diamond(x-4, y+4, al_map_rgb(0,0,0));

}
void swapSegments(void)
{
	WorldSegment* backupSegment = viewedSegment;
	viewedSegment = altSegment;
	altSegment = backupSegment;
}
ALLEGRO_COLOR operator*(const ALLEGRO_COLOR &color1, const ALLEGRO_COLOR &color2)
{
	ALLEGRO_COLOR temp;
	temp.r=color1.r*color2.r;
	temp.g=color1.g*color2.g;
	temp.b=color1.b*color2.b;
	temp.a=color1.a*color2.a;
	return temp;
}

ALLEGRO_COLOR operator+(const ALLEGRO_COLOR &color1, const ALLEGRO_COLOR &color2)
{
	ALLEGRO_COLOR temp;
	temp.r=color1.r+(color2.r*(1-color1.r));
	temp.g=color1.g+(color2.g*(1-color1.g));
	temp.b=color1.b+(color2.b*(1-color1.b));
	temp.a=color1.a+(color2.a*(1-color1.a));
	return temp;
}

ALLEGRO_COLOR partialBlend(const ALLEGRO_COLOR & color2, const ALLEGRO_COLOR & color1, int percent)
{
	float blend = percent/100.0;
	ALLEGRO_COLOR result;
	result.r=(blend*color1.r)+((1.0-blend)*color2.r);
	result.g=(blend*color1.g)+((1.0-blend)*color2.g);
	result.b=(blend*color1.b)+((1.0-blend)*color2.b);
	if(color1.a > color2.a)
		result.a = color1.a;
	else result.a = color2.a;
	return result;
}

ALLEGRO_COLOR getDayShade(int hour, int tick)
{
	ALLEGRO_COLOR nightShade = al_map_rgb(158,155,255);
	ALLEGRO_COLOR dawnShade = al_map_rgb(254,172,142);

	if(hour < 6)
		return nightShade;
	else if((hour < 7) && (tick < 25))
		return partialBlend(nightShade, dawnShade, (tick * 4));
	else if(hour < 7)
		return partialBlend(dawnShade, al_map_rgb(255,255,255), ((tick-25) * 4));
	else if((hour > 20) && (hour <= 21) && (tick < 25))
		return partialBlend(al_map_rgb(255,255,255), dawnShade, (tick * 4));
	else if((hour > 20) && (hour <= 21))
		return partialBlend(dawnShade, nightShade, ((tick-25) * 4));
	else if(hour > 21)
		return nightShade;
	return al_map_rgb(255,255,255);
}


void ScreenToPoint(int x,int y,int &x1, int &y1, int &z1)
{ //assume z of 0
	x-=al_get_bitmap_width(al_get_target_bitmap()) / 2;
	y-=al_get_bitmap_height(al_get_target_bitmap()) / 2;
	x/=config.scale;
	y/=config.scale;
	x+=al_get_bitmap_width(al_get_target_bitmap()) / 2;
	y+=al_get_bitmap_height(al_get_target_bitmap()) / 2;	x-=TILEWIDTH/2;
	y+=TILEWIDTH/2;
	z1 = -3;
	y+= z1*BLOCKHEIGHT/2;
	//y-=BLOCKHEIGHT;
	x+=TILEWIDTH>>1;
	int offx = al_get_bitmap_width(al_get_target_bitmap()) /2;
	int offy = (-20)-(BLOCKHEIGHT * config.lift_segment_offscreen);
	y-=offy;
	x-=offx;
	y1=y*2-x;
	x1=x*2+y1;
	x1/=TILEWIDTH;
	y1/=TILEWIDTH;

}

int get_textf_width(const ALLEGRO_FONT *font, const char *format, ...)
{
	ALLEGRO_USTR *buf;
	va_list ap;
	const char *s;

	int width;
	/* Fast path for common case. */
	if (0 == strcmp(format, "%s")) {
		va_start(ap, format);
		s = va_arg(ap, const char *);
		width = al_get_text_width(font, s);
		va_end(ap);
		return width;
	}

	va_start(ap, format);
	buf = al_ustr_new("");
	al_ustr_vappendf(buf, format, ap);
	va_end(ap);

	width = al_get_text_width(font, al_cstr(buf));

	al_ustr_free(buf);
	return width;
}
void draw_text_border(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, const char *ustr)
{
	int xx, yy, ww, hh;
	al_get_text_dimensions(font, ustr, &xx, &yy, &ww, &hh);
	if(flags & ALLEGRO_ALIGN_CENTRE)
		xx -= ww/2;
	else if(flags & ALLEGRO_ALIGN_RIGHT)
		xx -= ww;
	al_draw_filled_rectangle(x+xx, y+yy, x+xx+ww, y+yy+hh, al_map_rgba_f(0.0,0.0,0.0,0.75));
	al_draw_text(font, color, x, y, flags, ustr);
}

void draw_textf_border(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, const char *format, ...)
{
	ALLEGRO_USTR *buf;
	va_list arglist;
	const char *s;

	/* Fast path for common case. */
	if (0 == strcmp(format, "%s")) {
		va_start(arglist, format);
		s = va_arg(arglist, const char *);
		draw_text_border(font, color, x, y, flags, s);
		va_end(arglist);
		return;
	}


	va_start(arglist, format);
	buf = al_ustr_new("");
	al_ustr_vappendf(buf, format, arglist);
	va_end(arglist);
	draw_text_border(font, color, x, y, flags, al_cstr(buf));
	al_ustr_free(buf);
}
void draw_ustr_border(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, const ALLEGRO_USTR *ustr)
{
	int xx, yy, ww, hh;
	al_get_ustr_dimensions(font, ustr, &xx, &yy, &ww, &hh);
	if(flags & ALLEGRO_ALIGN_CENTRE)
		xx -= ww/2;
	else if(flags & ALLEGRO_ALIGN_RIGHT)
		xx -= ww;
	al_draw_filled_rectangle(x+xx, y+yy, x+xx+ww, y+yy+hh, al_map_rgba_f(0.0,0.0,0.0,0.75));
	al_draw_ustr(font, color, x, y, flags, ustr);
}
void pointToScreen(int *inx, int *iny, int inz){
	int offx = al_get_bitmap_width(al_get_target_bitmap()) / 2;
	int offy = (-20)-(BLOCKHEIGHT * config.lift_segment_offscreen);
	int z=inz-1;
	int x = *inx-*iny;
	int y = *inx+*iny;
	x = x * TILEWIDTH / 2;
	y = y * TILEHEIGHT / 2;
	y-=z * BLOCKHEIGHT;
	x+=offx;
	y+=offy;
	x-=al_get_bitmap_width(al_get_target_bitmap()) / 2;
	y-=al_get_bitmap_height(al_get_target_bitmap()) / 2;
	x*=config.scale;
	y*=config.scale;
	x+=al_get_bitmap_width(al_get_target_bitmap()) / 2;
	y+=al_get_bitmap_height(al_get_target_bitmap()) / 2;
	*inx=x;*iny=y;
}

Crd2D WorldBlockToScreen(int32_t x, int32_t y, int32_t z){
	correctBlockForSegmetOffset( x, y, z);
	return LocalBlockToScreen(x, y, z-1);
}

Crd2D LocalBlockToScreen(int32_t x, int32_t y, int32_t z){
	pointToScreen((int*)&x, (int*)&y, z);
	Crd2D result;
	result.x = x;
	result.y = y;
	return result;
}

void DrawCurrentLevelOutline(bool backPart){
	int x = DisplayedSegmentX+1;
	int y = DisplayedSegmentY+1;
	int z = DisplayedSegmentZ;
	int sizex = config.segmentSize.x-2;
	int sizey = config.segmentSize.y-2;

	if(config.hide_outer_blocks){
		x++;y++;
		sizex -= 2;
		sizey -= 2;
	}

	Crd2D p1 = WorldBlockToScreen(x, y, z);
	Crd2D p2 = WorldBlockToScreen(x, y + sizey , z);
	Crd2D p3 = WorldBlockToScreen(x + sizex , y, z);
	Crd2D p4 = WorldBlockToScreen(x + sizex , y + sizey , z);
	p1.y += FLOORHEIGHT;
	p2.y += FLOORHEIGHT;
	p3.y += FLOORHEIGHT;
	p4.y += FLOORHEIGHT;
	if(backPart){
		al_draw_line(p1.x, p1.y, p1.x, p1.y-BLOCKHEIGHT*config.scale, color_segmentoutline, 0);
		al_draw_line(p1.x, p1.y, p1.x, p1.y-BLOCKHEIGHT*config.scale, color_segmentoutline, 0);
		al_draw_line(p1.x, p1.y, p2.x, p2.y, color_segmentoutline, 0);
		al_draw_line(p1.x, p1.y-BLOCKHEIGHT*config.scale, p2.x, p2.y-BLOCKHEIGHT*config.scale, color_segmentoutline, 0);
		al_draw_line(p2.x, p2.y, p2.x, p2.y-BLOCKHEIGHT*config.scale, color_segmentoutline, 0);

		al_draw_line(p1.x, p1.y, p3.x, p3.y, color_segmentoutline, 0);
		al_draw_line(p1.x, p1.y-BLOCKHEIGHT*config.scale, p3.x, p3.y-BLOCKHEIGHT*config.scale, color_segmentoutline, 0);
		al_draw_line(p3.x, p3.y, p3.x, p3.y-BLOCKHEIGHT*config.scale, color_segmentoutline, 0);
	}else{
		al_draw_line(p4.x, p4.y, p4.x, p4.y-BLOCKHEIGHT*config.scale, color_segmentoutline, 0);
		al_draw_line(p4.x, p4.y, p2.x, p2.y, color_segmentoutline ,0);
		al_draw_line(p4.x, p4.y-BLOCKHEIGHT*config.scale, p2.x, p2.y-BLOCKHEIGHT*config.scale, color_segmentoutline ,0);

		al_draw_line(p4.x, p4.y, p3.x, p3.y, color_segmentoutline, 0);
		al_draw_line(p4.x, p4.y-BLOCKHEIGHT*config.scale, p3.x, p3.y-BLOCKHEIGHT*config.scale, color_segmentoutline, 0);
	}
}

void drawDebugCursorAndInfo(){
	if((config.dfCursorX != -30000) && config.follow_DFcursor)
	{
		int x = config.dfCursorX;
		int y = config.dfCursorY;
		int z = config.dfCursorZ;
		correctBlockForSegmetOffset(x,y,z);
		correctBlockForRotation( x, y, z, viewedSegment->rotation);
		debugCursor.x = x;
		debugCursor.y = y;
		debugCursor.z = z;
	}
	else debugCursor.z = 0;
	Crd2D point = LocalBlockToScreen(debugCursor.x, debugCursor.y, debugCursor.z);

	int spriteNum =  SPRITEOBJECT_CURSOR;
	int sheetx = spriteNum % SHEET_OBJECTSWIDE;
	int sheety = spriteNum / SHEET_OBJECTSWIDE;
	al_draw_bitmap_region(IMGObjectSheet, sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT, SPRITEWIDTH, SPRITEHEIGHT, point.x - SPRITEWIDTH/2, point.y - (WALLHEIGHT), 0);

	//get block info
	Block* b = viewedSegment->getBlockLocal( debugCursor.x, debugCursor.y, debugCursor.z+viewedSegment->sizez-2);
	int i = 10;
	draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, "Block 0x%x", b);

	draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
		"Coord:(%i,%i,%i)", debugCursor.x, debugCursor.y, debugCursor.z);

	draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
		"Game Mode:%i, Control Mode:%i", contentLoader->gameMode.g_mode, contentLoader->gameMode.g_type);

	if(!b) return;


	int ttype;
	const char* tform = NULL;
	if (b->tileShapeBasic==tiletype_shape_basic::Floor)
	{
		ttype=b->tileType;	
		tform="floor";
	}
	else if (b->tileShapeBasic==tiletype_shape_basic::Wall)
	{
		ttype=b->tileType;
		tform="wall";
	}
	else if (b->tileShapeBasic==tiletype_shape_basic::Ramp)
	{
		ttype=b->tileType;
		tform="ramp";
	}
	else if (b->tileShapeBasic==tiletype_shape_basic::Stair)
	{
		ttype=b->tileType;
		tform="stair";
	}

	if (tform != NULL)
	{
		const char* formName = lookupFormName(b->consForm);
		const char* matName = lookupMaterialTypeName(b->material.type);
		const char* subMatName = lookupMaterialName(b->material.type,b->material.index);
		draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,
			"%s %s:%i Material:%s%s%s (%d,%d)", formName, tform, ttype, 
			matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"", b->material.type,b->material.index);
	}
	//if (tform != NULL)
	//{
	//	draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,
	//		"MaterialType: %d, MaterialIndex: %d", b->material.type, b->material.index);
	//}
	if (tform != NULL)
	{
		const char* matName = lookupMaterialTypeName(b->layerMaterial.type);
		const char* subMatName = lookupMaterialName(b->layerMaterial.type,b->layerMaterial.index);
		draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,
			"Layer Material:%s%s%s", 
			matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
	} 
	if ((tform != NULL) && b->hasVein == 1)
	{
		const char* matName = lookupMaterialTypeName(b->veinMaterial.type);
		const char* subMatName = lookupMaterialName(b->veinMaterial.type,b->veinMaterial.index);
		draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,
			"Vein Material:%s%s%s", 
			matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
	} 
	if (tform != NULL)//(b->grasslevel > 0)
	{
		const char* subMatName = lookupMaterialName(WOOD,b->grassmat);
		draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,
			"Grass length:%d, Material: %s", 
			b->grasslevel, subMatName?subMatName:"");
	} 
	//for(int j = 0; j < b->grasslevels.size(); j++)
	//{
	//	const char* subMatName = lookupMaterialName(WOOD,b->grassmats.at(j));
	//	draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,
	//		"Grass length:%d, Material: %s", 
	//		b->grasslevels.at(j), subMatName?subMatName:"");
	//} 

	if(b->water.index > 0 || b->tree.index != 0)
		draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
		"tree:%i water:%i,%i", b->tree.index, b->water.type, b->water.index);
	if(b->tree.index != 0)
		draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
		"tree name:%s type:%i", lookupTreeName(b->tree.index), b->tree.type);
	if(b->building.sprites.size() != 0)
		draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
		"%i extra sprites.", b->building.sprites.size());

    // FIXME: classidstrings is no more
	//building
	if(b->building.info.type != BUILDINGTYPE_NA && b->building.info.type != BUILDINGTYPE_BLACKBOX && b->building.info.type != BUILDINGTYPE_TREE){
		const char* matName = lookupMaterialTypeName(b->building.info.material.type);
		const char* subMatName = lookupMaterialName(b->building.info.material.type,b->building.info.material.index);
		const char* subTypeName = lookupBuildingSubtype(b->building.info.type, b->building.info.subtype);
		draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
			"Building: game_type = %s(%i) game_subtype = %s(%i) Material: %s%s%s (%d,%d)", 
			ENUM_KEY_STR(building_type, (building_type::building_type)b->building.info.type),
			b->building.info.type,
			subTypeName,
			b->building.info.subtype,
			matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"",
			b->building.info.material.type,b->building.info.material.index);

		//if(b->building.custom_building_type != -1)
		//{
		//	draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
		//		"Custom workshop type %s (%d)", contentLoader->custom_workshop_types[b->building.custom_building_type].c_str(),b->building.custom_building_type);
		//}
	}

	//creatures
	if(b->creature != null){
		if(!config.skipCreatureTypes)
			draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
			"Creature:%s(%i) Job:%s", 
			contentLoader->Mats->race.at(b->creature->race).id.c_str(), b->creature->race, 
			contentLoader->professionStrings.at(b->creature->profession).c_str());

		char strCreature[150] = {0};
		generateCreatureDebugString( b->creature, strCreature );
		//memset(strCreature, -1, 50);
        /*
        // FIXME:: getJob is no more.
		try{
			draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
				"flag1: %s Sex: %d  Mood: %d Job: %s", strCreature, b->creature->sex + 1, b->creature->mood, (b->creature->current_job.active?contentLoader->MemInfo->getJob(b->creature->current_job.jobType).c_str():""));
		}
		catch(exception &e)
		{
			WriteErr("DFhack exeption: %s\n", e.what());
		}
		*/
		if((!config.skipCreatureTypes) && (!config.skipCreatureTypesEx) && (!config.skipDescriptorColors))
		{
			int yy = al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font));
			int xx = 2;
			for(unsigned int j = 0; j<b->creature->nbcolors ; j++)
			{
				if(contentLoader->Mats->raceEx.at(b->creature->race).castes.at(b->creature->caste).ColorModifier.at(j).colorlist.size() > b->creature->color[j])
				{
					uint32_t cr_color = contentLoader->Mats->raceEx[b->creature->race].castes[b->creature->caste].ColorModifier[j].colorlist[b->creature->color[j]];
					if(cr_color < contentLoader->Mats->color.size())
					{
						draw_textf_border(font, 
							al_map_rgb_f(
							contentLoader->Mats->color[cr_color].red,
							contentLoader->Mats->color[cr_color].green,
							contentLoader->Mats->color[cr_color].blue), xx, yy, 0,
							"%s ", contentLoader->Mats->raceEx[b->creature->race].castes[b->creature->caste].ColorModifier[j].part.c_str());
						xx += get_textf_width(font, "%s ", contentLoader->Mats->raceEx[b->creature->race].castes[b->creature->caste].ColorModifier[j].part.c_str());
					}
				}
			}
		}
	}
	if(b->designation.bits.traffic)
	{
		draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
			"Traffic: %d", b->designation.bits.traffic);
	}
	if(b->designation.bits.pile)
	{
		draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
			"Stockpile?");
	}
	if(b->designation.bits.water_table)
	{
		draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,"Water table");
	}
	if(b->designation.bits.rained)
	{
		draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,"Rained");
	}
	if(b->building.info.type != BUILDINGTYPE_BLACKBOX)
	{
		draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,
			"Temp1: %dU, %.2f'C, %d'F", b->temp1, (float)(b->temp1-10000)*5/9, b->temp1-9968);
	}
	if(b->snowlevel || b->mudlevel || b->bloodlevel)
	{
		draw_textf_border(font, al_map_rgb(255,255,255), 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,
			"Snow: %d, Mud: %d, Blood: %d", b->snowlevel, b->mudlevel, b->bloodlevel);
	}
	//borders
	/*
	int dray = al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font));
	draw_textf_border(font, al_map_rgb(255,255,255), 16, dray, 0,
	"Open: %d, floor: %d, Wall: %d, Ramp: %d Light: %d", b->openborders, b->floorborders, b->wallborders, b->rampborders, b->lightborders);
	draw_borders(8, dray, b->lightborders);
	*/

	////effects
	//if(b->blockeffects.lifetime > 0)
	//	draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
	//	"Effect Count:%i, Type:%i, Lifetime: %i, Direction:%i,%i", b->blockeffects.count, b->blockeffects.type, b->blockeffects.lifetime, b->blockeffects.x_direction, b->blockeffects.y_direction);
	//if(b->eff_miasma > 0)
	//	draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
	//	"Miasma: %d", b->eff_miasma);
	//if(b->eff_water > 0)
	//	draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
	//	"Water Mist: %d", b->eff_water);
	//if(b->eff_water2 > 0)
	//	draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
	//	"Water Mist 2: %d", b->eff_water2);
	//if(b->eff_blood > 0)
	//	draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
	//	"Blood Mist: %d", b->eff_blood);
	//if(b->eff_dust > 0)
	//	draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
	//	"Dust: %d", b->eff_dust);
	//if(b->eff_magma > 0)
	//	draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
	//	"Magma Mist: %d", b->eff_magma);
	//if(b->eff_smoke > 0)
	//	draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
	//	"Smoke: %d", b->eff_smoke);
	//if(b->eff_dragonfire > 0)
	//	draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
	//	"Dragonfire: %d", b->eff_dragonfire);
	//if(b->eff_fire > 0)
	//	draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
	//	"Fire: %d", b->eff_fire);
	//if(b->eff_webing > 0)
	//	draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
	//	"Webbing: %d", b->eff_webing);
	//if(b->eff_boiling > 0)
	//	draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
	//	"Boiling Substances: %d", b->eff_boiling);
	//if(b->eff_oceanwave > 0)
	//	draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
	//	"Ocean Wave: %d", b->eff_oceanwave);

	//basecon
	//textprintf(target, font, 2, config.screenHeight-20-(i--*10), 0xFFFFFF, 
	//   "base: %d %d %d ", b->basetile, b->basecon.type, b->basecon.index );
}

void DrawMinimap(){
	int size = 100;
	//double oneBlockInPixels;
	int posx = al_get_bitmap_width(al_get_target_bitmap())-size-10;
	int posy = 10;

	if(!viewedSegment || viewedSegment->regionSize.x == 0 || viewedSegment->regionSize.y == 0){
		draw_textf_border(font, al_map_rgb(255,255,255), posx, posy, 0, "No map loaded");
		return;
	}

	oneBlockInPixels = (double) size / viewedSegment->regionSize.x;
	//map outine
	int mapheight = (int)(viewedSegment->regionSize.y * oneBlockInPixels);
	al_draw_rectangle(posx, posy, posx+size, posy+mapheight, al_map_rgb(0,0,0),0);
	//current segment outline
	int x = (size * (viewedSegment->x+1)) / viewedSegment->regionSize.x;
	int y = (mapheight * (viewedSegment->y+1)) / viewedSegment->regionSize.y;
	MiniMapSegmentWidth = (viewedSegment->sizex-2) * oneBlockInPixels;
	MiniMapSegmentHeight = (viewedSegment->sizey-2) * oneBlockInPixels;
	al_draw_rectangle(posx+x, posy+y, posx+x+MiniMapSegmentWidth, posy+y+MiniMapSegmentHeight,al_map_rgb(0,0,0),0);
	MiniMapTopLeftX = posx;
	MiniMapTopLeftY = posy;
	MiniMapBottomRightX = posx+size;
	MiniMapBottomRightY = posy+mapheight;
}

void DrawSpriteFromSheet( int spriteNum, ALLEGRO_BITMAP* spriteSheet, ALLEGRO_COLOR color, float x, float y, Block * b){
	int sheetx = spriteNum % SHEET_OBJECTSWIDE;
	int sheety = spriteNum / SHEET_OBJECTSWIDE;
#ifdef _DEBUG
	config.drawcount ++;
#endif

	//
	/*
	static ALLEGRO_BITMAP* tiny = null;
	if(!tiny)
	tiny = create_bitmap_ex(32, 32, 32);

	blit(spriteSheet, tiny, sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT, 0, 0, SPRITEWIDTH, SPRITEHEIGHT);

	blit(tiny, target,
	0,0,
	10, 60 , SPRITEWIDTH, SPRITEHEIGHT);
	*/
	//draw_trans_sprite(target, tiny, x, y);
	if(b && (!b->designation.bits.pile) && config.fog_of_war && (contentLoader->gameMode.g_mode == GAMEMODE_ADVENTURE))
	{
		color.r *= 0.25f;
		color.g *= 0.25f;
		color.b *= 0.25f;
	}
	al_draw_tinted_scaled_bitmap(spriteSheet, premultiply(color), sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT, SPRITEWIDTH, SPRITEHEIGHT, x, y - (WALLHEIGHT)*config.scale, SPRITEWIDTH*config.scale, SPRITEHEIGHT*config.scale, 0);
}

ALLEGRO_BITMAP * CreateSpriteFromSheet( int spriteNum, ALLEGRO_BITMAP* spriteSheet)
{
	int sheetx = spriteNum % SHEET_OBJECTSWIDE;
	int sheety = spriteNum / SHEET_OBJECTSWIDE;
	return al_create_sub_bitmap(spriteSheet, sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT, SPRITEWIDTH, SPRITEHEIGHT);
}

void DrawSpriteIndexOverlay(int imageIndex){
	ALLEGRO_BITMAP* currentImage;
	if (imageIndex == -1)
	{
		currentImage=IMGObjectSheet;
	}
	else
	{
		if( imageIndex >= (int)IMGFilelist.size()) 
			return;
		currentImage=IMGFilelist[imageIndex];
	}
	al_clear_to_color(al_map_rgb(255, 0, 255));
	al_draw_bitmap(currentImage,0,0,0);
	for(int i =0; i<= 20*SPRITEWIDTH; i+=SPRITEWIDTH)
		al_draw_line(i,0,i, al_get_bitmap_height(al_get_target_bitmap()), al_map_rgb(0,0,0), 0);
	for(int i =0; i< al_get_bitmap_height(al_get_target_bitmap()); i+=SPRITEHEIGHT)
		al_draw_line(0,i, 20*SPRITEWIDTH,i,al_map_rgb(0,0,0), 0);

	for(int y = 0; y<20; y++){
		for(int x = 0; x<20; x+=5){
			int index = y * 20 + x;
			draw_textf_border(font, al_map_rgb(255,255,255),  x*SPRITEWIDTH+5, y* SPRITEHEIGHT+(al_get_font_line_height(font)/2), 0, "%i", index);
		}
	}	
	draw_textf_border(font, al_map_rgb(255,255,255), al_get_bitmap_width(al_get_target_bitmap())-10, al_get_bitmap_height(al_get_target_bitmap()) -al_get_font_line_height(font), ALLEGRO_ALIGN_RIGHT, 
		"%s (%d) (Press SPACE to return)",
		(imageIndex==-1?"objects.png":IMGFilenames[imageIndex]->c_str()), imageIndex);  
	al_flip_display();
}


void DoSpriteIndexOverlay()
{
	DrawSpriteIndexOverlay(-1);
	int index = 0;
	int max = (int)IMGFilenames.size();
	while(true)
	{
		while(!al_key_down(&keyboard,ALLEGRO_KEY_SPACE) && !al_key_down(&keyboard,ALLEGRO_KEY_F10)){
			al_get_keyboard_state(&keyboard);
			al_rest(ALLEGRO_MSECS_TO_SECS(50));
		}
		al_get_keyboard_state(&keyboard);
		if (al_key_down(&keyboard,ALLEGRO_KEY_SPACE))
		{
			break;
		}
		DrawSpriteIndexOverlay(index);
		index++;
		if (index >= max)
			index = -1;
		//debounce f10
		al_get_keyboard_state(&keyboard);
		while(al_key_down(&keyboard,ALLEGRO_KEY_F10)){
			al_get_keyboard_state(&keyboard);
			al_rest(ALLEGRO_MSECS_TO_SECS(50));
		}
	}
	//redraw screen again
	al_clear_to_color(al_map_rgb(config.backr,config.backg,config.backb));
	paintboard();
}

void paintboard(){
	uint32_t starttime = clock();
	//al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_NO_ALPHA);
	//if(!buffer)
	//	buffer = al_create_bitmap(al_get_display_width(al_get_current_display()), al_get_display_height(al_get_current_display()));
	//if(al_get_bitmap_width(buffer) != al_get_display_width(al_get_current_display()) || al_get_bitmap_height(buffer) != al_get_display_height(al_get_current_display()))
	//{
	//	al_destroy_bitmap(buffer);
	//	buffer = al_create_bitmap(al_get_display_width(al_get_current_display()), al_get_display_height(al_get_current_display()));
	//}
	//ALLEGRO_BITMAP * backup = al_get_target_bitmap();
	//al_set_target_bitmap(buffer);
	//al_set_separate_blender(ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA, ALLEGRO_ONE, ALLEGRO_ONE);
	//al_set_blender(ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA, al_map_rgba(255, 255, 255, 255));
	int op, src, dst, alpha_op, alpha_src, alpha_dst;
	al_get_separate_blender(&op, &src, &dst, &alpha_op, &alpha_src, &alpha_dst);
	al_set_separate_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO,ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	al_clear_to_color(al_map_rgba(0,0,0,0));
	if(!config.transparentScreenshots)
		al_clear_to_color(al_map_rgb(config.backr,config.backg,config.backb));
	al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst);

	//clear_to_color(buffer,makecol(12,7,49)); //this one is calm and nice

	if( viewedSegment == NULL ){
		draw_textf_border(font, al_map_rgb(255,255,255), al_get_bitmap_width(al_get_target_bitmap())/2, al_get_bitmap_height(al_get_target_bitmap())/2, ALLEGRO_ALIGN_CENTRE, "Could not find DF process");
		return;
	}

	al_lock_mutex(viewedSegment->mutie);

	viewedSegment->drawAllBlocks();
	if (config.show_osd) DrawCurrentLevelOutline(false);

	DebugInt1 = viewedSegment->getNumBlocks();

	uint32_t DrawTime = clock() - starttime;

	//teh drawtime indicator is too jumpy, so I'm averaging it out over 10 frames.
	static uint32_t DrawTimes[10];
	static int ind = 0;
	if(ind >= 10)
		ind = 0;
	DrawTimes[ind] = DrawTime;
	ind++;
	DrawTime = 0;
	for(int i = 0; i<10; i++)
		DrawTime += DrawTimes[i];
	DrawTime = DrawTime / 10;
	static double last_frame_time;
	double time_since_last_frame = al_get_time() - last_frame_time;
	last_frame_time = al_get_time();
	if (config.show_osd)
	{
		al_hold_bitmap_drawing(true);
		draw_textf_border(font, al_map_rgb(255,255,255), 10,al_get_font_line_height(font), 0, "%i,%i,%i, r%i, z%i", DisplayedSegmentX,DisplayedSegmentY,DisplayedSegmentZ, DisplayedRotation, config.zoom);

		if(config.debug_mode){
			draw_textf_border(font, al_map_rgb(255,255,255), 10, 3*al_get_font_line_height(font), 0, "Map Read Time: %dms", viewedSegment->read_time);
			draw_textf_border(font, al_map_rgb(255,255,255), 10, 4*al_get_font_line_height(font), 0, "Map Beautification Time: %ims", viewedSegment->beautify_time);
			draw_textf_border(font, al_map_rgb(255,255,255), 10, 2*al_get_font_line_height(font), 0, "FPS: %.2f", 1.0/time_since_last_frame);
			draw_textf_border(font, al_map_rgb(255,255,255), 10, 5*al_get_font_line_height(font), 0, "Draw: %ims", DrawTime);
			draw_textf_border(font, al_map_rgb(255,255,255), 10, 6*al_get_font_line_height(font), 0, "D1: %i", blockFactory.getPoolSize());
			draw_textf_border(font, al_map_rgb(255,255,255), 10, 7*al_get_font_line_height(font), 0, "%i/%i/%i, %i:%i", contentLoader->currentDay+1, contentLoader->currentMonth+1, contentLoader->currentYear, contentLoader->currentHour, (contentLoader->currentTickRel*60)/50);
			draw_textf_border(font, al_map_rgb(255,255,255), 10, 8*al_get_font_line_height(font), 0, "%i Sprites drawn, %i tiles drawn, %.1f sprites per tile.", config.drawcount, config.tilecount, ((float)config.drawcount/(float)config.tilecount));
			drawDebugCursorAndInfo();
		}
		config.drawcount = 0;
		config.tilecount = 0;
		int top = 0;
		if(config.follow_DFscreen)
		{
			top += al_get_font_line_height(font);
			draw_textf_border(font, al_map_rgb(255,255,255), al_get_bitmap_width(al_get_target_bitmap())/2,top, ALLEGRO_ALIGN_CENTRE, "Locked on DF screen + (%d,%d,%d)",config.viewXoffset,config.viewYoffset,config.viewZoffset);
		}
		if(config.follow_DFcursor && config.debug_mode)
		{
			top += al_get_font_line_height(font);
			if(config.dfCursorX != -30000)
				draw_textf_border(font, al_map_rgb(255,255,255), al_get_bitmap_width(al_get_target_bitmap())/2,top, ALLEGRO_ALIGN_CENTRE, "Following DF Cursor at: %d,%d,%d", config.dfCursorX,config.dfCursorY,config.dfCursorZ);
		}
		if(config.single_layer_view)
		{
			top += al_get_font_line_height(font);
			draw_textf_border(font, al_map_rgb(255,255,255), al_get_bitmap_width(al_get_target_bitmap())/2,top, ALLEGRO_ALIGN_CENTRE, "Single layer view");
		}
		if(config.automatic_reload_time)
		{
			top += al_get_font_line_height(font);
			draw_textf_border(font, al_map_rgb(255,255,255), al_get_bitmap_width(al_get_target_bitmap())/2,top, ALLEGRO_ALIGN_CENTRE, "Reloading every %0.1fs", (float)config.automatic_reload_time/1000);
		}
		al_hold_bitmap_drawing(false);
		DrawMinimap();
	}
	//al_set_target_bitmap(backup);
	//al_draw_bitmap(buffer, 0, 0, 0);
	al_unlock_mutex(viewedSegment->mutie);
	al_flip_display();
}


bool load_from_path (ALLEGRO_PATH * p, const char * filename, ALLEGRO_BITMAP *& imgd)
{
    int index;
    al_set_path_filename(p,filename);
    index = loadImgFile(al_path_cstr(p,ALLEGRO_NATIVE_PATH_SEP));
    if(index == -1)
    {
        return false;
    }
    imgd = al_create_sub_bitmap(IMGFilelist[index], 0, 0, al_get_bitmap_width(IMGFilelist[index]), al_get_bitmap_height(IMGFilelist[index]));
    return true;
}

void loadGraphicsFromDisk()
{
    ALLEGRO_PATH * p = al_create_path_for_directory("stonesense");
    if(!load_from_path(p, "objects.png", IMGObjectSheet)) return;
    if(!load_from_path(p, "creatures.png", IMGCreatureSheet)) return;
    if(!load_from_path(p, "ramps.png", IMGRampSheet)) return;
    if(!load_from_path(p, "SSStatusIcons.png", IMGStatusSheet)) return;
    if(!load_from_path(p, "gibs.png", IMGBloodSheet)) return;
    if(!load_from_path(p, "engravings_floor.png", IMGEngFloorSheet)) return;
    if(!load_from_path(p, "engravings_left.png", IMGEngLeftSheet)) return;
    if(!load_from_path(p, "engravings_right.png", IMGEngRightSheet)) return;
    al_destroy_path(p);
    createEffectSprites();
}

//delete and clean out the image files
void flushImgFiles()
{
	LogVerbose("flushing images...\n");
	destroyEffectSprites();
	//should be OK because we keep others from directly acccessing this stuff
	if(IMGObjectSheet)
	{
		al_destroy_bitmap(IMGObjectSheet);
		IMGObjectSheet = 0;
	}
	if(IMGCreatureSheet)
	{
		al_destroy_bitmap(IMGCreatureSheet);
		IMGCreatureSheet = 0;
	}
	if(IMGRampSheet)
	{
		al_destroy_bitmap(IMGRampSheet);
		IMGRampSheet = 0;
	}
	if(IMGStatusSheet)
	{
		al_destroy_bitmap(IMGStatusSheet);
		IMGStatusSheet = 0;
	}
	if(IMGEngFloorSheet)
	{
		al_destroy_bitmap(IMGEngFloorSheet);
		IMGEngFloorSheet = 0;
	}
	if(IMGEngLeftSheet)
	{
		al_destroy_bitmap(IMGEngLeftSheet);
		IMGEngLeftSheet = 0;
	}
	if(IMGEngRightSheet)
	{
		al_destroy_bitmap(IMGEngRightSheet);
		IMGEngRightSheet = 0;
	}
	uint32_t numFiles = (uint32_t)IMGFilelist.size();
	assert( numFiles == IMGFilenames.size());
	for(uint32_t i = 0; i < numFiles; i++)
	{
		al_destroy_bitmap(IMGFilelist[i]);
		//should be same length, I hope
		delete(IMGFilenames[i]);
	}
	uint32_t caches = (uint32_t)IMGCache.size();
	for(uint32_t i = 0; i < caches; i++)
		al_destroy_bitmap(IMGCache[i]);
	IMGFilelist.clear();
	IMGFilenames.clear();
	IMGCache.clear();

}

ALLEGRO_BITMAP* getImgFile(int index)
{
	return IMGFilelist[index];	
}

inline int returnGreater(int a, int b)
{
	if(a>b)
		return a;
	else return b;
}

int loadImgFile(const char* filename)
{
	if(config.cache_images)
	{
		static bool foundSize = false;
		if(!foundSize)
		{
			ALLEGRO_BITMAP* test = 0;
			while(true)
			{
				test = al_create_bitmap(config.imageCacheSize,config.imageCacheSize);
				if(test)
				{
					WriteErr("%i works.\n", config.imageCacheSize);
					break;
				}
				WriteErr("%i is too large. chopping it.\n", config.imageCacheSize);
				config.imageCacheSize = config.imageCacheSize / 2;
			}
			foundSize = true;
			al_destroy_bitmap(test);
		}
		int op, src, dst, alpha_op, alpha_src, alpha_dst;
		al_get_separate_blender(&op, &src, &dst, &alpha_op, &alpha_src, &alpha_dst);
		ALLEGRO_BITMAP* currentTarget = al_get_target_bitmap();
		uint32_t numFiles = (uint32_t)IMGFilelist.size();
		for(uint32_t i = 0; i < numFiles; i++)
		{
			if (strcmp(filename, IMGFilenames[i]->c_str()) == 0)
				return i;
		}
		/*
		al_clear_to_color(al_map_rgb(0,0,0));
		draw_textf_border(font, al_get_bitmap_width(al_get_target_bitmap())/2,
		al_get_bitmap_height(al_get_target_bitmap())/2,
		ALLEGRO_ALIGN_CENTRE, "Loading %s...", filename);
		al_flip_display();
		*/
		static int xOffset = 0;
		static int yOffset = 0;
		int currentCache = IMGCache.size() -1;
		static int columnWidth = 0;
		ALLEGRO_BITMAP* tempfile = load_bitmap_withWarning(filename);
		if(!tempfile)
			return -1;
		LogVerbose("New image: %s\n",filename);
		if(currentCache < 0)
		{
			IMGCache.push_back(al_create_bitmap(config.imageCacheSize, config.imageCacheSize));
			if(!IMGCache[0])
			{
				DFConsole->printerr("Cannot create bitmap sized %ix%i, please chose a smaller size",config.imageCacheSize,config.imageCacheSize);
			}
			currentCache = IMGCache.size() -1;
			LogVerbose("Creating image cache #%d\n",currentCache);
		}
		if((yOffset + al_get_bitmap_height(tempfile)) <= config.imageCacheSize)
		{
			al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
			al_set_target_bitmap(IMGCache[currentCache]);
			al_draw_bitmap(tempfile, xOffset, yOffset, 0);
			IMGFilelist.push_back(al_create_sub_bitmap(IMGCache[currentCache], xOffset, yOffset, al_get_bitmap_width(tempfile), al_get_bitmap_height(tempfile)));
			yOffset += al_get_bitmap_height(tempfile);
			columnWidth = returnGreater(columnWidth, al_get_bitmap_width(tempfile));
		}
		else if ((xOffset + al_get_bitmap_width(tempfile) + columnWidth) <= config.imageCacheSize)
		{
			yOffset = 0;
			xOffset += columnWidth;
			columnWidth = 0;
			al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
			al_set_target_bitmap(IMGCache[currentCache]);
			al_draw_bitmap(tempfile, xOffset, yOffset, 0);
			IMGFilelist.push_back(al_create_sub_bitmap(IMGCache[currentCache], xOffset, yOffset, al_get_bitmap_width(tempfile), al_get_bitmap_height(tempfile)));
			yOffset += al_get_bitmap_height(tempfile);
			columnWidth = returnGreater(columnWidth, al_get_bitmap_width(tempfile));
		}
		else
		{
			yOffset = 0;
			xOffset = 0;
			IMGCache.push_back(al_create_bitmap(config.imageCacheSize, config.imageCacheSize));
			currentCache = IMGCache.size() -1;
			LogVerbose("Creating image cache #%d\n",currentCache);
			al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
			al_set_target_bitmap(IMGCache[currentCache]);
			al_draw_bitmap(tempfile, xOffset, yOffset, 0);
			IMGFilelist.push_back(al_create_sub_bitmap(IMGCache[currentCache], xOffset, yOffset, al_get_bitmap_width(tempfile), al_get_bitmap_height(tempfile)));
			yOffset += al_get_bitmap_height(tempfile);
			columnWidth = returnGreater(columnWidth, al_get_bitmap_width(tempfile));
		}
		if(config.saveImageCache)
			saveImage(tempfile);
		al_destroy_bitmap(tempfile);
		al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
		IMGFilenames.push_back(new string(filename));
		al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst);
		if(config.saveImageCache)
			saveImage(IMGCache[currentCache]);
		al_clear_to_color(al_map_rgb(0,0,0));
		al_flip_display();
		return (int)IMGFilelist.size() - 1;
	}
	else
	{
		uint32_t numFiles = (uint32_t)IMGFilelist.size();
		for(uint32_t i = 0; i < numFiles; i++)
		{
			if (strcmp(filename, IMGFilenames[i]->c_str()) == 0)
				return i;
		}
		ALLEGRO_BITMAP * temp = load_bitmap_withWarning(filename);
		if(!temp)
			return -1;
		IMGFilelist.push_back(temp);
		IMGFilenames.push_back(new string(filename));
		LogVerbose("New image: %s\n",filename);
		return (int)IMGFilelist.size() - 1;
	}
}
/*
int loadImgFile(ALLEGRO_PATH* filepath)
{
    return loadImgFile(al_path_cstr(filepath, ALLEGRO_NATIVE_PATH_SEP));
}
*/
void saveScreenshot(){
	al_clear_to_color(al_map_rgb(config.backr,config.backg,config.backb));
	paintboard();
	//get filename
	char filename[20] ={0};
	FILE* fp;
	int index = 1;
	//search for the first screenshot# that does not exist already
	while(true){
		sprintf(filename, "screenshot%i.png", index);

		fp = fopen(filename, "r");
		if( fp != 0)
			fclose(fp);
		else
			//file does not exist, so exit loop
			break;
		index++;
	};
	//move image to 16 bits
	//al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_NO_ALPHA);
	ALLEGRO_BITMAP* temp = al_create_bitmap(al_get_bitmap_width(al_get_target_bitmap()), al_get_bitmap_height(al_get_target_bitmap()));
	al_set_target_bitmap(temp);
	if(!config.transparentScreenshots)
		al_clear_to_color(al_map_rgb(config.backr,config.backg,config.backb));
	paintboard();
	al_save_bitmap(filename, temp);
	al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
	al_destroy_bitmap(temp);
	//al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY);
}
void saveImage(ALLEGRO_BITMAP* image){
	//get filename
	char filename[20] ={0};
	FILE* fp;
	int index = 1;
	//search for the first screenshot# that does not exist already
	while(true){
		sprintf(filename, "Image%i.png", index);

		fp = fopen(filename, "r");
		if( fp != 0)
			fclose(fp);
		else
			//file does not exist, so exit loop
			break;
		index++;
	};
	al_save_bitmap(filename, image);
}

void dumpSegment()
{
	al_lock_mutex(config.readMutex);

	//back up all the relevant values
	Crd3D tempSize = config.segmentSize;
	int tempViewx = DisplayedSegmentX;
	int tempViewy = DisplayedSegmentY;
	bool tempFollow = config.follow_DFscreen;
	int tempLift = config.lift_segment_offscreen;
	int currentFlags = al_get_new_bitmap_flags();
	//now make them real big.
	config.follow_DFscreen = false;
	config.lift_segment_offscreen = 0;
	parms.x = 0;
	parms.y = 0;
	parms.z = config.cellDimZ - 1;
	parms.sizex = config.cellDimX;
	parms.sizey = config.cellDimY;
	parms.sizez = config.cellDimZ;

	read_segment(NULL);
	//get filename
	char filename[20] ={0};
	FILE* fp;
	int index = 1;
	//search for the first screenshot# that does not exist already
	while(true){
		sprintf(filename, "screenshot%i.png", index);

		fp = fopen(filename, "r");
		if( fp != 0)
			fclose(fp);
		else
			//file does not exist, so exit loop
			break;
		index++;
	};
	int tempFlags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);

	ALLEGRO_BITMAP * volume = al_create_bitmap(viewedSegment->sizex, (viewedSegment->sizez * viewedSegment->sizey));
	if(!volume)
	{
		DFConsole->printerr("Failed to create volumetric image.");
		al_set_new_bitmap_flags(tempFlags);
		return;
	}

	ALLEGRO_BITMAP * backup = al_get_target_bitmap();
	al_set_target_bitmap(volume);

	int op, src, dst, alpha_op, alpha_src, alpha_dst;
	al_get_separate_blender(&op, &src, &dst, &alpha_op, &alpha_src, &alpha_dst);
	al_set_separate_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO,ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	al_clear_to_color(al_map_rgba(0,0,0,0));
	al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst);
	viewedSegment->drawPixels();

	al_save_bitmap(filename, volume);
	al_destroy_bitmap(volume);

	al_set_target_bitmap(backup);
	al_set_new_bitmap_flags(tempFlags);

	//restore everything that we changed.
	config.segmentSize = tempSize;
	DisplayedSegmentX = tempViewx;
	DisplayedSegmentY = tempViewy;
	config.follow_DFscreen = tempFollow;
	config.lift_segment_offscreen = tempLift;

	al_unlock_mutex(config.readMutex);
}

void saveMegashot(bool tall){
	config.showRenderStatus = true;
	al_lock_mutex(config.readMutex);
	draw_textf_border(font, al_map_rgb(255,255,255), al_get_bitmap_width(al_get_target_bitmap())/2, al_get_bitmap_height(al_get_target_bitmap())/2, ALLEGRO_ALIGN_CENTRE, "Saving large screenshot...");
	al_flip_display();
	char filename[20] ={0};
	FILE* fp;
	int index = 1;
	//search for the first screenshot# that does not exist already
	while(true){
		sprintf(filename, "screenshot%i.png", index);
		fp = fopen(filename, "r");
		if( fp != 0)
			fclose(fp);
		else
			//file does not exist, so exit loop
			break;
		index++;
	};
	int timer = clock();
	//back up all the relevant values
	Crd3D backupsize = config.segmentSize;
	int tempViewx = DisplayedSegmentX;
	int tempViewy = DisplayedSegmentY;
	int tempViewz = DisplayedSegmentZ;
	bool tempFollow = config.follow_DFscreen;
	int tempLift = config.lift_segment_offscreen;
	//now make them real big.
	config.follow_DFscreen = false;
	config.lift_segment_offscreen = 0;
	config.segmentSize.x = config.cellDimX + 2;
	config.segmentSize.y = config.cellDimY + 2;
	if(tall)
		config.segmentSize.z = DisplayedSegmentZ + 1;
	int bigImageWidth = (config.cellDimX * TILEWIDTH);
	int bigImageHeight = ((config.cellDimX + config.cellDimY) * TILEHEIGHT / 2) + ((config.segmentSize.z - 1) * BLOCKHEIGHT);
	parms.sizex = config.segmentSize.x;
	parms.sizey = config.segmentSize.y;
	parms.sizez = config.segmentSize.z;
	DisplayedSegmentX = -1;
	DisplayedSegmentY = -1;
	parms.x = DisplayedSegmentX;
	parms.y = DisplayedSegmentY;
	parms.z = DisplayedSegmentZ;
	//Rebuild stuff
	read_segment(NULL);
	//Draw the image and save it
	bigFile = al_create_bitmap(bigImageWidth, bigImageHeight);
	if(bigFile)
	{
		DFConsole->print("\nSaving large screenshot to %s\n", filename);
		al_set_target_bitmap(bigFile);
		if(!config.transparentScreenshots)
			al_clear_to_color(al_map_rgb(config.backr,config.backg,config.backb));
		viewedSegment->drawAllBlocks();
		al_save_bitmap(filename, bigFile);
		al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
		al_destroy_bitmap(bigFile);
		timer = clock() - timer;
		DFConsole->print("Took %ims\n", timer);
	}
	else
	{
		DFConsole->printerr("Failed to take large screenshot. try using software mode\n");
	}
	//restore everything that we changed.
	config.segmentSize = backupsize;
	parms.sizex = config.segmentSize.x;
	parms.sizey = config.segmentSize.y;
	parms.sizez = config.segmentSize.z;
	DisplayedSegmentX = tempViewx;
	DisplayedSegmentY = tempViewy;
	DisplayedSegmentZ = tempViewz;
	parms.x = DisplayedSegmentX;
	parms.y = DisplayedSegmentY;
	parms.z = DisplayedSegmentZ;
	config.follow_DFscreen = tempFollow;
	config.lift_segment_offscreen = tempLift;
	config.showRenderStatus = false;
	al_unlock_mutex(config.readMutex);
}

void draw_particle_cloud(int count, float centerX, float centerY, float rangeX, float rangeY, ALLEGRO_BITMAP *sprite, ALLEGRO_COLOR tint)
{
	for(int i = 0;i < count;i++)
	{
		float drawx = centerX + ((((float)rand() / RAND_MAX) - 0.5) * rangeX);
		float drawy = centerY + ((((float)rand() / RAND_MAX) - 0.5) * rangeY);
		al_draw_tinted_bitmap(sprite, tint, drawx, drawy, 0);
	}
}
