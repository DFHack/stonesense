#include <fstream>
#include "common.h"
#include "ContentLoader.h"
#include "ContentBuildingReader.h"
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

ContentLoader * contentLoader;



ContentLoader::ContentLoader(void) { }
ContentLoader::~ContentLoader(void)
{ 
	//flush content on exit
	flushBuildingConfig(&buildingConfigs);
	flushTerrainConfig(terrainFloorConfigs);
	flushTerrainConfig(terrainBlockConfigs);
	flushItemConfig(itemConfigs);
	flushCreatureConfig();
	colorConfigs.clear();
}

void DumpMaterialNamesToDisk(vector<t_matgloss> material, const char* filename){
	FILE* fp = fopen(filename, "w");
	if(!fp) return;
	for(uint32_t j=0; j < material.size(); j++){
		fprintf(fp, "%i:%s\n",j, material[j].id.c_str());
	}
	fclose(fp);
}

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

void DumpPrefessionNamesToDisk(vector<string> material, const char* filename){
	FILE* fp = fopen(filename, "w");
	if(!fp) return;
	for(uint32_t j=0; j < material.size(); j++){
		fprintf(fp, "%i:%s\n",j, material[j].c_str());
	}
	fclose(fp);
}
bool ContentLoader::Load(){
	/*draw_textf_border(font, 
	al_get_bitmap_width(al_get_target_bitmap())/2,
	al_get_bitmap_height(al_get_target_bitmap())/2,
	ALLEGRO_ALIGN_CENTRE, "Loading...");
	al_flip_display();*/
	//flush old config
	flushBuildingConfig(&buildingConfigs);
	flushTerrainConfig(terrainFloorConfigs);
	flushTerrainConfig(terrainBlockConfigs);
	flushItemConfig(itemConfigs);
    colorConfigs.clear();
	creatureConfigs.clear();
	treeConfigs.clear();
	shrubConfigs.clear();
	flushImgFiles();

	// This is an extra suspend/resume, but it only happens when reloading the config
	// ie not enough to worry about
	//DF.Suspend();
	////read data from DF
	//const vector<string> *tempClasses = DF.getMemoryInfo()->getClassIDMapping();
	//// make a copy for our use
	//classIdStrings = *tempClasses;

	try
	{
		Mats = Core::getInstance().getMaterials();
	}
	catch(exception &e)
	{
		LogError("DFhack exeption: %s\n", e.what());
	}
	if(!config.skipCreatureTypes)
	{
		try
		{
			Mats->ReadCreatureTypes();
		}
		catch(exception &e)
		{
			LogError("DFhack exeption: %s\n", e.what());
			config.skipCreatureTypes = true;
		}
	}
	if(!config.skipCreatureTypesEx)
	{
		try
		{
			Mats->ReadCreatureTypesEx();
		}
		catch(exception &e)
		{
			LogError("DFhack exeption: %s\n", e.what());
			config.skipCreatureTypesEx = true;
		}
	}
	if(!config.skipDescriptorColors)
	{
		try
		{
			Mats->ReadDescriptorColors();
		}
		catch(exception &e)
		{
			LogError("DFhack exeption: %s\n", e.what());
			config.skipDescriptorColors = true;
		}
	}
	if(!config.skipInorganicMats)
	{
		if(!Mats->CopyInorganicMaterials(this->inorganic))
		{
			LogError("Missing inorganic materials!\n");
			config.skipInorganicMats = true;
		}
	}
	if(!config.skipOrganicMats)
	{
		if(!Mats->CopyOrganicMaterials(this->organic))
		{
			LogError("Missing organic materials!\n");
			config.skipOrganicMats = true;
		}
	}
    Buildings::ReadCustomWorkshopTypes(custom_workshop_types);
	if(professionStrings.empty())
	{
		FOR_ENUM_ITEMS(profession, i)
		{
			if(i<0)
				continue;
			professionStrings.push_back(string(ENUM_KEY_STR(profession, i)));
		}
	}
	/*
	if(classIdStrings.empty())
	{
		for(int i = 0; ; i++)
		{
			string temp;
			if(!MemInfo->resolveClassIDToClassname(i, temp))
			{
				break;
			}
			classIdStrings.push_back(temp);
		}
	}
	*/

	//DumpPrefessionNamesToDisk(professionStrings, "priofessiondump.txt");
	//DumpPrefessionNamesToDisk(classIdStrings, "buildingdump.txt");
	//DumpMaterialNamesToDisk(inorganicMaterials, "DUMPSES.txt");
	//DumpMaterialNamesToDisk(Mats->race, "creaturedump.txt");
	DumpItemNamesToDisk("itemdump.txt");
	//DF.Resume();

	//Find what is obsidian
	contentLoader->obsidian = lookupMaterialIndex(INORGANIC, "OBSIDIAN");

	loadGraphicsFromDisk(); //these get destroyed when flushImgFiles is called.
    ALLEGRO_PATH * p = al_create_path("stonesense/index.txt");
    bool overallResult = parseContentIndexFile( al_path_cstr(p, ALLEGRO_NATIVE_PATH_SEP) );
    al_destroy_path(p);
	translationComplete = false;

	return true;
}

