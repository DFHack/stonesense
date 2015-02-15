#include <fstream>
#include "common.h"
#include "ContentLoader.h"
#include "ContentBuildingReader.h"
#include "MapLoading.h"
#include "ColorConfiguration.h"
#include "TreeGrowthConfiguration.h"

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

#include "df/creature_raw.h"
#include "df/caste_raw.h"
#include "df/tissue_style_raw.h"
#include "df/entity_position_raw.h"
#include "df/entity_raw.h"
#include "df/historical_entity.h"
#include "df/entity_position.h"

#include "ConnectionState.h"
#include "EnumToString.h"

void DumpStringVector(const char* filename, vector<std::string> * input)
{
    FILE* fp = fopen(filename, "w");

    // Run through until perfect match found or hit end.
    for(int i = 0; i < input->size(); i++){
        fprintf(fp, "%i:%s\n", i, input->at(i).c_str());
    }
    fclose(fp);
}

ContentLoader * contentLoader;

ContentLoader::ContentLoader(void) { }
ContentLoader::~ContentLoader(void)
{
    //flush content on exit
    flushBuildingConfig(&buildingConfigs);
    flushTerrainConfig(terrainFloorConfigs);
    flushTerrainConfig(terrainWallConfigs);
    flushItemConfig(itemConfigs);
    flushCreatureConfig();
    colorConfigs.clear();
    materialColorConfigs.clear();
    growthTopConfigs.clear();
    growthBottomConfigs.clear();
}

