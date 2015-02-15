#include "common.h"
#include "Creatures.h"
#include "WorldSegment.h"
#include "CreatureConfiguration.h"
#include "ContentLoader.h"
#include "GUI.h"
#include "Tile.h"
#include "SpriteColors.h"
#include "DataDefs.h"
#include "df/world.h"
#include "df/unit.h"
#include "df/unit_soul.h"
#include "df/unit_skill.h"
#include "df/job.h"
#include "df/unit_inventory_item.h"
#include "df/item_constructed.h"
#include "df/itemimprovement.h"
#include "df/itemimprovement_threadst.h"

#include "df/profession.h"

#include "df/creature_raw.h"
#include "df/caste_raw.h"

#include "modules/Units.h"
#include "df/historical_entity.h"
#include "df/entity_position.h"

//vector<t_matgloss> v_creatureNames;
//vector<CreatureConfiguration> creatureTypes;

int32_t charToUpper(int32_t c)
{
    if(c >= 0x61 && c <= 0x7A) {
        return c-0x20;
    } else if(c >= 0xE0 && c <= 0xF6) {
        return c-0x20;
    } else if(c >= 0xF8 && c <= 0xFE) {
        return c-0x20;
    } else {
        return c;
    }
}

ALLEGRO_USTR* bufferToUstr(const char* buffer, int length)
{
    ALLEGRO_USTR* temp = al_ustr_new("");
    for (int i = 0; i < length; i++) {
        switch ((unsigned char)buffer[i])
        {
        case 0:
            i = length;
            break;
        case 1:
            al_ustr_append_chr(temp, 0x263A);
            break;
        case 2:
            al_ustr_append_chr(temp, 0x263B);
            break;
        case 3:
            al_ustr_append_chr(temp, 0x2665);
            break;
        case 4:
            al_ustr_append_chr(temp, 0x2666);
            break;
        case 5:
            al_ustr_append_chr(temp, 0x2663);
            break;
        case 6:
            al_ustr_append_chr(temp, 0x2660);
            break;
        case 7:
            al_ustr_append_chr(temp, 0x2022);
            break;
        case 8:
            al_ustr_append_chr(temp, 0x25D8);
            break;
        case 9:
            al_ustr_append_chr(temp, 0x25CB);
            break;
        case 10:
            al_ustr_append_chr(temp, 0x25D9);
            break;
        case 11:
            al_ustr_append_chr(temp, 0x2642);
            break;
        case 12:
            al_ustr_append_chr(temp, 0x2640);
            break;
        case 13:
            al_ustr_append_chr(temp, 0x266A);
            break;
        case 14:
            al_ustr_append_chr(temp, 0x266B);
            break;
        case 15:
            al_ustr_append_chr(temp, 0x263C);
            break;
        case 16:
            al_ustr_append_chr(temp, 0x25BA);
            break;
        case 17:
            al_ustr_append_chr(temp, 0x25C4);
            break;
        case 18:
            al_ustr_append_chr(temp, 0x2195);
            break;
        case 19:
            al_ustr_append_chr(temp, 0x203C);
            break;
        case 20:
            al_ustr_append_chr(temp, 0xB6);
            break;
        case 21:
            al_ustr_append_chr(temp, 0xA7);
            break;
        case 22:
            al_ustr_append_chr(temp, 0x25AC);
            break;
        case 23:
            al_ustr_append_chr(temp, 0x21A8);
            break;
        case 24:
            al_ustr_append_chr(temp, 0x2191);
            break;
        case 25:
            al_ustr_append_chr(temp, 0x2193);
            break;
        case 26:
            al_ustr_append_chr(temp, 0x2192);
            break;
        case 27:
            al_ustr_append_chr(temp, 0x2190);
            break;
        case 28:
            al_ustr_append_chr(temp, 0x221F);
            break;
        case 29:
            al_ustr_append_chr(temp, 0x2194);
            break;
        case 30:
            al_ustr_append_chr(temp, 0x25B2);
            break;
        case 31:
            al_ustr_append_chr(temp, 0x25BC);
            break;
        case 128:
            al_ustr_append_chr(temp, 0xC7);
            break;
        case 129:
            al_ustr_append_chr(temp, 0xFC);
            break;
        case 130:
            al_ustr_append_chr(temp, 0xE9);
            break;
        case 131:
            al_ustr_append_chr(temp, 0xE2);
            break;
        case 132:
            al_ustr_append_chr(temp, 0xE4);
            break;
        case 133:
            al_ustr_append_chr(temp, 0xE0);
            break;
        case 134:
            al_ustr_append_chr(temp, 0xE5);
            break;
        case 135:
            al_ustr_append_chr(temp, 0xE7);
            break;
        case 136:
            al_ustr_append_chr(temp, 0xEA);
            break;
        case 137:
            al_ustr_append_chr(temp, 0xEB);
            break;
        case 138:
            al_ustr_append_chr(temp, 0xE8);
            break;
        case 139:
            al_ustr_append_chr(temp, 0xEF);
            break;
        case 140:
            al_ustr_append_chr(temp, 0xEE);
            break;
        case 141:
            al_ustr_append_chr(temp, 0xC4);
            break;
        case 142:
            al_ustr_append_chr(temp, 0xC5);
            break;
        case 143:
            al_ustr_append_chr(temp, 0xC9);
            break;
        case 144:
            al_ustr_append_chr(temp, 0xFC);
            break;
        case 145:
            al_ustr_append_chr(temp, 0xE9);
            break;
        case 146:
            al_ustr_append_chr(temp, 0xC6);
            break;
        case 147:
            al_ustr_append_chr(temp, 0xF4);
            break;
        case 148:
            al_ustr_append_chr(temp, 0xF6);
            break;
        case 149:
            al_ustr_append_chr(temp, 0xF2);
            break;
        case 150:
            al_ustr_append_chr(temp, 0xFB);
            break;
        case 151:
            al_ustr_append_chr(temp, 0xF9);
            break;
        case 152:
            al_ustr_append_chr(temp, 0xFF);
            break;
        case 153:
            al_ustr_append_chr(temp, 0xD6);
            break;
        case 154:
            al_ustr_append_chr(temp, 0xDC);
            break;
        case 160:
            al_ustr_append_chr(temp, 0xE1);
            break;
        case 161:
            al_ustr_append_chr(temp, 0xED);
            break;
        case 162:
            al_ustr_append_chr(temp, 0xF3);
            break;
        case 163:
            al_ustr_append_chr(temp, 0xFA);
            break;
        case 164:
            al_ustr_append_chr(temp, 0xF1);
            break;
        case 165:
            al_ustr_append_chr(temp, 0xD1);
            break;
        default:
            al_ustr_append_chr(temp, buffer[i]);
            break;
        }
    }
    return temp;
}

