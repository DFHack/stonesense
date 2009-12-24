#pragma once
#include "dfhack/library/tinyxml/tinyxml.h"
#include "BuildingConfiguration.h"
#include "CreatureConfiguration.h"
#include "VegetationConfiguration.h"
#include "GroundMaterialConfiguration.h"

class ContentLoader{
private:
  bool parseContentIndexFile( char* filepath );
  bool parseContentXMLFile( char* filepath );
  bool parseBuildingContent( TiXmlElement* elemRoot );
  bool parseCreatureContent( TiXmlElement* elemRoot );
  bool parseTerrainContent ( TiXmlElement* elemRoot );
  bool parseTreeContent( TiXmlElement* elemRoot );
  bool parseShrubContent( TiXmlElement* elemRoot );
  void flushCreatureConfig();

  bool translationComplete;
public:
  ContentLoader(void);
  ~ContentLoader(void);

  bool Load(API& DF);
  
  vector<BuildingConfiguration> buildingConfigs;
  vector<vector<CreatureConfiguration>*> creatureConfigs;
  vector<VegetationConfiguration> treeConfigs;
  vector<VegetationConfiguration> shrubConfigs;
  vector<TerrainConfiguration*> terrainFloorConfigs;
  vector<TerrainConfiguration*> terrainBlockConfigs;

  vector<t_matgloss> creatureNameStrings;
  vector<t_matgloss> stoneNameStrings;
  vector<t_matgloss> woodNameStrings;
  vector<t_matgloss> metalNameStrings;
  vector<t_matgloss> plantNameStrings;
  vector<string> buildingNameStrings;
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

