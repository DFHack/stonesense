#pragma once


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


//void LoadGroundMaterialConfiguration( );
//void TranslateGroundMaterialNames();

extern bool GroundMaterialNamesTranslatedFromGame;
extern vector<GroundMaterialConfiguration*> groundTypes;