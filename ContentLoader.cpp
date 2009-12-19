#include <fstream>
#include "common.h"
#include "ContentLoader.h"
#include "ContentBuildingReader.h"
#include "MapLoading.h"

#include "dfhack/library/tinyxml/tinyxml.h"
#include "GUI.h"

ContentLoader contentLoader;



ContentLoader::ContentLoader(void) { }
ContentLoader::~ContentLoader(void)
{ 
  //flush content on exit
  flushBuildingConfig(&buildingConfigs);
  flushTerrainConfig(terrainFloorConfigs);
  flushTerrainConfig(terrainBlockConfigs);	
}


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
  bool overallResult = parseContentIndexFile( "index.txt" );
  translationComplete = false;

  return true;
}

// takes a filename and the file referring to it, and makes a combined filename in
// HTML style: (ie "/something" is relative to the stonesense root, everything
// else is relative to the referrer)
// buffer must be FILENAME_BUFFERSIZE chars
// returns true if it all works
bool getLocalFilename(char* buffer, const char* filename, const char* relativeto)
{
	char filetemp[FILENAME_BUFFERSIZE_LOCAL] = {0};
	char hometemp[FILENAME_BUFFERSIZE_LOCAL] = {0};	
	// allegro will avoid writing off the end of the buffer, but wont *tell* me
	// that the resulting filename is worthless
	// these give me a check of my own
	filetemp[FILENAME_BUFFERSIZE_LOCAL-1] = 1;
	hometemp[FILENAME_BUFFERSIZE_LOCAL-1] = 1;	
	buffer[FILENAME_BUFFERSIZE-1] = 1;
		
	char* buffertest;
	if (filename[0] == '/' || filename[0] == '\\')
	{
		buffertest = canonicalize_filename (filetemp, (filename+1), FILENAME_BUFFERSIZE_LOCAL);
		if (!buffertest || filetemp[FILENAME_BUFFERSIZE_LOCAL-1] != 1)
		{
			WriteErr("Failed to build path for: %s\n",filename);
			return false;
		}
	}
	else
	{
		buffertest = replace_filename (filetemp, relativeto, filename, FILENAME_BUFFERSIZE_LOCAL);
		if (!buffertest || filetemp[FILENAME_BUFFERSIZE_LOCAL-1] != 1)
		{
			WriteErr("Failed to build path for: %s\n",filename);
			return false;
		}
		buffertest = canonicalize_filename (filetemp, filetemp, FILENAME_BUFFERSIZE_LOCAL);
		if (!buffertest || filetemp[FILENAME_BUFFERSIZE_LOCAL-1] != 1)
		{
			WriteErr("Failed to build path for: %s\n",filename);
			return false;
		}
	}
	buffertest = canonicalize_filename (hometemp,"", FILENAME_BUFFERSIZE_LOCAL);
	if (!buffertest || hometemp[FILENAME_BUFFERSIZE_LOCAL-1] != 1)
	{
		WriteErr("Failed to build path for: %s\n",filename);
		return false;
	}
	buffertest = make_relative_filename (buffer,hometemp,filetemp, FILENAME_BUFFERSIZE);
	if (!buffertest || buffer[FILENAME_BUFFERSIZE-1] != 1)
	{
		WriteErr("Failed to build path for: %s\n",filename);
		return false;
	}
	return true;
}

bool ContentLoader::parseContentIndexFile( char* filepath )
{
  string line;
  ifstream myfile( filepath );
  if (myfile.is_open() == false){
    WriteErr( "Unable to load config index file at: %s!\n", filepath );
    return false;
  }
  LogVerbose("Reading index at %s...\n", filepath);
  
  while ( !myfile.eof() )
  {
    char configfilepath[FILENAME_BUFFERSIZE] = {0};

    getline (myfile,line);
     
    // some systems don't remove the \r char as a part of the line change:
    // also trim trailing space
    int resize = (int)line.size()-1;
    for (;resize>0;resize--)
    {
	    char test = line[resize];
	    if (test == '\r')
	    	continue;
	    if (test == '\t')
	    	continue;
	    if (test == ' ')
	    	continue;
	    break;
    }
 	if (resize <= 0)
 		continue;
    line.resize(resize+1);
	
    // allow comments
    if (line[0] == '#')
    	continue;
    
	if (!getLocalFilename(configfilepath,line.c_str(),filepath))
	{
		WriteErr("File name parsing failed on %s\n",line.c_str());
		continue;
	}
	char* extension = get_extension(configfilepath);
	if (strcmp(extension,"xml") == 0)
	{
	  LogVerbose("Reading xml %s...\n", configfilepath);
	  if (!parseContentXMLFile(configfilepath))
	  	WriteErr("Failure in reading %s\n",configfilepath);		  
	}
	else if (strcmp(extension,"txt") == 0)
	{
	  LogVerbose("Reading index %s...\n", configfilepath);
	  if (!parseContentIndexFile(configfilepath))
	  	WriteErr("Failure in reading %s\n",configfilepath);			  
	}
	else
	{
		WriteErr("Invalid filename: %s\n",configfilepath);
	}
  }
  myfile.close();

  return true;
}

bool ContentLoader::parseContentXMLFile( char* filepath ){
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
        runningResult &= parseBuildingContent( elemRoot );
    
    if( elementType.compare( "creatures" ) == 0 )
        runningResult &= parseCreatureContent( elemRoot );
    
    if( elementType.compare( "floors" ) == 0 )
        runningResult &= parseTerrainContent( elemRoot );

    if( elementType.compare( "blocks" ) == 0 )
        runningResult &= parseTerrainContent( elemRoot );

    if( elementType.compare( "shrubs" ) == 0 )
        runningResult &= parseShrubContent( elemRoot );

    if( elementType.compare( "trees" ) == 0 )
        runningResult &= parseTreeContent( elemRoot );

    elemRoot = elemRoot->NextSiblingElement();
  }

  return runningResult;
}


bool ContentLoader::parseBuildingContent(TiXmlElement* elemRoot ){
  return addSingleBuildingConfig( elemRoot, &buildingConfigs );
}

bool ContentLoader::parseCreatureContent(TiXmlElement* elemRoot ){
  return addCreaturesConfig( elemRoot, &creatureConfigs );
}

bool ContentLoader::parseShrubContent(TiXmlElement* elemRoot ){
  return addSingleVegetationConfig( elemRoot, &shrubConfigs, plantNameStrings );
}

bool ContentLoader::parseTreeContent(TiXmlElement* elemRoot ){
  return addSingleVegetationConfig( elemRoot, &treeConfigs, woodNameStrings );
}

bool ContentLoader::parseTerrainContent(TiXmlElement* elemRoot ){
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

int loadConfigImgFile(const char* filename, TiXmlElement* referrer)
{
	const char* documentRef = getDocument(referrer);
	char configfilepath[FILENAME_BUFFERSIZE] = {0};
	if (!getLocalFilename(configfilepath,filename,documentRef))
	{
		contentError("Failed to parse sprites filename",referrer);
		return -1;
	}
	return loadImgFile(configfilepath);
}
