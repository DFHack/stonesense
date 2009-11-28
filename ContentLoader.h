#pragma once
#include "dfhack/library/tinyxml/tinyxml.h"
#include "BuildingConfiguration.h"
#include "CreatureConfiguration.h"
#include "GroundMaterialConfiguration.h"

class ContentLoader{
private:
  bool parseContentIndexFile( char* filepath, char* homefolder  );
  bool parseContentXMLFile( char* filepath, char* homefolder  );
  bool parseBuildingContent( TiXmlElement* elemRoot, char* homefolder  );
  bool parseCreatureContent( TiXmlElement* elemRoot, char* homefolder  );
  bool parseTerrainContent ( TiXmlElement* elemRoot, char* homefolder  );

  bool translationComplete;
public:
  ContentLoader(void);
  ~ContentLoader(void);

  bool Load();
  void TranslateConfigsFromDFAPI( API& DF );
  bool Translated(){ return translationComplete; }
  
  vector<BuildingConfiguration> buildingConfigs;
  vector<CreatureConfiguration> creatureConfigs;
  vector<GroundMaterialConfiguration*> groundConfigs;

  vector<t_matgloss> creatureNameStrings;
  vector<string> buildingNameStrings;
  vector<preparseGroundMaterialConfiguration> unparsedGroundConfigs;
};

//singleton instance
extern ContentLoader contentLoader;

extern const char* getDocument(TiXmlNode* element);
extern void contentError(const char* message, TiXmlNode* element);
