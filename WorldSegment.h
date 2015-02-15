#pragma once


#include "common.h"

#include "Tile.h"

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
    std::vector<draw_event> todraw;

    std::vector<SS_Unit*> units;
    std::vector<Buildings::t_building*> buildings;

public:
    bool loaded;
    bool processed;
    //these are the coordinates and size of the loaded segment
    GameState segState;
    WorldSegment(GameState inState) {
        segState = inState;
        segState.Position.z = segState.Position.z - segState.Size.z + 1;


        uint32_t newNumTiles = inState.Size.x * inState.Size.y * inState.Size.z;
        uint32_t memoryNeeded = newNumTiles * sizeof(Tile);
        tiles = (Tile*) malloc( memoryNeeded );
        memset(tiles, 0, memoryNeeded);
    }

    ~WorldSegment() {
        uint32_t num = getNumTiles();
        for(uint32_t i = 0; i < num; i++) {
            Tile::InvalidateAndDestroy(& tiles[i]);
        }
        ClearBuildings();
        ClearUnits();
        free(tiles);
    }

    void Reset(GameState inState, bool hard=false) {
        //clear and free old data
        ClearBuildings();
        ClearUnits();
        todraw.clear();
        for(uint32_t i = 0; i < getNumTiles(); i++) {
            Tile::InvalidateAndDestroy(& tiles[i]);
        }

        uint32_t newNumTiles = inState.Size.x * inState.Size.y * inState.Size.z;
        uint32_t memoryNeeded = newNumTiles * sizeof(Tile);
        //if this is a hard reset, or if the size doesn't match what is needed, get a new segment
        if(hard || newNumTiles != getNumTiles()) {
            free(tiles);
            tiles = (Tile*) malloc( memoryNeeded );

            //on a hard reset, zero out the entire array
            if(hard) {
                memset(tiles, 0, memoryNeeded);
            //otherwise, just invalidate the entire set of tiles
            } else {
                for(uint32_t i = 0; i < newNumTiles; i++) {
                    tiles[i].Invalidate();
                }
            }
        }

        segState = inState;
        segState.Position.z = segState.Position.z - segState.Size.z + 1;
    }

    inline uint32_t getNumTiles() {
        return segState.Size.x * segState.Size.y * segState.Size.z;
    }

    Tile* ResetTile(int32_t x, int32_t y, int32_t z, df::tiletype type=tiletype::OpenSpace);
    Tile* getTile(int32_t x, int32_t y, int32_t z);
    Tile* getTileLocal(int32_t x, int32_t y, int32_t z);
    Tile* getTileRelativeTo(int32_t x, int32_t y, int32_t z,  dirRelative direction);
    Tile* getTileRelativeTo(int32_t x, int32_t y, int32_t z,  dirRelative direction, int distance);
    Tile* getTile(uint32_t index);
    bool ConvertToSegmentLocal(int32_t & x, int32_t & y, int32_t & z);
    uint32_t ConvertLocalToIndex(int32_t x, int32_t y, int32_t z);
    void CorrectTileForSegmentOffset(int32_t& x, int32_t& y, int32_t& z);
    void CorrectTileForSegmentRotation(int32_t& x, int32_t& y, int32_t& z);
    //void addTile(Tile* b);
    void AssembleBlockTiles(int32_t firstX, int32_t firstY, int32_t lastX, int32_t lastY, int32_t incrx, int32_t incry, int32_t z);
    void AssembleAllTiles();
    void AssembleSprite(draw_event d);
    void DrawAllTiles();
    //void drawPixels();
    bool CoordinateInsideSegment(int32_t x, int32_t y, int32_t z);
    bool RangeInsideSegment(int32_t min_x, int32_t min_y, int32_t min_z, int32_t max_x, int32_t max_y, int32_t max_z);
    bool CoordinateInteriorSegment(int32_t x, int32_t y, int32_t z, uint32_t shellthick);
    void PushBuilding( Buildings::t_building * tempbuilding);
    void ClearBuildings();
    void PushUnit( SS_Unit * unit);
    void ClearUnits();
};

// FIXME: make nicer. one day. maybe.
class SegmentWrap
{
private:
    static const GameState zeroState;
public:
    SegmentWrap() {
        drawsegment = new WorldSegment(zeroState);
        readsegment = new WorldSegment(zeroState);
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
        drawsegment->Reset(zeroState);
        readsegment->Reset(zeroState);
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

extern SegmentWrap map_segment;
