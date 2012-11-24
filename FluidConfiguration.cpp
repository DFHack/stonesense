#include "FluidConfiguration.h"
#include "ContentLoader.h"

FluidConfiguration::FluidConfiguration()
{
    fluidset = 0;
    sprite.reset();

}

void parseFluidElement( TiXmlElement* elemFluid, int basefile)
{
    c_sprite tempSprite;
    bool type = 0;
    unsigned char level;
    const char* fluidTypeStr = elemFluid->Attribute("type");
    if(fluidTypeStr == NULL || fluidTypeStr[0] == 0) {
        contentError("Invalid or missing type attribute",elemFluid);
        return; //nothing to work with
    }
    if (strcmp(fluidTypeStr, "water") == 0) {
        type = 0;
    } else if (strcmp(fluidTypeStr, "lava") == 0) {
        type = 1;
    } else if (strcmp(fluidTypeStr, "magma") == 0) {
        type = 1;
    } else {
        contentError("Invalid or missing type attribute",elemFluid);
    }

    const char* fluidLevelStr = elemFluid->Attribute("level");
    if(fluidLevelStr == NULL || fluidLevelStr[0] == 0) {
        contentError("Invalid or missing level attribute",elemFluid);
        return; //nothing to work with
    }
    level = atoi(fluidLevelStr);
    if( (level <= 0) || (level > 8)) {
        contentError("Invalid or missing level attribute",elemFluid);
        return; //nothing to work with
    }
    const char* spriteSheetIndexStr = elemFluid->Attribute("sheetIndex");
    const char* spriteSpriteStr = elemFluid->Attribute("sprite");
    const char* spriteIndexStr = elemFluid->Attribute("index");
    if ((spriteSheetIndexStr == NULL || spriteSheetIndexStr[0] == 0) && (spriteSpriteStr == NULL || spriteSpriteStr[0] == 0) && (spriteIndexStr == NULL || spriteIndexStr[0] == 0)) {
        contentError("Invalid or missing sprite attribute",elemFluid);
        return; //nothing to work with
    }

    tempSprite.set_by_xml(elemFluid, basefile);

    if(type) {
        if(contentLoader->lava[level-1].fluidset == 0) {
            contentLoader->lava[level-1].sprite = tempSprite;
            contentLoader->lava[level-1].fluidset = 1;
        }
    } else {
        if(contentLoader->water[level-1].fluidset == 0) {
            contentLoader->water[level-1].sprite = tempSprite;
            contentLoader->water[level-1].fluidset = 1;
        }
    }
}

bool addSingleFluidConfig( TiXmlElement* elemRoot)
{
    int basefile = 0;
    const char* filename = elemRoot->Attribute("file");
    if (filename != NULL && filename[0] != 0) {
        basefile = loadConfigImgFile((char*)filename,elemRoot);
        if(basefile == -1) {
            return false;
        }
    }

    string elementType = elemRoot->Value();
    if(elementType.compare( "fluids" ) == 0) {
        //parse floors
        TiXmlElement* elemFluid = elemRoot->FirstChildElement("fluid");
        while( elemFluid ) {
            parseFluidElement( elemFluid, basefile);
            elemFluid = elemFluid->NextSiblingElement("fluid");
        }
    }
    return true;
}
