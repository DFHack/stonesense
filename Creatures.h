#pragma once
#include "common.h"
#include "commonTypes.h"
#include "CreatureConfiguration.h"

#define	SPRITECRE_NA 0;

static t_SpriteWithOffset spriteCre_NA = {0, 0, 0,-1,1+2+4+8+16+32};

void ReadCreaturesToSegment(API& DF, WorldSegment* segment);

void DrawCreature( BITMAP* target, int drawx, int drawy, t_creature* c );
t_SpriteWithOffset GetCreatureSpriteMap( t_creature* c );
int GetCreatureShadowMap( t_creature* c );


void generateCreatureDebugString( t_creature* c, char* strbuffer);

//extern vector<t_matgloss> v_creatureNames;
//extern vector<CreatureConfiguration> creatureTypes;