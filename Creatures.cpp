#include "common.h"
#include "Creatures.h"
#include "WorldSegment.h"
#include "CreatureConfiguration.h"
#include "GUI.h"

#include "dfhack/library/tinyxml/tinyxml.h"

vector<t_matgloss> v_creatureNames;
vector<CreatureConfiguration> creatureTypes;



bool IsCreatureVisible( t_creature* c ){
  if( config.show_all_creatures ) return true;

  if( c->flags1.bits.dead )
    return false;
  if( c->flags1.bits.caged )
    return false;
  return true;
}

void DrawCreature( BITMAP* target, int drawx, int drawy, t_creature* creature ){
  int spriteNum = GetCreatureSpriteMap( creature );
  //if(creature->x == 151 && creature->y == 145)
  //  int j = 10;
  DrawSpriteFromSheet( spriteNum, target, IMGCreatureSheet, drawx, drawy );
  textprintf(target, font, drawx, drawy-10, 0xFFffFF, "%s", creature->first_name.c_str() );
}
//t_creature* global = 0;

void ReadCreaturesToSegment(API& DF, WorldSegment* segment){
  uint32_t numcreatures = DF.InitReadCreatures();

  DF.ReadCreatureMatgloss(v_creatureNames);
  if( !CreatureNamesTranslatedFromGame )
    TranslateCreatureNames();

  t_creature tempcreature;
  uint32_t index = 0;
	while(index < numcreatures )
  {
    DF.ReadCreature( index, tempcreature );
    if( IsCreatureVisible( &tempcreature )
        && segment->CoordinateInsideSegment(tempcreature.x, tempcreature.y, tempcreature.z) )
      {
      Block* b = segment->getBlock (tempcreature.x, tempcreature.y, tempcreature.z );
      if(!b){
        //inside segment, but no block to represent it
        b = new Block(segment);
        b->x = tempcreature.x;
        b->y = tempcreature.y;
        b->z = tempcreature.z;
        segment->addBlock( b );
      }

      //add copy of creature
      t_creature* storage = new t_creature();//(t_creature *)malloc( sizeof(t_creature) );
      //memcpy( storage, &tempcreature, sizeof(t_creature) );
      *storage = tempcreature;
//        if(storage->x == 151 && storage->y == 145)
//          global = storage;
      b->creature = storage;
    }
    index++;
  }
  DF.FinishReadCreatures();
}


int GetCreatureSpriteMap( t_creature* c ){
  uint32_t num = (uint32_t)creatureTypes.size();
  for(uint32_t i=0; i < num; i++)
    //TODO: Optimize. make a table lookup instead of a search
    if( c->type == creatureTypes[i].gameID )
      return creatureTypes[i].sheetIndex;

  return SPRITECRE_NA;
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
  if(c->flags1.bits.active_invader)
    strcat(strbuffer, "activeInvader ");
  if(c->flags1.bits.caged)
    strcat(strbuffer, "Caged ");
  if(c->flags1.bits.chained)
    strcat(strbuffer, "chained ");
  if(c->flags1.bits.coward)
    strcat(strbuffer, "coward ");
   if(c->flags1.bits.dead)
    strcat(strbuffer, "Dead ");
  if(c->flags1.bits.diplomat)
    strcat(strbuffer, "Diplomat ");
  if(c->flags1.bits.drowning)
    strcat(strbuffer, "drowning ");
  if(c->flags1.bits.forest)
    strcat(strbuffer, "lostLeaving ");
  if(c->flags1.bits.fortress_guard)
    strcat(strbuffer, "FortGuard ");
  if(c->flags1.bits.had_mood)
    strcat(strbuffer, "HadMood ");
  if(c->flags1.bits.has_mood)
    strcat(strbuffer, "Mood ");
  if(c->flags1.bits.hidden_ambusher)
    strcat(strbuffer, "hiddenAmbush ");
  if(c->flags1.bits.hidden_in_ambush)
    strcat(strbuffer, "hiddenInAmbush ");
  if(c->flags1.bits.important_historical_figure)
    strcat(strbuffer, "Historical ");
  if(c->flags1.bits.incoming)
    strcat(strbuffer, "Incoming ");
  if(c->flags1.bits.invades)
    strcat(strbuffer, "invading ");
  if(c->flags1.bits.marauder)
    strcat(strbuffer, "marauder ");
  if(c->flags1.bits.merchant)
    strcat(strbuffer, "merchant ");
  if(c->flags1.bits.on_ground)
    strcat(strbuffer, "onGround ");
  if(c->flags1.bits.projectile)
    strcat(strbuffer, "projectile ");
  if(c->flags1.bits.ridden)
    strcat(strbuffer, "ridden ");
  if(c->flags1.bits.royal_guard)
    strcat(strbuffer, "RoyGuard ");
  if(c->flags1.bits.skeleton)
    strcat(strbuffer, "Skeleton ");
  if(c->flags1.bits.tame)
    strcat(strbuffer, "Tame ");
  if(c->flags1.bits.zombie)
    strcat(strbuffer, "Zombie ");

  if(c->flags2.bits.slaughter)
    strcat(strbuffer, "ReadyToSlaughter ");
  if(c->flags2.bits.resident)
    strcat(strbuffer, "Resident ");
  if(c->flags2.bits.sparring)
    strcat(strbuffer, "Sparring ");
  if(c->flags2.bits.swimming)
    strcat(strbuffer, "Swimming ");
  if(c->flags2.bits.underworld)
    strcat(strbuffer, "Underworld ");

  //if(c->flags1.bits.can_swap)
  //  strcat(strbuffer, "canSwap ");
  //if(c->flags1.bits.check_flows)
  //  strcat(strbuffer, "checFlows ");
  //if(c->flags1.bits.invader_origin)
  //  strcat(strbuffer, "invader_origin ");
  
}