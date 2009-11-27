#include <fstream>
#include "common.h"
#include "ContentLoader.h"
#include "ContentBuildingReader.h"

#include "dfhack/library/tinyxml/tinyxml.h"

ContentLoader contentLoader;



ContentLoader::ContentLoader(void) { }
ContentLoader::~ContentLoader(void) { }


bool ContentLoader::Load(){
  //flush old config?
  buildingConfigs.clear();
  
  bool buildingResult = parseContentIndexFile( "buildings/index.txt", "buildings" );
  bool creatureResult = parseContentIndexFile( "creatures/index.txt", "creatures" );
  bool terrainResult = parseContentIndexFile( "terrain/index.txt", "terrain" );
  translationComplete = false;
  return true;
}

bool ContentLoader::parseContentIndexFile( char* filepath, char* homefolder ){
  string line;
  ifstream myfile( filepath );
  if (myfile.is_open() == false){
    WriteErr( "Unable to load config index file at: %s!\n", filepath );
    return false;
  }
  
  while ( !myfile.eof() )
  {
    char configfilepath[50] = {0};
    getline (myfile,line);
    
    //some systems don't remove the \r char as a part of the line change:
    if(line.size() > 0 &&  line[line.size() -1 ] == '\r' )
      line.resize(line.size() -1);

    if(line.size() > 0){
      sprintf(configfilepath, "%s/%s", homefolder, line.c_str() );
      WriteErr("Reading %s...\t\t", configfilepath);
      parseContentXMLFile(configfilepath, homefolder);
    }
  }
  myfile.close();


  return true;
}

bool ContentLoader::parseContentXMLFile( char* filepath, char* homefolder ){
  TiXmlDocument doc( filepath );
  bool loadOkay = doc.LoadFile();
  TiXmlHandle hDoc(&doc);
  TiXmlElement* elemRoot;
  if(!loadOkay)
  {
	  WriteErr("File load failed\n");
	  WriteErr("Line %d: %s\n",doc.ErrorRow(),doc.ErrorDesc());
	  return false;
  }

  bool runningResult = true;
  elemRoot = hDoc.FirstChildElement().Element();
  while( elemRoot ){
    string elementType = elemRoot->Value();
    if( elementType.compare( "building" ) == 0 )
        runningResult &= parseBuildingContent( elemRoot, homefolder );
    
    if( elementType.compare( "creature" ) == 0 )
        runningResult &= parseCreatureContent( elemRoot, homefolder );
    
    if( elementType.compare( "floors" ) == 0 )
        runningResult &= parseTerrainContent( elemRoot, homefolder );

    if( elementType.compare( "walls" ) == 0 )
        runningResult &= parseTerrainContent( elemRoot, homefolder );

    elemRoot = elemRoot->NextSiblingElement();
  }

  return runningResult;
}


bool ContentLoader::parseBuildingContent(TiXmlElement* elemRoot, char *homefolder){
  return addSingleBuildingConfig( elemRoot, &buildingConfigs );
}

bool ContentLoader::parseCreatureContent(TiXmlElement* elemRoot, char *homefolder){
  return addSingleCreatureConfig( elemRoot, &creatureConfigs );
}

bool ContentLoader::parseTerrainContent(TiXmlElement* elemRoot, char *homefolder){
  return addSingleTerrainConfig( elemRoot, &unparsedGroundConfigs );
}


void ContentLoader::TranslateConfigsFromDFAPI( API& DF ){
  //read data from DF
  DF.ReadCreatureMatgloss( creatureNameStrings );
  DF.InitReadBuildings( buildingNameStrings );
  DF.FinishReadBuildings();

  //do translations
  TranslateBuildingNames( buildingConfigs, buildingNameStrings );
  TranslateCreatureNames( creatureConfigs, creatureNameStrings );

  TranslateGroundMaterialNames( groundConfigs, unparsedGroundConfigs );

  translationComplete = true;
}