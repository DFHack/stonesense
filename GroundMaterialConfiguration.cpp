#include "common.h"
#include "SpriteMaps.h"
#include "GroundMaterialConfiguration.h"
#include <set>
#include "tinyxml.h"
#include "GUI.h"
#include "ContentLoader.h"
#include "EnumToString.h"
#include "MiscUtils.h"

using namespace std;
using namespace DFHack;
using namespace df::enums;

#define PRIORITY_SHAPE 8
#define PRIORITY_SPECIAL 4
#define PRIORITY_VARIANT 2
#define PRIORITY_MATERIAL 1
#define PRIORITY_TOTAL (PRIORITY_SHAPE+PRIORITY_SPECIAL+PRIORITY_VARIANT+PRIORITY_MATERIAL+1)

TerrainMaterialConfiguration::TerrainMaterialConfiguration()
{
    overridingMaterials.resize(NUM_FORMS);
    defaultSprite.resize(NUM_FORMS);
    for (int i = 0; i < NUM_FORMS; i++) {
        defaultSprite[i].first.set_fileindex(INVALID_INDEX);
        defaultSprite[i].first.set_sheetindex(UNCONFIGURED_INDEX);
        defaultSprite[i].second = INVALID_INDEX;
    }
    //dont really care about the rest of the sprite right now.

}

TerrainConfiguration::TerrainConfiguration()
{
    defaultSprite.resize(NUM_FORMS);
    for (int i = 0; i < NUM_FORMS; i++) {
        defaultSprite[i].first.set_fileindex(INVALID_INDEX);
        defaultSprite[i].first.set_sheetindex(UNCONFIGURED_INDEX);
        defaultSprite[i].second = INVALID_INDEX;
    }
    //dont really care about the rest of the sprite right now.
}

void DumpInorganicMaterialNamesToDisk()
{
    FILE* fp = fopen("dump.txt", "w");
    if (!fp) {
        return;
    }
    for (uint32_t j = 0; j < contentLoader->inorganic.size(); j++) {
        fprintf(fp, "%i:%s\n", j, contentLoader->inorganic[j].id.c_str());
    }
    fclose(fp);
}

