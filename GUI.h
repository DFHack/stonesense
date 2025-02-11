#pragma once

#include "common.h"
#include <filesystem>

void ScreenToPoint(int x,int y,int &x1, int &y1, int &z1);
void pointToScreen(int *inx, int *iny, int inz);
void correctForRotation(auto& x, auto& y, unsigned char rot, auto szx, auto szy);
df::coord2d WorldTileToScreen(auto x, auto y, auto z);
df::coord2d LocalTileToScreen(auto x, auto y, auto z);
void DrawCurrentLevelOutline(bool backPart);
void DrawMinimap(WorldSegment *);
void paintboard();
void draw_textf_border(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, const char *format, ...);
void draw_text_border(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, const char *ustr);
void draw_ustr_border(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, const ALLEGRO_USTR *ustr);
ALLEGRO_BITMAP* getImgFile(int index);
void flushImgFiles();
//returns index into getImgFile. Will only create new bitmaps when needed
int loadImgFile(std::filesystem::path filename);
ALLEGRO_BITMAP * CreateSpriteFromSheet( int spriteNum, ALLEGRO_BITMAP* spriteSheet);
ALLEGRO_BITMAP* load_bitmap_withWarning(std::filesystem::path path);
void DrawSpriteIndexOverlay(int i);
void DoSpriteIndexOverlay();
void loadGraphicsFromDisk();
void saveScreenshot();
void saveMegashot(bool tall);
void dumpSegment();
void saveImage(ALLEGRO_BITMAP* image);

void draw_loading_message(const char *format, ...);

ALLEGRO_COLOR morph_color(ALLEGRO_COLOR source, ALLEGRO_COLOR reference, ALLEGRO_COLOR target);