bool IsCreatureVisible( df::unit* c )
{
    if( ssConfig.show_all_creatures ) {
        return true;
    }

    if( c->flags1.bits.dead ) {
        return false;
    }
    if( c->flags1.bits.caged ) {
        return false;
    }
    if( c->flags1.bits.hidden_in_ambush ) {
        return false;
    }
    return true;
}


void AssembleCreature(int drawx, int drawy, SS_Unit* creature, Tile * b)
{
    c_sprite * sprite = GetCreatureSpriteMap( creature );
    if(sprite) {
        sprite->assemble_world(creature->x,creature->y, creature->z, b);
    } else {
        df::creature_raw *raw = df::global::world->raws.creatures.all[creature->race];
        int spritenum = raw->creature_tile;
        if(raw->caste[creature->caste]->caste_tile != 1) {
            spritenum = raw->caste[creature->caste]->caste_tile;
        }
        ALLEGRO_COLOR tilecolor = ssConfig.colors.getDfColor(DFHack::Units::getCasteProfessionColor(creature->race,creature->caste,(df::profession)creature->profession), ssConfig.useDfColors);
        int sheetx = spritenum % LETTERS_OBJECTSWIDE;
        int sheety = spritenum / LETTERS_OBJECTSWIDE;
        b->AssembleSprite(
            IMGLetterSheet,
            premultiply(b ? shadeAdventureMode(tilecolor, b->fog_of_war, b->designation.bits.outside) : tilecolor),
            sheetx * SPRITEWIDTH,
            sheety * SPRITEHEIGHT,
            SPRITEWIDTH,
            SPRITEHEIGHT,
            drawx,
            drawy - (WALLHEIGHT)*ssConfig.scale,
            SPRITEWIDTH*ssConfig.scale,
            SPRITEHEIGHT*ssConfig.scale,
            0);
    }
}

