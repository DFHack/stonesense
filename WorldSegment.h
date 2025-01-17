#pragma once

#include <variant>

#include "common.h"

#include "Tile.h"


enum draw_event_type{
    Fog,
    TintedScaledBitmap,
    CreatureText
};

struct Stonesense_Unit;

struct draw_event{
    draw_event_type type;
    std::variant<std::monostate,ALLEGRO_BITMAP*,Stonesense_Unit*> drawobject;
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

    std::vector<std::unique_ptr<Stonesense_Unit>> units;
    std::vector<std::unique_ptr<Stonesense_Building>> buildings;

public:
    bool loaded = false;
    bool processed = false;
    //these are the coordinates and size of the loaded segment
    GameState segState;
    WorldSegment(GameState inState) {
        segState = inState;
        segState.Position.z = segState.Position.z - segState.Size.z + 1;

        uint32_t newNumTiles = inState.Size.x * inState.Size.y * inState.Size.z;
        tiles = new Tile[newNumTiles]();
    }

    ~WorldSegment() {
        delete[] tiles;
        tiles = NULL;
        ClearBuildings();
        ClearUnits();
    }

    void Reset(GameState inState, bool hard=false) {
        //clear and free old data
        ClearBuildings();
        ClearUnits();
        todraw.clear();

        uint32_t newNumTiles = inState.Size.x * inState.Size.y * inState.Size.z;
        //if this is a hard reset, or if the size doesn't match what is needed, get a new segment
        if(hard || newNumTiles != getNumTiles()) {
            delete[] tiles;
            tiles = new Tile[newNumTiles]();
        }
        else {
            // Otherwise, reset all existing tiles to their initial state
            for(uint32_t i = 0; i < getNumTiles(); i++) {
                tiles[i].Reset();
            }
        }

        segState = inState;
        segState.Position.z = segState.Position.z - segState.Size.z + 1;
    }

    inline uint32_t getNumTiles() {
        return segState.Size.x * segState.Size.y * segState.Size.z;
    }

    Tile* ResetTile(int32_t x, int32_t y, int32_t z, df::tiletype type=DFHack::tiletype::OpenSpace);
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
    void PushBuilding( std::unique_ptr<Stonesense_Building> tempbuilding);
    void ClearBuildings();
    void PushUnit( std::unique_ptr<Stonesense_Unit> unit);
    void ClearUnits();
};

// FIXME: make nicer. one day. maybe.
class SegmentWrap
{
private:
    static const GameState zeroState;
public:
    SegmentWrap() {
        drawsegment = std::make_unique<WorldSegment>(zeroState);
        readsegment = std::make_unique<WorldSegment>(zeroState);
        drawmutex = al_create_mutex();
        readmutex = al_create_mutex();
    }
    ~SegmentWrap() {
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
        drawsegment.swap(readsegment);
    }
    WorldSegment * getRead() {
        return readsegment.get();
    }
    WorldSegment * getDraw() {
        return drawsegment.get();
    }
private:
    ALLEGRO_MUTEX * drawmutex;
    ALLEGRO_MUTEX * readmutex;
    std::unique_ptr<WorldSegment> drawsegment;
    std::unique_ptr<WorldSegment> readsegment;
};

extern SegmentWrap map_segment;
