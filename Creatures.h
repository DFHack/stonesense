#pragma once
#include "common.h"

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



void ReadCreaturesToSegment(DFHackAPI& DF, WorldSegment* segment);

int GetCreatureSpriteMap( t_creature* c );

extern vector<t_matgloss> v_creatureNames;