bool ContentLoader::reload_configs()
{
	flushBuildingConfig(&buildingConfigs);
	flushTerrainConfig(terrainFloorConfigs);
	flushTerrainConfig(terrainBlockConfigs);
	flushItemConfig(itemConfigs);
    colorConfigs.clear();
	creatureConfigs.clear();
	treeConfigs.clear();
	shrubConfigs.clear();
	grassConfigs.clear();
	flushImgFiles();

	loadGraphicsFromDisk(); //these get destroyed when flushImgFiles is called.
    ALLEGRO_PATH * p = al_create_path("stonesense/index.txt");
    bool overallResult = parseContentIndexFile( al_path_cstr(p, ALLEGRO_NATIVE_PATH_SEP) );
    al_destroy_path(p);

	return overallResult;
}

// takes a filename and the file referring to it, and makes a combined filename in
// HTML style: (ie "/something" is relative to the stonesense root, everything
// else is relative to the referrer)
// buffer must be FILENAME_BUFFERSIZE chars
// returns true if it all works
bool getLocalFilename(char * buffer, const char* filename, const char* relativeto)
{
	ALLEGRO_PATH * temppath;
	if (filename[0] == '/' || filename[0] == '\\')
	{
		temppath = al_create_path(filename);
		al_make_path_canonical(temppath);
	}
	else
	{
		temppath = al_create_path(relativeto);
		al_join_paths(temppath, al_create_path(filename));
		al_make_path_canonical(temppath);
	}
	buffer = strcpy(buffer, al_path_cstr(temppath, ALLEGRO_NATIVE_PATH_SEP));
	return true;
}

bool ContentLoader::parseContentIndexFile( const char* filepath )
{
	/*
	al_clear_to_color(al_map_rgb(0,0,0));
	draw_textf_border(font, al_get_bitmap_width(al_get_target_bitmap())/2, 
	al_get_bitmap_height(al_get_target_bitmap())/2,
	ALLEGRO_ALIGN_CENTRE, "Loading %s...", filepath);
	al_flip_display();
	*/
	string line;
	ifstream myfile( filepath );
	if (myfile.is_open() == false){
		LogError( "Unable to load config index file at: %s!\n", filepath );
		return false;
	}
	LogVerbose("Reading index at %s...\n", filepath);

	while ( !myfile.eof() )
	{
		char configfilepath[FILENAME_BUFFERSIZE] = {0};

		getline (myfile,line);

		// some systems don't remove the \r char as a part of the line change:
		// also trim trailing space
		int resize = (int)line.size()-1;
		for (;resize>0;resize--)
		{
			char test = line[resize];
			if (test == '\r')
				continue;
			if (test == '\t')
				continue;
			if (test == ' ')
				continue;
			break;
		}
		if (resize <= 0)
			continue;
		line.resize(resize+1);

		// allow comments
		if (line[0] == '#')
			continue;

		if (!getLocalFilename(configfilepath,line.c_str(),filepath))
		{
			LogError("File name parsing failed on %s\n",line.c_str());
			continue;
		}
		//WriteErr("but it's all fucked here: %s\n",configfilepath);
		ALLEGRO_PATH * temppath = al_create_path(configfilepath);
		const char* extension; 
		extension = al_get_path_extension(temppath);
		//WriteErr("extension: %s\n",extension);
		if (strcmp(extension,".xml") == 0)
		{
			LogVerbose("Reading xml %s...\n", configfilepath);
			if (!parseContentXMLFile(configfilepath))
				LogError("Failure in reading %s\n",configfilepath);
		}
		else if (strcmp(extension,".txt") == 0)
		{
			LogVerbose("Reading index %s...\n", configfilepath);
			if (!parseContentIndexFile(configfilepath))
				LogError("Failure in reading %s\n",configfilepath);
		}
		else
		{
			LogError("Invalid filename: %s\n",configfilepath);
		}
	}
	myfile.close();

	return true;
}

