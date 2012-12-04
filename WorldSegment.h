#pragma once

#include "Block.h"

extern SegmentWrap map_segment;

enum Draw_Event_Type{
    TintedScaledBitmap,
    CreatureText
};

struct Draw_Event{
    Draw_Event_Type type;
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
    vector<Block*> blocks;
    vector<Draw_Event> todraw;
public:
    bool loaded;
    bool processed;
    int x, y, z;
    int displayedx, displayedy, displayedz;
    int sizex, sizey, sizez;
    unsigned char rotation;
    Crd3D regionSize;
    Block** blocksAsPointerVolume;
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

        uint32_t memoryNeeded = sizex * sizey * sizez * sizeof(Block*);
        blocksAsPointerVolume = (Block**) malloc( memoryNeeded );
        memset(blocksAsPointerVolume, 0, memoryNeeded);
    }

    ~WorldSegment() {
        uint32_t num = (uint32_t)blocks.size();
        for(uint32_t i = 0; i < num; i++) {
            delete(blocks[i]);
        }
        blocks.clear();
    }

    void Dispose(void) {
        free(blocksAsPointerVolume);
    }

    uint32_t getNumBlocks() {
        return (uint32_t)blocks.size();
    }

    Block* getBlock(int32_t x, int32_t y, int32_t z);
    Block* getBlockLocal(uint32_t x, uint32_t y, uint32_t z);
    Block* getBlockRelativeTo(uint32_t x, uint32_t y, uint32_t z,  dirRelative direction);
    Block* getBlockRelativeTo(uint32_t x, uint32_t y, uint32_t z,  dirRelative direction, int distance);
    Block* getBlock(uint32_t index);
    void CorrectBlockForSegmentOffset(int32_t& x, int32_t& y, int32_t& z);
    void CorrectBlockForSegmentRotation(int32_t& x, int32_t& y, int32_t& z);
    void addBlock(Block* b);
    void AssembleAllBlocks();
    void AssembleSprite(Draw_Event d);
    void DrawAllBlocks();
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