void AssembleCreatureText(int drawx, int drawy, SS_Unit* creature, WorldSegment * seg){
    draw_event d = {CreatureText, creature, al_map_rgb(255,255,255), 0, 0, 0, 0, (float)drawx, (float)drawy, 0, 0, 0};
    seg->AssembleSprite(d);
}

void DrawCreatureText(int drawx, int drawy, SS_Unit* creature )
{
    vector<int> statusIcons;

    //if(ssConfig.show_creature_happiness)
    if(ssConfig.show_creature_moods && df::creature_raw::find(creature->race)->caste[creature->caste]->flags.is_set(caste_raw_flags::CAN_SPEAK)) {
        if(creature->stress_level <= 0) {
            statusIcons.push_back(0);
        } else if(creature->stress_level >= 1 && creature->stress_level <= 30000) {
            statusIcons.push_back(1);
        } else if(creature->stress_level >= 30001 && creature->stress_level <= 60000) {
            statusIcons.push_back(2);
        } else if(creature->stress_level >= 60001 && creature->stress_level <= 100000) {
            statusIcons.push_back(3);
        } else if(creature->stress_level >= 100001 && creature->stress_level <= 250000) {
            statusIcons.push_back(4);
        } else if(creature->stress_level >= 250001 && creature->stress_level <= 500000) {
            statusIcons.push_back(5);
        } else if(creature->stress_level >= 500001) {
            statusIcons.push_back(6);
        }

        if(creature->mood == 0) {
            statusIcons.push_back(19);
        } else if(creature->mood == 1) {
            statusIcons.push_back(19);
        } else if(creature->mood == 2) {
            statusIcons.push_back(21);
        } else if(creature->mood == 3) {
            statusIcons.push_back(19);
        } else if(creature->mood == 4) {
            statusIcons.push_back(19);
        } else if(creature->mood == 5) {
            statusIcons.push_back(18);
        } else if(creature->mood == 6) {
            statusIcons.push_back(18);
        }

        if(creature->current_job.active && creature->current_job.jobType == 21) {
            statusIcons.push_back(16);
        } else if(creature->current_job.active && creature->current_job.jobType == 52) {
            statusIcons.push_back(17);
        }
    }

    unsigned int offsety = 0;

    if(ssConfig.show_creature_jobs && creature->current_job.active) {
        df::job_type jtype = (df::job_type) creature->current_job.jobType;

        const char* jname = ENUM_ATTR(job_type,caption,jtype);

        //CAN'T DO THIS UNTIL DFHack t_job IMPORTS MATERIAL TYPE???
        //df::job_skill jskill = ENUM_ATTR(job_type,skill,jtype);
        //if(jskill == job_skill::NONE){
        //    check for material type of t_job
        //}
        //ALLEGRO_COLOR textcol;
        //if(jskill != job_skill::NONE) {
        //    const char* jprofname = ENUM_ATTR(job_skill,caption,jskill);
        //    textcol = ssConfig.colors.getDfColor(
        //        DFHack::Units::getCasteProfessionColor(
        //        creature->race,creature->caste,ENUM_ATTR(
        //        job_skill,profession,jskill
        //        )));
        //    draw_textf_border(font, textcol, drawx, drawy-((WALLHEIGHT*ssConfig.scale)+al_get_font_line_height(font) + offsety), 0,
        //        "%s (%s)", jname, jprofname );
        //} else {
        //    textcol = al_map_rgb(255,255,255);
        //    draw_textf_border(font, textcol, drawx, drawy-((WALLHEIGHT*ssConfig.scale)+al_get_font_line_height(font) + offsety), 0,
        //        "%s", jname );
        //}

        //go all kinds of crazy if it is a strange mood
        ALLEGRO_COLOR textcol = ENUM_ATTR(job_type,type,jtype) == df::job_type_class::StrangeMood
            ? blinkTechnicolor() : al_map_rgb(255,255,255);
        draw_textf_border(font, textcol, drawx, drawy-((WALLHEIGHT*ssConfig.scale)+al_get_font_line_height(font) + offsety), 0,
            "%s", jname );
    }

    offsety += (ssConfig.show_creature_jobs&&creature->current_job.active) ? al_get_font_line_height(font) : 0;

    if( ssConfig.show_creature_names ) {
        ALLEGRO_COLOR textcol;
        if(ssConfig.show_creature_professions == 2) {
            textcol = ssConfig.colors.getDfColor(DFHack::Units::getProfessionColor(creature->origin), ssConfig.useDfColors);
            //stupid hack to get legendary status of creatures
            if(creature->isLegend) {
                ALLEGRO_COLOR altcol;
                //military flash dark, civilians flash light
                if(ENUM_ATTR(profession,military,(df::profession)creature->profession)) {
                    altcol = partialBlend(textcol,al_map_rgb(0,0,0),25);
                } else {
                    altcol = partialBlend(textcol,al_map_rgb(255,255,255),50);
                }
                textcol = blink(textcol, altcol);
            }
        } else {
            textcol = al_map_rgb(255,255,255);
        }

        if (creature->name.nickname[0] && ssConfig.names_use_nick) {
            draw_textf_border(font, textcol, drawx, drawy-((WALLHEIGHT*ssConfig.scale)+al_get_font_line_height(font) + offsety), 0,
                              "%s", creature->name.nickname );
        }
        else if (creature->name.first_name[0])
        {
            char buffer[128];
            strncpy(buffer,creature->name.first_name,127);
            buffer[127]=0;
            ALLEGRO_USTR* temp = bufferToUstr(buffer, 128);
            al_ustr_set_chr(temp, 0, charToUpper(al_ustr_get(temp, 0)));
            draw_ustr_border(font, textcol, drawx, drawy-((WALLHEIGHT*ssConfig.scale)+al_get_font_line_height(font) + offsety), 0,
                temp );
            al_ustr_free(temp);
        }
        else if (ssConfig.names_use_species)
        {
            if(!ssConfig.skipCreatureTypes)
            {
                char buffer[128];
                strncpy(buffer,df::global::world->raws.creatures.all[creature->race]->caste[creature->caste]->caste_name[0].c_str(),127);
                buffer[127]=0;
                ALLEGRO_USTR* temp = bufferToUstr(buffer, 128);
                int32_t lastChar = ' ';
                for(int i = 0; al_ustr_get(temp, i) > 0; i++)
                {
                    if(lastChar == ' ')
                        al_ustr_set_chr(temp, i, charToUpper(al_ustr_get(temp, i)));
                    lastChar = al_ustr_get(temp, i);

                }
                draw_ustr_border(font, textcol, drawx, drawy-((WALLHEIGHT*ssConfig.scale)+al_get_font_line_height(font) + offsety), 0,
                temp);
                al_ustr_free(temp);
            }
        }
    }

    offsety += ssConfig.show_creature_names ? al_get_font_line_height(font) : 0;

    if(statusIcons.size()) {
        for(int i = 0; i < statusIcons.size(); i++) {
            unsigned int sheetx = 16 * (statusIcons[i] % 7);
            unsigned int sheety = 16 * (statusIcons[i] / 7);
            al_draw_bitmap_region(IMGStatusSheet, sheetx, sheety, 16, 16, drawx - (statusIcons.size()*8) + (16*i) + (SPRITEWIDTH*ssConfig.scale/2), drawy - (16 + WALLHEIGHT*ssConfig.scale + offsety), 0);
        }
    }

    offsety += ssConfig.show_creature_moods ? 16 : 0;

    if(ssConfig.show_creature_professions == 1) {
        unsigned int sheetx = 16 * (creature->profession % 7);
        unsigned int sheety = 16 * (creature->profession / 7);
        al_draw_bitmap_region(IMGProfSheet, sheetx, sheety, 16, 16, drawx -8 + (SPRITEWIDTH*ssConfig.scale/2), drawy - (16 + WALLHEIGHT*ssConfig.scale + offsety), 0);
    }
}

