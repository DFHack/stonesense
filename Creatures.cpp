#include "common.h"
#include "Creatures.h"
#include "WorldSegment.h"
#include "CreatureConfiguration.h"

#include "dfhack/library/tinyxml/tinyxml.h"

vector<t_matgloss> v_creatureNames;
vector<CreatureConfiguration> creatureTypes;


int GetCreatureSpriteMap( t_creature* c ){
  //TODO: optimize by putting into a clever little table
  /*char* strid = v_creatureNames[c->type].id;
  if(strcmpi(strid, "dwarf") == 0)         return SPRITECRE_DWARF_PEASANT;
  if(strcmpi(strid, "cat") == 0)           return SPRITECRE_CAT;
  if(strcmpi(strid, "dog") == 0)           return SPRITECRE_DOG;
  if(strcmpi(strid, "camel_1_hump") == 0)  return SPRITECRE_CAMEL1;
  if(strcmpi(strid, "camel_2_humps") == 0) return SPRITECRE_CAMEL2;
  if(strcmpi(strid, "mule") == 0)          return SPRITECRE_MULE;
  if(strcmpi(strid, "muskox") == 0)        return SPRITECRE_MUSKOX;
  if(strcmpi(strid, "horse") == 0)         return SPRITECRE_HORSE;
    */
  

  for(uint32_t i=0; i < creatureTypes.size(); i++)
    if( c->type == creatureTypes[i].gameID )
      return creatureTypes[i].sheetIndex;

  return SPRITECRE_NA;
}
bool IsCreatureVisible( t_creature* c){
  if( c->flags1.bits.dead )
    return false;
  return true;
}


void ReadCreaturesToSegment(DFHackAPI& DF, WorldSegment* segment){
  uint32_t numcreatures = DF.InitReadCreatures();

  DF.ReadCreatureMatgloss(v_creatureNames);
  if( !CreatureNamesTranslatedFromGame )
    TranslateCreatureNames();

  t_creature tempcreature;
  uint32_t index = 0;
	while(index < numcreatures )
  {
    DF.ReadCreature( index, tempcreature );
    if( IsCreatureVisible( &tempcreature ) ){
      Block* b;
      if( b = segment->getBlock (tempcreature.x, tempcreature.y, tempcreature.z ) )
        b->creature = tempcreature;
    }
    index++;
  }
  DF.FinishReadCreatures();
}




void LoadCreatureConfiguration( vector<CreatureConfiguration>* knownCreatures ){
  char* filename = "Creatures.xml";
  TiXmlDocument doc( filename );
  bool loadOkay = doc.LoadFile();
  TiXmlHandle hDoc(&doc);
  TiXmlElement* elemCreature;

  knownCreatures->clear();

  elemCreature = hDoc.FirstChildElement("Creature").Element();
  while( elemCreature ){
    const char* name = elemCreature->Attribute("gameID");
    const char* sheetIndexStr = elemCreature->Attribute("sheetIndex");

    CreatureConfiguration cre( (char*)name, atoi(sheetIndexStr) );
    //add a copy to known creatures
    knownCreatures->push_back( cre );
    
    elemCreature = elemCreature->NextSiblingElement("Creature");
  }
}