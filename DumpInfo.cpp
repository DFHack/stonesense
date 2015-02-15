/*
Here'a a good place to put textdump methods used as utilities.

Nothing here is sacred, feel free to stomp all over whatever you want,
but be aware that others are going to do the same.  Nothing here should
be intended for use by people not interested in development.
*/
#include <fstream>
#include "common.h"
#include "ContentLoader.h"
//#include "ContentBuildingReader.h"
//#include "DumpInfo.h"
//#include "MapLoading.h"
//#include "ColorConfiguration.h"
//#include "GUI.h"

#include "tinyxml.h"

//#include "df/world_raws.h"
#include "df/itemdef.h"
#include "df/itemdef_weaponst.h"
#include "df/itemdef_armorst.h"
#include "df/itemdef_shoesst.h"
#include "df/itemdef_shieldst.h"
#include "df/itemdef_helmst.h"
#include "df/itemdef_glovesst.h"
#include "df/itemdef_pantsst.h"

//FIXME: filled with black magic
void dumpSegment()
{
    return;
    /*
    al_lock_mutex(ssConfig.readMutex);

    //back up all the relevant values
    Crd3D tempSize = ssState.Size;
    int tempViewx = Position.x;
    int tempViewy = Position.y;
    bool tempFollow = ssConfig.follow_DFscreen;
    int tempLift = ssConfig.lift_segment_offscreen_y;
    int currentFlags = al_get_new_bitmap_flags();
    //now make them real big.
    ssConfig.follow_DFscreen = false;
    ssConfig.lift_segment_offscreen_y = 0;
    parms.x = 0;
    parms.y = 0;
    parms.z = ssState.RegionDim.z - 1;
    parms.sizex = ssState.RegionDim.x;
    parms.sizey = ssState.RegionDim.y;
    parms.sizez = ssState.RegionDim.z;

    read_segment(NULL);
    //get filename
    char filename[20] ={0};
    FILE* fp;
    int index = 1;
    //search for the first screenshot# that does not exist already
    while(true){
        sprintf(filename, "screenshot%i.png", index);

        fp = fopen(filename, "r");
        if( fp != 0)
            fclose(fp);
        else
            //file does not exist, so exit loop
            break;
        index++;
    };
    int tempFlags = al_get_new_bitmap_flags();
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);

    ALLEGRO_BITMAP * volume = al_create_bitmap(segment->sizex, (segment->sizez * segment->sizey));
    if(!volume)
    {
        DFConsole->printerr("Failed to create volumetric image.");
        al_set_new_bitmap_flags(tempFlags);
        return;
    }

    ALLEGRO_BITMAP * backup = al_get_target_bitmap();
    al_set_target_bitmap(volume);

    int op, src, dst, alpha_op, alpha_src, alpha_dst;
    al_get_separate_blender(&op, &src, &dst, &alpha_op, &alpha_src, &alpha_dst);
    al_set_separate_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO,ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
    al_clear_to_color(al_map_rgba(0,0,0,0));
    al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst);
    viewedSegment->drawPixels();

    al_save_bitmap(filename, volume);
    al_destroy_bitmap(volume);

    al_set_target_bitmap(backup);
    al_set_new_bitmap_flags(tempFlags);

    //restore everything that we changed.
    ssState.Size = tempSize;
    Position.x = tempViewx;
    Position.y = tempViewy;
    ssConfig.follow_DFscreen = tempFollow;
    ssConfig.lift_segment_offscreen_y = tempLift;

    al_unlock_mutex(ssConfig.readMutex);
    */
}

//void DumpMaterialNamesToDisk(vector<t_matgloss> material, const char* filename){
//    FILE* fp = fopen(filename, "w");
//    if(!fp) return;
//    for(uint32_t j=0; j < material.size(); j++){
//        fprintf(fp, "%i:%s\n",j, material[j].id.c_str());
//    }
//    fclose(fp);
//}
//
//void DumpPrefessionNamesToDisk(vector<string> material, const char* filename){
//    FILE* fp = fopen(filename, "w");
//    if(!fp) return;
//    for(uint32_t j=0; j < material.size(); j++){
//        fprintf(fp, "%i:%s\n",j, material[j].c_str());
//    }
//    fclose(fp);
//}
//
//void DumpInorganicNamesToDisk(vector<t_matglossInorganic> material, const char* filename){
//    FILE* fp = fopen(filename, "w");
//    if(!fp) return;
//    for(uint32_t j=0; j < material.size(); j++){
//        fprintf(fp, "%i:%s\n",j, material[j].id.c_str());
//    }
//    fclose(fp);
//}
//
//void DumpCreatureNamesToDisk(){
//    /*FILE* fp = fopen("dump.txt", "w");
//    if(!fp) return;
//    for(uint32_t j=0; j < v_creatureNames.size(); j++){
//    fprintf(fp, "%i:%s\n",j, v_creatureNames[j].id);
//    }
//    fclose(fp);*/
//}
//void DumpProfessionsToDisk(){
//    FILE* fp = fopen("dump.txt", "w");
//    if(!fp) return;
//    string proffStr;
//    for(int j=0; (proffStr = contentLoader->professionStrings[j]) != "" ; j++){
//        fprintf(fp, "%i:%s\n",j, proffStr.c_str());
//    }
//    fclose(fp);
//}