using df::global::world;

/**
 * Tries to figure out if the unit is a legend.
 */
bool hasLegendarySkill(df::unit * source){

    if(!source) {
        return false;
    }
    if(source->status.souls.size() <= 0) {
        return false;
    }
    df::unit_soul* soul = source->status.souls[0];
    if(!soul) {
        return false;
    }
    if(soul->skills.size() <= 0) {
        return false;
    }
    for(int i=0; i<soul->skills.size(); i++) {
        if(soul->skills[i] && soul->skills[i]->rating >= df::skill_rating::Legendary) {
            return true;
        }
    }
    //I feel dirty
    return false;
}

/**
 * Makes a copy of a DF creature for stonesense to use.
 *
 * If somebody feels like maintaining the DFHack version,
 * then we won't need to have our own written here >:(
 */
void copyCreature(df::unit * source, SS_Unit & furball)
{
    // read pointer from vector at position
    furball.origin = source;

    //read creature from memory
    // name
    Translation::readName(furball.name, &source->name);

    // basic stuff
    furball.id = source->id;
    furball.x = source->pos.x;
    furball.y = source->pos.y;
    furball.z = source->pos.z;
    furball.race = source->race;
    furball.civ = source->civ_id;
    furball.sex = source->sex;
    furball.caste = source->caste;
    furball.flags1.whole = source->flags1.whole;
    furball.flags2.whole = source->flags2.whole;
    furball.flags3.whole = source->flags3.whole;
    // custom profession
    furball.custom_profession = source->custom_profession;
    // profession
    furball.profession = source->profession;
    //figure out legendary status
    furball.isLegend = hasLegendarySkill(source);
    // stress level
    furball.stress_level = source->status.current_soul->personality.stress_level;
    // physical attributes
    memcpy(&furball.strength, source->body.physical_attrs, sizeof(source->body.physical_attrs));

    // mood stuff
    furball.mood = source->mood;
    furball.mood_skill = source->job.mood_skill;
    Translation::readName(furball.artifact_name, &source->status.artifact_name);

    // labors
    memcpy(&furball.labors, &source->status.labors, sizeof(furball.labors));

    furball.birth_year = source->relations.birth_year;
    furball.birth_time = source->relations.birth_time;
    furball.pregnancy_timer = source->relations.pregnancy_timer;
    // appearance
    furball.nbcolors = source->appearance.colors.size();
    if(furball.nbcolors>MAX_COLORS)
        furball.nbcolors = MAX_COLORS;
    // hair
    for(int i = 0; i < hairtypes_end; i++){
        furball.hairlength[i] = 1001;//default to long unkempt hair
        furball.hairstyle[i] = CLEAN_SHAVEN;
    }
    if(source->race < contentLoader->style_indices.size() && contentLoader->style_indices.at(source->race)){
        if(source->caste < contentLoader->style_indices.at(source->race)->size() && contentLoader->style_indices.at(source->race)->at(source->caste)){
            for(int i = 0; i < source->appearance.tissue_style_type.size(); i++){
                for(int j = 0; j < contentLoader->style_indices.at(source->race)->at(source->caste)->size();j++){
                    if(source->appearance.tissue_style_type[i] == contentLoader->style_indices.at(source->race)->at(source->caste)->at(j)){
                        furball.hairlength[j] = source->appearance.tissue_length[i];
                        furball.hairstyle[j] = (hairstyles)source->appearance.tissue_style[i];
                    }
                }
            }
        }
    }

    for(uint32_t i = 0; i < furball.nbcolors; i++) {
        furball.color[i] = source->appearance.colors[i];
    }
    df::job* unit_job = source->job.current_job;
    if(unit_job == NULL) {
        furball.current_job.active = false;
    } else {
        furball.current_job.active = true;
        furball.current_job.jobType = unit_job->job_type;
        furball.current_job.jobId = unit_job->id;
    }

    std::vector<Units::NoblePosition> np;
    if(Units::getNoblePositions(&np, source)){
        furball.profession = contentLoader->position_Indices[np[0].entity->id]->at(np[0].position->id);
    }


    furball.inv = NULL;
}