bool ContentLoader::parseContentXMLFile( const char* filepath ){
	/*
	al_clear_to_color(al_map_rgb(0,0,0));
	draw_textf_border(font, al_get_bitmap_width(al_get_target_bitmap())/2,
	al_get_bitmap_height(al_get_target_bitmap())/2,
	ALLEGRO_ALIGN_CENTRE, "Loading %s...", filepath);
	al_flip_display();*/
	TiXmlDocument doc( filepath );
	if(!doc.LoadFile())
	{
		LogError("File load failed: %s\n", filepath);
		return false;
	}
	TiXmlHandle hDoc(&doc);
	TiXmlElement* elemRoot;

	bool runningResult = true;
	elemRoot = hDoc.FirstChildElement().Element();
	while( elemRoot ){
		string elementType = elemRoot->Value();
		if( elementType.compare( "building" ) == 0 )
			runningResult &= parseBuildingContent( elemRoot );
		else if( elementType.compare( "creatures" ) == 0 )
			runningResult &= parseCreatureContent( elemRoot );
		else if( elementType.compare( "floors" ) == 0 )
			runningResult &= parseTerrainContent( elemRoot );
		else if( elementType.compare( "blocks" ) == 0 )
			runningResult &= parseTerrainContent( elemRoot );
		else if( elementType.compare( "shrubs" ) == 0 )
			runningResult &= parseShrubContent( elemRoot );
		else if( elementType.compare( "trees" ) == 0 )
			runningResult &= parseTreeContent( elemRoot );
		else if( elementType.compare( "grasses" ) == 0 )
			runningResult &= parseGrassContent( elemRoot );
		else if( elementType.compare( "colors" ) == 0 )
			runningResult &= parseColorContent( elemRoot );
		else if( elementType.compare( "fluids" ) == 0 )
			runningResult &= parseFluidContent( elemRoot );
		else if( elementType.compare( "items" ) == 0 )
			runningResult &= parseItemContent( elemRoot );
		else
			contentError("Unrecognised root element",elemRoot);

		elemRoot = elemRoot->NextSiblingElement();
	}

	return runningResult;
}


bool ContentLoader::parseBuildingContent(TiXmlElement* elemRoot ){
	return addSingleBuildingConfig( elemRoot, &buildingConfigs );
}

bool ContentLoader::parseCreatureContent(TiXmlElement* elemRoot ){
	return addCreaturesConfig( elemRoot, creatureConfigs );
}

bool ContentLoader::parseShrubContent(TiXmlElement* elemRoot ){
	return addSingleVegetationConfig( elemRoot, &shrubConfigs, organic );
}

bool ContentLoader::parseTreeContent(TiXmlElement* elemRoot ){
	return addSingleVegetationConfig( elemRoot, &treeConfigs, organic );
}

bool ContentLoader::parseGrassContent(TiXmlElement* elemRoot ){
	return addSingleVegetationConfig( elemRoot, &grassConfigs, organic );
}

bool ContentLoader::parseTerrainContent(TiXmlElement* elemRoot ){
	return addSingleTerrainConfig( elemRoot );
}

bool ContentLoader::parseColorContent(TiXmlElement* elemRoot ){
	return addSingleColorConfig( elemRoot );
}

bool ContentLoader::parseFluidContent(TiXmlElement* elemRoot ){
	return addSingleFluidConfig( elemRoot );
}

