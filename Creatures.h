#pragma once
#include "common.h"
#include "commonTypes.h"
#include "CreatureConfiguration.h"

#define	SPRITECRE_NA 0;

void ReadCreaturesToSegment(API& DF, WorldSegment* segment);

void DrawCreature(int drawx, int drawy, t_creature* creature );
void DrawCreatureText(int drawx, int drawy, t_creature* creature );
c_sprite* GetCreatureSpriteMap( t_creature* c );
int GetCreatureShadowMap( t_creature* c );


void generateCreatureDebugString( t_creature* c, char* strbuffer);
void generateCreatureDebugString2( t_creature* c, char* strbuffer);

//extern vector<t_matgloss> v_creatureNames;
//extern vector<CreatureConfiguration> creatureTypes;