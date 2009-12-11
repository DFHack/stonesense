#include <fstream>
#include "common.h"
#include "ContentLoader.h"
#include "ContentBuildingReader.h"
#include "MapLoading.h"

#include "dfhack/library/tinyxml/tinyxml.h"
#include "GUI.h"

ContentLoader contentLoader;



ContentLoader::ContentLoader(void) { }
ContentLoader::~ContentLoader(void) { }


bool ContentLoader::Load(API& DF){
	
  //flush old config
  flushBuildingConfig(&buildingConfigs);
  flushTerrainConfig(terrainFloorConfigs);
  flushTerrainConfig(terrainBlockConfigs);
  creatureConfigs.clear();
  treeConfigs.clear();
  shrubConfigs.clear();
  flushImgFiles();
  creatureNameStrings.clear();
  woodNameStrings.clear();
  plantNameStrings.clear();
  buildingNameStrings.clear();
  
  /// BAD
//  SUSPEND_DF;
  
  //read data from DF
  DF.ReadCreatureMatgloss( creatureNameStrings );
  DF.InitReadBuildings( buildingNameStrings );
  DF.FinishReadBuildings();
  //read stone material types
  DF.ReadStoneMatgloss( stoneNameStrings ); 
  DF.ReadMetalMatgloss( metalNameStrings );
  DF.ReadWoodMatgloss( woodNameStrings );
  DF.ReadPlantMatgloss( plantNameStrings );
  
//  RESUME_DF;
  
  bool buildingResult = parseContentIndexFile( "buildings/index.txt", "buildings" );
  bool creatureResult = parseContentIndexFile( "creatures/index.txt", "creatures" ); 
  bool terrainResult = parseContentIndexFile( "terrain/index.txt", "terrain" );
  bool plantResult = parseContentIndexFile( "vegetation/index.txt", "vegetation" );
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
  
  LogVerbose("Reading index at %s...\n", filepath);
  
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
      if (!parseContentXMLFile(configfilepath, homefolder))
      	WriteErr("Failure in reading %s\n",configfilepath);
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
    
    if( elementType.compare( "creatures" ) == 0 )
        runningResult &= parseCreatureContent( elemRoot, homefolder );
    
    if( elementType.compare( "floors" ) == 0 )
        runningResult &= parseTerrainContent( elemRoot, homefolder );

    if( elementType.compare( "blocks" ) == 0 )
        runningResult &= parseTerrainContent( elemRoot, homefolder );

    if( elementType.compare( "shrubs" ) == 0 )
        runningResult &= parseShrubContent( elemRoot, homefolder );

    if( elementType.compare( "trees" ) == 0 )
        runningResult &= parseTreeContent( elemRoot, homefolder );

    elemRoot = elemRoot->NextSiblingElement();
  }

  return runningResult;
}


bool ContentLoader::parseBuildingContent(TiXmlElement* elemRoot, char *homefolder){
  return addSingleBuildingConfig( elemRoot, &buildingConfigs );
}

bool ContentLoader::parseCreatureContent(TiXmlElement* elemRoot, char *homefolder){
  return addCreaturesConfig( elemRoot, &creatureConfigs );
}

bool ContentLoader::parseShrubContent(TiXmlElement* elemRoot, char *homefolder){
  return addSingleVegetationConfig( elemRoot, &shrubConfigs, plantNameStrings );
}

bool ContentLoader::parseTreeContent(TiXmlElement* elemRoot, char *homefolder){
  return addSingleVegetationConfig( elemRoot, &treeConfigs, woodNameStrings );
}

bool ContentLoader::parseTerrainContent(TiXmlElement* elemRoot, char *homefolder){
  return addSingleTerrainConfig( elemRoot );
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

int lookupMaterialType(const char* strValue)
{
	if (strValue == NULL || strValue[0] == 0)
		return INVALID_INDEX;
	if( strcmp(strValue, "Wood") == 0)
      return Mat_Wood;
    else if( strcmp(strValue, "Stone") == 0)
      return Mat_Stone;
    else if( strcmp(strValue, "Metal") == 0)
      return Mat_Metal;
     //TODO this needs fixing on dfhack side
    else if( strcmp(strValue, "Bone") == 0)
      return Mat_Plant;
    else if( strcmp(strValue, "Leather") == 0)
      return Mat_Leather;
    else if( strcmp(strValue, "Silk") == 0)
      return Mat_SilkCloth;
    else if( strcmp(strValue, "PlantCloth") == 0)
      return Mat_PlantCloth;
    else if( strcmp(strValue, "GreenGlass") == 0)
      return Mat_GreenGlass;
    else if( strcmp(strValue, "ClearGlass") == 0)
      return Mat_ClearGlass;
    else if( strcmp(strValue, "CrystalGlass") == 0)
      return Mat_CrystalGlass;
    else if( strcmp(strValue, "Ice") == 0)
      return Mat_Ice;
    else if( strcmp(strValue, "Charcoal") == 0)
      return Mat_Charcoal;
    else if( strcmp(strValue, "Soap") == 0) //you know you want it
      return Mat_Soap;
     return INVALID_INDEX;
}

int lookupIndexedType(const char* indexName, vector<t_matgloss>& typeVector)
{
	if (indexName == NULL || indexName[0] == 0)
	{
		return INVALID_INDEX;	
	}
	uint32_t vsize = (uint32_t)typeVector.size();
	for(uint32_t i=0; i < vsize; i++){
	if (strcmp(indexName,typeVector[i].id) == 0)
		return i;
	}
	return INVALID_INDEX;
}

int lookupMaterialIndex(int matType, const char* strValue)
{
	vector<t_matgloss>* typeVector;
	// for appropriate elements, look up subtype
	if (matType == Mat_Wood)
	{
		typeVector=&(contentLoader.woodNameStrings);
	}
	else if (matType == Mat_Stone)
	{
		typeVector=&(contentLoader.stoneNameStrings);
	}
	else if (matType == Mat_Metal)
	{
		typeVector=&(contentLoader.metalNameStrings);
	}
	else
	{
		//maybe allow some more in later
		return INVALID_INDEX;
	}
	return lookupIndexedType(strValue,*typeVector);
}
