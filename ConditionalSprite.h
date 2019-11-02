#pragma once

#include <memory>

#include "TileCondition.h"

// generic superclass
class SpriteNode
{
public:
    SpriteNode() = default;
    virtual ~SpriteNode() = default;

    // this returns true if the sprite matched, and also adds
    // sprites to the tile as required
    virtual bool copyToTile(Tile* b)=0;
    // adds a child if appropriate (vestigial in some cases)
    virtual void addChild(std::unique_ptr<SpriteNode> child) {};
};

// root nesting structure
class RootTile : public SpriteNode
{
    std::vector<std::unique_ptr<SpriteNode>> children;

public:
    RootTile(void);
    ~RootTile(void);

    bool copyToTile(Tile* b);
    void addChild(std::unique_ptr<SpriteNode> child);
};

// nesting conditional structure
class SpriteTile : public ConditionalNode, public SpriteNode
{
    std::unique_ptr<TileCondition> conditions;
    std::vector<std::unique_ptr<SpriteNode>> children;
    std::unique_ptr<SpriteNode> elsenode;

public:
    SpriteTile(void);
    ~SpriteTile(void);

    bool copyToTile(Tile* b);
    bool addCondition(std::unique_ptr<TileCondition> cond);
    void addChild(std::unique_ptr<SpriteNode> child);
    void addElse(std::unique_ptr<SpriteNode> child);
};

// rotational conditional structure
class RotationTile : public ConditionalNode, public SpriteNode
{
    std::vector<std::unique_ptr<SpriteNode>> children;

public:
    RotationTile(void);
    ~RotationTile(void);

    bool copyToTile(Tile* b);
    bool addCondition(std::unique_ptr<TileCondition> cond);
    void addChild(std::unique_ptr<SpriteNode> child);
};

// display element
class SpriteElement : public SpriteNode
{
public:
    c_sprite sprite;

    SpriteElement(void);
    ~SpriteElement() = default;

    bool copyToTile(Tile* b);
};
