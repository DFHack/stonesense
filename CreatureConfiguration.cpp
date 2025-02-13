#include "common.h"
#include "CreatureConfiguration.h"
#include "Creatures.h"
#include "MapLoading.h"
#include "GUI.h"
#include "ContentLoader.h"
#include "GameConfiguration.h"
#include "StonesenseState.h"

#include "tinyxml.h"

using std::string;
using std::vector;

namespace {
    int translateProfession(const char* currentProf)
    {
        if (currentProf == NULL || currentProf[0] == 0) {
            return INVALID_INDEX;
        }
        auto& contentLoader = stonesenseState.contentLoader;

        auto it = std::find(
            contentLoader->professionStrings.begin(),
            contentLoader->professionStrings.end(),
            currentProf);
        if (it != contentLoader->professionStrings.end())
            return it - contentLoader->professionStrings.begin();

        LogVerbose("Unable to match profession '%s' to anything in-game\n", currentProf);
        return INT_MAX; //if it is left at INVALID_INDEX, the condition is ignored entierly.
    }

    void pushCreatureConfig(vector<std::unique_ptr<vector<CreatureConfiguration>>>&knownCreatures, unsigned int gameID, CreatureConfiguration & cre)
    {
        auto& ssConfig = stonesenseState.ssConfig;

        if (!ssConfig.skipCreatureTypes) {
            if (knownCreatures.size() <= size_t(gameID)) {
                //resize using hint from creature name list
                size_t newsize = size_t(gameID) + 1;
                auto& contentLoader = stonesenseState.contentLoader;
                if (newsize <= contentLoader->Mats->race.size()) {
                    newsize = contentLoader->Mats->race.size() + 1;
                }
                while (knownCreatures.size() < newsize) {
                    knownCreatures.push_back(nullptr);
                }
            }
            auto& creatureList = knownCreatures[gameID];
            if (creatureList == nullptr) {
                creatureList = std::make_unique<vector<CreatureConfiguration>>();
            }
            creatureList->push_back(cre);
        }
    }

    bool addSingleCreatureConfig(TiXmlElement * elemCreature, vector<std::unique_ptr<vector<CreatureConfiguration>>>&knownCreatures, int basefile)
    {
        using df::pronoun_type;
        auto& ssConfig = stonesenseState.ssConfig;

        if (ssConfig.skipCreatureTypes) {
            return false;
        }
        auto& contentLoader = stonesenseState.contentLoader;
        int gameID = lookupIndexedType(elemCreature->Attribute("gameID"), contentLoader->Mats->race);
        if (gameID == INVALID_INDEX) {
            return false;
        }
        const char* sheetIndexStr;
        int defaultFile = basefile;
        c_sprite sprite;
        sprite.set_fileindex(basefile);
        uint8_t baseShadow = DEFAULT_SHADOW;
        const char* shadowStr = elemCreature->Attribute("shadow");
        if (shadowStr != NULL && shadowStr[0] != 0) {
            baseShadow = atoi(shadowStr);
        }
        if (baseShadow > MAX_SHADOW) {
            baseShadow = DEFAULT_SHADOW;
        }
        const char* filename = elemCreature->Attribute("file");
        if (filename != NULL && filename[0] != 0) {
            defaultFile = loadConfigImgFile((char*)filename, elemCreature);
            if (defaultFile == -1) {
                return false;
            }
        }
        TiXmlElement* elemVariant = elemCreature->FirstChildElement("variant");
        while (elemVariant) {
            int professionID = INVALID_INDEX;
            const char* profStr = elemVariant->Attribute("prof");
            if (profStr == NULL || profStr[0] == 0) {
                profStr = elemVariant->Attribute("profession");
            }
            professionID = translateProfession(profStr);

            const char* customStr = elemVariant->Attribute("custom");
            if (customStr != NULL && customStr[0] == 0) {
                customStr = NULL;
            }

            if (customStr != NULL) {
                LogError("custom: %s\n", customStr);
            }

            const char* sexstr = elemVariant->Attribute("sex");
            df::pronoun_type cresex = pronoun_type::it;
            if (sexstr) {
                if (strcmp(sexstr, "M") == 0) {
                    cresex = pronoun_type::he;
                }
                if (strcmp(sexstr, "F") == 0) {
                    cresex = pronoun_type::she;
                }
            }
            int caste = -1;
            const char* caststr = elemVariant->Attribute("caste");
            if (caststr != NULL && caststr[0] != 0) {
                caste = lookupIndexedType(caststr, contentLoader->Mats->raceEx[gameID].castes);
            }
            const char* specstr = elemVariant->Attribute("special");
            enumCreatureSpecialCases crespec = eCSC_Any;
            if (specstr) {
                if (strcmp(specstr, "Normal") == 0) {
                    crespec = eCSC_Normal;
                }
                if (strcmp(specstr, "Ghost") == 0) {
                    crespec = eCSC_Ghost;
                }
                if (strcmp(specstr, "Military") == 0) {
                    crespec = eCSC_Military;
                }
            }

            int shadow = baseShadow;
            const char* shadowStr = elemVariant->Attribute("shadow");
            if (shadowStr != NULL && shadowStr[0] != 0) {
                shadow = atoi(shadowStr);
            }
            if (shadow < 0 || shadow > MAX_SHADOW) {
                shadow = baseShadow;
            }

            sprite.set_by_xml(elemVariant, defaultFile, gameID, caste);
            sprite.animate = 0;
            CreatureConfiguration cre(professionID, customStr, cresex, caste, crespec, sprite, shadow);
            //add a copy to known creatures
            pushCreatureConfig(knownCreatures, gameID, cre);
            elemVariant = elemVariant->NextSiblingElement("variant");
        }

        sheetIndexStr = elemCreature->Attribute("sheetIndex");
        if (sheetIndexStr) {
            sprite.set_by_xml(elemCreature, basefile);
            CreatureConfiguration cre(INVALID_INDEX, NULL, pronoun_type::it, INVALID_INDEX, eCSC_Any, sprite, baseShadow);
            //add a copy to known creatures
            pushCreatureConfig(knownCreatures, gameID, cre);
        }
        return true;
    }
}

bool addCreaturesConfig( TiXmlElement* elemRoot, vector<std::unique_ptr<vector<CreatureConfiguration>>>& knownCreatures )
{
    int basefile = -1;
    const char* filename = elemRoot->Attribute("file");
    if (filename != NULL && filename[0] != 0) {
        basefile = loadConfigImgFile((char*)filename,elemRoot);
        if(basefile == -1) {
            return false;
        }
    }
    TiXmlElement* elemCreature = elemRoot->FirstChildElement("creature");
    if (elemCreature == NULL) {
        contentError("No creatures found",elemRoot);
        return false;
    }
    while( elemCreature ) {
        addSingleCreatureConfig(elemCreature,knownCreatures,basefile );
        elemCreature = elemCreature->NextSiblingElement("creature");
    }
    return true;
}
