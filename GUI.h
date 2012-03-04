#pragma once

#include "common.h"

void ScreenToPoint(int x,int y,int &x1, int &y1, int &z1);
void pointToScreen(int *inx, int *iny, int inz);
Crd2D WorldBlockToScreen(int32_t x, int32_t y, int32_t z);
Crd2D LocalBlockToScreen(int32_t x, int32_t y, int32_t z);
void DrawCurrentLevelOutline(bool backPart);
void DrawMinimap(WorldSegment *);
void paintboard(DFHack::Core * c);
void draw_textf_border(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, const char *format, ...);
void draw_text_border(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, const char *ustr);
void draw_ustr_border(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, const ALLEGRO_USTR *ustr);
ALLEGRO_BITMAP* getImgFile(int index);
void flushImgFiles();
//returns index into getImgFile. Will only create new bitmaps when needed
int loadImgFile(const char* filename);
//int loadImgFile(ALLEGRO_PATH* filepath);
void DrawSpriteFromSheet( int spriteNum,ALLEGRO_BITMAP* spriteSheet, ALLEGRO_COLOR color, float x, float y, Block * b = 0, float scale = 1.0f);
ALLEGRO_BITMAP * CreateSpriteFromSheet( int spriteNum, ALLEGRO_BITMAP* spriteSheet);
ALLEGRO_BITMAP* load_bitmap_withWarning(const char* path);
void DrawSpriteIndexOverlay(int i);
void DoSpriteIndexOverlay(DFHack::Core * c);
void loadGraphicsFromDisk();
void saveScreenshot(DFHack::Core * c);
void saveMegashot(bool tall);
void dumpSegment();
void saveImage(ALLEGRO_BITMAP* image);
void draw_particle_cloud(int count, float centerX, float centerY, float rangeX, float rangeY, ALLEGRO_BITMAP *sprite, ALLEGRO_COLOR tint);

/// currently visible map segment
extern SegmentWrap* map_segment;

extern int DisplayedSegmentX;
extern int DisplayedSegmentY;
extern int DisplayedSegmentZ;
extern int DisplayedRotation;

extern int MiniMapTopLeftX;
extern int MiniMapTopLeftY;
extern int MiniMapBottomRightX;
extern int MiniMapBottomRightY;
extern int MiniMapSegmentWidth;
extern int MiniMapSegmentHeight;
extern double oneBlockInPixels;

extern ALLEGRO_BITMAP* IMGObjectSheet; 
extern ALLEGRO_BITMAP* IMGCreatureSheet; 
extern ALLEGRO_BITMAP* IMGRampSheet;  
extern ALLEGRO_BITMAP* IMGStatusSheet; 
extern ALLEGRO_BITMAP* IMGBloodSheet;
extern ALLEGRO_BITMAP* IMGEngFloorSheet;
extern ALLEGRO_BITMAP* IMGEngLeftSheet;
extern ALLEGRO_BITMAP* IMGEngRightSheet;

extern Crd3D debugCursor;
extern vector<string*> IMGFilenames;

ALLEGRO_COLOR operator*(const ALLEGRO_COLOR &color1, const ALLEGRO_COLOR &color2);
ALLEGRO_COLOR operator+(const ALLEGRO_COLOR &color1, const ALLEGRO_COLOR &color2);
ALLEGRO_COLOR getDayShade(int hour, int tick);
ALLEGRO_COLOR partialBlend(const ALLEGRO_COLOR & color2, const ALLEGRO_COLOR & color1, int percent);

void swapSegments(void);