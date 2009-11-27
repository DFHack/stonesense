#pragma once
#include "dfhack/library/tinyxml/tinyxml.h"
#include "BuildingConfiguration.h"
#include "CreatureConfiguration.h"

class ContentLoader{
private:
  bool parseContentIndexFile( char* filepath, char* homefolder  );
  bool parseContentXMLFile( char* filepath, char* homefolder  );
  bool parseBuildingContent( TiXmlElement* elemRoot, char* homefolder  );
  bool parseCreatureContent( TiXmlElement* elemRoot, char* homefolder  );

  bool translationComplete;
public:
  ContentLoader(void);
  ~ContentLoader(void);

  bool Load();
  void TranslateConfigsFromDFAPI( API& DF );
  bool Translated(){ return translationComplete; }
  
  vector<BuildingConfiguration> buildingConfigs;
  vector<CreatureConfiguration> creatureConfigs;


  vector<t_matgloss> creatureNameStrings;
  vector<string> buildingNameStrings;
};

//singleton instance
extern ContentLoader contentLoader;
