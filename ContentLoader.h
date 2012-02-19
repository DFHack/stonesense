#pragma once
#include "tinyxml.h"
#include "BuildingConfiguration.h"
#include "CreatureConfiguration.h"
#include "VegetationConfiguration.h"
#include "GroundMaterialConfiguration.h"
#include "ColorConfiguration.h"
#include "commonTypes.h"
#include "FluidConfiguration.h"

class ContentLoader{
private:
	bool parseContentIndexFile( const char* filepath );
	bool parseContentXMLFile( const char* filepath );
	bool parseBuildingContent( TiXmlElement* elemRoot );
	bool parseCreatureContent( TiXmlElement* elemRoot );
	bool parseTerrainContent ( TiXmlElement* elemRoot );
	bool parseTreeContent( TiXmlElement* elemRoot );
	bool parseShrubContent( TiXmlElement* elemRoot );
	bool parseColorContent( TiXmlElement* elemRoot );
	bool parseFluidContent( TiXmlElement* elemRoot );
	bool parseGrassContent( TiXmlElement* elemRoot );
	void flushCreatureConfig();

	bool translationComplete;
public:
	ContentLoader(void);
	~ContentLoader(void);

	bool Load( DFHack::Core& DF);

	bool reload_configs();

	vector<BuildingConfiguration> buildingConfigs;
	vector<vector<CreatureConfiguration>*> creatureConfigs;
	vector<VegetationConfiguration> treeConfigs;
	vector<VegetationConfiguration> shrubConfigs;
	vector<VegetationConfiguration> grassConfigs;
	vector<TerrainConfiguration*> terrainFloorConfigs;
	vector<TerrainConfiguration*> terrainBlockConfigs;
	vector<ColorConfiguration> colorConfigs;
	FluidConfiguration lava[8];
	FluidConfiguration water[8];

	vector<string> professionStrings;
	std::map <uint32_t, std::string> custom_workshop_types;
	VersionInfo *MemInfo;
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
extern char getAnimFrames(const char* framestring);
extern int loadConfigImgFile(const char* filename, TiXmlElement* referrer);
MAT_BASICS lookupMaterialType(const char* strValue);
int lookupMaterialIndex(int matType, const char* strValue);
template <typename T>
int lookupIndexedType(const char* indexName, std::vector<T>& typeVector)
{
    if (indexName == NULL || indexName[0] == 0)
    {
        return INVALID_INDEX;
    }
    uint32_t vsize = (uint32_t)typeVector.size();
    for(uint32_t i=0; i < vsize; i++)
    {
        if (typeVector[i].id == indexName)
            return i;
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
ALLEGRO_COLOR lookupMaterialColor(int matType,int matIndex);
const char * lookupFormName(int formType);
ShadeBy getShadeType(const char* Input);
