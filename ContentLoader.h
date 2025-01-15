#pragma once
#include <filesystem>

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

namespace df {
    struct world_raws;
}

class ContentLoader
{
private:
    bool parseContentIndexFile(std::filesystem::path filepath );
    bool parseContentXMLFile( std::filesystem::path filepath );
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

    bool translationComplete = false;

    void gatherStyleIndices(df::world_raws * raws);
public:
    ContentLoader(void);
    ~ContentLoader(void);

    bool Load();

    bool reload_configs();

    std::vector<std::unique_ptr<BuildingConfiguration>> buildingConfigs;
    std::vector<std::unique_ptr<std::vector<CreatureConfiguration>>> creatureConfigs;
    std::vector<std::unique_ptr<VegetationConfiguration>> treeConfigs;
    std::vector<std::unique_ptr<VegetationConfiguration>> shrubConfigs;
    std::vector<std::unique_ptr<VegetationConfiguration>> grassConfigs;
    std::vector<std::unique_ptr<TerrainConfiguration>> terrainFloorConfigs;
    std::vector<std::unique_ptr<TerrainConfiguration>> terrainWallConfigs;
    std::vector<ColorConfiguration> colorConfigs;
    MaterialMatcher<ALLEGRO_COLOR> materialColorConfigs;
    MaterialMatcher<c_sprite> growthTopConfigs;
    MaterialMatcher<c_sprite> growthBottomConfigs;
    std::vector<std::unique_ptr<ItemConfiguration>> itemConfigs;
    FluidConfiguration lava[8];
    FluidConfiguration water[8];

    class StyleIndices {
        //race.caste.hairtype.styletype
        std::vector<std::vector<std::vector<int32_t>*>*> style_indices;
    public:
        void clear()
        {
            for (size_t i = 0; i < style_indices.size(); i++) {
                if (style_indices[i]) {
                    for (size_t j = 0; j < style_indices[i]->size(); j++) {
                        if (style_indices[i]->at(j)) {
                            style_indices[i]->at(j)->clear();
                            delete style_indices[i]->at(j);// EXPLICIT DELETE
                        }
                    }
                    style_indices[i]->clear();
                    delete style_indices[i];// EXPLICIT DELETE
                }
            }
            style_indices.clear();
        }
        void add(int creatureIndex, int casteIndex, int type, int id)
        {
            if (creatureIndex >= style_indices.size())
                style_indices.resize(creatureIndex + 1, NULL);
            if (!style_indices.at(creatureIndex))
                style_indices.at(creatureIndex) = new std::vector<std::vector<int32_t>*>; // EXPLICIT NEW
            std::vector<std::vector<int32_t>*>* creatureStyle = style_indices.at(creatureIndex);
            if (casteIndex >= creatureStyle->size())
                creatureStyle->resize(casteIndex + 1, NULL);
            if (!creatureStyle->at(casteIndex))
                creatureStyle->at(casteIndex) = new std::vector<int32_t>;  // EXPLICIT NEW
            std::vector<int32_t>* casteStyle = creatureStyle->at(casteIndex);
            size_t typeIdx{ size_t(type) };
            if (typeIdx >= casteStyle->size())
                casteStyle->resize(typeIdx + 1, 0);
            casteStyle->at(typeIdx) = id;
        }
        int lookup(int race, int caste, int style_type)
        {
            if (size_t(race) < style_indices.size() && style_indices.at(race)) {
                if (size_t(caste) < style_indices.at(race)->size() && style_indices.at(race)->at(caste)) {
                    for (size_t j = 0; j < style_indices.at(race)->at(caste)->size(); j++) {
                        if (style_type == style_indices.at(race)->at(caste)->at(j)) {
                            return j;
                        }
                    }
                }
            }
            return -1;
        }
    };
    StyleIndices style_indices;
    std::vector<std::vector<int32_t>*> position_Indices;

    std::vector<std::string> professionStrings;
    std::map <uint32_t, std::string> custom_workshop_types;
    DFHack::Materials* Mats = nullptr;
    std::vector<DFHack::t_matgloss> organic;
    std::vector<DFHack::t_matglossInorganic> inorganic;

    uint32_t currentTick = 0;
    uint32_t currentYear = 0;
    uint8_t currentMonth = 0;
    uint8_t currentDay = 0;
    uint8_t currentHour = 0;
    uint8_t currentTickRel = 0;
    DFHack::t_gamemodes gameMode{
        df::enums::game_mode::NONE,
        df::enums::game_type::NONE
        };

    int obsidian = 0;
};

extern ContentLoader * contentLoader;

extern const char* getDocument(TiXmlNode* element);
std::filesystem::path getLocalFilename(std::filesystem::path filename, std::filesystem::path relativeto);
extern void contentError(const std::string& message, TiXmlNode* element);
extern void contentWarning(const std::string& message, TiXmlNode* element);
extern char getAnimFrames(const char* framestring);
extern int loadConfigImgFile(std::filesystem::path filename, TiXmlElement* referrer);
MAT_BASICS lookupMaterialType(const char* strValue);
int lookupMaterialIndex(int matType, const char* strValue);

template <typename T, typename Index = decltype(T::id)>
int lookupIndexedType(const Index& indexName, const std::vector<T>& typeVector)
{
    auto get_id = [](auto tv) {
        if constexpr (std::is_pointer_v<T>) return tv->id;
        else return tv.id;
        };
    auto it = std::find_if(typeVector.begin(), typeVector.end(),
        [&](auto tv) -> bool { return get_id(tv) == indexName; });
    return it != typeVector.end() ? it - typeVector.begin() : INVALID_INDEX;
}
template <typename T, typename Index = decltype(T::id)>
int lookupIndexedPointerType(const Index& indexName, const std::vector<T*>& typeVector)
{
    return lookupIndexedType<T*, Index>(indexName, typeVector);
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
