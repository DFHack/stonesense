#pragma once

#include "Tile.h"

extern SegmentWrap map_segment;

enum draw_event_type{
    TintedScaledBitmap,
    CreatureText
};

struct draw_event{
    draw_event_type type;
    void * drawobject;
    ALLEGRO_COLOR tint;
    float sx;
    float sy;
    float sw;
    float sh;
    float dx;
    float dy;
    float dw;
    float dh;
    int flags;
};

class WorldSegment
{
private:
    vector<Tile*> tiles;
    vector<draw_event> todraw;
public:
    bool loaded;
    bool processed;
    int x, y, z;
    int displayedx, displayedy, displayedz;
    int sizex, sizey, sizez;
    unsigned char rotation;
    Crd3D regionSize;
    Crd3D regionPos;
    Tile** tilesAsPointerVolume;
    WorldSegment(int x, int y, int z, int sizex, int sizey, int sizez) {
        this->x = x;
        this->y = y;
        this->z = z - sizez + 1;
        this->sizex = sizex;
        this->sizey = sizey;
        this->sizez = sizez;
        this->displayedx = ssState.DisplayedSegmentX;
        this->displayedy = ssState.DisplayedSegmentY;
        this->displayedz = ssState.DisplayedSegmentZ;
        
        regionSize.x = regionSize.y = regionSize.z = 0;
        regionPos.x = regionPos.y = regionPos.z = 0;

        uint32_t memoryNeeded = sizex * sizey * sizez * sizeof(Tile*);
        tilesAsPointerVolume = (Tile**) malloc( memoryNeeded );
        memset(tilesAsPointerVolume, 0, memoryNeeded);
    }

    ~WorldSegment() {
        uint32_t num = (uint32_t)tiles.size();
        for(uint32_t i = 0; i < num; i++) {
            delete(tiles[i]);
        }
        tiles.clear();
    }

    void Dispose(void) {
        free(tilesAsPointerVolume);
    }

    uint32_t getNumTiles() {
        return (uint32_t)tiles.size();
    }

    Tile* getTile(int32_t x, int32_t y, int32_t z);
    Tile* getTileLocal(uint32_t x, uint32_t y, uint32_t z);
    Tile* getTileRelativeTo(uint32_t x, uint32_t y, uint32_t z,  dirRelative direction);
    Tile* getTileRelativeTo(uint32_t x, uint32_t y, uint32_t z,  dirRelative direction, int distance);
    Tile* getTile(uint32_t index);
    void CorrectTileForSegmentOffset(int32_t& x, int32_t& y, int32_t& z);
    void CorrectTileForSegmentRotation(int32_t& x, int32_t& y, int32_t& z);
    void addTile(Tile* b);
    void AssembleBlockTiles(int32_t firstX, int32_t firstY, int32_t lastX, int32_t lastY, int32_t incrx, int32_t incry, int32_t z);
    void AssembleAllTiles();
    void AssembleSprite(draw_event d);
    void DrawAllTiles();
    //void drawPixels();
    bool CoordinateInsideSegment(uint32_t x, uint32_t y, uint32_t z);
    bool CoordinateInteriorSegment(uint32_t x, uint32_t y, uint32_t z, uint32_t shellthick);
};

// FIXME: make nicer. one day. maybe.
class SegmentWrap
{
public:
    SegmentWrap() {
        init();
    }
    ~SegmentWrap() {
        die();
    }
    void lock() {
        al_lock_mutex(mutex);
        locked = true;
    }
    void unlock() {
        al_unlock_mutex(mutex);
        locked = false;
    }
    void init() {
        segment = NULL;
        mutex = al_create_mutex();
        locked = false;
    }
    void die() {
        al_destroy_mutex(mutex);
    }
    WorldSegment * swap(WorldSegment * newsegment) {
        WorldSegment * temp = segment;
        segment = newsegment;
        return temp;
    }
    WorldSegment * get() {
        return segment;
    }
private:
    ALLEGRO_MUTEX * mutex;
    WorldSegment * segment;
    bool locked;
};
