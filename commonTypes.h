#pragma once

#include "common.h"

enum ShadeBy
{
	ShadeNone,
	ShadeXml,
	ShadeMat,
	ShadeLayer,
	ShadeVein,
	ShadeMatFore,
	ShadeMatBack,
	ShadeLayerFore,
	ShadeLayerBack,
	ShadeVeinFore,
	ShadeVeinBack,
	ShadeBodyPart,
	ShadeJob
} ;
struct t_subSprite
{
	int32_t sheetIndex;
	int32_t fileIndex;
	ALLEGRO_COLOR shadeColor;
	ShadeBy shadeBy;
	char bodyPart[128];
	uint8_t snowMin;
	uint8_t snowMax;
} ;

struct t_SpriteWithOffset
{
	int32_t sheetIndex;
	int16_t x;
	int16_t y;
	int32_t fileIndex;
	uint8_t numVariations;
	char animFrames;
	ALLEGRO_COLOR shadeColor;
	bool needOutline;
	vector<t_subSprite> subSprites;
	ShadeBy shadeBy;
	char bodyPart[128];
	uint8_t snowMin;
	uint8_t snowMax;
} ;

typedef struct dfColors
{
	uint8_t black_r;
	uint8_t black_g;
	uint8_t black_b;
	uint8_t blue_r;
	uint8_t blue_g;
	uint8_t blue_b;
	uint8_t green_r;
	uint8_t green_g;
	uint8_t green_b;
	uint8_t cyan_r;
	uint8_t cyan_g;
	uint8_t cyan_b;
	uint8_t red_r;
	uint8_t red_g;
	uint8_t red_b;
	uint8_t magenta_r;
	uint8_t magenta_g;
	uint8_t magenta_b;
	uint8_t brown_r;
	uint8_t brown_g;
	uint8_t brown_b;
	uint8_t lgray_r;
	uint8_t lgray_g;
	uint8_t lgray_b;
	uint8_t dgray_r;
	uint8_t dgray_g;
	uint8_t dgray_b;
	uint8_t lblue_r;
	uint8_t lblue_g;
	uint8_t lblue_b;
	uint8_t lgreen_r;
	uint8_t lgreen_g;
	uint8_t lgreen_b;
	uint8_t lcyan_r;
	uint8_t lcyan_g;
	uint8_t lcyan_b;
	uint8_t lred_r;
	uint8_t lred_g;
	uint8_t lred_b;
	uint8_t lmagenta_r;
	uint8_t lmagenta_g;
	uint8_t lmagenta_b;
	uint8_t yellow_r;
	uint8_t yellow_g;
	uint8_t yellow_b;
	uint8_t white_r;
	uint8_t white_g;
	uint8_t white_b;
} dfColors;

typedef struct Crd2D {
	int32_t x,y;
}Crd2D;
typedef struct Crd3D {
	int32_t x,y,z;
}Crd3D;


typedef struct {
	bool show_zones;
	bool show_stockpiles;
	bool show_osd;
	bool single_layer_view;
	bool shade_hidden_blocks;
	bool show_hidden_blocks;
	bool show_creature_names;
	bool names_use_nick;
	bool names_use_species;
	bool show_all_creatures;
	bool load_ground_materials;
	bool hide_outer_blocks;
	bool debug_mode;
	bool track_center;
	int lift_segment_offscreen;
	bool truncate_walls;
	bool follow_DFscreen;
	bool verbose_logging;
	int viewXoffset;
	int viewYoffset;
	int viewZoffset;
	int automatic_reload_time;
	int automatic_reload_step;
	int animation_step;
	int fontsize;
	ALLEGRO_PATH * font;
	int screenWidth;
	int screenHeight;
	bool Fullscreen;
	bool show_intro;
	int fogr;
	int fogg;
	int fogb;
	int foga;
	int backr;
	int backg;
	int backb;
	bool fogenable;
	Crd3D segmentSize;
	
	bool follow_DFcursor;
	int dfCursorX;
	int dfCursorY;
	int dfCursorZ;
	unsigned int cellDimX;
	unsigned int cellDimY;
	unsigned int cellDimZ;

	bool saveImageCache;
	bool cache_images;
	int imageCacheSize;
	dfColors colors;
	bool opengl;
	bool directX;
	bool software;

	uint32_t menustate;
	t_viewscreen viewscreen;

	bool spriteIndexOverlay;
	bool creditScreen;
	int currentSpriteOverlay;

	bool showRenderStatus;
	bool dayNightCycle;

	bool show_creature_moods;

	//follows are anti-crash things
	bool skipWorld;
	bool skipCreatures;
	bool skipCreatureTypes;
	bool skipCreatureTypesEx;
	bool skipDescriptorColors;
	bool skipBuildings;
	bool skipVegetation;
	bool skipConstructions;
	bool skipMaps;
	bool skipInorganicMats;
	bool skipOrganicMats;
} GameConfiguration;


enum enumCreatureSex{
	eCreatureSex_NA,
	eCreatureSex_Male,
	eCreatureSex_Female
};