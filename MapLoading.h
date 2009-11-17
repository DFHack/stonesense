#pragma once




inline bool IDisWall(int in);
inline bool IDisFloor(int in);
void reloadDisplayedSegment();

void DisconnectFromDF();
bool IsConnectedToDF();

extern memory_info dfMemoryInfo;
