#include <fstream>
#include <filesystem>

#include "common.h"
#include "ContentLoader.h"
#include "ContentBuildingReader.h"
#include "MapLoading.h"
#include "ColorConfiguration.h"
#include "TreeGrowthConfiguration.h"
#include "GameConfiguration.h"
#include "StonesenseState.h"

#include "tinyxml.h"
#include "GUI.h"

#include "df/caste_raw.h"
#include "df/creature_raw.h"
#include "df/entity_position.h"
#include "df/entity_position_raw.h"
#include "df/entity_raw.h"
#include "df/historical_entity.h"
#include "df/itemdef.h"
#include "df/itemdef_armorst.h"
#include "df/itemdef_glovesst.h"
#include "df/itemdef_helmst.h"
#include "df/itemdef_pantsst.h"
#include "df/itemdef_shieldst.h"
#include "df/itemdef_shoesst.h"
#include "df/itemdef_weaponst.h"
#include "df/material.h"
#include "df/tissue_style_raw.h"
#include "df/world.h"

using std::vector;
using std::string;

ContentLoader::ContentLoader(void) { }
ContentLoader::~ContentLoader(void)
{
    //flush content on exit
    buildingConfigs.clear();
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
    using std::exception;
    /*draw_textf_border(font,
    ssState.ScreenW/2,
    ssState.ScreenH/2,
    ALLEGRO_ALIGN_CENTRE, "Loading...");
    al_flip_display();*/
    //flush old config
    buildingConfigs.clear();
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

    // This is an extra suspend/resume, but it only happens when reloading the config
    // ie not enough to worry about
    //DF.Suspend();
    ////read data from DF
    //const vector<string> *tempClasses = DF.getMemoryInfo()->getClassIDMapping();
    //// make a copy for our use
    //classIdStrings = *tempClasses;

    try {
        Mats = DFHack::Core::getInstance().getMaterials();
    } catch(exception &e) {
        LogError("DFhack exeption: %s\n", e.what());
    }
    auto& ssConfig = stonesenseState.ssConfig;
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
    DFHack::Buildings::ReadCustomWorkshopTypes(custom_workshop_types);
    draw_loading_message("Reading Professions");

    if(professionStrings.empty()) {
        FOR_ENUM_ITEMS(profession, i) {
            if(i<0) {
                continue;
            }
            professionStrings.push_back(string(ENUM_KEY_STR(profession, i)));
        }
        for(size_t i = 0; i < df::global::world->entities.all.size(); i++){
            df::historical_entity * currentity = df::global::world->entities.all[i];
            if(!currentity) continue;
            auto addEntityPosition = [&](auto currentpos)
                {
                    auto foundIt = std::find_if(
                        professionStrings.begin(),
                        professionStrings.end(),
                        [&](auto pStr) { return pStr == currentpos->code; });
                    if (foundIt == professionStrings.end())
                    {
                        professionStrings.push_back(currentpos->code);
                        foundIt = professionStrings.end() - 1;
                    }
                    auto found = foundIt - professionStrings.begin();

                    size_t ent_id = currentity->id;
                    size_t pos_id = currentpos->id;
                    position_Indices.add(ent_id, pos_id, found);
                };

            for(size_t j = 0; j < currentity->positions.own.size(); j++) {
                df::entity_position * currentpos = currentity->positions.own[j];
                if(!currentpos) continue;
                addEntityPosition(currentpos);
                //LogError("%d(%d):%s->%d(%d):%s = %d\n", i, currentity->id, currentity->entity_raw->code.c_str(), j,currentpos->id, currentpos->code.c_str(), found);
            }
            for(size_t j = 0; j < currentity->positions.site.size(); j++) {
                df::entity_position * currentpos = currentity->positions.site[j];
                if (!currentpos) continue;
                addEntityPosition(currentpos);
                //LogError("%d(%d):%s->%d(%d):%s = %d\n", i, currentity->id, currentity->entity_raw->code.c_str(), j,currentpos->id, currentpos->code.c_str(), found);
            }
            for(size_t j = 0; j < currentity->positions.conquered_site.size(); j++) {
                df::entity_position * currentpos = currentity->positions.conquered_site[j];
                if(!currentpos) continue;
                addEntityPosition(currentpos);
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
    stonesenseState.contentLoader->obsidian = lookupMaterialIndex(INORGANIC, "OBSIDIAN");

    loadGraphicsFromDisk(); //these get destroyed when flushImgFiles is called.
    std::filesystem::path p{ "stonesense" };
    bool overallResult = parseContentIndexFile( p / "index.txt");
    translationComplete = false;

    return overallResult;
}

bool ContentLoader::reload_configs()
{
    buildingConfigs.clear();
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
    std::filesystem::path p = std::filesystem::path{} / "stonesense" / "index.txt";
    bool overallResult = parseContentIndexFile( p );

    return overallResult;
}

std::filesystem::path getLocalFilename(std::filesystem::path filename, std::filesystem::path relativeto)
{
    return relativeto.remove_filename() / filename;
}

bool ContentLoader::parseContentIndexFile( std::filesystem::path filepath )
{
    /*
    al_clear_to_color(al_map_rgb(0,0,0));
    draw_textf_border(font, ssState.ScreenW/2,
    ssState.ScreenH/2,
    ALLEGRO_ALIGN_CENTRE, "Loading %s...", filepath);
    al_flip_display();
    */
    string line;
    std::ifstream myfile( filepath );
    if (myfile.is_open() == false) {
        LogError( "Unable to load config index file at: %s!\n", filepath.string().c_str() );
        return false;
    }
    LogVerbose("Reading index at %s...\n", filepath.string().c_str());

    while ( !myfile.eof() ) {

        getline (myfile,line);
        if (line.empty())
            continue;

        // some systems don't remove the \r char as a part of the line change:
        // also trim trailing space
        size_t resize = line.size()-1;
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

        std::filesystem::path configfilepath = filepath.remove_filename() / std::filesystem::path{ line }.make_preferred();
        auto extension = configfilepath.extension();
        if (extension == ".xml") {
            LogVerbose("Reading xml %s...\n", configfilepath.string().c_str());
            if (!parseContentXMLFile(configfilepath)) {
                LogError("Failure in reading %s\n",configfilepath.string().c_str());
            }
        } else if (extension == ".txt") {
            LogVerbose("Reading index %s...\n", configfilepath.string().c_str());
            if (!parseContentIndexFile(configfilepath)) {
                LogError("Failure in reading %s\n",configfilepath.string().c_str());
            }
        } else {
            LogError("Invalid filename: %s\n",configfilepath.string().c_str());
        }
    }
    myfile.close();

    return true;
}

bool ContentLoader::parseContentXMLFile( std::filesystem::path filepath )
{
    /*
    al_clear_to_color(al_map_rgb(0,0,0));
    draw_textf_border(font, ssState.ScreenW/2,
    ssState.ScreenH/2,
    ALLEGRO_ALIGN_CENTRE, "Loading %s...", filepath);
    al_flip_display();*/
    TiXmlDocument doc( filepath.string().c_str() );
    if(!doc.LoadFile()) {
        LogError("File load failed: %s\n", filepath.string().c_str());
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
    while (parent != nullptr) {
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

void contentError(const string& message, TiXmlNode* element)
{
    auto safeStr = [](const char* s)->const char* {return s ? s : "(unknown)"; };

    LogError("%s: %s: %s (Line %d)\n",
        safeStr(getDocument(element)),
        message.c_str(),
        element ? safeStr(element->Value()) : "(no element)",
        element ? element->Row() : -1);
}
void contentWarning(const string& message, TiXmlNode* element)
{
    LogVerbose("%s: %s: %s (Line %d)\n",getDocument(element),message.c_str(),element->Value(),element->Row());
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
    auto& contentLoader = stonesenseState.contentLoader;
    auto& ssConfig = stonesenseState.ssConfig;

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
        using df::building_type;
        using DFHack::enum_item_key_str;
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
    case building_type::Trap:
        return enum_item_key_str((df::trap_type)i);
    default:
        return "UnhandledType";
    }
    return "Error";
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

using DFHack::t_matgloss;

const char *lookupMaterialName(int matType,int matIndex)
{
    auto& contentLoader = stonesenseState.contentLoader;
    auto& ssConfig = stonesenseState.ssConfig;

    if (matIndex < 0) {
        return NULL;
    }
    vector<t_matgloss>* typeVector;
    // for appropriate elements, look up subtype
    if ((matType == INORGANIC) && (!ssConfig.skipInorganicMats)) {
        if(size_t(matIndex) < contentLoader->inorganic.size()) {
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
        } else {
            return NULL;
        }
    } else {
        //maybe allow some more in later
        return NULL;
    }
    if (size_t(matIndex) >= typeVector->size()) {
        return NULL;
    }
    return (*typeVector)[matIndex].id.c_str();
}

const char *lookupTreeName(int matIndex)
{
    auto& contentLoader = stonesenseState.contentLoader;
    auto& ssConfig = stonesenseState.ssConfig;

    if(ssConfig.skipOrganicMats) {
        return NULL;
    }
    if (matIndex < 0) {
        return NULL;
    }
    vector<t_matgloss>* typeVector;
    // for appropriate elements, look up subtype
    typeVector=&(contentLoader->organic);
    if (size_t(matIndex) >= typeVector->size()) {
        return NULL;
    }
    return (*typeVector)[matIndex].id.c_str();
}

const char * lookupFormName(int formType)
{
    switch (formType) {
        using df::item_type;
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
    // TODO: use DFHack::MaterialInfo to get more appropriate info
    return 0;
}

uint8_t lookupMaterialBack(int matType,int matIndex)
{
    // TODO: use DFHack::MaterialInfo to get more appropriate info
    return 0;
}

uint8_t lookupMaterialBright(int matType,int matIndex)
{
    // TODO: use DFHack::MaterialInfo to get more appropriate info
    return 0;
}

int loadConfigImgFile(std::filesystem::path filename, TiXmlElement* referrer)
{
    std::filesystem::path documentRef = getDocument(referrer);
    std::filesystem::path configfilepath = getLocalFilename(filename, documentRef);
    return loadImgFile(configfilepath);
}

int loadImgFromXML(TiXmlElement* elemRoot)
{
    const char* filename = elemRoot->Attribute("file");
    return (filename != NULL && filename[0] != 0) ? loadConfigImgFile(filename, elemRoot) : INVALID_INDEX;
}

void ContentLoader::flushCreatureConfig()
{
    creatureConfigs.clear();
    style_indices.clear();
}

void ContentLoader::gatherStyleIndices(df::world::T_raws * raws)
{
    for(size_t creatureIndex = 0; creatureIndex < raws->creatures.all.size(); creatureIndex++)
    {
        df::creature_raw * cre = raws->creatures.all[creatureIndex];
        for(size_t casteIndex = 0; casteIndex < cre->caste.size(); casteIndex++)
        {
            df::caste_raw * cas = cre->caste[casteIndex];
            for(size_t styleIndex = 0; styleIndex < cas->tissue_styles.size(); styleIndex++)
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
                    style_indices.add(creatureIndex, casteIndex, type, sty->id);
                    LogVerbose("%s:%s : %d:%s\n",
                        raws->creatures.all[creatureIndex]->creature_id.c_str(),
                        raws->creatures.all[creatureIndex]->caste[casteIndex]->caste_id.c_str(),
                        sty->id, sty->token.c_str());
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
    auto& contentLoader = stonesenseState.contentLoader;
    ALLEGRO_COLOR dyeColor = al_map_rgb(255,255,255);
    DFHack::MaterialInfo dye;
    if (dyeType >= 0 && dyeIndex >= 0 && dye.decode(dyeType, dyeIndex))
        dyeColor = al_map_rgb_f(
        contentLoader->Mats->color[dye.material->powder_dye].red,
        contentLoader->Mats->color[dye.material->powder_dye].green,
        contentLoader->Mats->color[dye.material->powder_dye].blue);
    // FIXME integer truncation: matType should not be an int
    DFHack::t_matglossPair matPair{ int16_t(matType), matIndex };
    if (ALLEGRO_COLOR * matResult = contentLoader->materialColorConfigs.get(matPair))
    {
        return *matResult * dyeColor;
    }
    if (matType < 0) {
        //This should not normally happen, but if it does, we don't want crashes, so we'll return magic pink so show something's wrong.
        return al_map_rgb(255, 0, 255) * dyeColor;
    }
    if (size_t(matType) >= contentLoader->colorConfigs.size()) {
        //if it's more than the size of our colorconfigs, then just make a guess based off what DF tells us.
        goto DFColor;
    }
    if (matIndex < 0) {
        return contentLoader->colorConfigs.at(matType).color * dyeColor;
    }
    if (size_t(matIndex) >= contentLoader->colorConfigs.at(matType).colorMaterials.size()) {
        goto DFColor;
    }
    if (contentLoader->colorConfigs.at(matType).colorMaterials.at(matIndex).colorSet) {
        return contentLoader->colorConfigs.at(matType).colorMaterials.at(matIndex).color * dyeColor;
    }
DFColor:
    DFHack::MaterialInfo mat;
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