bool ContentLoader::parseItemContent(TiXmlElement* elemRoot ){
	return addSingleItemConfig( elemRoot );
}


const char* getDocument(TiXmlNode* element)
{
	//walk up the tree to the root
	TiXmlNode* parent = element->Parent();
	while (parent != null)
	{
		element = parent;
		parent = element->Parent();	
	}
	// topmost node *should* be a document, but lets be sure
	parent = dynamic_cast<TiXmlDocument*>(element);
	if (parent == NULL)
		return NULL;
	return parent->Value();
}

void contentError(const char* message, TiXmlNode* element)
{
	LogError("%s: %s: %s (Line %d)\n",getDocument(element),message,element->Value(),element->Row());
}

// converts list of characters 0-5 into bits, ignoring garbage
// eg  "035" or "0  3 5" or "0xx3x5" are all good
char getAnimFrames(const char* framestring)
{
	if (framestring == NULL)
		return ALL_FRAMES;
	char aframes=0;
	for (int i=0;i<6;i++)
	{
		if (framestring[i]==0)
			return aframes;
		char temp = framestring[i]-'0';
		if (temp < 0 || temp > 5)
			continue;
		aframes = aframes | (1 << temp);
	}
	return aframes;
}

int lookupMaterialIndex(int matType, const char* strValue)
{
    // for appropriate elements, look up subtype
    if (matType == INORGANIC && !config.skipInorganicMats)
    {
        return lookupIndexedType(strValue,contentLoader->inorganic);
    }
    else if (matType == WOOD && !config.skipOrganicMats)
    {
        return lookupIndexedType(strValue,contentLoader->organic);
    }
    else if (matType == PLANTCLOTH && !config.skipOrganicMats)
    {
        return lookupIndexedType(strValue,contentLoader->organic);
    }
    else if (matType == LEATHER && !config.skipCreatureTypes)
    {
        return lookupIndexedType(strValue,contentLoader->Mats->race);
    }
    else
    {
        //maybe allow some more in later
        return INVALID_INDEX;
    }
}

#include "df/siegeengine_type.h"
#include "df/workshop_type.h"
#include "df/trap_type.h"
#include "df/shop_type.h"
#include "df/construction_type.h"
#include "df/furnace_type.h"

const char *lookupBuildingSubtype(int main_type, int i)
{    
	// process types
	switch (main_type)
	{
	case building_type::Furnace:
		return ENUM_KEY_STR(furnace_type,(furnace_type::furnace_type)i).c_str();
	case building_type::Construction:
		return ENUM_KEY_STR(construction_type,(construction_type::construction_type)i).c_str();
	case building_type::SiegeEngine:
		return ENUM_KEY_STR(siegeengine_type,(siegeengine_type::siegeengine_type)i).c_str();
	case building_type::Shop:
		return ENUM_KEY_STR(shop_type,(shop_type::shop_type)i).c_str();
	case building_type::Workshop:
		return ENUM_KEY_STR(workshop_type,(workshop_type::workshop_type)i).c_str();
	default:
		return "NA";
	}
	return "NA";
}

const char *lookupMaterialTypeName(int matType)
{
	switch (matType)
	{
    case INORGANIC:
		return "Inorganic";
    case GREEN_GLASS:
		return "GreenGlass";
    case WOOD:
		return "Wood";
    case ICE:
		return "Ice";
    case CLEAR_GLASS:
		return "ClearGlass";
    case CRYSTAL_GLASS:
		return "CrystalGlass";
    case PLANTCLOTH:
		return "PlantCloth";
    case LEATHER:
		return "Leather";
	default:
		return NULL;
	}
}

MAT_BASICS lookupMaterialType(const char* strValue)
{
	if (strValue == NULL || strValue[0] == 0)
        return INVALID;
	else if( strcmp(strValue, "Stone") == 0)
        return INORGANIC;
	else if( strcmp(strValue, "Metal") == 0)
        return INORGANIC;
	else if( strcmp(strValue, "Inorganic") == 0)
        return INORGANIC;
	else if( strcmp(strValue, "GreenGlass") == 0)
        return GREEN_GLASS;
	else if( strcmp(strValue, "Wood") == 0)
        return WOOD;
	else if( strcmp(strValue, "Ice") == 0)
        return ICE;
	else if( strcmp(strValue, "ClearGlass") == 0)
        return CLEAR_GLASS;
	else if( strcmp(strValue, "CrystalGlass") == 0)
        return CRYSTAL_GLASS;
	else if( strcmp(strValue, "PlantCloth") == 0)
        return PLANTCLOTH;
	else if( strcmp(strValue, "Leather") == 0)
        return LEATHER;
	//TODO this needs fixing on dfhack side
    return INVALID;
}

