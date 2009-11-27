#pragma once

#include "dfhack/library/tinyxml/tinyxml.h"

typedef struct OverridingMaterial{
	int gameID;
	int spriteIndex;
  int fillerFloorSpriteIndex;
}OverridingMaterial;

class GroundMaterialConfiguration
{
public:
  int defaultSprite;
  int fillerFloorSpriteIndex;
	vector<OverridingMaterial> overridingMaterials;

  GroundMaterialConfiguration( );
  //GroundMaterialConfiguration(char* gameIDstr, int wallSheetIndex,int floorSheetIndex);
  ~GroundMaterialConfiguration(){}
};



class preparseGroundMaterialConfiguration
{
public:
	int spriteIndex;
  int fillerFloorSpriteIndex;
  vector<int> wallFloorIDs;
	vector<string> overridingMaterials;
};

bool addSingleTerrainConfig( TiXmlElement* elemRoot,  vector<preparseGroundMaterialConfiguration>* rawGroundTypes );
void TranslateGroundMaterialNames(vector<GroundMaterialConfiguration*>& configs, vector<preparseGroundMaterialConfiguration>& rawGroundConfigs);
//void LoadGroundMaterialConfiguration( );
//void TranslateGroundMaterialNames();

//extern bool GroundMaterialNamesTranslatedFromGame;
//extern vector<GroundMaterialConfiguration*> groundTypes;