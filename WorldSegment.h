#pragma once

#include "Block.h"

class WorldSegment
{
private:
    vector<Block*> blocks;
public:
    bool loaded;
    bool processed;
    int x, y, z;
    int sizex, sizey, sizez;
    unsigned char rotation;
    long read_time;
    long beautify_time;
    Crd3D regionSize;
    Block** blocksAsPointerVolume;
    WorldSegment(int x, int y, int z, int sizex, int sizey, int sizez) {
        this->x = x;
        this->y = y;
        this->z = z - sizez + 1;
        this->sizex = sizex;
        this->sizey = sizey;
        this->sizez = sizez;

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
    void addBlock(Block* b);
    void drawAllBlocks();
    void drawPixels();
    bool CoordinateInsideSegment(uint32_t x, uint32_t y, uint32_t z);
    bool CoordinateInteriorSegment(uint32_t x, uint32_t y, uint32_t z, uint32_t shellthick);
};

// FIXME: make nicer. one day. maybe.
class SegmentWrap
{
public:
    SegmentWrap() {
        segment = NULL;
        mutex = al_create_mutex();
        locked = false;
    }
    ~SegmentWrap() {
        al_destroy_mutex(mutex);
    }
    void lock() {
        al_lock_mutex(mutex);
        locked = true;
    }
    void unlock() {
        al_unlock_mutex(mutex);
        locked = false;
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
