#include <fstream>
#include "common.h"
#include "ContentLoader.h"
#include "ContentBuildingReader.h"

#include "dfhack/library/tinyxml/tinyxml.h"
#include "GUI.h"

ContentLoader contentLoader;



ContentLoader::ContentLoader(void) { }
ContentLoader::~ContentLoader(void) { }


bool ContentLoader::Load(){
  //flush old config?
  
  flushBuildingConfig(&buildingConfigs);
  creatureConfigs.clear();
  groundConfigs.clear();
  flushImgFiles();
  creatureNameStrings.clear();
  buildingNameStrings.clear();
  unparsedGroundConfigs.clear();

  
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
      LogVerbose("Reading %s...\n", configfilepath);
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

const char* getDocument(TiXmlNode* element)
{
	//walk up the tree to the root
	TiXmlNode* parent = element->Parent();
	while (parent != null)
	{
		element = parent;
		parent = element->Parent();	
	}
	// topmost node *should* be a document, but lets be sure
	parent = dynamic_cast<TiXmlDocument*>(element);
	if (parent == NULL)
		return NULL;
	return parent->Value();
}

void contentError(const char* message, TiXmlNode* element)
{
	WriteErr("%s: %s: %s (Line %d)\n",getDocument(element),message,element->Value(),element->Row());
}

// converts list of characters 0-5 into bits, ignoring garbage
// eg  "035" or "0  3 5" or "0xx3x5" are all good
char getAnimFrames(const char* framestring)
{
	if (framestring == NULL)
		return ALL_FRAMES;
	char aframes=0;
	for (int i=0;i<6;i++)
	{
		if (framestring[i]==0)
			return aframes;
		char temp = framestring[i]-'0';
		if (temp < 0 || temp > 5)
			continue;
		aframes = aframes | (1 << temp);
	}
	return aframes;
}
