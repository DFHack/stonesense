#include "TileTree.h"
#include "GameBuildings.h"
#include "GUI.h"

c_tile_tree_twig::c_tile_tree_twig()
{
    own_sprite.set_sheetindex(-1);
}

c_tile_tree_twig::~c_tile_tree_twig()
{
}

void insert_sprite(WorldSegment *w, int x, int y, int z, Tile * parent, c_sprite sprite)
{
    Tile * b_orig = w->getTile(x, y, z);
    if(!b_orig) {
        b_orig = w->ResetTile(x, y, z, tiletype::OpenSpace);
        if(!b_orig) {
            return;
        }
    }
    b_orig->building.sprites.push_back(sprite);
    if(b_orig->building.type == BUILDINGTYPE_NA
        || ((!ssConfig.show_stockpiles) && b_orig->building.type == building_type::Stockpile)
        || ((!ssConfig.show_zones) && b_orig->building.type == building_type::Civzone)) {
        b_orig->building.type = BUILDINGTYPE_TREE;
    }
    b_orig->building.parent = parent;
}

void c_tile_tree_twig::insert_sprites(WorldSegment *w, int x, int y, int z, Tile * parent)
{
    if(w->CoordinateInsideSegment(x,y,z)) {
        if(own_sprite.get_sheetindex() >= 0) {
            insert_sprite(w,x,y,z,parent, own_sprite);
        }
    }
    switch(ssState.Rotation) {
    case 0:
        for(unsigned int i = 0; i < eastward_growth.size(); i++) {
            if(w->CoordinateInsideSegment(x + i + 1,y,z)) {
                if(eastward_growth[i].get_sheetindex() >= 0) {
                    insert_sprite( w, x + i + 1, y, z, parent, eastward_growth[i] );
                }
            }
        }
        for(unsigned int i = 0; i < westward_growth.size(); i++) {
            if(w->CoordinateInsideSegment(x - i - 1,y,z)) {
                if(westward_growth[i].get_sheetindex() >= 0) {
                    insert_sprite( w, x - i - 1, y, z, parent, westward_growth[i] );
                }
            }
        }
        break;
    case 1:
        for(unsigned int i = 0; i < westward_growth.size(); i++) {
            if(w->CoordinateInsideSegment(x, y + i + 1, z)) {
                if(westward_growth[i].get_sheetindex() >= 0) {
                    insert_sprite( w, x, y + i + 1, z, parent, westward_growth[i] );
                }
            }
        }
        for(unsigned int i = 0; i < eastward_growth.size(); i++) {
            if(w->CoordinateInsideSegment(x, y - i - 1, z)) {
                if(eastward_growth[i].get_sheetindex() >= 0) {
                    insert_sprite( w, x, y - i - 1, z, parent, eastward_growth[i] );
                }
            }
        }
        break;
    case 2:
        for(unsigned int i = 0; i < eastward_growth.size(); i++) {
            if(w->CoordinateInsideSegment(x - i - 1,y,z)) {
                if(eastward_growth[i].get_sheetindex() >= 0) {
                    insert_sprite( w, x - i - 1, y, z, parent, eastward_growth[i] );
                }
            }
        }
        for(unsigned int i = 0; i < westward_growth.size(); i++) {
            if(w->CoordinateInsideSegment(x + i + 1,y,z)) {
                if(westward_growth[i].get_sheetindex() >= 0) {
                    insert_sprite( w, x + i + 1, y, z, parent, westward_growth[i] );
                }
            }
        }
        break;
    case 3:
        for(unsigned int i = 0; i < westward_growth.size(); i++) {
            if(w->CoordinateInsideSegment(x, y - i - 1, z)) {
                if(westward_growth[i].get_sheetindex() >= 0) {
                    insert_sprite( w, x, y - i - 1, z, parent, westward_growth[i] );
                }
            }
        }
        for(unsigned int i = 0; i < eastward_growth.size(); i++) {
            if(w->CoordinateInsideSegment(x, y + i + 1, z)) {
                if(eastward_growth[i].get_sheetindex() >= 0) {
                    insert_sprite( w, x, y + i + 1, z, parent, eastward_growth[i] );
                }
            }
        }
        break;
    }
}

