#include "common.h"
#include "Creatures.h"
#include "WorldSegment.h"
#include "CreatureConfiguration.h"
#include "ContentLoader.h"
#include "GUI.h"
#include "Block.h"
#include "SpriteColors.h"
#include "DataDefs.h"
#include "df/world.h"
#include "df/unit_inventory_item.h"

//vector<t_matgloss> v_creatureNames;
//vector<CreatureConfiguration> creatureTypes;

int32_t charToUpper(int32_t c)
{
	if(c >= 0x61 && c <= 0x7A)
		return c-0x20;
	else if(c >= 0xE0 && c <= 0xF6)
		return c-0x20;
	else if(c >= 0xF8 && c <= 0xFE)
		return c-0x20;
	else return c;
}



ALLEGRO_USTR* bufferToUstr(const char* buffer, int length)
{
	ALLEGRO_USTR* temp = al_ustr_new("");
	for(int i = 0; i < length; i++)
	{
		switch((unsigned char)buffer[i])
		{
		case 0:
			{
				i = length;
				break;
			}
		case 128:
			{
				al_ustr_append_chr(temp, 0xC7);
				break;
			}
		case 129:
			{
				al_ustr_append_chr(temp, 0xFC);
				break;
			}
		case 130:
			{
				al_ustr_append_chr(temp, 0xE9);
				break;
			}
		case 131:
			{
				al_ustr_append_chr(temp, 0xE2);
				break;
			}
		case 132:
			{
				al_ustr_append_chr(temp, 0xE4);
				break;
			}
		case 133:
			{
				al_ustr_append_chr(temp, 0xE0);
				break;
			}
		case 134:
			{
				al_ustr_append_chr(temp, 0xE5);
				break;
			}
		case 135:
			{
				al_ustr_append_chr(temp, 0xE7);
				break;
			}
		case 136:
			{
				al_ustr_append_chr(temp, 0xEA);
				break;
			}
		case 137:
			{
				al_ustr_append_chr(temp, 0xEB);
				break;
			}
		case 138:
			{
				al_ustr_append_chr(temp, 0xE8);
				break;
			}
		case 139:
			{
				al_ustr_append_chr(temp, 0xEF);
				break;
			}
		case 140:
			{
				al_ustr_append_chr(temp, 0xEE);
				break;
			}
		case 141:
			{
				al_ustr_append_chr(temp, 0xC4);
				break;
			}
		case 142:
			{
				al_ustr_append_chr(temp, 0xC5);
				break;
			}
		case 143:
			{
				al_ustr_append_chr(temp, 0xC9);
				break;
			}
		case 144:
			{
				al_ustr_append_chr(temp, 0xFC);
				break;
			}
		case 145:
			{
				al_ustr_append_chr(temp, 0xE9);
				break;
			}
		case 146:
			{
				al_ustr_append_chr(temp, 0xC6);
				break;
			}
		case 147:
			{
				al_ustr_append_chr(temp, 0xF4);
				break;
			}
		case 148:
			{
				al_ustr_append_chr(temp, 0xF6);
				break;
			}
		case 149:
			{
				al_ustr_append_chr(temp, 0xF2);
				break;
			}
		case 150:
			{
				al_ustr_append_chr(temp, 0xFB);
				break;
			}
		case 151:
			{
				al_ustr_append_chr(temp, 0xF9);
				break;
			}
		case 152:
			{
				al_ustr_append_chr(temp, 0xFF);
				break;
			}
		case 153:
			{
				al_ustr_append_chr(temp, 0xD6);
				break;
			}
		case 154:
			{
				al_ustr_append_chr(temp, 0xDC);
				break;
			}
		case 160:
			{
				al_ustr_append_chr(temp, 0xE1);
				break;
			}
		case 161:
			{
				al_ustr_append_chr(temp, 0xED);
				break;
			}
		case 162:
			{
				al_ustr_append_chr(temp, 0xF3);
				break;
			}
		case 163:
			{
				al_ustr_append_chr(temp, 0xFA);
				break;
			}
		case 164:
			{
				al_ustr_append_chr(temp, 0xF1);
				break;
			}
		case 165:
			{
				al_ustr_append_chr(temp, 0xD1);
				break;
			}
		default:
			{
				al_ustr_append_chr(temp, buffer[i]);
				break;
			}
		}
	}
	return temp;		
}

