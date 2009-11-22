#pragma once

#include "common.h"

void ScreenToPoint(int x,int y,int &x1, int &y1, int &z1);
void pointToScreen(int *inx, int *iny, int inz);
Crd2D WorldBlockToScreen(int32_t x, int32_t y, int32_t z);
Crd2D LocalBlockToScreen(int32_t x, int32_t y, int32_t z);
void DrawMinimap(BITMAP* target);
void paintboard();

BITMAP* getImgFile(int index);
void flushImgFiles();
//returns index into getImgFile. Will only create new bitmaps when needed
int loadImgFile(char* filename);
void DrawSpriteFromSheet( int spriteNum, BITMAP* target, BITMAP* spriteSheet, int x, int y);
BITMAP* load_bitmap_withWarning(char* path);
void DrawSpriteIndexOverlay(int i);
void DoSpriteIndexOverlay();
void loadGraphicsFromDisk();
void destroyGraphics();
void saveScreenshot();

extern WorldSegment* viewedSegment;//current, loaded
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


extern BITMAP* IMGFloorSheet; 
extern BITMAP* IMGObjectSheet; 
extern BITMAP* IMGCreatureSheet; 
extern BITMAP* IMGRampSheet; 
extern BITMAP* IMGRamptopSheet; 

extern Crd2D debugCursor;
