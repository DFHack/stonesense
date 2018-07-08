#pragma once

bool addSingleBuildingConfig( TiXmlElement* elemRoot,  std::vector<BuildingConfiguration>* knownBuildings );
void flushBuildingConfig( std::vector<BuildingConfiguration>* knownBuildings );
