#pragma once
#include "common.h"
#include "CreatureConfiguration.h"

enum enumCreatureSprites{
	SPRITECRE_NA = 0,
  SPRITECRE_DWARF_PEASANT = 1,

  SPRITECRE_CAT = 20,
  SPRITECRE_DOG = 21,
  SPRITECRE_CAMEL1 = 22,
  SPRITECRE_CAMEL2 = 23,
  SPRITECRE_MULE = 24,
  SPRITECRE_MUSKOX = 25,
  SPRITECRE_HORSE = 26,
};



void ReadCreaturesToSegment(API& DF, WorldSegment* segment);

void DrawCreature( BITMAP* target, int drawx, int drawy, t_creature* c );
int GetCreatureSpriteMap( t_creature* c );

void LoadCreatureConfiguration( vector<CreatureConfiguration>* knownCreatures );

void generateCreatureDebugString( t_creature* c, char* strbuffer);

extern vector<t_matgloss> v_creatureNames;
extern vector<CreatureConfiguration> creatureTypes;