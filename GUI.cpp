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
ALLEGRO_BITMAP* buffer = 0;
vector<ALLEGRO_BITMAP*> IMGCache;
vector<ALLEGRO_BITMAP*> IMGFilelist;
vector<string*> IMGFilenames;
GLhandleARB tinter;
GLhandleARB tinter_shader;
Crd3D debugCursor;


void ScreenToPoint(int x,int y,int &x1, int &y1, int &z1)
{ //assume z of 0
	x-=TILEWIDTH/2;
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
void draw_textf_border(const ALLEGRO_FONT *font, float x, float y, int flags, const char *format, ...)
{
	ALLEGRO_STATE backup;
	al_store_state(&backup, ALLEGRO_STATE_BLENDER);
	int op, src, dst, alpha_op, alpha_src, alpha_dst;
	ALLEGRO_COLOR color;
	al_get_separate_blender(&op, &src, &dst, &alpha_op, &alpha_src, &alpha_dst, &color);
	al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, al_map_rgb(0, 0, 0));

	ALLEGRO_USTR *buf;
	va_list arglist;
	const char *s;

	/* Fast path for common case. */
	if (0 == strcmp(format, "%s")) {
		va_start(arglist, format);
		s = va_arg(arglist, const char *);
		al_draw_text(font, x-1, y-1, flags, s);
		al_draw_text(font, x-1, y+1, flags, s);
		al_draw_text(font, x+1, y+1, flags, s);
		al_draw_text(font, x+1, y-1, flags, s);
		al_restore_state(&backup);
		al_draw_text(font, x, y, flags, s);
		va_end(arglist);
		return;
	}


	va_start(arglist, format);
	buf = al_ustr_new("");
	al_ustr_vappendf(buf, format, arglist);
	va_end(arglist);
	al_draw_text(font, x-1, y-1, flags, al_cstr(buf));
	al_draw_text(font, x-1, y+1, flags, al_cstr(buf));
	al_draw_text(font, x+1, y+1, flags, al_cstr(buf));
	al_draw_text(font, x+1, y-1, flags, al_cstr(buf));
	al_draw_text(font, x-1, y, flags, al_cstr(buf));
	al_draw_text(font, x, y+1, flags, al_cstr(buf));
	al_draw_text(font, x+1, y, flags, al_cstr(buf));
	al_draw_text(font, x, y-1, flags, al_cstr(buf));
	al_restore_state(&backup);
	al_draw_text(font, x, y, flags, al_cstr(buf));
	al_ustr_free(buf);
}
void draw_text_border(const ALLEGRO_FONT *font, float x, float y, int flags, const char *ustr)
{
	ALLEGRO_STATE backup;
	al_store_state(&backup, ALLEGRO_STATE_BLENDER);
	int op, src, dst, alpha_op, alpha_src, alpha_dst;
	ALLEGRO_COLOR color;
	al_get_separate_blender(&op, &src, &dst, &alpha_op, &alpha_src, &alpha_dst, &color);
	al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, al_map_rgb(0, 0, 0));

	al_draw_text(font, x-1, y-1, flags, ustr);
	al_draw_text(font, x-1, y+1, flags, ustr);
	al_draw_text(font, x+1, y+1, flags, ustr);
	al_draw_text(font, x+1, y-1, flags, ustr);

	al_draw_text(font, x-1, y, flags, ustr);
	al_draw_text(font, x, y+1, flags, ustr);
	al_draw_text(font, x+1, y, flags, ustr);
	al_draw_text(font, x, y-1, flags, ustr);
	al_restore_state(&backup);
	al_draw_text(font, x, y, flags, ustr);
}
void draw_ustr_border(const ALLEGRO_FONT *font, float x, float y, int flags, const ALLEGRO_USTR *ustr)
{
	ALLEGRO_STATE backup;
	al_store_state(&backup, ALLEGRO_STATE_BLENDER);
	int op, src, dst, alpha_op, alpha_src, alpha_dst;
	ALLEGRO_COLOR color;
	al_get_separate_blender(&op, &src, &dst, &alpha_op, &alpha_src, &alpha_dst, &color);
	al_set_separate_blender(op, src, dst,alpha_op, alpha_src, alpha_dst, al_map_rgb(0, 0, 0));

	al_draw_ustr(font, x-1, y-1, flags, ustr);
	al_draw_ustr(font, x-1, y+1, flags, ustr);
	al_draw_ustr(font, x+1, y+1, flags, ustr);
	al_draw_ustr(font, x+1, y-1, flags, ustr);
	al_draw_ustr(font, x-1, y, flags, ustr);
	al_draw_ustr(font, x, y+1, flags, ustr);
	al_draw_ustr(font, x+1, y, flags, ustr);
	al_draw_ustr(font, x, y-1, flags, ustr);
	al_restore_state(&backup);
	al_draw_ustr(font, x, y, flags, ustr);
}
void pointToScreen(int *inx, int *iny, int inz){
	int offx = al_get_bitmap_width(al_get_target_bitmap()) / 2;
	int offy = (-20)-(BLOCKHEIGHT * config.lift_segment_offscreen);
	int z=inz-1;
	int x = *inx-*iny;
	int y = *inx+*iny;
	x = x * TILEWIDTH / 2;
	y = y * TILEHEIGHT / 2;
	x+=offx;
	y+=offy;
	y-=z * BLOCKHEIGHT;
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
	int x = viewedSegment->x+1;
	int y = viewedSegment->y+1;
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
		al_draw_line(p1.x, p1.y, p1.x, p1.y-BLOCKHEIGHT, color_segmentoutline, 0);
		al_draw_line(p1.x, p1.y, p1.x, p1.y-BLOCKHEIGHT, color_segmentoutline, 0);
		al_draw_line(p1.x, p1.y, p2.x, p2.y, color_segmentoutline, 0);
		al_draw_line(p1.x, p1.y-BLOCKHEIGHT, p2.x, p2.y-BLOCKHEIGHT, color_segmentoutline, 0);
		al_draw_line(p2.x, p2.y, p2.x, p2.y-BLOCKHEIGHT, color_segmentoutline, 0);

		al_draw_line(p1.x, p1.y, p3.x, p3.y, color_segmentoutline, 0);
		al_draw_line(p1.x, p1.y-BLOCKHEIGHT, p3.x, p3.y-BLOCKHEIGHT, color_segmentoutline, 0);
		al_draw_line(p3.x, p3.y, p3.x, p3.y-BLOCKHEIGHT, color_segmentoutline, 0);
	}else{
		al_draw_line(p4.x, p4.y, p4.x, p4.y-BLOCKHEIGHT, color_segmentoutline, 0);
		al_draw_line(p4.x, p4.y, p2.x, p2.y, color_segmentoutline ,0);
		al_draw_line(p4.x, p4.y-BLOCKHEIGHT, p2.x, p2.y-BLOCKHEIGHT, color_segmentoutline ,0);

		al_draw_line(p4.x, p4.y, p3.x, p3.y, color_segmentoutline, 0);
		al_draw_line(p4.x, p4.y-BLOCKHEIGHT, p3.x, p3.y-BLOCKHEIGHT, color_segmentoutline, 0);
	}
}