const char *lookupMaterialName(int matType,int matIndex)
{
	if (matIndex < 0)
		return NULL;
	vector<t_matgloss>* typeVector;
	// for appropriate elements, look up subtype
	if ((matType == INORGANIC) && (!config.skipInorganicMats))
	{
        if(matIndex < contentLoader->inorganic.size())
        {
            return contentLoader->inorganic[matIndex].id.c_str();
        }
        else return NULL;
	}
	else if ((matType == WOOD) && (!config.skipOrganicMats))
	{
		typeVector=&(contentLoader->organic);
	}
	else if ((matType == PLANTCLOTH) && (!config.skipOrganicMats))
	{
		typeVector=&(contentLoader->organic);
	}
	else if (matType == LEATHER)
	{
		if(!config.skipCreatureTypes)
			typeVector=&(contentLoader->Mats->race);
	}
	else
	{
		//maybe allow some more in later
		return NULL;
	}
	if (matIndex >= typeVector->size())
		return NULL;
	return (*typeVector)[matIndex].id.c_str();
}

const char *lookupTreeName(int matIndex)
{
	if(config.skipOrganicMats)
		return NULL;
	if (matIndex < 0)
		return NULL;
	vector<t_matgloss>* typeVector;
	// for appropriate elements, look up subtype
	typeVector=&(contentLoader->organic);
	if (matIndex >= typeVector->size())
		return NULL;
	return (*typeVector)[matIndex].id.c_str();
}

const char * lookupFormName(int formType)
{
	switch (formType)
	{
    case item_type::BAR:
		return "bar";
    case item_type::BLOCKS:
		return "block";
    case item_type::BOULDER:
		return "boulder";
    case item_type::WOOD:
		return "log";
	default:
		return NULL;
	}
}

uint8_t lookupMaterialFore(int matType,int matIndex)
{
	if (matIndex < 0)
		return 0;
	vector<t_matgloss>* typeVector;
	//// for appropriate elements, look up subtype
	//if (matType == Mat_Wood)
	//{
	//	typeVector=&(contentLoader.woodNameStrings);
	//}
	//else if (matType == 0)
	//{
	//	typeVector=&(contentLoader.stoneNameStrings);
	//}
	//else if (matType == Mat_Metal)
	//{
	//	typeVector=&(contentLoader.metalNameStrings);
	//}
	//else
	//{
	//maybe allow some more in later
	return 0;
	//}
	if (matIndex >= typeVector->size())
		return 0;
	return (*typeVector)[matIndex].fore;
}

uint8_t lookupMaterialBack(int matType,int matIndex)
{
	if (matIndex < 0)
		return 0;
	vector<t_matgloss>* typeVector;
	//// for appropriate elements, look up subtype
	//if (matType == Mat_Wood)
	//{
	//	typeVector=&(contentLoader.woodNameStrings);
	//}
	//else if (matType == 0)
	//{
	//	typeVector=&(contentLoader.stoneNameStrings);
	//}
	//else if (matType == Mat_Metal)
	//{
	//	typeVector=&(contentLoader.metalNameStrings);
	//}
	//else
	//{
	//maybe allow some more in later
	return 0;
	//}
	if (matIndex >= typeVector->size())
		return 0;
	return (*typeVector)[matIndex].back;
}

uint8_t lookupMaterialBright(int matType,int matIndex)
{
	if (matIndex < 0)
		return 0;
	vector<t_matgloss>* typeVector;
	//// for appropriate elements, look up subtype
	//if (matType == Mat_Wood)
	//{
	//	typeVector=&(contentLoader.woodNameStrings);
	//}
	//else if (matType == 0)
	//{
	//	typeVector=&(contentLoader.stoneNameStrings);
	//}
	//else if (matType == Mat_Metal)
	//{
	//	typeVector=&(contentLoader.metalNameStrings);
	//}
	//else
	//{
	//maybe allow some more in later
	return 0;
	//}
	if (matIndex >= typeVector->size())
		return 0;
	return (*typeVector)[matIndex].bright;
}

