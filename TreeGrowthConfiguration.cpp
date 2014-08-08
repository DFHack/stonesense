#include "TreeGrowthConfiguration.h"


void parseGrowthElement(TiXmlElement* elemGrowthSprite, MaterialMatcher<c_sprite> & growthTopConfigs, MaterialMatcher<c_sprite> & growthBottomConfigs, int basefile)
{
    const char* spriteSheetIndexStr = elemGrowthSprite->Attribute("sheetIndex");
    const char* spriteSpriteStr = elemGrowthSprite->Attribute("sprite");
    const char* spriteIndexStr = elemGrowthSprite->Attribute("index");
    if ((spriteSheetIndexStr == NULL || spriteSheetIndexStr[0] == 0) && (spriteSpriteStr == NULL || spriteSpriteStr[0] == 0) && (spriteIndexStr == NULL || spriteIndexStr[0] == 0)) {
        contentError("Invalid or missing sprite attribute", elemGrowthSprite);
        return; //nothing to work with
    }
    // make a base sprite
    c_sprite sprite;
    sprite.set_by_xml(elemGrowthSprite, basefile);
    sprite.set_size(SPRITEWIDTH, (TILETOPHEIGHT + FLOORHEIGHT));
    sprite.set_offset(0, (WALLHEIGHT));
    bool bottomLayer = false;

    const char* layerStr = elemGrowthSprite->Attribute("layer");
    if (layerStr && layerStr[0])
        bottomLayer = (strcmp(layerStr, "bottom") == 0);


    //parse material elements
    TiXmlElement* elemPart = elemGrowthSprite->FirstChildElement("part");
    if (elemPart == NULL) {
        //if none, there's nothing to be done with this color.
        contentError("Invalid or missing part attribute", elemGrowthSprite);
        return;
    }
    for (; elemPart; elemPart = elemPart->NextSiblingElement("part"))
    {
        const char* elemToken = elemPart->Attribute("token");
        if (elemToken && elemToken[0])
        {
            if (bottomLayer)
                growthBottomConfigs.set(sprite, elemToken, contentLoader->growthNameList.mutable_material_list());
            else
                growthTopConfigs.set(sprite, elemToken, contentLoader->growthNameList.mutable_material_list());

        }
    }
}

bool addSingleGrowthConfig(TiXmlElement* elemRoot)
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
    if (elementType.compare("growths") == 0) {
        //parse colors
        TiXmlElement* elemGrowth = elemRoot->FirstChildElement("growth");
        while (elemGrowth) {
            parseGrowthElement(elemGrowth, contentLoader->growthTopConfigs, contentLoader->growthBottomConfigs, basefile);
            elemGrowth = elemGrowth->NextSiblingElement("growth");
        }
    }
    return true;
}