void ReadCreaturesToSegment( DFHack::Core& DF, WorldSegment* segment)
{
    if(ssConfig.skipCreatures) {
        return;
    }

    if(world->units.active.size() <= 0) {
        return;
    }

    df::unit *unit_ptr = 0;
    for(uint32_t index=0; index<world->units.active.size(); index++) {
        unit_ptr = world->units.active[index];

        if(!segment->CoordinateInsideSegment(unit_ptr->pos.x,unit_ptr->pos.y,unit_ptr->pos.z)
            || (!IsCreatureVisible(unit_ptr) && !ssConfig.show_all_creatures)){
                continue;
        }

        Tile* b = segment->getTile(unit_ptr->pos.x, unit_ptr->pos.y, unit_ptr->pos.z );
        if(!b) {
            b = segment->ResetTile(unit_ptr->pos.x, unit_ptr->pos.y, unit_ptr->pos.z, tiletype::OpenSpace);
            if(!b) {
                continue;
            }
        }

        // creature already there? SKIP.
        if(b->occ.bits.unit && b->creature) {
            continue;
        }

        // make a copy of some creature data
        SS_Unit * tempcreature = new SS_Unit();
        copyCreature(unit_ptr,*tempcreature);

        // add shadow to nearest floor tile
        for (int bz = tempcreature->z; bz>=0; bz--) {
            Tile * floor_tile = segment->getTile (tempcreature->x, tempcreature->y, bz );
            if (!floor_tile) {
                continue;
            }
            if (floor_tile->tileShapeBasic()==tiletype_shape_basic::Floor ||
                floor_tile->tileShapeBasic()==tiletype_shape_basic::Wall  ||
                floor_tile->tileShapeBasic()==tiletype_shape_basic::Ramp) {
                    // todo figure out appropriate shadow size
                    uint8_t tempShadow = GetCreatureShadowMap( tempcreature );
                    if (floor_tile->shadow < tempShadow) {
                        floor_tile->shadow=tempShadow;
                    }
                    break;
            }
        }
        for (auto iter = unit_ptr->inventory.begin(); iter != unit_ptr->inventory.end(); iter++) {
            df::unit_inventory_item * itemslot = *iter;
            // skip if invalid
            if(!itemslot) {
                continue;
            }

            df::item * item = itemslot->item;
            // skip if no item associated with item slot
            if(!item) {
                continue;
            }

            // skip if not weapon or worn item
            if(itemslot->mode != df::unit_inventory_item::T_mode::Weapon &&
                itemslot->mode != df::unit_inventory_item::T_mode::Worn &&
                itemslot->mode != df::unit_inventory_item::T_mode::Piercing) {
                    continue;
            }

            //If there's no subtype, mae it 0, and let DFhack handle it.
            int subtype = item->getSubtype();
            if(subtype < 0) subtype = 0;

            item_type::item_type type = item->getType();
            int8_t armor = item->getEffectiveArmorLevel();

            worn_item equipment;

            equipment.matt.type = item->getActualMaterial();
            equipment.matt.index = item->getActualMaterialIndex();

            if(item->isDyed()) {
                auto Constructed_Item = virtual_cast<df::item_constructed>(item);
                if(Constructed_Item) {
                    for(int idex = 0; idex < Constructed_Item->improvements.size(); idex++) {
                        if(!Constructed_Item->improvements[idex]) {
                            continue;
                        }
                        if(Constructed_Item->improvements[idex]->getType() != improvement_type::THREAD) {
                            continue;
                        }
                        auto Improvement_Thread = virtual_cast<df::itemimprovement_threadst>(Constructed_Item->improvements[idex]);
                        if(!Improvement_Thread) {
                            continue;
                        }
                        equipment.dyematt.type = Improvement_Thread->dye.mat_type;
                        equipment.dyematt.index = Improvement_Thread->dye.mat_index;
                    }
                }
            }

            //FIXME: this could be made nicer. Somehow
            if(!tempcreature->inv) {
                tempcreature->inv = new(unit_inventory);
            }
            if(tempcreature->inv->item.size() <= type) {
                tempcreature->inv->item.resize(type+1);
            }
            if(tempcreature->inv->item[type].size() <= subtype) {
                tempcreature->inv->item[type].resize(subtype+1);
            }
            tempcreature->inv->item[type][subtype].push_back(equipment);
        }

        b->occ.bits.unit = true;
        b->creature = tempcreature;
        segment->PushUnit(tempcreature);
    }
}


