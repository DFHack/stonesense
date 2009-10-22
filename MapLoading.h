#pragma once




inline bool IDisWall(int in);
inline bool IDisFloor(int in);
void reloadDisplayedSegment();

WorldSegment* ReadMapSegment(int x, int y, int z, int sizex, int sizey, int sizez);