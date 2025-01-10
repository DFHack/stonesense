#include "ColorConfiguration.h"
#include "ContentLoader.h"
#include <vector>

using namespace DFHack;
using namespace df::enums;

using std::string;
using std::vector;

namespace {
    void parseColorElement(TiXmlElement* elemColor, vector<ColorConfiguration> & configTable, MaterialMatcher<ALLEGRO_COLOR> & materialConfig)
    {
        const char* colorRedStr = elemColor->Attribute("red");
        if(colorRedStr == NULL || colorRedStr[0] == 0) {
            contentError("Invalid or missing color attribute",elemColor);
            return; //nothing to work with
        }
        const char* colorGreenStr = elemColor->Attribute("green");
        if(colorGreenStr == NULL || colorGreenStr[0] == 0) {
            contentError("Invalid or missing color attribute",elemColor);
            return; //nothing to work with
        }
        const char* colorBlueStr = elemColor->Attribute("blue");
        if (colorBlueStr == NULL || colorBlueStr[0] == 0) {
            contentError("Invalid or missing color attribute", elemColor);
            return; //nothing to work with
        }
        int alpha = 255;
        const char* colorAlphaStr = elemColor->Attribute("alpha");
        if (colorAlphaStr != NULL && colorAlphaStr[0] != 0) {
            alpha = atoi(colorAlphaStr);
        }

        int red = atoi(colorRedStr);
        int green = atoi(colorGreenStr);
        int blue = atoi(colorBlueStr);
        ALLEGRO_COLOR color = al_map_rgba(red, green, blue, alpha);

        //parse material elements
        TiXmlElement* elemMaterial = elemColor->FirstChildElement("material");
        if(elemMaterial == NULL) {
            //if none, there's nothing to be done with this color.
            contentError("Invalid or missing material attribute",elemColor);
            return;
        }
        for( ; elemMaterial; elemMaterial = elemMaterial->NextSiblingElement("material")) {
            //first try to match with a material token
            const char* elemToken = elemMaterial->Attribute("token");
            if (elemToken && elemToken[0])
            {
                materialConfig.set_material(color, elemToken);
                continue;
            }
            // get material type
            int elemIndex = lookupMaterialType(elemMaterial->Attribute("value"));
            if (elemIndex == INVALID_INDEX) {
                contentError("Invalid or missing value or token attribute",elemMaterial);
                continue;
            }

            // parse subtype elements
            size_t newIndex{ size_t(elemIndex) };
            TiXmlElement* elemSubtype = elemMaterial->FirstChildElement("subtype");
            if (elemSubtype == NULL) {
                // add the configurations
                if (configTable.size() <= newIndex) {
                    //increase size if needed
                    configTable.resize(newIndex+1);
                }
                if(configTable.at(newIndex).colorSet == false) {
                    configTable.at(newIndex).color = color;
                    configTable.at(newIndex).colorSet = true;
                }
                return;
            }
            for ( ; elemSubtype; elemSubtype = elemSubtype ->NextSiblingElement("subtype")) {
                // get subtype
                int subtypeId = lookupMaterialIndex( newIndex,elemSubtype->Attribute("value"));
                if (subtypeId == INVALID_INDEX) {
                    contentError("Invalid or missing value attribute",elemSubtype);
                    continue;
                }

                size_t newSubtypeIndex{ size_t(subtypeId) };
                // add the configurations
                if (configTable.size() <= newSubtypeIndex) {
                    //increase size if needed
                    configTable.resize(newIndex+1);
                }

                if (configTable.at(newIndex).colorMaterials.size() <= newSubtypeIndex) {
                    //increase size if needed
                    configTable.at(newIndex).colorMaterials.resize(newSubtypeIndex+1);
                }
                if (configTable.at(newIndex).colorMaterials.at(newSubtypeIndex).colorSet == false) {
                    configTable.at(newIndex).colorMaterials.at(newSubtypeIndex).color = color;
                    configTable.at(newIndex).colorMaterials.at(newSubtypeIndex).colorSet = true;
                }
            }
        }
    }

}

bool addSingleColorConfig( TiXmlElement* elemRoot)
{
    string elementType = elemRoot->Value();
    if(elementType.compare( "colors" ) == 0) {
        //parse colors
        TiXmlElement* elemColor = elemRoot->FirstChildElement("color");
        while( elemColor ) {
            parseColorElement( elemColor, contentLoader->colorConfigs, contentLoader->materialColorConfigs);
            elemColor = elemColor->NextSiblingElement("color");
        }
    }
    return true;
}
