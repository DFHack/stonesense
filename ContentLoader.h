#pragma once
#include "dfhack/depends/tinyxml/tinyxml.h"
#include "BuildingConfiguration.h"
#include "CreatureConfiguration.h"
#include "VegetationConfiguration.h"
#include "GroundMaterialConfiguration.h"
#include "ColorConfiguration.h"
#include "commonTypes.h"
#include "FluidConfiguration.h"

class ContentLoader{
private:
	bool parseContentIndexFile( char* filepath );
	bool parseContentXMLFile( char* filepath );
	bool parseBuildingContent( TiXmlElement* elemRoot );
	bool parseCustomBuildingContent( TiXmlElement* elemRoot );
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

	bool Load( DFHack::Context& DF);

	bool reload_configs();

	vector<BuildingConfiguration> buildingConfigs;
	vector<BuildingConfiguration> customBuildingConfigs;
	vector<vector<CreatureConfiguration>*> creatureConfigs;
	vector<VegetationConfiguration> treeConfigs;
	vector<VegetationConfiguration> shrubConfigs;
	vector<VegetationConfiguration> grassConfigs;
	vector<TerrainConfiguration*> terrainFloorConfigs;
	vector<TerrainConfiguration*> terrainBlockConfigs;
	vector<ColorConfiguration> colorConfigs;
	FluidConfiguration lava[8];
	FluidConfiguration water[8];

	vector<string> classIdStrings;
	vector<string> professionStrings;
	std::map <uint32_t, std::string> custom_workshop_types;
	VersionInfo *MemInfo;
	DFHack::Buildings * Bld;
	DFHack::Materials * Mats;
	std::vector<t_matgloss> organic;
	std::vector<t_matgloss> inorganic;

	uint32_t currentTick;
	uint32_t currentYear;
	uint8_t currentMonth;
	uint8_t currentDay;
	uint8_t currentHour;
	uint8_t currentTickRel;

	int stockpileNum;
	int civzoneNum;

	int obsidian;
};

//singleton instance
extern ContentLoader contentLoader;

extern const char* getDocument(TiXmlNode* element);
extern void contentError(const char* message, TiXmlNode* element);
extern char getAnimFrames(const char* framestring);
extern int loadConfigImgFile(const char* filename, TiXmlElement* referrer);
int lookupMaterialType(const char* strValue);
int lookupMaterialIndex(int matType, const char* strValue);
int lookupIndexedType(const char* indexName, vector<t_matgloss>& typeVector);
const char *lookupMaterialTypeName(int matType);
const char *lookupMaterialName(int matType,int matIndex);
uint8_t lookupMaterialFore(int matType,int matIndex);
uint8_t lookupMaterialBack(int matType,int matIndex);
uint8_t lookupMaterialBright(int matType,int matIndex);
const char *lookupTreeName(int matIndex);
ALLEGRO_COLOR lookupMaterialColor(int matType,int matIndex);
const char * lookupFormName(int formType);
ShadeBy getShadeType(const char* Input);