CreatureConfiguration *GetCreatureConfig( SS_Unit* c )
{
    //find list for creature type
    vector<CreatureConfiguration>* creatureData;
    uint32_t num = (uint32_t)contentLoader->creatureConfigs.size();
    if (c->race >= num) {
        return NULL;
    }
    creatureData = contentLoader->creatureConfigs[c->race];
    if (creatureData == NULL) {
        return NULL;
    }
    int rando = randomCube[c->x%RANDOM_CUBE][c->y%RANDOM_CUBE][c->z%RANDOM_CUBE];
    int offsetAnimFrame = (currentAnimationFrame + rando) % MAX_ANIMFRAME;

    num = (uint32_t)creatureData->size();
    for(uint32_t i=0; i < num; i++) {
        CreatureConfiguration *testConfig = &((*creatureData)[i]);

        bool creatureMatchesJob = true;
        if( testConfig->professionID != INVALID_INDEX ) {
            creatureMatchesJob = testConfig->professionID == c->profession;
        }
        if(!creatureMatchesJob) {
            continue;
        }

        bool creatureMatchesSex = true;
        if( testConfig->sex != 0 ) {
            creatureMatchesSex =
                (c->sex == testConfig->sex-1);
        }
        if(!creatureMatchesSex) {
            continue;
        }

        bool creatureMatchesCaste = true;
        if( testConfig->caste != INVALID_INDEX ) {
            creatureMatchesCaste = testConfig->caste == c->caste;
        }
        if(!creatureMatchesCaste) {
            continue;
        }

        bool creatureMatchesSpecial = true;
        if (testConfig->special != eCSC_Any) {
            if (testConfig->special == eCSC_Zombie && !c->flags1.bits.zombie) {
                creatureMatchesSpecial = false;
            }
            if (testConfig->special == eCSC_Skeleton && !c->flags1.bits.skeleton) {
                creatureMatchesSpecial = false;
            }
            if (testConfig->special == eCSC_Ghost && !c->flags3.bits.ghostly) {
                creatureMatchesSpecial = false;
            }
            if (testConfig->special == eCSC_Military) {
                df::profession profession = (df::profession) c->profession;
                if(!ENUM_ATTR(profession,military,profession)) {
                    creatureMatchesSpecial = false;
                }
            }
            if (testConfig->special == eCSC_Normal && (c->flags1.bits.zombie || c->flags1.bits.skeleton)) {
                creatureMatchesSpecial = false;
            }
        }
        if(!creatureMatchesSpecial) {
            continue;
        }

        if (!(testConfig->sprite.get_animframes() & (1 << offsetAnimFrame))) {
            continue;
        }

        // dont try to match strings until other tests pass
        if( testConfig->professionstr[0]) {
            //cant be NULL, so check has length
            creatureMatchesJob = (c->custom_profession == testConfig->professionstr);
        }
        if(!creatureMatchesJob) {
            continue;
        }

        return testConfig;
    }
    return NULL;
}

