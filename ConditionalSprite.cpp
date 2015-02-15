#include "common.h"
#include "ConditionalSprite.h"
#include "Tile.h"
#include "GameBuildings.h"
#include "GUI.h"

/* RootTile */

RootTile::RootTile()
    : SpriteNode()
{
    //cout << "RootTile +" << endl;
}

RootTile::~RootTile(void)
{
    //cout << "RootTile -" << endl;
    uint32_t max = (uint32_t)children.size();
    for(uint32_t i=0; i<max; i++) {
        delete(children[i]);
    }
}

bool RootTile::copyToTile(Tile* b)
{
    bool haveMatch = false;
    uint32_t max = (uint32_t)children.size();

    for(uint32_t i=0; i<max; i++) {
        if (children[i]->copyToTile(b)) {
            haveMatch = true;
        }
    }
    return haveMatch;
}

void RootTile::addChild(SpriteNode* child)
{
    children.push_back(child);
}

/* SpriteTile */

SpriteTile::SpriteTile()
    : ConditionalNode(), SpriteNode()
{
    //cout << "SpriteTile +" << endl;
    conditions = NULL;
    elsenode = NULL;
}

SpriteTile::~SpriteTile(void)
{
    //cout << "SpriteTile -" << endl;
    delete(elsenode);
    delete(conditions);
    uint32_t max = (uint32_t)children.size();
    for(uint32_t i=0; i<max; i++) {
        delete(children[i]);
    }
};

bool SpriteTile::copyToTile(Tile* b)
{
    bool condMatch = false;
    if (conditions == NULL) {
        condMatch = true;
    } else {
        condMatch = conditions->Matches( b );
    }

    bool haveMatch=false;
    if (condMatch) {
        uint32_t max = (uint32_t)children.size();
        for(uint32_t i=0; i<max; i++) {
            if (children[i]->copyToTile(b)) {
                haveMatch = true;
            }
        }
    } else if (elsenode != NULL) {
        haveMatch = elsenode->copyToTile(b);
    }
    return haveMatch;
}

bool SpriteTile::addCondition(TileCondition* cond)
{
    if (conditions != NULL) {
        LogError("Too many condition elements for SpriteTile\n");
        return false;
    }
    conditions = cond;
    return true;
}

void SpriteTile::addChild(SpriteNode* child)
{
    children.push_back(child);
}

void SpriteTile::addElse(SpriteNode* child)
{
    elsenode = child;
}


/* RotationTile */

RotationTile::RotationTile()
    : ConditionalNode(), SpriteNode()
{
    //cout << "SpriteTile +" << endl;
}

RotationTile::~RotationTile(void)
{
    //cout << "SpriteTile -" << endl;
    uint32_t max = (uint32_t)children.size();
    for(uint32_t i=0; i<max; i++) {
        delete(children[i]);
    }
};

bool RotationTile::copyToTile(Tile* b)
{
    int index = ssState.Rotation;
    int max = (int)children.size();
    if (max == 0) {
        return false;
    }
    while (index >= max) {
        index = index - max;
    }
    return children[index]->copyToTile(b);
}

bool RotationTile::addCondition(TileCondition* cond)
{
    LogError("Condition elements not permitted for RotationTile\n");
    return false;
}

void RotationTile::addChild(SpriteNode* child)
{
    children.push_back(child);
}

/* SpriteElement */

SpriteElement::SpriteElement()
    : SpriteNode()
{
    //cout << "SpriteElement +" << endl;
    sprite.set_sheetindex(-1);
}

bool SpriteElement::copyToTile(Tile* b)
{
    if (sprite.get_sheetindex() > -1) {
        b->building.sprites.push_back(sprite);
    }
    return true;
}
