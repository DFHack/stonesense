#pragma once
#include "common.h"
#include "CreatureConfiguration.h"

#define	SPRITECRE_NA 0;



void ReadCreaturesToSegment(API& DF, WorldSegment* segment);

void DrawCreature( BITMAP* target, int drawx, int drawy, t_creature* c );
int GetCreatureSpriteMap( t_creature* c );



void generateCreatureDebugString( t_creature* c, char* strbuffer);

extern vector<t_matgloss> v_creatureNames;
extern vector<CreatureConfiguration> creatureTypes;