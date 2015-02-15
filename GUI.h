#pragma once

#include "common.h"

void ScreenToPoint(int x,int y,int &x1, int &y1, int &z1);
void pointToScreen(int *inx, int *iny, int inz);
void correctTileForDisplayedOffset(int32_t&, int32_t&, int32_t&);
void correctForRotation(int32_t& x, int32_t& y, unsigned char rot, int32_t szx, int32_t szy);
Crd2D WorldTileToScreen(int32_t x, int32_t y, int32_t z);
Crd2D LocalTileToScreen(int32_t x, int32_t y, int32_t z);
void DrawCurrentLevelOutline(bool backPart);
void DrawMinimap(WorldSegment *);
void paintboard();
void draw_textf_border(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, const char *format, ...);
void draw_text_border(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, const char *ustr);
void draw_ustr_border(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, const ALLEGRO_USTR *ustr);
ALLEGRO_BITMAP* getImgFile(int index);
void flushImgFiles();
//returns index into getImgFile. Will only create new bitmaps when needed
int loadImgFile(const char* filename);
//int loadImgFile(ALLEGRO_PATH* filepath);
ALLEGRO_BITMAP * CreateSpriteFromSheet( int spriteNum, ALLEGRO_BITMAP* spriteSheet);
ALLEGRO_BITMAP* load_bitmap_withWarning(const char* path);
void DrawSpriteIndexOverlay(int i);
void DoSpriteIndexOverlay();
void loadGraphicsFromDisk();
void saveScreenshot();
void saveMegashot(bool tall);
void dumpSegment();
void saveImage(ALLEGRO_BITMAP* image);

void draw_loading_message(const char *format, ...);

extern int MiniMapTopLeftX;
extern int MiniMapTopLeftY;
extern int MiniMapBottomRightX;
extern int MiniMapBottomRightY;
extern int MiniMapSegmentWidth;
extern int MiniMapSegmentHeight;
extern double oneTileInPixels;

extern ALLEGRO_BITMAP* IMGObjectSheet;
extern ALLEGRO_BITMAP* IMGCreatureSheet;
extern ALLEGRO_BITMAP* IMGRampSheet;
extern ALLEGRO_BITMAP* IMGStatusSheet;
extern ALLEGRO_BITMAP* IMGProfSheet;
extern ALLEGRO_BITMAP* IMGJobSheet;
extern ALLEGRO_BITMAP* IMGBloodSheet;
extern ALLEGRO_BITMAP* IMGEngFloorSheet;
extern ALLEGRO_BITMAP* IMGEngLeftSheet;
extern ALLEGRO_BITMAP* IMGEngRightSheet;
extern ALLEGRO_BITMAP* IMGLetterSheet;

extern vector<string*> IMGFilenames;

void swapSegments(void);

ALLEGRO_COLOR morph_color(ALLEGRO_COLOR source, ALLEGRO_COLOR reference, ALLEGRO_COLOR target);