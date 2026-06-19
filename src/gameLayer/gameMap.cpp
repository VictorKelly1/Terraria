#include "gameMap.hpp"
#include "blocks.hpp"
#include <asserts.hpp>

void GameMap::create(int w, int h)
{
    *this = {};
    mapData.resize(w * h);
    
    this->w = w;
    this->h = h;

    for (auto &e : mapData) { e = {}; }
}

Block &GameMap::getBlock(int x, int y){
    permaAssertCommentDevelopement(mapData.size() == w * h, "Map data not initialized");

    permaAssertCommentDevelopement(x >= 0 && y >= 0 && x < w && y < h, "GetBlockUnsafe out of bounds");

    return mapData[x + y * w];
}

Block *GameMap::getBlockSafe(int x, int y) {
    permaAssertCommentDevelopement(mapData.size() == w * h, "Map data not initialized");

    if ( x < 0 || y < 0 || x >= w || y >= h) { return nullptr; }

    return &mapData[x + y * w];
}