c_sprite* GetCreatureSpriteMap( SS_Unit* c )
{
    CreatureConfiguration *testConfig = GetCreatureConfig( c );
    if (testConfig == NULL) {
        return NULL;
    }
    testConfig->sprite.set_defaultsheet(IMGCreatureSheet);
    return &(testConfig->sprite);
}

uint8_t GetCreatureShadowMap( SS_Unit* c )
{
    CreatureConfiguration *testConfig = GetCreatureConfig( c );
    if (testConfig == NULL) {
        return DEFAULT_SHADOW;
    }
    return testConfig->shadow;
}

void generateCreatureDebugString( SS_Unit* c, char* strbuffer)
{
    if(c->flags1.bits.active_invader) {
        strcat(strbuffer, "activeInvader ");
    }
    if(c->flags1.bits.caged) {
        strcat(strbuffer, "Caged ");
    }
    if(c->flags1.bits.chained) {
        strcat(strbuffer, "chained ");
    }
    if(c->flags1.bits.coward) {
        strcat(strbuffer, "coward ");
    }
    if(c->flags1.bits.dead) {
        strcat(strbuffer, "Dead ");
    }
    if(c->flags1.bits.diplomat) {
        strcat(strbuffer, "Diplomat ");
    }
    if(c->flags1.bits.drowning) {
        strcat(strbuffer, "drowning ");
    }
    if(c->flags1.bits.forest) {
        strcat(strbuffer, "lostLeaving ");
    }
    if(c->flags1.bits.fortress_guard) {
        strcat(strbuffer, "FortGuard ");
    }
    if(c->flags1.bits.had_mood) {
        strcat(strbuffer, "HadMood ");
    }
    if(c->flags1.bits.has_mood) {
        strcat(strbuffer, "Mood ");
    }
    if(c->flags1.bits.hidden_ambusher) {
        strcat(strbuffer, "hiddenAmbush ");
    }
    if(c->flags1.bits.hidden_in_ambush) {
        strcat(strbuffer, "hiddenInAmbush ");
    }
    if(c->flags1.bits.important_historical_figure) {
        strcat(strbuffer, "Historical ");
    }
    if(c->flags1.bits.incoming) {
        strcat(strbuffer, "Incoming ");
    }
    if(c->flags1.bits.invades) {
        strcat(strbuffer, "invading ");
    }
    if(c->flags1.bits.marauder) {
        strcat(strbuffer, "marauder ");
    }
    if(c->flags1.bits.merchant) {
        strcat(strbuffer, "merchant ");
    }
    if(c->flags1.bits.on_ground) {
        strcat(strbuffer, "onGround ");
    }
    if(c->flags1.bits.projectile) {
        strcat(strbuffer, "projectile ");
    }
    if(c->flags1.bits.ridden) {
        strcat(strbuffer, "ridden ");
    }
    if(c->flags1.bits.royal_guard) {
        strcat(strbuffer, "RoyGuard ");
    }
    if(c->flags1.bits.skeleton) {
        strcat(strbuffer, "Skeleton ");
    }
    if(c->flags1.bits.tame) {
        strcat(strbuffer, "Tame ");
    }
    if(c->flags1.bits.zombie) {
        strcat(strbuffer, "Zombie ");
    }

    if(c->flags2.bits.slaughter) {
        strcat(strbuffer, "ReadyToSlaughter ");
    }
    if(c->flags2.bits.resident) {
        strcat(strbuffer, "Resident ");
    }
    if(c->flags2.bits.sparring) {
        strcat(strbuffer, "Sparring ");
    }
    if(c->flags2.bits.swimming) {
        strcat(strbuffer, "Swimming ");
    }
    if(c->flags2.bits.underworld) {
        strcat(strbuffer, "Underworld ");
    }

    //if(c->flags1.bits.can_swap)
    //  strcat(strbuffer, "canSwap ");
    //if(c->flags1.bits.check_flows)
    //  strcat(strbuffer, "checFlows ");
    //if(c->flags1.bits.invader_origin)
    //  strcat(strbuffer, "invader_origin ");

}
