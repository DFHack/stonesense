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
    Tile* tiles;
    vector<draw_event> todraw;
public:
    bool loaded;
    bool processed;
    //these are the coordinates and size of the loaded segment
    Crd3D pos;
    Crd3D size;
    //these are the coordinates at which the viewport is currently located
    // note that this may not be the same as the actual coordinates of the segment
    // due to concurrency
    Crd3D displayed;
    unsigned char rotation;
    //these are the size and position of the DF map region to which this segment is a part
    Crd3D regionSize;
    Crd3D regionPos;
    WorldSegment(int x=0, int y=0, int z=0, int sizex=0, int sizey=0, int sizez=0) {
        this->pos.x = x;
        this->pos.y = y;
        this->pos.z = z - sizez + 1;
        this->size.x = sizex;
        this->size.y = sizey;
        this->size.z = sizez;
        this->displayed.x = ssState.DisplayedSegmentX;
        this->displayed.y = ssState.DisplayedSegmentY;
        this->displayed.z = ssState.DisplayedSegmentZ;
        regionSize.x = regionSize.y = regionSize.z = 0;
        regionPos.x = regionPos.y = regionPos.z = 0;

        uint32_t memoryNeeded = sizex * sizey * sizez * sizeof(Tile);
        tiles = (Tile*) malloc( memoryNeeded );
        memset(tiles, 0, memoryNeeded);
    }

    ~WorldSegment() {
        uint32_t num = getNumTiles();
        for(uint32_t i = 0; i < num; i++) {
            if(tiles[i].IsValid()) {
                tiles[i].~Tile();
            }
        }
        free(tiles);
    }

    void Reset(int x=0, int y=0, int z=0, int sizex=0, int sizey=0, int sizez=0, bool hard=false) {
        todraw.clear();

        uint32_t num = getNumTiles();

        for(uint32_t i = 0; i < num; i++) {
            tiles[i].Invalidate();
        }
        uint32_t memoryNeeded = sizex * sizey * sizez * sizeof(Tile);
        //if this is a hard reset, or if the size doesn't match what is needed, get a new segment
        if(hard || sizex*sizey*sizez != size.x*size.y*size.z) {
            free(tiles);
            uint32_t memoryNeeded = sizex * sizey * sizez * sizeof(Tile);
            tiles = (Tile*) malloc( memoryNeeded );
        }
        if(hard) {
            memset(tiles, 0, memoryNeeded);
        }
        this->pos.x = x;
        this->pos.y = y;
        this->pos.z = z - sizez + 1;
        this->size.x = sizex;
        this->size.y = sizey;
        this->size.z = sizez;
        this->displayed.x = ssState.DisplayedSegmentX;
        this->displayed.y = ssState.DisplayedSegmentY;
        this->displayed.z = ssState.DisplayedSegmentZ;
        regionSize.x = regionSize.y = regionSize.z = 0;
        regionPos.x = regionPos.y = regionPos.z = 0;
    }

    uint32_t getNumTiles() {
        return size.x * size.y * size.z;
    }
    
    Tile* ResetTile(int32_t x, int32_t y, int32_t z, df::tiletype type=tiletype::OpenSpace);
    Tile* getTile(int32_t x, int32_t y, int32_t z);
    Tile* getTileLocal(uint32_t x, uint32_t y, uint32_t z);
    Tile* getTileRelativeTo(uint32_t x, uint32_t y, uint32_t z,  dirRelative direction);
    Tile* getTileRelativeTo(uint32_t x, uint32_t y, uint32_t z,  dirRelative direction, int distance);
    Tile* getTile(uint32_t index);
    void CorrectTileForSegmentOffset(int32_t& x, int32_t& y, int32_t& z);
    void CorrectTileForSegmentRotation(int32_t& x, int32_t& y, int32_t& z);
    //void addTile(Tile* b);
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
        drawsegment = new WorldSegment();
        readsegment = new WorldSegment();
        drawmutex = al_create_mutex();
        readmutex = al_create_mutex();
    }
    ~SegmentWrap() {
        delete drawsegment;
        delete readsegment;
        al_destroy_mutex(drawmutex);
        al_destroy_mutex(readmutex);
    }
    void shutdown(){
        drawsegment->Reset();
        readsegment->Reset();
    }
    void lock() {
        al_lock_mutex(drawmutex);
        al_lock_mutex(readmutex);
    }
    void unlock() {
        al_unlock_mutex(drawmutex);
        al_unlock_mutex(readmutex);
    }
    void lockDraw() {
        al_lock_mutex(drawmutex);
    }
    void unlockDraw() {
        al_unlock_mutex(drawmutex);
    }
    void lockRead() {
        al_lock_mutex(readmutex);
    }
    void unlockRead() {
        al_unlock_mutex(readmutex);
    }
    void swap() {
        WorldSegment * temp = drawsegment;
        drawsegment = readsegment;
        readsegment = temp;
    }
    WorldSegment * getRead() {
        return readsegment;
    }
    WorldSegment * getDraw() {
        return drawsegment;
    }
private:
    ALLEGRO_MUTEX * drawmutex;
    ALLEGRO_MUTEX * readmutex;
    WorldSegment * drawsegment;
    WorldSegment * readsegment;
};
