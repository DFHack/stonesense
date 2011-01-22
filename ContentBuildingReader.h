#pragma once

bool addSingleBuildingConfig( TiXmlElement* elemRoot,  vector<BuildingConfiguration>* knownBuildings );
void flushBuildingConfig( vector<BuildingConfiguration>* knownBuildings );