void drawDebugCursorAndInfo(){
	if((config.dfCursorX != -30000) && config.follow_DFcursor)
	{
		int x = config.dfCursorX;
		int y = config.dfCursorY;
		int z = config.dfCursorZ;
		correctBlockForSegmetOffset(x,y,z);
		correctBlockForRotation( x, y, z);
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
	draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, "Block 0x%x", b);
	if(!b) return;

	draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
		"Coord:(%i,%i,%i)", b->x,b->y,b->z);

	int ttype;
	char* tform = NULL;
	if (b->floorType>0)
	{
		ttype=b->floorType;	  
		tform="floor";
	}
	else if (b->wallType > 0)
	{
		ttype=b->wallType;	  
		tform="wall";	 
	}
	else if (b->ramp.type > 0)
	{
		ttype=b->ramp.type;	  
		tform="ramp";	 
	}
	else if (b->stairType > 0)
	{
		ttype=b->stairType;	  
		tform="stair";	 
	}

	if (tform != NULL)
	{
		const char* matName = lookupMaterialTypeName(b->material.type);
		const char* subMatName = lookupMaterialName(b->material.type,b->material.index);
		draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,
			"%s:%i Material:%s%s%s", tform, ttype, 
			matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
	}
	if (tform != NULL)
	{
		draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,
			"MaterialType: %d, MaterialIndex: %d", b->material.type, b->material.index);
	}
	if (tform != NULL)
	{
		const char fore = lookupMaterialFore(b->material.type, b->material.index);
		const char back = lookupMaterialBack(b->material.type, b->material.index);
		const char bright = lookupMaterialBright(b->material.type, b->material.index);
		draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,
			"Fore: %d, Back: %d, Bright: %d", fore, back, bright);
	}
	if (tform != NULL)
	{
		const char* matName = lookupMaterialTypeName(b->layerMaterial.type);
		const char* subMatName = lookupMaterialName(b->layerMaterial.type,b->layerMaterial.index);
		draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,
			"Layer Material:%s%s%s", 
			matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
	} 
	if ((tform != NULL) && b->hasVein == 1)
	{
		const char* matName = lookupMaterialTypeName(b->veinMaterial.type);
		const char* subMatName = lookupMaterialName(b->veinMaterial.type,b->veinMaterial.index);
		draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,
			"Vein Material:%s%s%s", 
			matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
	} 
	char occs[33];
	itoa(b->occ.whole, occs, 2);
	draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,
		"Occupancy flags: %s", occs );

	if(b->water.index > 0 || b->tree.index != 0)
		draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
		"tree:%i water:%i,%i", b->tree.index, b->water.type, b->water.index);
	if(b->tree.index != 0)
		draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
		"tree name:%s type:%i", lookupTreeName(b->tree.index), b->tree.type);
	//building
	if(b->building.info.type != BUILDINGTYPE_NA && b->building.info.type != BUILDINGTYPE_BLACKBOX){
		const char* matName = lookupMaterialTypeName(b->building.info.material.type);
		const char* subMatName = lookupMaterialName(b->building.info.material.type,b->building.info.material.index);
		draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
			"Building: %s(%i,0x%x) Material: %s%s%s (%d,%d)", 
			contentLoader.classIdStrings.at(b->building.info.type).c_str(),
			b->building.info.type, b->building.info.vtable,
			matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"",
			b->building.info.material.type,b->building.info.material.index);
	}

	//creatures
	if(b->creature != null){
		draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
			"Creature:%s(%i) Job:%s", 
			contentLoader.creatureMaterials.at(b->creature->race).id, b->creature->race, 
			contentLoader.professionStrings.at(b->creature->profession).c_str());

		char strCreature[150] = {0};
		generateCreatureDebugString( b->creature, strCreature );
		//memset(strCreature, -1, 50);
		draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
			"flag1: %s ", strCreature );
	}
	if(b->designation.bits.traffic)
	{
		draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0, 
			"Traffic: %d", b->designation.bits.traffic);
	}
	if(b->designation.bits.water_table)
	{
		draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,"Water table");
	}
	if(b->designation.bits.rained)
	{
		draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,"Rained");
	}
	if(b->building.info.type != BUILDINGTYPE_BLACKBOX)
	{
		draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,
			"Temp1: %dU, %.2f'C, %d'F, Temp2: %dU, %.2f'C, %d'F", b->temp1, (float)(b->temp1-10000)*5/9, b->temp1-9968, b->temp2, (float)(b->temp2-10000)*5/9, b->temp2-9968);
	}
	if(b->snowlevel || b->mudlevel)
	{
		draw_textf_border(font, 2, al_get_bitmap_height(al_get_target_bitmap())-20-(i--*al_get_font_line_height(font)), 0,
			"Snow: %d, Mud: %d", b->snowlevel, b->mudlevel);
	}

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
		draw_textf_border(font, posx, posy, 0, "No map loaded");
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