int loadConfigImgFile(const char* filename, TiXmlElement* referrer)
{
	const char* documentRef = getDocument(referrer);
	char configfilepath[FILENAME_BUFFERSIZE] = {0};
	if (!getLocalFilename(configfilepath,filename,documentRef))
	{
		contentError("Failed to parse sprites filename",referrer);
		return -1;
	}
	return loadImgFile(configfilepath);
}

void ContentLoader::flushCreatureConfig()
{
	uint32_t num = (uint32_t)creatureConfigs.size();
	for ( int i = 0 ; i < num; i++ )
	{
		if (creatureConfigs[i])
			delete creatureConfigs[i];
	}
	// make big enough to hold all creatures
	creatureConfigs.clear();
}
ALLEGRO_COLOR lookupMaterialColor(int matType,int matIndex, bool dye)
{
	if (matType < 0)
	{
		//This should not normally happen, but if it does, we don't want crashes, so we'll return magic pink so show something's wrong.
		return al_map_rgb(255, 0, 255);
	}
	if (matType >= contentLoader->colorConfigs.size())
	{
		//if it's more than the size of our colorconfigs, then just make a guess based off what DF tells us.
		goto DFColor;
	}
	if (matIndex < 0)
	{
		return contentLoader->colorConfigs.at(matType).color;
	}
	if (matIndex >= contentLoader->colorConfigs.at(matType).colorMaterials.size())
	{
		goto DFColor;
	}
	if (contentLoader->colorConfigs.at(matType).colorMaterials.at(matIndex).colorSet)
	{
		return contentLoader->colorConfigs.at(matType).colorMaterials.at(matIndex).color;
	}
	DFColor:
	MaterialInfo mat;
	if(mat.decode(matType, matIndex))
	{
		if(dye)
			return al_map_rgb_f(
			contentLoader->Mats->color[mat.material->powder_dye].red,
			contentLoader->Mats->color[mat.material->powder_dye].green,
			contentLoader->Mats->color[mat.material->powder_dye].blue);
		else
			return al_map_rgb_f(
			contentLoader->Mats->color[mat.material->state_color[0]].red,
			contentLoader->Mats->color[mat.material->state_color[0]].green,
			contentLoader->Mats->color[mat.material->state_color[0]].blue);
	}
	else return al_map_rgb(255,255,255);
}

ShadeBy getShadeType(const char* Input)
{
	if( strcmp(Input, "none") == 0)
		return ShadeNone;
	if( strcmp(Input, "xml") == 0)
		return ShadeXml;
	if( strcmp(Input, "material") == 0)
		return ShadeMat;
	if( strcmp(Input, "layer") == 0)
		return ShadeLayer;
	if( strcmp(Input, "vein") == 0)
		return ShadeVein;
	if( strcmp(Input, "material_fore") == 0)
		return ShadeMatFore;
	if( strcmp(Input, "material_back") == 0)
		return ShadeMatBack;
	if( strcmp(Input, "layer_fore") == 0)
		return ShadeLayerFore;
	if( strcmp(Input, "layer_back") == 0)
		return ShadeLayerBack;
	if( strcmp(Input, "vein_fore") == 0)
		return ShadeVeinFore;
	if( strcmp(Input, "vein_back") == 0)
		return ShadeVeinBack;
	if( strcmp(Input, "bodypart") == 0)
		return ShadeBodyPart;
	if( strcmp(Input, "profession") == 0)
		return ShadeJob;
	if( strcmp(Input, "blood") == 0)
		return ShadeBlood;
	if( strcmp(Input, "building") == 0)
		return ShadeBuilding;
	if( strcmp(Input, "grass") == 0)
		return ShadeGrass;
	if( strcmp(Input, "equipment") == 0)
		return ShadeEquip;
	if( strcmp(Input, "item") == 0)
		return ShadeItem;
	return ShadeNone;
}