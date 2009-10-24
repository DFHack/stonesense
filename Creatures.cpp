#include "common.h"
#include "Creatures.h"
#include "WorldSegment.h"


vector<t_matgloss> v_creatureNames;

int GetCreatureSpriteMap( t_creature* c ){
  //TODO: optimize by putting into a clever little table
  char* strid = v_creatureNames[c->type].id;
  if(strcmpi(strid, "dwarf") == 0)         return SPRITECRE_DWARF_PEASANT;
  if(strcmpi(strid, "cat") == 0)           return SPRITECRE_CAT;
  if(strcmpi(strid, "dog") == 0)           return SPRITECRE_DOG;
  if(strcmpi(strid, "camel_1_hump") == 0)  return SPRITECRE_CAMEL1;
  if(strcmpi(strid, "camel_2_humps") == 0) return SPRITECRE_CAMEL2;
  if(strcmpi(strid, "mule") == 0)          return SPRITECRE_MULE;
  if(strcmpi(strid, "muskox") == 0)        return SPRITECRE_MUSKOX;
  if(strcmpi(strid, "horse") == 0)         return SPRITECRE_HORSE;
    

  return SPRITECRE_NA;
}


void ReadCreaturesToSegment(DFHackAPI& DF, WorldSegment* segment){
  uint32_t numcreatures = DF.InitReadCreatures();

  DF.ReadCreatureMatgloss(v_creatureNames);
  t_creature tempcreature;
  uint32_t index = 0;
	while(index < numcreatures )
  {
    DF.ReadCreature( index, tempcreature );
    assert(tempcreature.type != 0);
    Block* b;
    if( b = segment->getBlock (tempcreature.x, tempcreature.y, tempcreature.z ) )
      b->creature = tempcreature;
    index++;
  }
  DF.FinishReadCreatures();
}


