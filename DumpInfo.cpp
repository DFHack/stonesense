#pragma once
/*
Here'a a good place to put textdump methods used as utilities.  

Nothing here is sacred, feel free to stomp all over whatever you want, 
but be aware that others are going to do the same.  Nothing here should 
be intended for use by people not interested in development.  
*/
#include <fstream>
#include "common.h"
#include "ContentLoader.h"
#include "ContentBuildingReader.h"
#include "DumpInfo.h"
#include "MapLoading.h"
#include "ColorConfiguration.h"

#include "tinyxml.h"
#include "GUI.h"

#include "df/world_raws.h"
#include "df/itemdef.h"
#include "df/itemdef_weaponst.h"
#include "df/itemdef_armorst.h"
#include "df/itemdef_shoesst.h"
#include "df/itemdef_shieldst.h"
#include "df/itemdef_helmst.h"
#include "df/itemdef_glovesst.h"
#include "df/itemdef_pantsst.h"

//void DumpMaterialNamesToDisk(vector<t_matgloss> material, const char* filename){
//	FILE* fp = fopen(filename, "w");
//	if(!fp) return;
//	for(uint32_t j=0; j < material.size(); j++){
//		fprintf(fp, "%i:%s\n",j, material[j].id.c_str());
//	}
//	fclose(fp);
//}
//
//void DumpPrefessionNamesToDisk(vector<string> material, const char* filename){
//	FILE* fp = fopen(filename, "w");
//	if(!fp) return;
//	for(uint32_t j=0; j < material.size(); j++){
//		fprintf(fp, "%i:%s\n",j, material[j].c_str());
//	}
//	fclose(fp);
//}
//
//void DumpInorganicNamesToDisk(vector<t_matglossInorganic> material, const char* filename){
//	FILE* fp = fopen(filename, "w");
//	if(!fp) return;
//	for(uint32_t j=0; j < material.size(); j++){
//		fprintf(fp, "%i:%s\n",j, material[j].id.c_str());
//	}
//	fclose(fp);
//}
//
//void DumpCreatureNamesToDisk(){
//	/*FILE* fp = fopen("dump.txt", "w");
//	if(!fp) return;
//	for(uint32_t j=0; j < v_creatureNames.size(); j++){
//	fprintf(fp, "%i:%s\n",j, v_creatureNames[j].id);
//	}
//	fclose(fp);*/
//}
//void DumpProfessionsToDisk(){
//	FILE* fp = fopen("dump.txt", "w");
//	if(!fp) return;
//	string proffStr;
//	for(int j=0; (proffStr = contentLoader->professionStrings[j]) != "" ; j++){
//		fprintf(fp, "%i:%s\n",j, proffStr.c_str());
//	}
//	fclose(fp);
//}

void DumpItemNamesToDisk(const char* filename)
{
	df::world_raws::T_itemdefs &defs = df::global::world->raws.itemdefs;
	FILE* fp = fopen(filename, "w");
	if(!fp) return;
	fprintf(fp, "WEAPON:\n");
	for(uint32_t j=0; j < defs.weapons.size(); j++){
		fprintf(fp, "%i:%s\n",j, defs.weapons[j]->id.c_str());
	}
	fprintf(fp, "ARMOR:\n");
	for(uint32_t j=0; j < defs.armor.size(); j++){
		fprintf(fp, "%i:%s\n",j, defs.armor[j]->id.c_str());
	}
	fprintf(fp, "SHOES:\n");
	for(uint32_t j=0; j < defs.shoes.size(); j++){
		fprintf(fp, "%i:%s\n",j, defs.shoes[j]->id.c_str());
	}
	fprintf(fp, "SHIELD:\n");
	for(uint32_t j=0; j < defs.shields.size(); j++){
		fprintf(fp, "%i:%s\n",j, defs.shields[j]->id.c_str());
	}
	fprintf(fp, "HELM:\n");
	for(uint32_t j=0; j < defs.helms.size(); j++){
		fprintf(fp, "%i:%s\n",j, defs.helms[j]->id.c_str());
	}
	fprintf(fp, "GLOVES:\n");
	for(uint32_t j=0; j < defs.gloves.size(); j++){
		fprintf(fp, "%i:%s\n",j, defs.gloves[j]->id.c_str());
	}
	fprintf(fp, "PANTS:\n");
	for(uint32_t j=0; j < defs.pants.size(); j++){
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
	FOR_ENUM_ITEMS(tiletype, tt){
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
	FOR_ENUM_ITEMS(tiletype, tt){
			if(tileShapeBasic(tileShape(tt)) == shape){
			name = tileName(tt);
			fprintf(fp, "\t<!--%s--> \n\t<terrain value=%i/> \n", name, j);
		}
		j++;
	}
	fclose(fp);
}

df::tiletype_shape_basic GetBasicShape(string & shapeName){
		if(shapeName == "None")
			return tiletype_shape_basic::None;
		if(shapeName == "Open")
			return tiletype_shape_basic::Open;
		if(shapeName == "Floor")
			return tiletype_shape_basic::Floor;
		if(shapeName == "Wall")
			return tiletype_shape_basic::Wall;
		if(shapeName == "Stair")
			return tiletype_shape_basic::Stair;
		return tiletype_shape_basic::None;
}

void DumpInfo(color_ostream & out, std::vector<std::string> & params){
	ContentLoader * contentLoader = new ContentLoader();
	string & p1 = params[0];
	if(p1 == "dumpitems"){
		out.print("dumping equippable item names to 'itemdump.txt'...\n");
		DumpItemNamesToDisk("itemdump.txt");
		out.print("...done\n");
	}
	else if(p1 == "dumptiles"){
		out.print("dumping equippable item names to 'tiledump.txt'...\n");
		DumpTileTypes("tiledump.txt");
		out.print("...done\n");
	}
	else if(p1 == "genterrain"){
		if(params.size() > 1){
			out.print("generating 'terrain.xml'...\n");
			GenerateTerrainXml("terrain.xml", params[1], GetBasicShape(params[1]));
			out.print("...done\n");
		}
		else
			out.printerr("invalid argument\n");
	}
	else
		out.printerr("invalid argument\n");

	free(contentLoader);
}