void DrawSpriteFromSheet( int spriteNum, ALLEGRO_BITMAP* spriteSheet, int x, int y){
	int sheetx = spriteNum % SHEET_OBJECTSWIDE;
	int sheety = spriteNum / SHEET_OBJECTSWIDE;
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
	al_draw_bitmap_region(spriteSheet, sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT, SPRITEWIDTH, SPRITEHEIGHT, x, y - (WALLHEIGHT), 0);
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
			draw_textf_border(font,  x*SPRITEWIDTH+5, y* SPRITEHEIGHT+(al_get_font_line_height(font)/2), 0, "%i", index);
		}
	}	
	draw_textf_border(font, al_get_bitmap_width(al_get_target_bitmap())-10, al_get_bitmap_height(al_get_target_bitmap()) -al_get_font_line_height(font), ALLEGRO_ALIGN_RIGHT, 
		"%s (Press SPACE to return)",
		(imageIndex==-1?"objects.png":IMGFilenames[imageIndex]->c_str()));  
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
	paintboard();
}

void paintboard(){
	uint32_t starttime = clock();
	//al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_NO_ALPHA);
	//if(!buffer)
	//	buffer = al_create_bitmap(al_get_display_width(), al_get_display_height());
	//if(al_get_bitmap_width(buffer) != al_get_display_width() || al_get_bitmap_height(buffer) != al_get_display_height())
	//{
	//	al_destroy_bitmap(buffer);
	//	buffer = al_create_bitmap(al_get_display_width(), al_get_display_height());
	//}
	//al_set_target_bitmap(al_get_backbuffer());
	//al_set_separate_blender(ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA, ALLEGRO_ONE, ALLEGRO_ONE, al_map_rgba(255, 255, 255, 255));
	//al_set_blender(ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA, al_map_rgba(255, 255, 255, 255));
	al_clear_to_color(al_map_rgb(config.backr,config.backg,config.backb));
	//clear_to_color(buffer,makecol(12,7,49)); //this one is calm and nice

	if( viewedSegment == NULL ){
		draw_textf_border(font, al_get_bitmap_width(al_get_target_bitmap())/2, al_get_bitmap_height(al_get_target_bitmap())/2, ALLEGRO_ALIGN_CENTRE, "Could not find DF process");
		return;
	}



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
	if (config.show_osd)
	{
		al_hold_bitmap_drawing(true);
		draw_textf_border(font, 10,al_get_font_line_height(font), 0, "%i,%i,%i, r%i", DisplayedSegmentX,DisplayedSegmentY,DisplayedSegmentZ, DisplayedRotation);

		if(config.debug_mode){
			draw_textf_border(font, 10, 2*al_get_font_line_height(font), 0, "Timer1: %ims", ClockedTime);
			draw_textf_border(font, 10, 3*al_get_font_line_height(font), 0, "Timer2: %ims", ClockedTime2);
			draw_textf_border(font, 10, 4*al_get_font_line_height(font), 0, "Draw: %ims", DrawTime);
			draw_textf_border(font, 10, 5*al_get_font_line_height(font), 0, "D1: %i", blockFactory.getPoolSize());
			drawDebugCursorAndInfo();
		}
		int top = 0;
		if(config.follow_DFscreen)
		{
			top += al_get_font_line_height(font);
			draw_textf_border(font, al_get_bitmap_width(al_get_target_bitmap())/2,top, ALLEGRO_ALIGN_CENTRE, "Locked on DF screen + (%d,%d,%d)",config.viewXoffset,config.viewYoffset,config.viewZoffset);
		}
		if(config.follow_DFcursor)
		{
			top += al_get_font_line_height(font);
			if(config.dfCursorX != -30000)
				draw_textf_border(font, al_get_bitmap_width(al_get_target_bitmap())/2,top, ALLEGRO_ALIGN_CENTRE, "Following DF Cursor at: %d,%d,%d", config.dfCursorX,config.dfCursorY,config.dfCursorZ);
			else
				draw_textf_border(font, al_get_bitmap_width(al_get_target_bitmap())/2,top, ALLEGRO_ALIGN_CENTRE, "No DF cursor to follow");
		}
		if(config.single_layer_view)
		{
			top += al_get_font_line_height(font);
			draw_textf_border(font, al_get_bitmap_width(al_get_target_bitmap())/2,top, ALLEGRO_ALIGN_CENTRE, "Single layer view");
		}
		if(config.automatic_reload_time)
		{
			top += al_get_font_line_height(font);
			draw_textf_border(font, al_get_bitmap_width(al_get_target_bitmap())/2,top, ALLEGRO_ALIGN_CENTRE, "Reloading every %0.1fs", (float)config.automatic_reload_time/1000);
		}
		al_hold_bitmap_drawing(false);
		DrawMinimap();
	}
	//al_set_target_bitmap(al_get_backbuffer());
	//al_draw_bitmap(buffer, 0, 0, 0);
	al_flip_display();
}




