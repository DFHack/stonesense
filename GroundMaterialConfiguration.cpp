#include "common.h"
#include "SpriteMaps.h"
#include "GroundMaterialConfiguration.h"
#include <set>
#include "tinyxml.h"
#include "GUI.h"
#include "ContentLoader.h"
#include "EnumToString.h"

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

TerrainConfiguration::~TerrainConfiguration()
{
    uint32_t currentsize = (uint32_t)terrainMaterials.size();
    for (uint32_t i = 0; i < currentsize; i++) {
        if (terrainMaterials[i] != NULL) {
            delete(terrainMaterials[i]);
        }
    }
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

void parseWallFloorSpriteElement(TiXmlElement* elemWallFloorSprite, vector<TerrainConfiguration*>& configTable, int basefile, bool floor)
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
    TiXmlElement* elemTerrain = elemWallFloorSprite->FirstChildElement("terrain");
    for (TiXmlElement* elemTerrain = elemWallFloorSprite->FirstChildElement("terrain");
        elemTerrain;
        elemTerrain = elemTerrain->NextSiblingElement("terrain")) {
        //get a terrain type
        int targetElem = INVALID_INDEX;
        const char* gameIDstr = elemTerrain->Attribute("value");
        if (!(gameIDstr == NULL || gameIDstr[0] == 0))
        {
            targetElem = atoi(gameIDstr);
        }
        if (targetElem >= 0)
        {
            char buf[500];
            if (targetElem < contentLoader->tiletypeNameList.tiletype_list_size())
                sprintf(buf, "Use of deprecated terrain value \"%d\", use one of the following instead:\n <terrain token = \"%s\" />\n <terrain%s%s%s%s%s%s%s%s%s%s%s%s />\n in element",
                targetElem,
                contentLoader->tiletypeNameList.tiletype_list(targetElem).name().c_str(),
                contentLoader->tiletypeNameList.tiletype_list(targetElem).shape() == RemoteFortressReader::NO_SHAPE ? "" : " shape = \"",
                contentLoader->tiletypeNameList.tiletype_list(targetElem).shape() == RemoteFortressReader::NO_SHAPE ? "" : TiletypeShapeToString(contentLoader->tiletypeNameList.tiletype_list(targetElem).shape()),
                contentLoader->tiletypeNameList.tiletype_list(targetElem).shape() == RemoteFortressReader::NO_SHAPE ? "" : "\"",
                contentLoader->tiletypeNameList.tiletype_list(targetElem).special() == RemoteFortressReader::NO_SPECIAL ? "" : " special = \"",
                contentLoader->tiletypeNameList.tiletype_list(targetElem).special() == RemoteFortressReader::NO_SPECIAL ? "" : TiletypeSpecialToString(contentLoader->tiletypeNameList.tiletype_list(targetElem).special()),
                contentLoader->tiletypeNameList.tiletype_list(targetElem).special() == RemoteFortressReader::NO_SPECIAL ? "" : "\"",
                contentLoader->tiletypeNameList.tiletype_list(targetElem).variant() == RemoteFortressReader::NO_VARIANT ? "" : " variant = \"",
                contentLoader->tiletypeNameList.tiletype_list(targetElem).variant() == RemoteFortressReader::NO_VARIANT ? "" : TiletypeVariantToString(contentLoader->tiletypeNameList.tiletype_list(targetElem).variant()),
                contentLoader->tiletypeNameList.tiletype_list(targetElem).variant() == RemoteFortressReader::NO_VARIANT ? "" : "\"",
                contentLoader->tiletypeNameList.tiletype_list(targetElem).material() == RemoteFortressReader::NO_MATERIAL ? "" : " material = \"",
                contentLoader->tiletypeNameList.tiletype_list(targetElem).material() == RemoteFortressReader::NO_MATERIAL ? "" : TiletypeMaterialToString(contentLoader->tiletypeNameList.tiletype_list(targetElem).material()),
                contentLoader->tiletypeNameList.tiletype_list(targetElem).material() == RemoteFortressReader::NO_MATERIAL ? "" : "\""
                );
            else
                sprintf(buf, "Terrain value \"%d\" is invalid", targetElem);
            contentError(buf, elemTerrain);
        }
        const char* gameTokenstr = elemTerrain->Attribute("token");
        RemoteFortressReader::TiletypeShape elemShape = StringToTiletypeShape(elemTerrain->Attribute("shape"));
        RemoteFortressReader::TiletypeSpecial elemSpecial = StringToTiletypeSpecial(elemTerrain->Attribute("special"));
        RemoteFortressReader::TiletypeVariant elemVariant = StringToTiletypeVariant(elemTerrain->Attribute("variant"));
        RemoteFortressReader::TiletypeMaterial elemMaterial = StringToTiletypeMaterial(elemTerrain->Attribute("material"));
        
        int i = 0;
        if (targetElem >= i)
        {
            i = targetElem;
        }
        for (int i = 0; i < contentLoader->tiletypeNameList.tiletype_list_size(); i++)
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
                if (contentLoader->tiletypeNameList.tiletype_list(i).name() == gameTokenstr)
                    matchness = 0;
                else
                    valid = false;
            }
            if (matchness != 0) //this means there's no exact match made.
            {
                int partialMatch = 0;
                if (elemShape != RemoteFortressReader::NO_SHAPE)
                {
                    if (contentLoader->tiletypeNameList.tiletype_list(i).shape() == elemShape)
                        partialMatch += PRIORITY_SHAPE;
                    else
                        valid = false;
                }
                if (elemSpecial != RemoteFortressReader::NO_SPECIAL)
                {
                    if (contentLoader->tiletypeNameList.tiletype_list(i).special() == elemSpecial)
                        partialMatch += PRIORITY_SPECIAL;
                    else
                        valid = false;
                }
                if (elemVariant != RemoteFortressReader::NO_VARIANT)
                {
                    if (contentLoader->tiletypeNameList.tiletype_list(i).variant() == elemVariant)
                        partialMatch += PRIORITY_VARIANT;
                    else
                        valid = false;
                }
                if (elemMaterial != RemoteFortressReader::NO_MATERIAL)
                {
                    if (contentLoader->tiletypeNameList.tiletype_list(i).material() == elemMaterial)
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
                if (configTable.size() <= (uint32_t)i) {
                    //increase size if needed
                    configTable.resize(i + 1, NULL);
                }
                if (configTable[i] == NULL) {
                    // cleaned up in flushTerrainConfig
                    configTable[i] = new TerrainConfiguration();
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
            TerrainConfiguration *tConfig = configTable[lookupKeys[i].first];
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
                TerrainConfiguration *tConfig = configTable[lookupKeys[i].first];
                // if that was null we have *really* screwed up earlier
                // create a new TerrainMaterialConfiguration if required
                // make sure we have room for it first
                if (tConfig->terrainMaterials.size() <= (uint32_t)elemIndex) {
                    // dont make a full size vector in advance- most of the time
                    // we will only need the first few
                    tConfig->terrainMaterials.resize(elemIndex + 1, NULL);
                }
                if (tConfig->terrainMaterials[elemIndex] == NULL) {
                    tConfig->terrainMaterials[elemIndex] = new TerrainMaterialConfiguration();
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
                TerrainConfiguration *tConfig = configTable[lookupKeys[i].first];
                //if that was null we have *really* screwed up earlier
                //create a new TerrainMaterialConfiguration if required
                //make sure we have room for it first
                if (tConfig->terrainMaterials.size() <= (uint32_t)elemIndex) {
                    //dont make a full size vector in advance- we wont need it except
                    //for those who insist on Soap Fortresses
                    tConfig->terrainMaterials.resize(elemIndex + 1, NULL);
                }
                if (tConfig->terrainMaterials[elemIndex] == NULL) {
                    tConfig->terrainMaterials[elemIndex] = new TerrainMaterialConfiguration();
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

void flushTerrainConfig(vector<TerrainConfiguration*>& config)
{
    uint32_t currentsize = (uint32_t)config.size();
    for (uint32_t i = 0; i < currentsize; i++) {
        if (config[i] != NULL) {
            delete(config[i]);
        }
    }

    config.clear();
    if (currentsize < MAX_BASE_TERRAIN + FAKE_TERRAIN_COUNT) {
        currentsize = MAX_BASE_TERRAIN + FAKE_TERRAIN_COUNT;
    }
    config.resize(currentsize, NULL);
}
