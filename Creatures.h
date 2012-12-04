#pragma once
#include "common.h"
#include "commonTypes.h"
#include "CreatureConfiguration.h"

#define	SPRITECRE_NA 0;

class Block;

using DFHack::Units::t_unit;

void ReadCreaturesToSegment( DFHack::Core& DF, WorldSegment* segment);

void AssembleCreature(int drawx, int drawy, t_unit* creature, Block * b);
void AssembleCreatureText(int drawx, int drawy, t_unit* creature, WorldSegment * b);
void DrawCreatureText(int drawx, int drawy, t_unit* creature );
c_sprite* GetCreatureSpriteMap( t_unit* c );
int GetCreatureShadowMap( t_unit* c );


void generateCreatureDebugString( t_unit* c, char* strbuffer);
void generateCreatureDebugString2( t_unit* c, char* strbuffer);

//extern vector<t_matgloss> v_creatureNames;
//extern vector<CreatureConfiguration> creatureTypes;