bool ContentLoader::Load()
{
    /*draw_textf_border(font,
    ssState.ScreenW/2,
    ssState.ScreenH/2,
    ALLEGRO_ALIGN_CENTRE, "Loading...");
    al_flip_display();*/
    //flush old config
    flushBuildingConfig(&buildingConfigs);
    flushTerrainConfig(terrainFloorConfigs);
    flushTerrainConfig(terrainWallConfigs);
    flushItemConfig(itemConfigs);
    colorConfigs.clear();
    materialColorConfigs.clear();
    growthTopConfigs.clear();
    growthBottomConfigs.clear();
    flushCreatureConfig();
    treeConfigs.clear();
    shrubConfigs.clear();
    flushImgFiles();

    //pull all the material names through the RPC stuff. Mostly a test at this point.
    if (!connection_state)
        connection_state = new ConnectionState();
    connection_state->Connect();
    if (connection_state)
    {
        connection_state->MaterialListCall(&(connection_state->empty_message), &materialNameList);
        connection_state->GrowthListCall(&(connection_state->empty_message), &growthNameList);
        connection_state->TiletypeListCall(&(connection_state->empty_message), &tiletypeNameList);
        connection_state->Disconnect();
    }

    draw_loading_message("Reading Material Names");
    remove("MatList.csv");
    FILE* fp = fopen("MatList.csv", "a");
    if (fp) {
        fprintf(fp, "#;mat_type;mat_index;id;name;color\n");
        for (int i = 0; i < materialNameList.material_list_size(); i++)
        {
            fprintf(fp, "%d;%d;%d;%s;%s;#%02X%02X%02X\n",
                i,
                materialNameList.material_list(i).mat_pair().mat_type(),
                materialNameList.material_list(i).mat_pair().mat_index(),
                materialNameList.material_list(i).id().c_str(),
                materialNameList.material_list(i).name().c_str(),
                materialNameList.material_list(i).state_color().red(),
                materialNameList.material_list(i).state_color().green(),
                materialNameList.material_list(i).state_color().blue());
        }
        fclose(fp);
    }
    draw_loading_message("Reading Growth Names");
    remove("GrowthList.csv");
    fp = fopen("GrowthList.csv", "a");
    if (fp) {
        fprintf(fp, "#;mat_type;mat_index;id;name;color\n");
        for (int i = 0; i < growthNameList.material_list_size(); i++)
        {
            fprintf(fp, "%d;%d;%d;%s;%s;#%02X%02X%02X\n",
                i,
                growthNameList.material_list(i).mat_pair().mat_type(),
                growthNameList.material_list(i).mat_pair().mat_index(),
                growthNameList.material_list(i).id().c_str(),
                growthNameList.material_list(i).name().c_str(),
                growthNameList.material_list(i).state_color().red(),
                growthNameList.material_list(i).state_color().green(),
                growthNameList.material_list(i).state_color().blue());
        }
        fclose(fp);
    }

    draw_loading_message("Reading TileType Names");
    remove("TiletypeList.csv");
    fp = fopen("TiletypeList.csv", "a");
    if (fp) {
        fprintf(fp, "id;name;shape;special;material;variant\n");
        for (int i = 0; i < tiletypeNameList.tiletype_list_size(); i++)
        {
            fprintf(fp, "%d;%s;%s;%s;%s;%s\n",
                tiletypeNameList.tiletype_list(i).id(),
                tiletypeNameList.tiletype_list(i).name().c_str(),
                TiletypeShapeToString(tiletypeNameList.tiletype_list(i).shape()),
                TiletypeSpecialToString(tiletypeNameList.tiletype_list(i).special()),
                TiletypeMaterialToString(tiletypeNameList.tiletype_list(i).material()),
                TiletypeVariantToString(tiletypeNameList.tiletype_list(i).variant())
                );
        }
        fclose(fp);
    }

    // This is an extra suspend/resume, but it only happens when reloading the config
    // ie not enough to worry about
    //DF.Suspend();
    ////read data from DF
    //const vector<string> *tempClasses = DF.getMemoryInfo()->getClassIDMapping();
    //// make a copy for our use
    //classIdStrings = *tempClasses;

    try {
        Mats = Core::getInstance().getMaterials();
    } catch(exception &e) {
        LogError("DFhack exeption: %s\n", e.what());
    }
    draw_loading_message("Reading Creature Names");
    if (!ssConfig.skipCreatureTypes) {
        try {
            Mats->ReadCreatureTypes();
        } catch(exception &e) {
            LogError("DFhack exeption: %s\n", e.what());
            ssConfig.skipCreatureTypes = true;
        }
    }
    if(!ssConfig.skipCreatureTypesEx) {
        try {
            Mats->ReadCreatureTypesEx();
        } catch(exception &e) {
            LogError("DFhack exeption: %s\n", e.what());
            ssConfig.skipCreatureTypesEx = true;
        }
    }
    draw_loading_message("Reading Color Descriptors");
    if (!ssConfig.skipDescriptorColors) {
        try {
            Mats->ReadDescriptorColors();
        } catch(exception &e) {
            LogError("DFhack exeption: %s\n", e.what());
            ssConfig.skipDescriptorColors = true;
        }
    }
    draw_loading_message("Reading Inorganic Materials");
    if (!ssConfig.skipInorganicMats) {
        if(!Mats->CopyInorganicMaterials(this->inorganic)) {
            LogError("Missing inorganic materials!\n");
            ssConfig.skipInorganicMats = true;
        }
    }
    draw_loading_message("Reading Organic Materials");
    if (!ssConfig.skipOrganicMats) {
        if(!Mats->CopyOrganicMaterials(this->organic)) {
            LogError("Missing organic materials!\n");
            ssConfig.skipOrganicMats = true;
        }
    }
    draw_loading_message("Reading Custom Workshop Types");
    Buildings::ReadCustomWorkshopTypes(custom_workshop_types);
    draw_loading_message("Reading Professions");

    if(professionStrings.empty()) {
        FOR_ENUM_ITEMS(profession, i) {
            if(i<0) {
                continue;
            }
            professionStrings.push_back(string(ENUM_KEY_STR(profession, i)));
        }
        for(int i = 0; i < df::global::world->entities.all.size(); i++){
            df::historical_entity * currentity = df::global::world->entities.all[i];
            if(!currentity) continue;
            for(int j = 0; j < currentity->positions.own.size(); j++) {
                df::entity_position * currentpos = currentity->positions.own[j];
                if(!currentpos) continue;
                int found = -1;
                for(int k = 0; k < professionStrings.size(); k++){
                    if( professionStrings[k] == currentpos->code){
                        found = k;
                        break;
                    }
                }
                if(found < 0){
                    professionStrings.push_back(currentpos->code);
                    found = professionStrings.size()-1;
                }
                int ent_id = currentity->id;
                int pos_id = currentpos->id;
                if(ent_id  >= position_Indices.size())
                    position_Indices.resize(ent_id+1, NULL);
                if(!position_Indices[ent_id])
                    position_Indices[ent_id] = new vector<int32_t>;
                if(pos_id  >= position_Indices[ent_id]->size())
                    position_Indices[ent_id]->resize(pos_id+1, NULL);
                position_Indices[ent_id]->at(pos_id) = found;
                //LogError("%d(%d):%s->%d(%d):%s = %d\n", i, currentity->id, currentity->entity_raw->code.c_str(), j,currentpos->id, currentpos->code.c_str(), found);
            }
            for(int j = 0; j < currentity->positions.site.size(); j++) {
                df::entity_position * currentpos = currentity->positions.site[j];
                if(!currentpos) continue;
                int found = -1;
                for(int k = 0; k < professionStrings.size(); k++){
                    if( professionStrings[k] == currentpos->code){
                        found = k;
                        break;
                    }
                }
                if(found < 0){
                    professionStrings.push_back(currentpos->code);
                    found = professionStrings.size()-1;
                }
                int ent_id = currentity->id;
                int pos_id = currentpos->id;
                if(ent_id  >= position_Indices.size())
                    position_Indices.resize(ent_id+1, NULL);
                if(!position_Indices[ent_id])
                    position_Indices[ent_id] = new vector<int32_t>;
                if(pos_id  >= position_Indices[ent_id]->size())
                    position_Indices[ent_id]->resize(pos_id+1, NULL);
                position_Indices[ent_id]->at(pos_id) = found;
                //LogError("%d(%d):%s->%d(%d):%s = %d\n", i, currentity->id, currentity->entity_raw->code.c_str(), j,currentpos->id, currentpos->code.c_str(), found);
            }
            for(int j = 0; j < currentity->positions.conquered_site.size(); j++) {
                df::entity_position * currentpos = currentity->positions.conquered_site[j];
                if(!currentpos) continue;
                int found = -1;
                for(int k = 0; k < professionStrings.size(); k++){
                    if( professionStrings[k] == currentpos->code){
                        found = k;
                        break;
                    }
                }
                if(found < 0){
                    professionStrings.push_back(currentpos->code);
                    found = professionStrings.size()-1;
                }
                int ent_id = currentity->id;
                int pos_id = currentpos->id;
                if(ent_id  >= position_Indices.size())
                    position_Indices.resize(ent_id+1, NULL);
                if(!position_Indices[ent_id])
                    position_Indices[ent_id] = new vector<int32_t>;
                if(pos_id  >= position_Indices[ent_id]->size())
                    position_Indices[ent_id]->resize(pos_id+1, NULL);
                position_Indices[ent_id]->at(pos_id) = found;
                //LogError("%d(%d):%s->%d(%d):%s = %d\n", i, currentity->id, currentity->entity_raw->code.c_str(), j,currentpos->id, currentpos->code.c_str(), found);
            }
        }
    }
    //DumpStringVector("professiondump.txt", &professionStrings);
    draw_loading_message("Reading Hairstyles");
    gatherStyleIndices(&df::global::world->raws);
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
    flushTerrainConfig(terrainWallConfigs);
    flushItemConfig(itemConfigs);
    colorConfigs.clear();
    materialColorConfigs.clear();
    growthTopConfigs.clear();
    growthBottomConfigs.clear();
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
    if (filename[0] == '/' || filename[0] == '\\') {
        temppath = al_create_path(filename);
        al_make_path_canonical(temppath);
    } else {
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
    draw_textf_border(font, ssState.ScreenW/2,
    ssState.ScreenH/2,
    ALLEGRO_ALIGN_CENTRE, "Loading %s...", filepath);
    al_flip_display();
    */
    string line;
    ifstream myfile( filepath );
    if (myfile.is_open() == false) {
        LogError( "Unable to load config index file at: %s!\n", filepath );
        return false;
    }
    LogVerbose("Reading index at %s...\n", filepath);

    while ( !myfile.eof() ) {
        char configfilepath[FILENAME_BUFFERSIZE] = {0};

        getline (myfile,line);

        // some systems don't remove the \r char as a part of the line change:
        // also trim trailing space
        int resize = (int)line.size()-1;
        for (; resize>0; resize--) {
            char test = line[resize];
            if (test == '\r') {
                continue;
            }
            if (test == '\t') {
                continue;
            }
            if (test == ' ') {
                continue;
            }
            break;
        }
        if (resize <= 0) {
            continue;
        }
        line.resize(resize+1);

        // allow comments
        if (line[0] == '#') {
            continue;
        }

        if (!getLocalFilename(configfilepath,line.c_str(),filepath)) {
            LogError("File name parsing failed on %s\n",line.c_str());
            continue;
        }
        //WriteErr("but it's all fucked here: %s\n",configfilepath);
        ALLEGRO_PATH * temppath = al_create_path(configfilepath);
        const char* extension;
        extension = al_get_path_extension(temppath);
        //WriteErr("extension: %s\n",extension);
        if (strcmp(extension,".xml") == 0) {
            LogVerbose("Reading xml %s...\n", configfilepath);
            if (!parseContentXMLFile(configfilepath)) {
                LogError("Failure in reading %s\n",configfilepath);
            }
        } else if (strcmp(extension,".txt") == 0) {
            LogVerbose("Reading index %s...\n", configfilepath);
            if (!parseContentIndexFile(configfilepath)) {
                LogError("Failure in reading %s\n",configfilepath);
            }
        } else {
            LogError("Invalid filename: %s\n",configfilepath);
        }
    }
    myfile.close();

    return true;
}

bool ContentLoader::parseContentXMLFile( const char* filepath )
{
    /*
    al_clear_to_color(al_map_rgb(0,0,0));
    draw_textf_border(font, ssState.ScreenW/2,
    ssState.ScreenH/2,
    ALLEGRO_ALIGN_CENTRE, "Loading %s...", filepath);
    al_flip_display();*/
    TiXmlDocument doc( filepath );
    if(!doc.LoadFile()) {
        LogError("File load failed: %s\n", filepath);
        return false;
    }
    TiXmlHandle hDoc(&doc);
    TiXmlElement* elemRoot;

    bool runningResult = true;
    elemRoot = hDoc.FirstChildElement().Element();
    while( elemRoot ) {
        draw_loading_message("Loading %s", getDocument(elemRoot));
        string elementType = elemRoot->Value();
        if( elementType.compare( "building" ) == 0 ) {
            runningResult &= parseBuildingContent( elemRoot );
        }
        else if( elementType.compare( "creatures" ) == 0 ) {
            runningResult &= parseCreatureContent( elemRoot );
        }
        else if( elementType.compare( "floors" ) == 0 ) {
            runningResult &= parseTerrainContent( elemRoot );
        }
        else if( elementType.compare( "walls" ) == 0 ) {
            runningResult &= parseTerrainContent( elemRoot );
        }
        else if( elementType.compare( "shrubs" ) == 0 ) {
            runningResult &= parseShrubContent( elemRoot );
        }
        else if( elementType.compare( "trees" ) == 0 ) {
            runningResult &= parseTreeContent( elemRoot );
        }
        else if( elementType.compare( "grasses" ) == 0 ) {
            runningResult &= parseGrassContent( elemRoot );
        }
        else if( elementType.compare( "colors" ) == 0 ) {
            runningResult &= parseColorContent( elemRoot );
        }
        else if( elementType.compare( "fluids" ) == 0 ) {
            runningResult &= parseFluidContent( elemRoot );
        }
        else if( elementType.compare( "items" ) == 0 ) {
            runningResult &= parseItemContent( elemRoot );
        }
        else if (elementType.compare("growths") == 0) {
            runningResult &= parseGrowthContent(elemRoot);
        }
        else {
            contentError("Unrecognised root element",elemRoot);
        }

        elemRoot = elemRoot->NextSiblingElement();
    }

    return runningResult;
}


bool ContentLoader::parseBuildingContent(TiXmlElement* elemRoot )
{
    return addSingleBuildingConfig( elemRoot, &buildingConfigs );
}

bool ContentLoader::parseCreatureContent(TiXmlElement* elemRoot )
{
    return addCreaturesConfig( elemRoot, creatureConfigs );
}

bool ContentLoader::parseShrubContent(TiXmlElement* elemRoot )
{
    return addSingleVegetationConfig( elemRoot, &shrubConfigs, organic );
}

bool ContentLoader::parseTreeContent(TiXmlElement* elemRoot )
{
    return addSingleVegetationConfig( elemRoot, &treeConfigs, organic );
}

bool ContentLoader::parseGrassContent(TiXmlElement* elemRoot )
{
    return addSingleVegetationConfig( elemRoot, &grassConfigs, organic );
}

bool ContentLoader::parseTerrainContent(TiXmlElement* elemRoot )
{
    return addSingleTerrainConfig( elemRoot );
}

bool ContentLoader::parseGrowthContent(TiXmlElement* elemRoot)
{
    return addSingleGrowthConfig(elemRoot);
}

bool ContentLoader::parseColorContent(TiXmlElement* elemRoot )
{
    return addSingleColorConfig( elemRoot );
}

bool ContentLoader::parseFluidContent(TiXmlElement* elemRoot )
{
    return addSingleFluidConfig( elemRoot );
}

bool ContentLoader::parseItemContent(TiXmlElement* elemRoot )
{
    return addSingleItemConfig( elemRoot );
}


const char* getDocument(TiXmlNode* element)
{
    //walk up the tree to the root
    TiXmlNode* parent = element->Parent();
    while (parent != null) {
        element = parent;
        parent = element->Parent();
    }
    // topmost node *should* be a document, but lets be sure
    parent = dynamic_cast<TiXmlDocument*>(element);
    if (parent == NULL) {
        return NULL;
    }
    return parent->Value();
}

void contentError(const char* message, TiXmlNode* element)
{
    LogError("%s: %s: %s (Line %d)\n",getDocument(element),message,element->Value(),element->Row());
}
void contentWarning(const char* message, TiXmlNode* element)
{
    LogVerbose("%s: %s: %s (Line %d)\n",getDocument(element),message,element->Value(),element->Row());
}
// converts list of characters 0-5 into bits, ignoring garbage
// eg  "035" or "0  3 5" or "0xx3x5" are all good
char getAnimFrames(const char* framestring)
{
    if (framestring == NULL) {
        return ALL_FRAMES;
    }
    char aframes=0;
    for (int i=0; i<6; i++) {
        if (framestring[i]==0) {
            return aframes;
        }
        char temp = framestring[i]-'0';
        if (temp < 0 || temp > 5) {
            continue;
        }
        aframes = aframes | (1 << temp);
    }
    return aframes;
}

int lookupMaterialIndex(int matType, const char* strValue)
{
    // for appropriate elements, look up subtype
    if (matType == INORGANIC && !ssConfig.skipInorganicMats) {
        return lookupIndexedType(strValue,contentLoader->inorganic);
    } else if (matType == WOOD && !ssConfig.skipOrganicMats) {
        return lookupIndexedType(strValue,contentLoader->organic);
    } else if (matType == PLANT && !ssConfig.skipOrganicMats) {
        return lookupIndexedType(strValue,contentLoader->organic);
    } else if (matType == PLANTCLOTH && !ssConfig.skipOrganicMats) {
        return lookupIndexedType(strValue,contentLoader->organic);
    } else if (matType == LEATHER && !ssConfig.skipCreatureTypes) {
        return lookupIndexedType(strValue,contentLoader->Mats->race);
    } else {
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
    switch (main_type) {
    case building_type::Furnace:
        return enum_item_key_str((df::furnace_type)i);
    case building_type::Construction:
        return enum_item_key_str((df::construction_type)i);
    case building_type::SiegeEngine:
        return enum_item_key_str((df::siegeengine_type)i);
    case building_type::Shop:
        return enum_item_key_str((df::shop_type)i);
    case building_type::Workshop:
        return enum_item_key_str((df::workshop_type)i);
    default:
        return "NA";
    }
    return "NA";
}

const char *lookupMaterialTypeName(int matType)
{
    switch (matType) {
    case INORGANIC:
        return "Inorganic";
    case GREEN_GLASS:
        return "GreenGlass";
    case WOOD:
        return "Wood";
    case PLANT:
        return "Plant";
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
    case VOMIT:
        return "Vomit";
    case DESIGNATION:
        return "Designation";
    case CONSTRUCTION:
        return "Construction";
    default:
        return NULL;
    }
}

MAT_BASICS lookupMaterialType(const char* strValue)
{
    if (strValue == NULL || strValue[0] == 0) {
        return INVALID;
    } else if( strcmp(strValue, "Stone") == 0) {
        return INORGANIC;
    } else if( strcmp(strValue, "Metal") == 0) {
        return INORGANIC;
    } else if( strcmp(strValue, "Inorganic") == 0) {
        return INORGANIC;
    } else if( strcmp(strValue, "GreenGlass") == 0) {
        return GREEN_GLASS;
    } else if( strcmp(strValue, "Wood") == 0) {
        return WOOD;
    } else if( strcmp(strValue, "Plant") == 0) {
        return PLANT;
    } else if( strcmp(strValue, "Ice") == 0) {
        return ICE;
    } else if( strcmp(strValue, "ClearGlass") == 0) {
        return CLEAR_GLASS;
    } else if( strcmp(strValue, "CrystalGlass") == 0) {
        return CRYSTAL_GLASS;
    } else if( strcmp(strValue, "PlantCloth") == 0) {
        return PLANTCLOTH;
    } else if( strcmp(strValue, "Leather") == 0) {
        return LEATHER;
    } else if( strcmp(strValue, "Vomit") == 0) {
        return VOMIT;
    } else if( strcmp(strValue, "Designation") == 0) {
        return DESIGNATION;
    } else if( strcmp(strValue, "Construction") == 0) {
        return CONSTRUCTION;
    }
    //TODO this needs fixing on dfhack side
    return INVALID;
}

const char *lookupMaterialName(int matType,int matIndex)
{
    if (matIndex < 0) {
        return NULL;
    }
    vector<t_matgloss>* typeVector;
    // for appropriate elements, look up subtype
    if ((matType == INORGANIC) && (!ssConfig.skipInorganicMats)) {
        if(matIndex < contentLoader->inorganic.size()) {
            return contentLoader->inorganic[matIndex].id.c_str();
        } else {
            return NULL;
        }
    }
    else if (((matType == WOOD) || (matType == PLANT)) && (!ssConfig.skipOrganicMats)) {
        typeVector=&(contentLoader->organic);
    } else if ((matType == PLANTCLOTH) && (!ssConfig.skipOrganicMats)) {
        typeVector=&(contentLoader->organic);
    } else if (matType == LEATHER) {
        if(!ssConfig.skipCreatureTypes) {
            typeVector=&(contentLoader->Mats->race);
        }
    } else {
        //maybe allow some more in later
        return NULL;
    }
    if (matIndex >= typeVector->size()) {
        return NULL;
    }
    return (*typeVector)[matIndex].id.c_str();
}

const char *lookupTreeName(int matIndex)
{
    if(ssConfig.skipOrganicMats) {
        return NULL;
    }
    if (matIndex < 0) {
        return NULL;
    }
    vector<t_matgloss>* typeVector;
    // for appropriate elements, look up subtype
    typeVector=&(contentLoader->organic);
    if (matIndex >= typeVector->size()) {
        return NULL;
    }
    return (*typeVector)[matIndex].id.c_str();
}

const char * lookupFormName(int formType)
{
    switch (formType) {
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
    if (matIndex < 0) {
        return 0;
    }
    vector<t_matgloss>* typeVector;
    //// for appropriate elements, look up subtype
    //if (matType == Mat_Wood)
    //{
    //    typeVector=&(contentLoader.woodNameStrings);
    //}
    //else if (matType == 0)
    //{
    //    typeVector=&(contentLoader.stoneNameStrings);
    //}
    //else if (matType == Mat_Metal)
    //{
    //    typeVector=&(contentLoader.metalNameStrings);
    //}
    //else
    //{
    //maybe allow some more in later
    return 0;
    //}
    if (matIndex >= typeVector->size()) {
        return 0;
    }
    return (*typeVector)[matIndex].fore;
}

uint8_t lookupMaterialBack(int matType,int matIndex)
{
    if (matIndex < 0) {
        return 0;
    }
    vector<t_matgloss>* typeVector;
    //// for appropriate elements, look up subtype
    //if (matType == Mat_Wood)
    //{
    //    typeVector=&(contentLoader.woodNameStrings);
    //}
    //else if (matType == 0)
    //{
    //    typeVector=&(contentLoader.stoneNameStrings);
    //}
    //else if (matType == Mat_Metal)
    //{
    //    typeVector=&(contentLoader.metalNameStrings);
    //}
    //else
    //{
    //maybe allow some more in later
    return 0;
    //}
    if (matIndex >= typeVector->size()) {
        return 0;
    }
    return (*typeVector)[matIndex].back;
}

uint8_t lookupMaterialBright(int matType,int matIndex)
{
    if (matIndex < 0) {
        return 0;
    }
    vector<t_matgloss>* typeVector;
    //// for appropriate elements, look up subtype
    //if (matType == Mat_Wood)
    //{
    //    typeVector=&(contentLoader.woodNameStrings);
    //}
    //else if (matType == 0)
    //{
    //    typeVector=&(contentLoader.stoneNameStrings);
    //}
    //else if (matType == Mat_Metal)
    //{
    //    typeVector=&(contentLoader.metalNameStrings);
    //}
    //else
    //{
    //maybe allow some more in later
    return 0;
    //}
    if (matIndex >= typeVector->size()) {
        return 0;
    }
    return (*typeVector)[matIndex].bright;
}

int loadConfigImgFile(const char* filename, TiXmlElement* referrer)
{
    const char* documentRef = getDocument(referrer);
    char configfilepath[FILENAME_BUFFERSIZE] = {0};
    if (!getLocalFilename(configfilepath,filename,documentRef)) {
        contentError("Failed to parse sprites filename",referrer);
        return -1;
    }
    return loadImgFile(configfilepath);
}

void ContentLoader::flushCreatureConfig()
{
    uint32_t num = (uint32_t)creatureConfigs.size();
    for ( int i = 0 ; i < num; i++ ) {
        if (creatureConfigs[i]) {
            delete creatureConfigs[i];
        }
    }
    // make big enough to hold all creatures
    creatureConfigs.clear();
    for ( int i = 0; i < style_indices.size();i++){
        if(style_indices[i]){
            for ( int j = 0; j < style_indices[i]->size();j++){
                if(style_indices[i]->at(j)){
                    style_indices[i]->at(j)->clear();
                    delete style_indices[i]->at(j);
                }
            }
            style_indices[i]->clear();
            delete style_indices[i];
        }
    }
    style_indices.clear();
}

void ContentLoader::gatherStyleIndices(df::world_raws * raws)
{
    for(int creatureIndex = 0; creatureIndex < raws->creatures.all.size(); creatureIndex++)
    {
        df::creature_raw * cre = raws->creatures.all[creatureIndex];
        for(int casteIndex = 0; casteIndex < cre->caste.size(); casteIndex++)
        {
            df::caste_raw * cas = cre->caste[casteIndex];
            for(int styleIndex = 0; styleIndex < cas->tissue_styles.size(); styleIndex++)
            {
                df::tissue_style_raw * sty = cas->tissue_styles[styleIndex];
                hairtypes type = hairtypes_invalid;
                if(sty->token == "HAIR")
                    type = HAIR;
                else if(sty->token == "BEARD")
                    type = BEARD;
                else if(sty->token == "MOUSTACHE")
                    type = MOUSTACHE;
                else if(sty->token == "SIDEBURNS")
                    type = SIDEBURNS;
                else LogError("Unknown hair type: %s", raws->creatures.all[creatureIndex]->caste[casteIndex]->tissue_styles[styleIndex]->token.c_str());
                if(type != hairtypes_invalid)
                {
                    if(creatureIndex >= style_indices.size())
                        style_indices.resize(creatureIndex+1, NULL);
                    if(!style_indices.at(creatureIndex))
                        style_indices.at(creatureIndex) = new vector<vector<int32_t>*>;
                    vector<vector<int32_t>*>* creatureStyle = style_indices.at(creatureIndex);
                    if(casteIndex >= creatureStyle->size())
                        creatureStyle->resize(casteIndex+1, NULL);
                    if(!creatureStyle->at(casteIndex))
                        creatureStyle->at(casteIndex) = new vector<int32_t>;
                    vector<int32_t>* casteStyle = creatureStyle->at(casteIndex);
                    if(type >= casteStyle->size())
                        casteStyle->resize(type+1, 0);
                    casteStyle->at(type) = sty->id;
                    LogVerbose("%s:%s : %d:%s\n", raws->creatures.all[creatureIndex]->creature_id.c_str(),raws->creatures.all[creatureIndex]->caste[casteIndex]->caste_id.c_str(), sty->id, sty->token.c_str());
                }
            }
        }
    }
}

ALLEGRO_COLOR lookupMaterialColor(DFHack::t_matglossPair matt, ALLEGRO_COLOR defaultColor)
{
    return lookupMaterialColor((int) matt.type, (int) matt.index, -1, -1, defaultColor);
}

ALLEGRO_COLOR lookupMaterialColor(DFHack::t_matglossPair matt, DFHack::t_matglossPair dyematt, ALLEGRO_COLOR defaultColor)
{
    return lookupMaterialColor((int) matt.type, (int) matt.index, (int) dyematt.type, (int) dyematt.index, defaultColor);
}

ALLEGRO_COLOR lookupMaterialColor(int matType, int matIndex, ALLEGRO_COLOR defaultColor)
{
    return lookupMaterialColor( matType, matIndex, -1, -1, defaultColor);
}

ALLEGRO_COLOR lookupMaterialColor(int matType, int matIndex, int dyeType, int dyeIndex, ALLEGRO_COLOR defaultColor)
{
    ALLEGRO_COLOR dyeColor = al_map_rgb(255,255,255);
    MaterialInfo dye;
    if (dyeType >= 0 && dyeIndex >= 0 && dye.decode(dyeType, dyeIndex))
        dyeColor = al_map_rgb_f(
        contentLoader->Mats->color[dye.material->powder_dye].red,
        contentLoader->Mats->color[dye.material->powder_dye].green,
        contentLoader->Mats->color[dye.material->powder_dye].blue);
    t_matglossPair matPair;
    matPair.index = matIndex;
    matPair.type = matType;
    if (ALLEGRO_COLOR * matResult = contentLoader->materialColorConfigs.get(matPair))
    {
        return *matResult * dyeColor;
    }
    if (matType < 0) {
        //This should not normally happen, but if it does, we don't want crashes, so we'll return magic pink so show something's wrong.
        return al_map_rgb(255, 0, 255) * dyeColor;
    }
    if (matType >= contentLoader->colorConfigs.size()) {
        //if it's more than the size of our colorconfigs, then just make a guess based off what DF tells us.
        goto DFColor;
    }
    if (matIndex < 0) {
        return contentLoader->colorConfigs.at(matType).color * dyeColor;
    }
    if (matIndex >= contentLoader->colorConfigs.at(matType).colorMaterials.size()) {
        goto DFColor;
    }
    if (contentLoader->colorConfigs.at(matType).colorMaterials.at(matIndex).colorSet) {
        return contentLoader->colorConfigs.at(matType).colorMaterials.at(matIndex).color * dyeColor;
    }
DFColor:
    MaterialInfo mat;
    if(mat.decode(matType, matIndex)) {
            return al_map_rgb_f(
                       contentLoader->Mats->color[mat.material->state_color[0]].red,
                       contentLoader->Mats->color[mat.material->state_color[0]].green,
                       contentLoader->Mats->color[mat.material->state_color[0]].blue) * dyeColor;
    }
    return defaultColor * dyeColor;
}

ShadeBy getShadeType(const char* Input)
{
    if( strcmp(Input, "none") == 0) {
        return ShadeNone;
    }
    if( strcmp(Input, "xml") == 0) {
        return ShadeXml;
    }
    if( strcmp(Input, "named") == 0) {
        return ShadeNamed;
    }
    if( strcmp(Input, "material") == 0) {
        return ShadeMat;
    }
    if( strcmp(Input, "layer") == 0) {
        return ShadeLayer;
    }
    if( strcmp(Input, "vein") == 0) {
        return ShadeVein;
    }
    if( strcmp(Input, "material_fore") == 0) {
        return ShadeMatFore;
    }
    if( strcmp(Input, "material_back") == 0) {
        return ShadeMatBack;
    }
    if( strcmp(Input, "layer_fore") == 0) {
        return ShadeLayerFore;
    }
    if( strcmp(Input, "layer_back") == 0) {
        return ShadeLayerBack;
    }
    if( strcmp(Input, "vein_fore") == 0) {
        return ShadeVeinFore;
    }
    if( strcmp(Input, "vein_back") == 0) {
        return ShadeVeinBack;
    }
    if( strcmp(Input, "bodypart") == 0) {
        return ShadeBodyPart;
    }
    if( strcmp(Input, "profession") == 0) {
        return ShadeJob;
    }
    if( strcmp(Input, "blood") == 0) {
        return ShadeBlood;
    }
    if( strcmp(Input, "building") == 0) {
        return ShadeBuilding;
    }
    if( strcmp(Input, "grass") == 0) {
        return ShadeGrass;
    }
    if( strcmp(Input, "equipment") == 0) {
        return ShadeEquip;
    }
    if (strcmp(Input, "item") == 0) {
        return ShadeItem;
    }
    if (strcmp(Input, "wood") == 0) {
        return ShadeWood;
    }
    if (strcmp(Input, "growth") == 0) {
        return ShadeGrowth;
    }
    return ShadeNone;
}