void DumpItemNamesToDisk(const char* filename)
{
    df::world_raws::T_itemdefs &defs = df::global::world->raws.itemdefs;
    FILE* fp = fopen(filename, "w");
    if(!fp) {
        return;
    }
    fprintf(fp, "WEAPON:\n");
    for(uint32_t j=0; j < defs.weapons.size(); j++) {
        fprintf(fp, "%i:%s\n",j, defs.weapons[j]->id.c_str());
    }
    fprintf(fp, "ARMOR:\n");
    for(uint32_t j=0; j < defs.armor.size(); j++) {
        fprintf(fp, "%i:%s\n",j, defs.armor[j]->id.c_str());
    }
    fprintf(fp, "SHOES:\n");
    for(uint32_t j=0; j < defs.shoes.size(); j++) {
        fprintf(fp, "%i:%s\n",j, defs.shoes[j]->id.c_str());
    }
    fprintf(fp, "SHIELD:\n");
    for(uint32_t j=0; j < defs.shields.size(); j++) {
        fprintf(fp, "%i:%s\n",j, defs.shields[j]->id.c_str());
    }
    fprintf(fp, "HELM:\n");
    for(uint32_t j=0; j < defs.helms.size(); j++) {
        fprintf(fp, "%i:%s\n",j, defs.helms[j]->id.c_str());
    }
    fprintf(fp, "GLOVES:\n");
    for(uint32_t j=0; j < defs.gloves.size(); j++) {
        fprintf(fp, "%i:%s\n",j, defs.gloves[j]->id.c_str());
    }
    fprintf(fp, "PANTS:\n");
    for(uint32_t j=0; j < defs.pants.size(); j++) {
        fprintf(fp, "%i:%s\n",j, defs.pants[j]->id.c_str());
    }
    fclose(fp);
}

void DumpTileTypes(const char* filename)
{
    FILE* fp = fopen(filename, "w");

    int j = 0;
    const char * name;
    // Run through until perfect match found or hit end.
    FOR_ENUM_ITEMS(tiletype, tt) {
        name = tileName(tt);
        fprintf(fp, "%i:%s\n", j, name);
        j++;
    }
    fclose(fp);
}

void GenerateTerrainXml(const char* filename, string & type, df::tiletype_shape_basic shape)
{
    FILE* fp = fopen(filename, "w");

    int j = 0;
    const char * name;
    fprintf(fp, "<!--%s-->\n", type.c_str());
    FOR_ENUM_ITEMS(tiletype, tt) {
        if(tileShapeBasic(tileShape(tt)) == shape) {
            name = tileName(tt);
            fprintf(fp, "\t<!--%s--> \n\t<terrain value=%i/> \n", name, j);
        }
        j++;
    }
    fclose(fp);
}

df::tiletype_shape_basic GetBasicShape(string & shapeName)
{
    if(shapeName == "None") {
        return tiletype_shape_basic::None;
    }
    if(shapeName == "Open") {
        return tiletype_shape_basic::Open;
    }
    if(shapeName == "Floor") {
        return tiletype_shape_basic::Floor;
    }
    if(shapeName == "Wall") {
        return tiletype_shape_basic::Wall;
    }
    if(shapeName == "Ramp") {
        return tiletype_shape_basic::Ramp;
    }
    if(shapeName == "Stair") {
        return tiletype_shape_basic::Stair;
    }
    return tiletype_shape_basic::None;
}

void DumpInfo(color_ostream & out, std::vector<std::string> & params)
{
    string & p1 = params[0];
    if(p1 == "dumpitems") {
        out.print("dumping equippable item names to 'itemdump.txt'...\n");
        DumpItemNamesToDisk("itemdump.txt");
        out.print("...done\n");
    } else if(p1 == "dumptiles") {
        out.print("dumping equippable item names to 'tiledump.txt'...\n");
        DumpTileTypes("tiledump.txt");
        out.print("...done\n");
    } else if(p1 == "genterrain") {
        if(params.size() > 1) {
            out.print("generating 'terrain.xml'...\n");
            GenerateTerrainXml("terrain.xml", params[1], GetBasicShape(params[1]));
            out.print("...done\n");
        } else {
            out.printerr("invalid argument\n");
        }
    } else {
        out.printerr("invalid argument\n");
    }

}