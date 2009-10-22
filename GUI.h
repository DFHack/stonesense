#pragma once

#include "common.h"



void pointToScreen(int *inx, int *iny, int inz);
Crd2D WorldBlockToScreen(uint32_t x, uint32_t y, uint32_t z);
void DrawMinimap(BITMAP* target);
void paintboard();


BITMAP* load_bitmap_withWarning(char* path);
void loadGraphicsFromDisk();
void destroyGraphics();
void saveScreenshot();

extern WorldSegment* viewedSegment;//current, loaded
extern int DisplayedSegmentX;
extern int DisplayedSegmentY;
extern int DisplayedSegmentZ;


extern BITMAP* IMGFloorSheet; 
extern BITMAP* IMGWallSheet; 
extern BITMAP* IMGStairSheet; 
extern BITMAP* IMGRampSheet; 