#include "common.h"
#include "SpriteMaps.h"
#include "GroundMaterialConfiguration.h"
#include <set>
#include "tinyxml.h"
#include "GUI.h"
#include "ContentLoader.h"
#include "MiscUtils.h"
#include "StonesenseState.h"

constexpr auto PRIORITY_SHAPE = 8;
constexpr auto PRIORITY_SPECIAL = 4;
constexpr auto PRIORITY_VARIANT = 2;
constexpr auto PRIORITY_MATERIAL = 1;
constexpr auto PRIORITY_TOTAL = (PRIORITY_SHAPE + PRIORITY_SPECIAL + PRIORITY_VARIANT + PRIORITY_MATERIAL + 1);

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
    std::ofstream fp{ std::filesystem::path { "dump.txt"} };
    auto& contentLoader = stonesenseState.contentLoader;
    for (uint32_t j = 0; j < contentLoader->inorganic.size(); j++) {
        fp << j << ':' << contentLoader->inorganic[j].id << '\n';
    }
}

void TerrainMaterialConfiguration::updateSprite(int j, c_sprite& sprite, int x)
{
    if (defaultSprite[j].second == INVALID_INDEX || defaultSprite[j].second > x)
    {
        defaultSprite[j].first = sprite;
        defaultSprite[j].second = x;
    }
}

void TerrainMaterialConfiguration::updateOverridingMaterials(auto j, auto subtypeId, auto sprite, auto x) {
    if (overridingMaterials[j].count(subtypeId))
    {
        if (overridingMaterials[j][subtypeId].second > x)
            overridingMaterials[j][subtypeId].first = sprite;
    }
    else
    {
        overridingMaterials[j][subtypeId].first = sprite;
    }
    overridingMaterials[j][subtypeId].second = x;
}

void TerrainConfiguration::updateSprite(auto j, auto sprite, auto x)
{
    if (defaultSprite[j].second == INVALID_INDEX || defaultSprite[j].second > x) {
        defaultSprite[j].first = sprite;
        defaultSprite[j].second = x;
    }
}

void TerrainConfiguration::expand(auto elemIndex)
{
    if (!terrainMaterials.contains(elemIndex))
        terrainMaterials.emplace(elemIndex, std::make_unique<TerrainMaterialConfiguration>());
}

namespace
{
    template<typename T>
    T StringToTiletypeEnum(const char* input)
    {
        T t{};
        return (input != nullptr && DFHack::find_enum_item(&t, input)) ? t : T::NONE;
    }

    void parseWallFloorSpriteElement(TiXmlElement* elemWallFloorSprite, std::vector<std::unique_ptr<TerrainConfiguration>>& configTable, int basefile, bool floor)
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

        std::vector<std::pair<int, int>> lookupKeys;