void c_tile_tree_twig::add_sprite(int x, c_sprite sprite)
{
    if(x == 0) {
        own_sprite = sprite;
    } else if(x > 0) {
        //c_sprite detaultSprite;
        //detaultSprite.set_sheetindex(-1);
        if(eastward_growth.size() < x) {
            eastward_growth.resize(x);
        }
        eastward_growth[x-1] = sprite;
    } else if(x < 0) {
        //c_sprite detaultSprite;
        //detaultSprite.set_sheetindex(-1);
        if(westward_growth.size() < abs(x)) {
            westward_growth.resize(abs(x));
        }
        westward_growth[abs(x)-1] = sprite;
    }
}

void c_tile_tree_twig::reset()
{
    own_sprite.reset();
    own_sprite.set_sheetindex(-1);
    eastward_growth.clear();
    westward_growth.clear();
}
/// Branch stuff follows

c_tile_tree_branch::c_tile_tree_branch()
{
}

c_tile_tree_branch::~c_tile_tree_branch()
{
}

void c_tile_tree_branch::add_sprite(int x, int y, c_sprite sprite)
{
    if(y == 0) {
        own_twig.add_sprite(x, sprite);
    } else if(y > 0) {
        if(northward_growth.size() < y) {
            northward_growth.resize(y);
        }
        northward_growth[y-1].add_sprite(x, sprite);
    } else if(y < 0) {
        if(southward_growth.size() < abs(y)) {
            southward_growth.resize(abs(y));
        }
        southward_growth[abs(y)-1].add_sprite(x, sprite);
    }
}

void c_tile_tree_branch::insert_sprites(WorldSegment *w, int x, int y, int z, Tile * parent)
{
    own_twig.insert_sprites(w, x, y, z, parent);
    switch(ssState.Rotation) {
    case 0:
        for(int i = 0; i < southward_growth.size(); i++) {
            Tile * b = w->getTile(x, y + i + 1, z);
            if(b && (b->tileShapeBasic()==tiletype_shape_basic::Wall || b->tileShapeBasic()==tiletype_shape_basic::Stair)) {
                break;
            }
            southward_growth[i].insert_sprites(w, x, y + i + 1, z, parent);
        }
        for(int i = 0; i < northward_growth.size(); i++) {
            Tile * b = w->getTile(x, y - i - 1, z);
            if(b && (b->tileShapeBasic()==tiletype_shape_basic::Wall || b->tileShapeBasic()==tiletype_shape_basic::Stair)) {
                break;
            }
            northward_growth[i].insert_sprites(w, x, y - i - 1, z, parent);
        }
        break;
    case 1:
        for(int i = 0; i < southward_growth.size(); i++) {
            Tile * b = w->getTile(x + i + 1, y , z);
            if(b && (b->tileShapeBasic()==tiletype_shape_basic::Wall || b->tileShapeBasic()==tiletype_shape_basic::Stair)) {
                break;
            }
            southward_growth[i].insert_sprites(w, x + i + 1, y , z, parent);
        }
        for(int i = 0; i < northward_growth.size(); i++) {
            Tile * b = w->getTile(x - i - 1, y , z);
            if(b && (b->tileShapeBasic()==tiletype_shape_basic::Wall || b->tileShapeBasic()==tiletype_shape_basic::Stair)) {
                break;
            }
            northward_growth[i].insert_sprites(w, x - i - 1, y , z, parent);
        }
        break;
    case 2:
        for(int i = 0; i < southward_growth.size(); i++) {
            Tile * b = w->getTile(x, y - i - 1, z);
            if(b && (b->tileShapeBasic()==tiletype_shape_basic::Wall || b->tileShapeBasic()==tiletype_shape_basic::Stair)) {
                break;
            }
            southward_growth[i].insert_sprites(w, x, y - i - 1, z, parent);
        }
        for(int i = 0; i < northward_growth.size(); i++) {
            Tile * b = w->getTile(x, y + i + 1, z);
            if(b && (b->tileShapeBasic()==tiletype_shape_basic::Wall || b->tileShapeBasic()==tiletype_shape_basic::Stair)) {
                break;
            }
            northward_growth[i].insert_sprites(w, x, y + i + 1, z, parent);
        }
        break;
    case 3:
        for(int i = 0; i < southward_growth.size(); i++) {
            Tile * b = w->getTile(x - i - 1, y , z);
            if(b && (b->tileShapeBasic()==tiletype_shape_basic::Wall || b->tileShapeBasic()==tiletype_shape_basic::Stair)) {
                break;
            }
            southward_growth[i].insert_sprites(w, x - i - 1, y , z, parent);
        }
        for(int i = 0; i < northward_growth.size(); i++) {
            Tile * b = w->getTile(x + i + 1, y , z);
            if(b && (b->tileShapeBasic()==tiletype_shape_basic::Wall || b->tileShapeBasic()==tiletype_shape_basic::Stair)) {
                break;
            }
            northward_growth[i].insert_sprites(w, x + i + 1, y , z, parent);
        }
        break;
    }
}

