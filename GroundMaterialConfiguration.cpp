#include "common.h"
#include "GroundMaterialConfiguration.h"

#include "dfhack/library/tinyxml/tinyxml.h"

bool GroundMaterialNamesTranslatedFromGame = false;

vector<GroundMaterialConfiguration> groundTypes;
vector<GroundMaterialConfiguration> xmlDefinedGroundTypes;

GroundMaterialConfiguration::GroundMaterialConfiguration(char* gameIDstr, int wallSheetIndex,int floorSheetIndex)
{
  this->wallSheetIndex = wallSheetIndex;
  this->floorSheetIndex = floorSheetIndex;

  int len = (int) strlen(gameIDstr);
  if(len > 100) len = 100;
  memcpy(this->gameIDstr, gameIDstr, len);
  this->gameIDstr[len] = 0;
}

void DumpGroundMaterialNamesToDisk(){
  FILE* fp = fopen("dump.txt", "w");
  if(!fp) return;
  for(uint32_t j=0; j < v_stonetypes.size(); j++){
    fprintf(fp, "%i:%s\n",j, v_stonetypes[j].id);
  }
  fclose(fp);
}


void TranslateGroundMaterialNames(){
  //create and entry for every known ground material type
  uint32_t num = (uint32_t) v_stonetypes.size();
  uint32_t index = 0;
  while( index < num ){
    GroundMaterialConfiguration newConfig( v_stonetypes[index].id, INVALID_INDEX, INVALID_INDEX );
    //add a copy to groundTypes
    groundTypes.push_back( newConfig );
    index++;
  }
  
  //now translate configuration data loaded from the XML files
  uint32_t numConfs = (uint32_t) xmlDefinedGroundTypes.size();
  index = 0;
  while( index < numConfs ){
    char* str = xmlDefinedGroundTypes[index].gameIDstr;
    //find this string id in the material list, and update the entry
    for(uint32_t i=0; i<num; i++){
      if( strcmp( str, groundTypes[i].gameIDstr) != 0) continue;
      groundTypes[i].floorSheetIndex = xmlDefinedGroundTypes[index].floorSheetIndex;
      groundTypes[i].wallSheetIndex = xmlDefinedGroundTypes[index].wallSheetIndex;
      
    }
    index++;
  }
  GroundMaterialNamesTranslatedFromGame = true;
}

void LoadGroundMaterialConfiguration(  ){

  char* filename = "GroundMaterials.xml";
  TiXmlDocument doc( filename );
  bool loadOkay = doc.LoadFile();
  TiXmlHandle hDoc(&doc);
  TiXmlElement* elemMaterial;

  xmlDefinedGroundTypes.clear();

  elemMaterial = hDoc.FirstChildElement("Material").Element();
  while( elemMaterial ){
    const char* name = elemMaterial->Attribute("gameID");
    const char* wallSheetIndexStr = elemMaterial->Attribute("wallSheetIndex");
    const char* floorSheetIndexStr = elemMaterial->Attribute("floorSheetIndex");

    GroundMaterialConfiguration mat( (char*)name, atoi(wallSheetIndexStr), atoi(floorSheetIndexStr) );
    //add a copy to known materials list
    xmlDefinedGroundTypes.push_back( mat );
    
    elemMaterial = elemMaterial->NextSiblingElement("Material");
  }

  GroundMaterialNamesTranslatedFromGame = false;
  
}

