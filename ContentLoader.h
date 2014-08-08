#pragma once
#include "tinyxml.h"
#include "BuildingConfiguration.h"
#include "CreatureConfiguration.h"
#include "VegetationConfiguration.h"
#include "GroundMaterialConfiguration.h"
#include "ColorConfiguration.h"
#include "commonTypes.h"
#include "FluidConfiguration.h"
#include "ItemConfiguration.h"
#include "common.h"

class ContentLoader
{
private:
    bool parseContentIndexFile( const char* filepath );
    bool parseContentXMLFile( const char* filepath );
    bool parseBuildingContent( TiXmlElement* elemRoot );
    bool parseCreatureContent( TiXmlElement* elemRoot );
    bool parseTerrainContent ( TiXmlElement* elemRoot );
    bool parseGrowthContent(TiXmlElement* elemRoot);
    bool parseTreeContent( TiXmlElement* elemRoot );
    bool parseShrubContent( TiXmlElement* elemRoot );
    bool parseColorContent( TiXmlElement* elemRoot );
    bool parseFluidContent( TiXmlElement* elemRoot );
    bool parseGrassContent( TiXmlElement* elemRoot );
    bool parseItemContent( TiXmlElement* elemRoot );
    void flushCreatureConfig();

    bool translationComplete;

    void gatherStyleIndices(df::world_raws * raws); 
public:
    ContentLoader(void);
    ~ContentLoader(void);

    bool Load();

    bool reload_configs();

    std::vector<BuildingConfiguration> buildingConfigs;
    std::vector<std::vector<CreatureConfiguration>*> creatureConfigs;
    std::vector<VegetationConfiguration> treeConfigs;
    std::vector<VegetationConfiguration> shrubConfigs;
    std::vector<VegetationConfiguration> grassConfigs;
    std::vector<TerrainConfiguration*> terrainFloorConfigs;
    std::vector<TerrainConfiguration*> terrainWallConfigs;
    std::vector<ColorConfiguration> colorConfigs;
    MaterialMatcher<ALLEGRO_COLOR> materialColorConfigs;
    MaterialMatcher<c_sprite> growthTopConfigs;
    MaterialMatcher<c_sprite> growthBottomConfigs;
    std::vector<ItemConfiguration*> itemConfigs;
    FluidConfiguration lava[8];
    FluidConfiguration water[8];

    //race.caste.hairtype.styletype
    std::vector<std::vector<std::vector<int32_t>*>*> style_indices;
    std::vector<std::vector<int32_t>*> position_Indices;

    RemoteFortressReader::MaterialList materialNameList;
    RemoteFortressReader::MaterialList growthNameList;
    RemoteFortressReader::TiletypeList tiletypeNameList;
    std::vector<std::string> professionStrings;
    std::map <uint32_t, std::string> custom_workshop_types;
    DFHack::Materials * Mats;
    std::vector<t_matgloss> organic;
    std::vector<t_matglossInorganic> inorganic;

    uint32_t currentTick;
    uint32_t currentYear;
    uint8_t currentMonth;
    uint8_t currentDay;
    uint8_t currentHour;
    uint8_t currentTickRel;
    t_gamemodes gameMode;

    int obsidian;
};

extern ContentLoader * contentLoader;

extern const char* getDocument(TiXmlNode* element);
bool getLocalFilename(char * buffer, const char* filename, const char* relativeto);
extern void contentError(const char* message, TiXmlNode* element);
extern void contentWarning(const char* message, TiXmlNode* element);
extern char getAnimFrames(const char* framestring);
extern int loadConfigImgFile(const char* filename, TiXmlElement* referrer);
MAT_BASICS lookupMaterialType(const char* strValue);
int lookupMaterialIndex(int matType, const char* strValue);
template <typename T>
int lookupIndexedType(const char* indexName, std::vector<T>& typeVector)
{
    if (indexName == NULL || indexName[0] == 0) {
        return INVALID_INDEX;
    }
    uint32_t vsize = (uint32_t)typeVector.size();
    for(uint32_t i=0; i < vsize; i++) {
        if (typeVector[i].id == indexName) {
            return i;
        }
    }
    return INVALID_INDEX;
}
template <typename T>
int lookupIndexedPonterType(const char* indexName, std::vector<T*>& typeVector)
{
    if (indexName == NULL || indexName[0] == 0) {
        return INVALID_INDEX;
    }
    uint32_t vsize = (uint32_t)typeVector.size();
    for(uint32_t i=0; i < vsize; i++) {
        if (typeVector[i]->id == indexName) {
            return i;
        }
    }
    return INVALID_INDEX;
}
const char *lookupMaterialTypeName(int matType);
const char *lookupMaterialName(int matType,int matIndex);
const char *lookupBuildingSubtype(int main_type, int i);
uint8_t lookupMaterialFore(int matType,int matIndex);
uint8_t lookupMaterialBack(int matType,int matIndex);
uint8_t lookupMaterialBright(int matType,int matIndex);
const char *lookupTreeName(int matIndex);
ALLEGRO_COLOR lookupMaterialColor(DFHack::t_matglossPair matt, DFHack::t_matglossPair dyematt, ALLEGRO_COLOR defaultColor=al_map_rgb(255,255,255));
ALLEGRO_COLOR lookupMaterialColor(DFHack::t_matglossPair matt, ALLEGRO_COLOR defaultColor=al_map_rgb(255,255,255));
ALLEGRO_COLOR lookupMaterialColor(int matType, int matIndex, int dyeType, int dyeIndex, ALLEGRO_COLOR defaultColor=al_map_rgb(255,255,255));
ALLEGRO_COLOR lookupMaterialColor(int matType, int matIndex, ALLEGRO_COLOR defaultColor=al_map_rgb(255,255,255));
const char * lookupFormName(int formType);
ShadeBy getShadeType(const char* Input);
