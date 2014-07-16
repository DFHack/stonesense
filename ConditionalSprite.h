#pragma once

#include "TileCondition.h"

// generic superclass
class SpriteNode
{
public:
    SpriteNode(void) {};
    virtual ~SpriteNode(void) {};

    // this returns true if the sprite matched, and also adds
    // sprites to the tile as required
    virtual bool copyToTile(Tile* b)=0;
    // adds a child if appropriate (vestigial in some cases)
    virtual void addChild(SpriteNode* child) {};
};

// root nesting structure
class RootTile : public SpriteNode
{
    std::vector<SpriteNode*> children;

public:
    RootTile(void);
    ~RootTile(void);

    bool copyToTile(Tile* b);
    void addChild(SpriteNode* child);
};

// nesting conditional structure
class SpriteTile : public ConditionalNode, public SpriteNode
{
    TileCondition* conditions;
    std::vector<SpriteNode*> children;
    SpriteNode* elsenode;

public:
    SpriteTile(void);
    ~SpriteTile(void);

    bool copyToTile(Tile* b);
    bool addCondition(TileCondition* cond);
    void addChild(SpriteNode* child);
    void addElse(SpriteNode* child);
};

// rotational conditional structure
class RotationTile : public ConditionalNode, public SpriteNode
{
    std::vector<SpriteNode*> children;

public:
    RotationTile(void);
    ~RotationTile(void);

    bool copyToTile(Tile* b);
    bool addCondition(TileCondition* cond);
    void addChild(SpriteNode* child);
};

// display element
class SpriteElement : public SpriteNode
{
private:

public:
    c_sprite sprite;

    SpriteElement(void);
    ~SpriteElement(void) {};

    bool copyToTile(Tile* b);
};
