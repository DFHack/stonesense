#pragma once

#include <variant>

#include "common.h"

#include "Tile.h"
#include "GameState.h"

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
    std::vector<Tile> tiles;
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
        tiles.clear();
        tiles.insert(tiles.end(), newNumTiles, {});
    }

    ~WorldSegment() {
        tiles.clear();
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
            tiles.clear();
            tiles.insert(tiles.end(), newNumTiles, {});
        }
        else {
            // Otherwise, reset all existing tiles to their initial state
            for (auto& t : tiles)
                t.Reset();
        }

        segState = inState;
        segState.Position.z = segState.Position.z - segState.Size.z + 1;
    }

    inline uint32_t getNumTiles() {
        return segState.Size.x * segState.Size.y * segState.Size.z;
    }

    Tile* ResetTile(auto x, auto y, auto z, df::tiletype type=DFHack::tiletype::OpenSpace);
    Tile* getTile(auto x, auto y, auto z);
    Tile* getTileLocal(auto x, auto y, auto z);
    Tile* getTileRelativeTo(auto x, auto y, auto z,  dirRelative direction);
    Tile* getTileRelativeTo(auto x, auto y, auto z,  dirRelative direction, int distance);
    Tile* getTile(uint32_t index);
    bool ConvertToSegmentLocal(auto & x, auto & y, auto & z);
    uint32_t ConvertLocalToIndex(auto x, auto y, auto z);
    void CorrectTileForSegmentOffset(auto& x, auto& y, auto& z);
    void CorrectTileForSegmentRotation(auto& x, auto& y, auto& z);
    //void addTile(Tile* b);
    // void AssembleBlockTiles(int32_t firstX, int32_t firstY, int32_t lastX, int32_t lastY, int32_t incrx, int32_t incry, int32_t z);
    void AssembleAllTiles();
    void AssembleSprite(draw_event d);
    void DrawAllTiles();
    //void drawPixels();
    bool CoordinateInsideSegment(auto x, auto y, auto z);
    bool RangeInsideSegment(auto min_x, auto min_y, auto min_z, auto max_x, auto max_y, auto max_z);
    bool CoordinateInteriorSegment(auto x, auto y, auto z, uint32_t shellthick);
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
        al_lock_mutex(drawmutex);
        al_lock_mutex(readmutex);
        drawsegment->Reset(zeroState);
        readsegment->Reset(zeroState);
        al_unlock_mutex(readmutex);
        al_unlock_mutex(drawmutex);
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