bool IsCreatureVisible( df::unit* c ){
	if( config.show_all_creatures ) return true;

	if( c->flags1.bits.dead )
		return false;
	if( c->flags1.bits.caged )
		return false;
	if( c->flags1.bits.hidden_in_ambush )
		return false;
	return true;
}

void DrawCreature(int drawx, int drawy, t_unit* creature, Block * b){
	vector<int> statusIcons;

	//if(config.show_creature_happiness)
	if(config.show_creature_moods)
	{
		if(creature->happiness == 0)
			statusIcons.push_back(6);
		else if(creature->happiness >= 1 && creature->happiness <= 25)
			statusIcons.push_back(5);
		else if(creature->happiness >= 26 && creature->happiness <= 50)
			statusIcons.push_back(4);
		else if(creature->happiness >= 51 && creature->happiness <= 75)
			statusIcons.push_back(3);
		else if(creature->happiness >= 76 && creature->happiness <= 125)
			statusIcons.push_back(2);
		else if(creature->happiness >= 126 && creature->happiness <= 150)
			statusIcons.push_back(1);
		else if(creature->happiness >= 151)
			statusIcons.push_back(0);

		if(creature->mood == 0)
			statusIcons.push_back(19);
		else if(creature->mood == 1)
			statusIcons.push_back(19);
		else if(creature->mood == 2)
			statusIcons.push_back(21);
		else if(creature->mood == 3)
			statusIcons.push_back(19);
		else if(creature->mood == 4)
			statusIcons.push_back(19);
		else if(creature->mood == 5)
			statusIcons.push_back(18);
		else if(creature->mood == 6)
			statusIcons.push_back(18);

		if(creature->current_job.active && creature->current_job.jobType == 21)
			statusIcons.push_back(16);
		else if(creature->current_job.active && creature->current_job.jobType == 52)
			statusIcons.push_back(17);
	}

	c_sprite * sprite = GetCreatureSpriteMap( creature );
	//if(creature->x == 151 && creature->y == 145)
	//  int j = 10; 
	sprite->draw_world(creature->x,creature->y, creature->z, b);
	if(statusIcons.size())
	{
		for(int i = 0; i < statusIcons.size(); i++)
		{
			unsigned int sheetx = 16 * (statusIcons[i] % 7);
			unsigned int sheety = 16 * (statusIcons[i] / 7);
			al_draw_bitmap_region(IMGStatusSheet, sheetx, sheety, 16, 16, drawx - (statusIcons.size()*8) + (16*i) + (SPRITEWIDTH/2), drawy - (16 + WALLHEIGHT + al_get_font_line_height(font)), 0);
		}
	}
}

void DrawCreatureText(int drawx, int drawy, t_unit* creature ){
	if( config.show_creature_names )
		if (creature->name.nickname[0] && config.names_use_nick)
		{
			draw_textf_border(font, al_map_rgb(255,255,255), drawx, drawy-(WALLHEIGHT+al_get_font_line_height(font)), 0, 
				"%s", creature->name.nickname );
		}
		else if (creature->name.first_name[0])
		{
			char buffer[128];
			strncpy(buffer,creature->name.first_name,127);
			buffer[127]=0;
			ALLEGRO_USTR* temp = bufferToUstr(buffer, 128);
			al_ustr_set_chr(temp, 0, charToUpper(al_ustr_get(temp, 0)));
			draw_ustr_border(font, al_map_rgb(255,255,255), drawx, drawy-((WALLHEIGHT*config.scale)+al_get_font_line_height(font)), 0,
				temp );
			al_ustr_free(temp);
		}
		else if (config.names_use_species)
		{
			if(!config.skipCreatureTypes)
				draw_textf_border(font, al_map_rgb(255,255,255), drawx, drawy-(WALLHEIGHT*config.scale+al_get_font_line_height(font)), 0, 
				"[%s]", contentLoader->Mats->race.at(creature->race).id.c_str());
		}
}