void c_tile_tree_branch::reset()
{
    own_twig.reset();
    northward_growth.clear();
    southward_growth.clear();
}


//Tree stuff follows

c_tile_tree::c_tile_tree()
{
    upward_growth;
}

c_tile_tree::~c_tile_tree()
{
}

void c_tile_tree::add_sprite(int x, int y, int z, c_sprite sprite)
{
    if(z == 0) {
        own_branch.add_sprite(x, y, sprite);
    } else if(z > 0) {
        if(upward_growth.size() < z) {
            upward_growth.resize(z);
        }
        upward_growth[z-1].add_sprite(x, y, sprite);
    }
}

void c_tile_tree::insert_sprites(WorldSegment *w, int x, int y, int z, Tile * parent)
{
    own_branch.insert_sprites(w, x, y, z, parent);
    for(int i = 0; i < upward_growth.size(); i++) {
        Tile * b = w->getTile(x, y, z + i + 1);
        if((b && (
                    b->tileShapeBasic()==tiletype_shape_basic::Floor ||
                    b->tileShapeBasic()==tiletype_shape_basic::Wall ||
                    b->tileShapeBasic()==tiletype_shape_basic::Stair)) ||
                ((z + i + 1) > w->segState.Position.z + w->segState.Size.z)
          ) {
            break;
        }
        upward_growth[i].insert_sprites(w, x, y, z + i + 1, parent);
    }
}

void c_tile_tree::set_by_xml(TiXmlElement *elemTree, int fileindex)
{
    //reset();
    int x = 0, y = 0, z = 0;
    const char* XCoordString;
    XCoordString = elemTree->Attribute("X");
    if (XCoordString != NULL && XCoordString[0] != 0) {
        x = atoi(XCoordString);
    }
    const char* YCoordString;
    YCoordString = elemTree->Attribute("Y");
    if (YCoordString != NULL && YCoordString[0] != 0) {
        y = atoi(YCoordString);
    }
    const char* ZCoordString;
    ZCoordString = elemTree->Attribute("Z");
    if (ZCoordString != NULL && ZCoordString[0] != 0) {
        z = atoi(ZCoordString);
    }
    c_sprite sprite;
    sprite.set_by_xml(elemTree, fileindex);
    add_sprite(x, y, z, sprite);

    //add branches, if any.
    //TiXmlElement* elemBranch = elemTree->FirstChildElement("branch");
    for(TiXmlElement* elemBranch = elemTree->FirstChildElement("branch");
            elemBranch;
            elemBranch = elemBranch->NextSiblingElement("branch")) {
        set_by_xml(elemBranch, fileindex);
    }
}

void c_tile_tree::reset()
{
    own_branch.reset();
    upward_growth.clear();
}