void parseWallFloorSpriteElement(TiXmlElement* elemWallFloorSprite, vector<std::unique_ptr<TerrainConfiguration>>& configTable, int basefile, bool floor)
{
    const char* spriteSheetIndexStr = elemWallFloorSprite->Attribute("sheetIndex");
    const char* spriteSpriteStr = elemWallFloorSprite->Attribute("sprite");
    const char* spriteIndexStr = elemWallFloorSprite->Attribute("index");
    if ((spriteSheetIndexStr == NULL || spriteSheetIndexStr[0] == 0) && (spriteSpriteStr == NULL || spriteSpriteStr[0] == 0) && (spriteIndexStr == NULL || spriteIndexStr[0] == 0)) {
        contentError("Invalid or missing sprite attribute", elemWallFloorSprite);
        return; //nothing to work with
    }
    // make a base sprite
    c_sprite sprite;
    if (floor) {
        sprite.set_size(SPRITEWIDTH, (TILETOPHEIGHT + FLOORHEIGHT));
        sprite.set_offset(0, (WALLHEIGHT));
    }
    sprite.set_needoutline(1);
    sprite.set_by_xml(elemWallFloorSprite, basefile);

    vector<pair<int, int>> lookupKeys;

    // look through terrain elements
    for (TiXmlElement* elemTerrain = elemWallFloorSprite->FirstChildElement("terrain");
        elemTerrain;
        elemTerrain = elemTerrain->NextSiblingElement("terrain")) {
        //get a terrain type
        df::tiletype targetElem = df::tiletype(INVALID_INDEX);
        const char* gameIDstr = elemTerrain->Attribute("value");
        if (!(gameIDstr == NULL || gameIDstr[0] == 0))
        {
            targetElem = df::tiletype(atoi(gameIDstr));
        }
        if (targetElem >= 0)
        {
            char buf[500];
            if (is_valid_enum_item(targetElem))
            {
                auto shape = ENUM_ATTR(tiletype, shape, targetElem);
                auto special = ENUM_ATTR(tiletype, special, targetElem);
                auto variant = ENUM_ATTR(tiletype, variant, targetElem);
                auto material = ENUM_ATTR(tiletype, material, targetElem);
                sprintf(buf, "Use of deprecated terrain value \"%d\", use one of the following instead:\n <terrain token = \"%s\" />\n <terrain%s%s%s%s%s%s%s%s%s%s%s%s />\n in element",
                targetElem,
                enum_item_key_str(targetElem),
                shape == tiletype_shape::NONE ? "" : " shape = \"",
                shape == tiletype_shape::NONE ? "" : enum_item_key_str(shape),
                shape == tiletype_shape::NONE ? "" : "\"",
                special == tiletype_special::NONE ? "" : " special = \"",
                special == tiletype_special::NONE ? "" : enum_item_key_str(special),
                special == tiletype_special::NONE ? "" : "\"",
                variant == tiletype_variant::NONE ? "" : " variant = \"",
                variant == tiletype_variant::NONE ? "" : enum_item_key_str(variant),
                variant == tiletype_variant::NONE ? "" : "\"",
                material == tiletype_material::NONE ? "" : " material = \"",
                material == tiletype_material::NONE ? "" : enum_item_key_str(material),
                material == tiletype_material::NONE ? "" : "\""
                );
            }
            else
                sprintf(buf, "Terrain value \"%d\" is invalid", targetElem);
            contentError(buf, elemTerrain);
        }
        const char* gameTokenstr = elemTerrain->Attribute("token");
        df::tiletype_shape elemShape = StringToTiletypeShape(elemTerrain->Attribute("shape"));
        df::tiletype_special elemSpecial = StringToTiletypeSpecial(elemTerrain->Attribute("special"));
        df::tiletype_variant elemVariant = StringToTiletypeVariant(elemTerrain->Attribute("variant"));
        df::tiletype_material elemMaterial = StringToTiletypeMaterial(elemTerrain->Attribute("material"));

        FOR_ENUM_ITEMS(tiletype, i)
        {
            bool valid = true;
            int matchness = INVALID_INDEX;
            if (targetElem >= 0)
            {
                if (i == targetElem)
                    matchness = 0;
                else
                    valid = false;
            }
            if (!(gameTokenstr == NULL || gameTokenstr[0] == 0))
            {
                if (enum_item_key(i) == gameTokenstr)
                    matchness = 0;
                else
                    valid = false;
            }
            if (matchness != 0) //this means there's no exact match made.
            {
                int partialMatch = 0;
                if (elemShape != tiletype_shape::NONE)
                {
                    if (ENUM_ATTR(tiletype, shape, i) == elemShape)
                        partialMatch += PRIORITY_SHAPE;
                    else
                        valid = false;
                }
                if (elemSpecial != tiletype_special::NONE)
                {
                    if (ENUM_ATTR(tiletype, special, i) == elemSpecial)
                        partialMatch += PRIORITY_SPECIAL;
                    else
                        valid = false;
                }
                if (elemVariant != tiletype_variant::NONE)
                {
                    if (ENUM_ATTR(tiletype, variant, i) == elemVariant)
                        partialMatch += PRIORITY_VARIANT;
                    else
                        valid = false;
                }
                if (elemMaterial != tiletype_material::NONE)
                {
                    if (ENUM_ATTR(tiletype, material, i) == elemMaterial)
                        partialMatch += PRIORITY_MATERIAL;
                    else
                        valid = false;
                }
                if (partialMatch > 0 && valid)
                    matchness = PRIORITY_TOTAL - partialMatch;
            }
            if (matchness >= 0 && valid)
            {
                //add it to the lookup vector
                lookupKeys.push_back(make_pair(i, matchness));
                //increase size if needed
                while (configTable.size() <= (uint32_t)i) {
                    configTable.push_back(nullptr);
                }

                if (configTable[i] == nullptr) {
                    configTable[i] = std::make_unique<TerrainConfiguration>();
                }
            }
        }
    }

    // check we have some terrain types set
    int elems = (int)lookupKeys.size();
    if (elems == 0) {
        return;    //nothing to link to
    }

    vector<bool> formToggle;
    formToggle.resize(NUM_FORMS);
    // parse weather plate is for a block, log, etc
    TiXmlElement* elemForm = elemWallFloorSprite->FirstChildElement("form");
    if (elemForm == NULL) {
        formToggle[0] = true;
    }
    for (; elemForm; elemForm = elemForm->NextSiblingElement("form")) {
        const char * strForm = elemForm->Attribute("value");

        if (strcmp(strForm, "bar") == 0) {
            formToggle[FORM_BAR] = true;
        }
        if (strcmp(strForm, "block") == 0) {
            formToggle[FORM_BLOCK] = true;
        }
        if (strcmp(strForm, "boulder") == 0) {
            formToggle[FORM_BOULDER] = true;
        }
        if (strcmp(strForm, "log") == 0) {
            formToggle[FORM_LOG] = true;
        }
    }
    // parse material elements
    TiXmlElement* elemMaterial = elemWallFloorSprite->FirstChildElement("material");
    if (elemMaterial == NULL) {
        // if none, set default terrain sprites for each terrain type
        for (int i = 0; i < elems; i++) {
            TerrainConfiguration *tConfig = configTable[lookupKeys[i].first].get();
            // if that was null we have *really* screwed up earlier
            // only update if not by previous configs
            for (int j = 0; j < NUM_FORMS; j++) {
                if (formToggle[j])
                if (tConfig->defaultSprite[j].second == INVALID_INDEX || tConfig->defaultSprite[j].second > lookupKeys[i].second) {
                    tConfig->defaultSprite[j].first = sprite;
                    tConfig->defaultSprite[j].second = lookupKeys[i].second;
                }
            }
        }
    }
    for (; elemMaterial; elemMaterial = elemMaterial->NextSiblingElement("material")) {
        // get material type
        int elemIndex = lookupMaterialType(elemMaterial->Attribute("value"));
        if (elemIndex == INVALID_INDEX) {
            contentError("Invalid or missing value attribute", elemMaterial);
            continue;
        }

        // parse subtype elements
        TiXmlElement* elemSubtype = elemMaterial->FirstChildElement("subtype");
        if (elemSubtype == NULL) {
            // if none, set material default for each terrain type
            for (int i = 0; i < elems; i++) {
                TerrainConfiguration *tConfig = configTable[lookupKeys[i].first].get();
                // if that was null we have *really* screwed up earlier
                // create a new TerrainMaterialConfiguration if required
                // make sure we have room for it first
                while (tConfig->terrainMaterials.size() <= (uint32_t)elemIndex) {
                    // dont make a full size vector in advance- most of the time
                    // we will only need the first few
                    tConfig->terrainMaterials.push_back(nullptr);
                }
                if (tConfig->terrainMaterials[elemIndex] == nullptr) {
                    tConfig->terrainMaterials[elemIndex] = std::make_unique<TerrainMaterialConfiguration>();
                }
                // only update if not set by earlier configs,
                //FIXME: figure out how to manage priorities here.
                for (int j = 0; j < NUM_FORMS; j++) {
                    if (formToggle[j])
                    if (tConfig->terrainMaterials[elemIndex]->defaultSprite[j].second == INVALID_INDEX
                        || tConfig->terrainMaterials[elemIndex]->defaultSprite[j].second > lookupKeys[i].second)
                    {
                        tConfig->terrainMaterials[elemIndex]->defaultSprite[j].first = sprite;
                        tConfig->terrainMaterials[elemIndex]->defaultSprite[j].second = lookupKeys[i].second;
                    }
                }
            }
        }
        for (; elemSubtype; elemSubtype = elemSubtype->NextSiblingElement("subtype")) {
            // get subtype
            int subtypeId = lookupMaterialIndex(elemIndex, elemSubtype->Attribute("value"));
            if (subtypeId == INVALID_INDEX) {
                contentError("Invalid or missing value attribute", elemSubtype);
                continue;
            }

            // set subtype sprite for each terrain type
            for (int i = 0; i < elems; i++) {
                TerrainConfiguration *tConfig = configTable[lookupKeys[i].first].get();
                //if that was null we have *really* screwed up earlier
                //create a new TerrainMaterialConfiguration if required
                //make sure we have room for it first
                while (tConfig->terrainMaterials.size() <= (uint32_t)elemIndex) {
                    //dont make a full size vector in advance- we wont need it except
                    //for those who insist on Soap Fortresses
                    tConfig->terrainMaterials.push_back(nullptr);
                }
                if (tConfig->terrainMaterials[elemIndex] == nullptr) {
                    tConfig->terrainMaterials[elemIndex] = std::make_unique<TerrainMaterialConfiguration>();
                }
                // add to map (if not already present)
                for (int j = 0; j < NUM_FORMS; j++) {
                    if (formToggle[j]) {
                        if (tConfig->terrainMaterials[elemIndex]->overridingMaterials[j].count(subtypeId))
                        {
                            if (tConfig->terrainMaterials[elemIndex]->overridingMaterials[j][subtypeId].second > lookupKeys[i].second)
                                tConfig->terrainMaterials[elemIndex]->overridingMaterials[j][subtypeId].first = sprite;
                            tConfig->terrainMaterials[elemIndex]->overridingMaterials[j][subtypeId].second = lookupKeys[i].second;
                        }
                        else
                        {
                            tConfig->terrainMaterials[elemIndex]->overridingMaterials[j][subtypeId].first = sprite;
                            tConfig->terrainMaterials[elemIndex]->overridingMaterials[j][subtypeId].second = lookupKeys[i].second;
                        }
                    }
                }
            }
        }
    }
}