//t_creature* global = 0;
using df::global::world;
void ReadCreaturesToSegment( DFHack::Core& DF, WorldSegment* segment)
{
    if(config.skipCreatures)
        return;
	int x1 = segment->x;
	int x2 = segment->x + segment->sizex;
	int y1 = segment->y;
	int y2 = segment->y + segment->sizey;
	int z1 = segment->z;
	int z2 = segment->z + segment->sizez;
	uint32_t numcreatures;

    numcreatures = DFHack::Simple::Units::getNumCreatures();
	if(!numcreatures) return;
	if(x1<0) x1=0;
	if(y1<0) y1=0;
	if(z1<0) z1=0;
	if(x2<0) x2=0;
	if(y2<0) y2=0;
	if(z2<0) z2=0;

	t_unit *tempcreature = new t_unit();
    df::unit *unit_ptr = 0;
	uint32_t index = 0;
    while((index = DFHack::Simple::Units::GetCreatureInBox( index, &unit_ptr, x1,y1,z1,x2,y2,z2)) != -1 )
    {
        index++;
        // if the creature isn't visible, we need not process it further.
        if( !IsCreatureVisible( unit_ptr ) )
            continue;
        // make a copy of some creature data
        DFHack::Simple::Units::CopyCreature(unit_ptr,*tempcreature);
        // Acquire a cube element thingie!
        Block* b = segment->getBlock (tempcreature->x, tempcreature->y, tempcreature->z );
        // If we failed at that, make a new one out of fairy dust and makebelieve ;)
        if(!b)
        {
            //inside segment, but no block to represent it
            b = new Block(segment);
            b->x = tempcreature->x;
            b->y = tempcreature->y;
            b->z = tempcreature->z;
            // fake block occupancy where needed. This is starting to get hacky...
            b->creaturePresent=true;
            segment->addBlock( b );
        }

        // creature already there? SKIP.
        if(b->creature)
            continue;

        //Creature not yet there, we process...
        b->creaturePresent=true;
        b->creature = tempcreature;
        // add shadow to nearest floor block
        for (int bz = tempcreature->z;bz>=z1;bz--)
        {
            b = segment->getBlock (tempcreature->x, tempcreature->y, bz );
            if (!b) continue;
            if (b->tileShapeBasic==tiletype_shape_basic::Floor ||
                b->tileShapeBasic==tiletype_shape_basic::Wall  ||
                b->tileShapeBasic==tiletype_shape_basic::Ramp)
            {
                // todo figure out appropriate shadow size
                int tempShadow = GetCreatureShadowMap( tempcreature );
                if (b->shadow < tempShadow)
                    b->shadow=tempShadow;
                break;
            }
        }
        // add the materials of what the creature's wearing.
        // FIXME: this is constructor material. Don't clutter normal code with this.
        b->Weapon.matt.index=INVALID_INDEX;
        b->Weapon.matt.type=INVALID_INDEX;
        b->Weapon.rating = 0;
        b->Armor.matt.index=INVALID_INDEX;
        b->Armor.matt.type=INVALID_INDEX;
        b->Armor.rating = 0;
        b->Shoes.matt.index=INVALID_INDEX;
        b->Shoes.matt.type=INVALID_INDEX;
        b->Shoes.rating = 0;
        b->Shield.matt.index=INVALID_INDEX;
        b->Shield.matt.type=INVALID_INDEX;
        b->Shield.rating = 0;
        b->Helm.matt.index=INVALID_INDEX;
        b->Helm.matt.type=INVALID_INDEX;
        b->Helm.rating = 0;
        b->Gloves.matt.index=INVALID_INDEX;
        b->Gloves.matt.type=INVALID_INDEX;
        b->Gloves.rating = 0;
        for (auto iter = unit_ptr->inventory.begin(); iter != unit_ptr->inventory.end(); iter++)
        {
            df::unit_inventory_item * itemslot = *iter;
            // skip if invalid
            if(!itemslot)
                continue;

            df::item * item = itemslot->item;
            // skip if no item associated with item slot
            if(!item)
                continue;

            // skip if not weapon or worn item
            if(itemslot->mode != df::unit_inventory_item::T_mode::Weapon &&
               itemslot->mode != df::unit_inventory_item::T_mode::Worn)
                continue;

            item_type::item_type type = item->getType();
            int8_t armor = item->getEffectiveArmorLevel();
            //FIXME: this could be made nicer. Somehow.
            switch (item->getType())
            {
                case item_type::WEAPON:
                    //if(armor > b->Weapon.rating)
                    //{
                    b->Weapon.rating = armor;
                    b->Weapon.matt.type = item->getActualMaterial();
                    b->Weapon.matt.index = item->getActualMaterialIndex();
                    //}
                break;
                case item_type::ARMOR:
                    //if(armor > b->Weapon.rating)
                    //{
                    b->Armor.rating = armor;
                    b->Armor.matt.type = item->getActualMaterial();
                    b->Armor.matt.index = item->getActualMaterialIndex();
                    //}
                break;
                case item_type::SHOES:
                    //if(armor > b->Weapon.rating)
                    //{
                    b->Shoes.rating = armor;
                    b->Shoes.matt.type = item->getActualMaterial();
                    b->Shoes.matt.index = item->getActualMaterialIndex();
                    //}
                break;
                case item_type::SHIELD:
                    //if(armor > b->Weapon.rating)
                    //{
                    b->Shield.rating = armor;
                    b->Shield.matt.type = item->getActualMaterial();
                    b->Shield.matt.index = item->getActualMaterialIndex();
                    //}
                break;
                case item_type::HELM:
                    //if(armor > b->Weapon.rating)
                    //{
                    b->Helm.rating = armor;
                    b->Helm.matt.type = item->getActualMaterial();
                    b->Helm.matt.index = item->getActualMaterialIndex();
                    //}
                break;
                case item_type::GLOVES:
                    //if(armor > b->Weapon.rating)
                    //{
                    b->Gloves.rating = armor;
                    b->Gloves.matt.type = item->getActualMaterial();
                    b->Gloves.matt.index = item->getActualMaterialIndex();
                    //}
                break;
                default:
                    // something unexpected. Should we react at all?
                    break;
            }
        }
        // need a new tempcreature now
        // old tempcreature should be deleted when b is
        tempcreature = new t_unit;
    }
	delete(tempcreature); // there will be one left over
}