ALLEGRO_BITMAP* load_bitmap_withWarning(char* path){
	ALLEGRO_BITMAP* img = 0;
	img = al_load_bitmap(path);
	if(!img){
		DisplayErr("Cannot load image: %s", path);
		exit(0);
	}
	al_convert_mask_to_alpha(img, al_map_rgb(255, 0, 255));
	return img;
}

void loadGraphicsFromDisk(){
	/*al_clear_to_color(al_map_rgb(0,0,0));
	draw_textf_border(font,
		al_get_bitmap_width(al_get_target_bitmap())/2,
		al_get_bitmap_height(al_get_target_bitmap())/2,
		ALLEGRO_ALIGN_CENTRE, "Loading...");
	al_flip_display();*/
	int index;
	index = loadImgFile("objects.png");
	IMGObjectSheet = al_create_sub_bitmap(IMGFilelist[index], 0, 0, al_get_bitmap_width(IMGFilelist[index]), al_get_bitmap_height(IMGFilelist[index]));
	index = loadImgFile("creatures.png");
	IMGCreatureSheet = al_create_sub_bitmap(IMGFilelist[index], 0, 0, al_get_bitmap_width(IMGFilelist[index]), al_get_bitmap_height(IMGFilelist[index]));
	index = loadImgFile("ramps.png");
	IMGRampSheet = al_create_sub_bitmap(IMGFilelist[index], 0, 0, al_get_bitmap_width(IMGFilelist[index]), al_get_bitmap_height(IMGFilelist[index]));
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

int loadImgFile(char* filename)
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
		ALLEGRO_COLOR color;
		al_get_separate_blender(&op, &src, &dst, &alpha_op, &alpha_src, &alpha_dst, &color);
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
		LogVerbose("New image: %s\n",filename);
		if(currentCache < 0)
		{
			IMGCache.push_back(al_create_bitmap(config.imageCacheSize, config.imageCacheSize));
			if(!IMGCache[0])
			{
				DisplayErr("Cannot create bitmap sized %ix%i, please chose a smaller size",config.imageCacheSize,config.imageCacheSize);
			}
			currentCache = IMGCache.size() -1;
			LogVerbose("Creating image cache #%d\n",currentCache);
		}
		if((yOffset + al_get_bitmap_height(tempfile)) <= config.imageCacheSize)
		{
			al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO, al_map_rgba(255, 255, 255, 255));
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
			al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO, al_map_rgba(255, 255, 255, 255));
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
			al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO, al_map_rgba(255, 255, 255, 255));
			al_set_target_bitmap(IMGCache[currentCache]);
			al_draw_bitmap(tempfile, xOffset, yOffset, 0);
			IMGFilelist.push_back(al_create_sub_bitmap(IMGCache[currentCache], xOffset, yOffset, al_get_bitmap_width(tempfile), al_get_bitmap_height(tempfile)));
			yOffset += al_get_bitmap_height(tempfile);
			columnWidth = returnGreater(columnWidth, al_get_bitmap_width(tempfile));
		}
		if(config.saveImageCache)
			saveImage(tempfile);
		al_destroy_bitmap(tempfile);
		al_set_target_bitmap(al_get_backbuffer());
		IMGFilenames.push_back(new string(filename));
		al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
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
		IMGFilelist.push_back(load_bitmap_withWarning(filename));
		IMGFilenames.push_back(new string(filename));
		LogVerbose("New image: %s\n",filename);
		return (int)IMGFilelist.size() - 1;
	}
}
int loadImgFile(ALLEGRO_PATH* filepath)
{
	char *filename = strcpy(filename,al_path_cstr(filepath, ALLEGRO_NATIVE_PATH_SEP));
	return loadImgFile(filename);
}

