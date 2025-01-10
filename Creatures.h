#pragma once
#include "common.h"
#include "commonTypes.h"
#include "CreatureConfiguration.h"

constexpr auto SPRITECRE_NA = 0;

class Tile;

void ReadCreaturesToSegment( DFHack::Core& DF, WorldSegment* segment);

void AssembleCreature(int drawx, int drawy, SS_Unit* creature, Tile * b);
void AssembleCreatureText(int drawx, int drawy, SS_Unit* creature, WorldSegment * b);
void DrawCreatureText(int drawx, int drawy, SS_Unit* creature );
c_sprite* GetCreatureSpriteMap( SS_Unit* c );
uint8_t GetCreatureShadowMap( SS_Unit* c );


void generateCreatureDebugString( SS_Unit* c, char* strbuffer);
void generateCreatureDebugString2( SS_Unit* c, char* strbuffer);