CreatureConfiguration *GetCreatureConfig( t_unit* c ){
	//find list for creature type
	vector<CreatureConfiguration>* creatureData;
	uint32_t num = (uint32_t)contentLoader->creatureConfigs.size();
	if (c->race >= num)
	{
		return NULL;	
	}
	creatureData = contentLoader->creatureConfigs[c->race];
	if (creatureData == NULL)
	{
		return NULL;
	}
	int rando = randomCube[c->x%RANDOM_CUBE][c->y%RANDOM_CUBE][c->z%RANDOM_CUBE];
	int offsetAnimFrame = (currentAnimationFrame + rando) % MAX_ANIMFRAME;

	num = (uint32_t)creatureData->size();
	for(uint32_t i=0; i < num; i++)
	{
		CreatureConfiguration *testConfig = &((*creatureData)[i]);

		bool creatureMatchesJob = true;
		if( testConfig->professionID != INVALID_INDEX )
		{
			creatureMatchesJob = testConfig->professionID == c->profession;
		}
		if(!creatureMatchesJob) continue;

		bool creatureMatchesSex = true;
		if( testConfig->sex != 0 )
		{
			creatureMatchesSex = 
				(c->sex == testConfig->sex-1);
		}
		if(!creatureMatchesSex) continue;

		bool creatureMatchesCaste = true;
		if( testConfig->caste != INVALID_INDEX )
		{
			creatureMatchesCaste = testConfig->caste == c->caste;
		}
		if(!creatureMatchesCaste) continue;

		bool creatureMatchesSpecial = true;
		if (testConfig->special != eCSC_Any)
		{
			if (testConfig->special == eCSC_Zombie && !c->flags1.bits.zombie) creatureMatchesSpecial = false;
			if (testConfig->special == eCSC_Skeleton && !c->flags1.bits.skeleton) creatureMatchesSpecial = false;
			if (testConfig->special == eCSC_Normal && (c->flags1.bits.zombie || c->flags1.bits.skeleton)) creatureMatchesSpecial = false;
		}
		if(!creatureMatchesSpecial) continue;

		if (!(testConfig->sprite.get_animframes() & (1 << offsetAnimFrame)))
			continue;

		// dont try to match strings until other tests pass
		if( testConfig->professionstr[0])
		{ //cant be NULL, so check has length
					creatureMatchesJob = (c->custom_profession == testConfig->professionstr);
		}
		if(!creatureMatchesJob) continue;

		return testConfig;
	}
	return NULL;
}


c_sprite* GetCreatureSpriteMap( t_unit* c )
{
	static c_sprite * defaultSprite = new c_sprite;
	defaultSprite->reset();
	defaultSprite->set_defaultsheet(IMGCreatureSheet);
	CreatureConfiguration *testConfig = GetCreatureConfig( c );
	if (testConfig == NULL)
		return defaultSprite;
	testConfig->sprite.set_defaultsheet(IMGCreatureSheet);
	return &(testConfig->sprite);
}

int GetCreatureShadowMap( t_unit* c )
{
	CreatureConfiguration *testConfig = GetCreatureConfig( c );
	if (testConfig == NULL)
		return 4;
	return testConfig->shadow;
}

void generateCreatureDebugString( t_unit* c, char* strbuffer){
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