void saveScreenshot(){
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
	paintboard();
	al_save_bitmap(filename, temp);
	al_set_target_bitmap(al_get_backbuffer());
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
void saveMegashot(){
	draw_textf_border(font, al_get_bitmap_width(al_get_target_bitmap())/2, al_get_bitmap_height(al_get_target_bitmap())/2, ALLEGRO_ALIGN_CENTRE, "Saving large screenshot...");
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
	WriteErr("\nSaving large screenshot to %s\n", filename);
	int timer = clock();
	//back up all the relevant values
	Crd3D tempSize = config.segmentSize;
	int tempViewx = DisplayedSegmentX;
	int tempViewy = DisplayedSegmentY;
	bool tempFollow = config.follow_DFscreen;
	int tempLift = config.lift_segment_offscreen;
	//now make them real big.
	config.follow_DFscreen = false;
	config.lift_segment_offscreen = 0;
	int bigImageWidth = (config.cellDimX * TILEWIDTH);
	int bigImageHeight = ((config.cellDimX + config.cellDimY) * TILEHEIGHT / 2) + ((config.segmentSize.z - 1) * BLOCKHEIGHT);
	config.segmentSize.x = config.cellDimX + 2;
	config.segmentSize.y = config.cellDimY + 2;
	DisplayedSegmentX = -1;
	DisplayedSegmentY = -1;
	//Rebuild stuff
	reloadDisplayedSegment();
	//Draw the image and save it
	//al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_NO_ALPHA);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	ALLEGRO_BITMAP* bigFile = al_create_bitmap(bigImageWidth, bigImageHeight);
	al_set_target_bitmap(bigFile);
	al_clear_to_color(al_map_rgb(config.backr,config.backg,config.backb));
	viewedSegment->drawAllBlocks();
	al_save_bitmap(filename, bigFile);
	al_set_target_bitmap(al_get_backbuffer());
	al_destroy_bitmap(bigFile);
	//restore everything that we changed.
	config.segmentSize = tempSize;
	DisplayedSegmentX = tempViewx;
	DisplayedSegmentY = tempViewy;
	config.follow_DFscreen = tempFollow;
	config.lift_segment_offscreen = tempLift;
	timer = clock() - timer;
	WriteErr("Took %ims\n", timer);
	//al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY);
	al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
}

void draw_particle_cloud(int count, float centerX, float centerY, float rangeX, float rangeY, ALLEGRO_BITMAP *sprite)
{
	for(int i = 0;i < count;i++)
	{
		float drawx = centerX + ((((float)rand() / RAND_MAX) - 0.5) * rangeX);
		float drawy = centerY + ((((float)rand() / RAND_MAX) - 0.5) * rangeY);
		al_draw_bitmap(sprite, drawx, drawy, 0);
	}
}
