#pragma once 

#include <vector>
#include <blocks.hpp>

#include <gameMap.hpp>
#include <raylib.h>

struct Structure {

    int w { 0 };
    int h { 0 };

    std::vector<Block> mapData;

    void create(int x, int h);

    Block &getBlockUnsafe(int x, int h);
    Block *getBlockSafe(int x, int y);
     
};