bool addSingleTerrainConfig(TiXmlElement* elemRoot)
{
    int basefile = INVALID_INDEX;
    const char* filename = elemRoot->Attribute("file");
    if (filename != NULL && filename[0] != 0) {
        basefile = loadConfigImgFile((char*)filename, elemRoot);
        if (basefile == -1) {
            return false;
        }
    }

    string elementType = elemRoot->Value();
    if (elementType.compare("floors") == 0) {
        //parse floors
        TiXmlElement* elemFloor = elemRoot->FirstChildElement("floor");
        while (elemFloor) {
            parseWallFloorSpriteElement(elemFloor, contentLoader->terrainFloorConfigs, basefile, true);
            elemFloor = elemFloor->NextSiblingElement("floor");
        }
    }
    if (elementType.compare("walls") == 0) {
        //parse walls
        TiXmlElement* elemWall = elemRoot->FirstChildElement("wall");
        while (elemWall) {
            parseWallFloorSpriteElement(elemWall, contentLoader->terrainWallConfigs, basefile, false);
            elemWall = elemWall->NextSiblingElement("wall");
        }
    }
    return true;
}

void flushTerrainConfig(vector<std::unique_ptr<TerrainConfiguration>>& config)
{
    uint32_t currentsize = (uint32_t)config.size();
    config.clear();
    if (currentsize < MAX_BASE_TERRAIN + FAKE_TERRAIN_COUNT) {
        currentsize = MAX_BASE_TERRAIN + FAKE_TERRAIN_COUNT;
    }

    while (config.size() < currentsize) {
        config.push_back(nullptr);
    }
}
