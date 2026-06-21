#include "worldGenerator.hpp"
#include "blocks.hpp"
#include "gameMap.hpp"
#include "randomStuff.hpp"

void generateWorld(GameMap &gameMap, int seed){
    const int w { 900 };
    const int h { 500 };

    gameMap.create(w, h);

    int stoneSize { 380 };
    int dirtSize { 50 };

    std::ranlux24_base rng(seed);

    for (int x = 0; x < w; x++){
        
        
        for(int y = 0; y < h; y++){
            Block b;

            if(y < h - (dirtSize + stoneSize)){
                //air
            }else if(y == h - (dirtSize + stoneSize)){
                b.type = Block::grassBlock;
            }else if(y < h - stoneSize){
                b.type = Block::dirt;
            }else{
                b.type = Block::stone;

                if(getRandomChance(rng, 0.01f)){
                    b.type = Block::gold;
                }
            }

            gameMap.getBlock(x, y) = b;
        }
    }
}

