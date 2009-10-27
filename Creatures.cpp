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
  
  uint32_t num = (uint32_t)creatureTypes.size();
  for(uint32_t i=0; i < num; i++)
    if( c->type == creatureTypes[i].gameID )
      return creatureTypes[i].sheetIndex;

  return SPRITECRE_NA;
}
bool IsCreatureVisible( t_creature* c){
  if( config.show_all_creatures ) return true;

  if( c->flags1.bits.dead )
    return false;
  if( c->flags1.bits.unk26_invisible_hidden )
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

      if(tempcreature.x == 143 && tempcreature.y == 332)
        int j = 10;
      if(tempcreature.x == 146 && tempcreature.y == 334)
        int j = 10;
      
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

  CreatureNamesTranslatedFromGame = false;
}



void generateCreatureDebugString( t_creature* c, char* strbuffer){
  if(c->flags1.bits.dead)
    strcat(strbuffer, "Dead ");
  if(c->flags1.bits.hostile)
    strcat(strbuffer, "hostile ");
  if(c->flags1.bits.fortress_guard)
    strcat(strbuffer, "F_guard ");
  if(c->flags1.bits.invader1)
    strcat(strbuffer, "invader1 ");
  if(c->flags1.bits.invader2)
    strcat(strbuffer, "invader2 ");
  if(c->flags1.bits.mood_survivor)
    strcat(strbuffer, "mood_surv ");
  if(c->flags1.bits.royal_guard)
    strcat(strbuffer, "R_guard ");
  if(c->flags1.bits.skeletal)
    strcat(strbuffer, "skeletal ");
  if(c->flags1.bits.tame)
    strcat(strbuffer, "tame ");
  if(c->flags1.bits.unconscious)
    strcat(strbuffer, "unconscious ");
  if(c->flags1.bits.unk1)
    strcat(strbuffer, "u1 ");
  if(c->flags1.bits.unk10)
    strcat(strbuffer, "u10 ");
  if(c->flags1.bits.unk11_not_on_unit_screen2)
    strcat(strbuffer, "u11_not_on_unit_screen ");
  if(c->flags1.bits.unk12_friendly)
    strcat(strbuffer, "u12friendly ");
  if(c->flags1.bits.unk15_not_part_of_fortress)
    strcat(strbuffer, "u15_notpart_of_fort ");
  if(c->flags1.bits.unk17_not_visible)
    strcat(strbuffer, "u17notvis ");
  if(c->flags1.bits.unk19_not_listed_among_dwarves)
    strcat(strbuffer, "u19notlisted ");
  if(c->flags1.bits.unk21)
    strcat(strbuffer, "u21 ");
  if(c->flags1.bits.hidden_ambusher)
    strcat(strbuffer, "hidden_ambusher ");
  if(c->flags1.bits.unk23)
    strcat(strbuffer, "u23 ");
  if(c->flags1.bits.unk24)
    strcat(strbuffer, "u24 ");
  if(c->flags1.bits.unk25)
    strcat(strbuffer, "u25 ");
  if(c->flags1.bits.unk26_invisible_hidden)
    strcat(strbuffer, "u26invHidden ");
  if(c->flags1.bits.unk28)
    strcat(strbuffer, "u28 ");
  if(c->flags1.bits.unk3)
    strcat(strbuffer, "u3 ");
  if(c->flags1.bits.unk31)
    strcat(strbuffer, "u31 ");
  if(c->flags1.bits.unk32)
    strcat(strbuffer, "u32 ");
  if(c->flags1.bits.unk6)
    strcat(strbuffer, "u6 ");
  if(c->flags1.bits.unk7_friendly)
    strcat(strbuffer, "u7Friendly ");
  if(c->flags1.bits.unk8_friendly)
    strcat(strbuffer, "u8Friendly ");
  if(c->flags1.bits.unk9_not_on_unit_screen1)
    strcat(strbuffer, "u9NotListed ");
  if(c->flags1.bits.zombie)
    strcat(strbuffer, "ZOMBIE! ");


}