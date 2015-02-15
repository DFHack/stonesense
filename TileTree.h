#pragma once
#include "common.h"
#include "SpriteObjects.h"
#include "Tile.h"
#include "WorldSegment.h"

class Tile;

class c_tile_tree_twig
{
    c_sprite own_sprite;
    std::vector<c_sprite> westward_growth;
    std::vector<c_sprite> eastward_growth;
public:
    c_tile_tree_twig(void);
    ~c_tile_tree_twig(void);

    void insert_sprites(WorldSegment * w, int x, int y, int z, Tile * parent);
    void set_single_sprite(void);
    void set_sheetindex(int32_t in) {
        own_sprite.set_sheetindex(in);
    }
    void set_fileindex(int32_t in) {
        own_sprite.set_fileindex(in);
    }
    int32_t get_sheetindex(void) {
        return own_sprite.get_sheetindex();
    }
    void set_by_xml(TiXmlElement* elemSprite, int32_t fileindex);
    void add_sprite(int x, c_sprite sprite);
    void reset();

    void assemble_world(int x, int y, int z, Tile * b, bool chop = false) {
        own_sprite.assemble_world(x,y,z,b,chop);
    }
};

class c_tile_tree_branch
{
    c_tile_tree_twig own_twig;
    std::vector<c_tile_tree_twig> northward_growth;
    std::vector<c_tile_tree_twig> southward_growth;
public:
    c_tile_tree_branch(void);
    ~c_tile_tree_branch(void);

    void insert_sprites(WorldSegment * w, int x, int y, int z, Tile * parent);
    void set_sheetindex(int32_t in) {
        own_twig.set_sheetindex(in);
    }
    void set_fileindex(int32_t in) {
        own_twig.set_fileindex(in);
    }
    int32_t get_sheetindex(void) {
        return own_twig.get_sheetindex();
    }
    void set_by_xml(TiXmlElement* elemSprite, int32_t fileindex);
    void add_sprite(int x, int y, c_sprite sprite);
    void reset();

    void assemble_world(int x, int y, int z, Tile * b, bool chop = false) {
        own_twig.assemble_world(x,y,z,b,chop);
    }
};

class c_tile_tree
{
    c_tile_tree_branch own_branch;
    std::vector<c_tile_tree_branch> upward_growth;
public:
    c_tile_tree(void);
    ~c_tile_tree(void);

    void insert_sprites(WorldSegment * w, int x, int y, int z, Tile * parent);
    void set_sheetindex(int32_t in) {
        own_branch.set_sheetindex(in);
    }
    void set_fileindex(int32_t in) {
        own_branch.set_fileindex(in);
    }
    int32_t get_sheetindex(void) {
        return own_branch.get_sheetindex();
    }
    void set_by_xml(TiXmlElement* elemSprite, int32_t fileindex);
    void add_sprite(int x, int y, int z, c_sprite sprite);
    void reset();

    void assemble_world(int x, int y, int z, Tile * b, bool chop = false) {
        own_branch.assemble_world(x,y,z,b,chop);
    }
};
