#pragma once

class GroundMaterialConfiguration
{
public:
  char gameIDstr[100];
  int wallSheetIndex;
  int floorSheetIndex;

  GroundMaterialConfiguration(char* gameIDstr, int wallSheetIndex,int floorSheetIndex);
  ~GroundMaterialConfiguration(){}
};

typedef struct OverridingMaterial{
	int id;
	int spriteIndex;
}OverridingMaterial;

class FloorConfiguration
{
public:
	int defaultSprite;
	vector<OverridingMaterial> overridingMaterials;
};


void LoadGroundMaterialConfiguration( );
void TranslateGroundMaterialNames();

extern bool GroundMaterialNamesTranslatedFromGame;
extern vector<GroundMaterialConfiguration> groundTypes;