        // look through terrain elements
        for (TiXmlElement* elemTerrain = elemWallFloorSprite->FirstChildElement("terrain");
            elemTerrain;
            elemTerrain = elemTerrain->NextSiblingElement("terrain")) {
            //get a terrain type
            //
            // NOTE(myk002): targetElem was changed from df::tiletype to an int because the underlying type of df::tiltype changed
            // from signed to unsigned as we canonicalized DFHack xml structures against DF headers. This caused issues in this
            // code because:
            // - negative values (like INVALID_INDEX) are given special meaning
            // - the code here (and elsewhere in stonesense) depends on signed comparisons
            // - the value of int matchedness (below) is assigned to a tiletype field (though I can't determine why this is desired)
            //
            // a proper fix would take a fair bit of rearchitecting throughout stonesense
            int targetElem = INVALID_INDEX;
            const char* gameIDstr = elemTerrain->Attribute("value");
            if (!(gameIDstr == NULL || gameIDstr[0] == 0))
            {
                targetElem = atoi(gameIDstr);
            }
            if (targetElem >= 0)
            {
                char buf[500];
                if (DFHack::is_valid_enum_item((df::tiletype)targetElem))
                {
                    df::tiletype tt = (df::tiletype)targetElem;
                    auto shape = ENUM_ATTR(tiletype, shape, tt);
                    auto special = ENUM_ATTR(tiletype, special, tt);
                    auto variant = ENUM_ATTR(tiletype, variant, tt);
                    auto material = ENUM_ATTR(tiletype, material, tt);
                    using df::tiletype_shape, df::tiletype_special, df::tiletype_variant, df::tiletype_material;
                    using DFHack::enum_item_key_str;
                    sprintf(buf, "Use of deprecated terrain value \"%d\", use one of the following instead:\n <terrain token = \"%s\" />\n <terrain%s%s%s%s%s%s%s%s%s%s%s%s />\n in element",
                        targetElem,
                        DFHack::enum_item_key_str(tt),
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
            df::tiletype_shape elemShape{ StringToTiletypeEnum<df::tiletype_shape>(elemTerrain->Attribute("shape")) };
            df::tiletype_special elemSpecial{ StringToTiletypeEnum<df::tiletype_special>(elemTerrain->Attribute("special")) };
            df::tiletype_variant elemVariant{ StringToTiletypeEnum<df::tiletype_variant>(elemTerrain->Attribute("variant")) };
            df::tiletype_material elemMaterial{ StringToTiletypeEnum<df::tiletype_material>(elemTerrain->Attribute("material")) };

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
                    if (DFHack::enum_item_key(i) == gameTokenstr)
                        matchness = 0;
                    else
                        valid = false;
                }
                if (matchness != 0) //this means there's no exact match made.
                {
                    using df::tiletype_shape, df::tiletype_special, df::tiletype_variant, df::tiletype_material;
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
                    lookupKeys.push_back(std::make_pair(i, matchness));
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

        std::vector<bool> formToggle;
        formToggle.resize(NUM_FORMS);
        // parse weather plate is for a block, log, etc
        TiXmlElement* elemForm = elemWallFloorSprite->FirstChildElement("form");
        if (elemForm == NULL) {
            formToggle[0] = true;
        }
        for (; elemForm; elemForm = elemForm->NextSiblingElement("form")) {
            const char* strForm = elemForm->Attribute("value");

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
                TerrainConfiguration* tConfig = configTable[lookupKeys[i].first].get();
                // if that was null we have *really* screwed up earlier
                // only update if not by previous configs
                for (int j = 0; j < NUM_FORMS; j++) {
                    if (formToggle[j])
                        tConfig->updateSprite(j, sprite, lookupKeys[i].second);
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
                    TerrainConfiguration* tConfig = configTable[lookupKeys[i].first].get();
                    // if that was null we have *really* screwed up earlier
                    // create a new TerrainMaterialConfiguration if required
                    // make sure we have room for it first
                    tConfig->expand(elemIndex);
                    // only update if not set by earlier configs,
                    //FIXME: figure out how to manage priorities here.
                    for (int j = 0; j < NUM_FORMS; j++) {
                        if (formToggle[j])
                            tConfig->getTerrainMaterials(elemIndex)->updateSprite(j, sprite, lookupKeys[i].second);
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
                    TerrainConfiguration* tConfig = configTable[lookupKeys[i].first].get();
                    //if that was null we have *really* screwed up earlier
                    //create a new TerrainMaterialConfiguration if required
                    //make sure we have room for it first
                    tConfig->expand(elemIndex);
                    // add to map (if not already present)
                    for (int j = 0; j < NUM_FORMS; j++) {
                        if (formToggle[j]) {
                            tConfig->getTerrainMaterials(elemIndex)->updateOverridingMaterials(j, subtypeId, sprite, lookupKeys[i].second);
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

    std::string elementType = elemRoot->Value();
    auto& contentLoader = stonesenseState.contentLoader;
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

void flushTerrainConfig(std::vector<std::unique_ptr<TerrainConfiguration>>& config)
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
