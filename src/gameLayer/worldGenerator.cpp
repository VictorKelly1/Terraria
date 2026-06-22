#include "worldGenerator.hpp"
#include "blocks.hpp"
#include "gameMap.hpp"
#include "randomStuff.hpp"
#include <FastNoiseSIMD.h>
#include <cstdlib>
#include <memory>
#include <random>
#include <unistd.h>

void generateWorld(GameMap &gameMap, int seed){
    const int w { 900 };
    const int h { 500 };

    gameMap.create(w, h);

    std::ranlux24_base rng(seed++);

    // Desierto 
    int desertStart = getRandomInt(rng, 10, w - 210);
    int desertEnd = desertStart + 100 + getRandomInt(rng, 0, 100);
    if (desertEnd > w) { desertEnd = w; }

    std::unique_ptr<FastNoiseSIMD> dirtNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());
    std::unique_ptr<FastNoiseSIMD> stoneNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());
    std::unique_ptr<FastNoiseSIMD> cavesNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());

    dirtNoiseGenerator->SetSeed(seed++);
    stoneNoiseGenerator->SetSeed(seed++);
    cavesNoiseGenerator->SetSeed(seed++);

    dirtNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
    dirtNoiseGenerator->SetFractalOctaves(1);
    dirtNoiseGenerator->SetFrequency(0.005f);

    stoneNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
    stoneNoiseGenerator->SetFractalOctaves(4);
    stoneNoiseGenerator->SetFrequency(0.005f);

    cavesNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
    cavesNoiseGenerator->SetFractalOctaves(3);
    cavesNoiseGenerator->SetFrequency(0.02f);

    float* dirtNoise = FastNoiseSIMD::GetEmptySet(w);
    float* stoneNoise = FastNoiseSIMD::GetEmptySet(w);
    float* cavesNoise = FastNoiseSIMD::GetEmptySet(w * h);

    dirtNoiseGenerator->FillNoiseSet(dirtNoise, 0, 0, 0, w, 1, 1);
    stoneNoiseGenerator->FillNoiseSet(stoneNoise, 0, 0, 0, w, 1, 1);
    
    // CORREGIDO: El orden de dimensiones debe ser w (ancho) y luego h (alto)
    cavesNoiseGenerator->FillNoiseSet(cavesNoise, 0, 0, 0, h, w, 1);

    // Convertir de [-1 1] a [0 1] para la superficie
    for (int i = 0; i < w; i++)
    {
        dirtNoise[i] = (dirtNoise[i] + 1) / 2;
        stoneNoise[i] = (stoneNoise[i] + 1) / 2;
    }

    // CORREGIDO: Mapeo correcto del array plano indexado por FastNoise (Y * ancho + X)
    auto getCaveNoise = [&] (int x, int y){
        return cavesNoise[y * w + x];
    }; 

    int dirtOffsetStart = -5;
    int dirtOffsetEnd = 60;

    int stoneHeightStart = 80;
    int stoneHeightEnd = 170;

    for (int x = 0; x < w; x++)
    {
        bool inDesert = (x >= desertStart && x <= desertEnd);

        int stoneHeight = stoneHeightStart + (stoneHeightEnd - stoneHeightStart) * stoneNoise[x];
        int dirtHeight = dirtOffsetStart + (dirtOffsetEnd - dirtOffsetStart) * dirtNoise[x];
        dirtHeight = stoneHeight - dirtHeight;

        int dirtType = Block::dirt;
        int grassType = Block::grassBlock;
        int stoneType = Block::stone;

        if(inDesert){
            dirtType = Block::sand;
            grassType = Block::sand;
            stoneType = Block::sandStone;
        }

        for (int y = 0; y < h; y++)
        {
            Block b;
            b.type = Block::air; // Inicializar por defecto como cielo/aire

            // 1. Construir capas sólidas primarias
            if (y > dirtHeight)
            {
                b.type = dirtType;
            }

            if (y == dirtHeight)
            {
                b.type = grassType;
            }

            if (y >= stoneHeight)
            {
                b.type = stoneType;
            }

            // 2. Modificadores estructurales del Desierto
            if (inDesert){
                int desertMid = (desertStart + desertEnd) / 2;
                int desertHalfWidth = (desertEnd - desertStart) / 2;
                int distanceFromDesertMid = std::abs(x - desertMid); 

                float desertDistance = 1 - distanceFromDesertMid / float(desertHalfWidth);

                int desertStoneStart = 10 + stoneHeight;
                int desertStoneDepth = 20 + stoneHeight;

                int triangleStoneY = desertStoneStart + desertDistance * desertStoneDepth;

                if (y > triangleStoneY){
                    b.type = Block::stone;
                }
            }
            
            // 3. Sistema de Cuevas (Solo excavar si hay terreno y el umbral de ruido coincide)
            // Cambiado a < -0.30f o > 0.30f dependiendo de la densidad que busques (ej: < -0.40f genera túneles óptimos)
            /*if (y > dirtHeight) // Evita que las cuevas destruyan el cielo abierto
            {
                if (getCaveNoise(x, y) < -0.30f) 
                {
                    b.type = Block::air;
                }
            }*/

            gameMap.getBlock(x, y) = b;
        }
    }

    FastNoiseSIMD::FreeNoiseSet(dirtNoise);
    FastNoiseSIMD::FreeNoiseSet(stoneNoise);
    FastNoiseSIMD::FreeNoiseSet(cavesNoise);


    /*for (auto i{0}; i < 20; ++i){
        float x = getRandomInt(rng, 10, w - 10);
        float y = getRandomInt(rng, 51, h - 10);

        float dirtX = (getRandomFloat(rng, -1, 1));
        float dirtY = (getRandomFloat(rng, -1, 1));

        int wormLength = getRandomInt(rng, 5, 20);
        float radius = 2.5f;

        int changeDirectionTime = getRandomInt(rng, 5, 20);

        for(auto j{0}; j < wormLength; ++j){
            int intRadius = std::ceil(radius);

            for(auto ox {-intRadius}; ox <= intRadius; ++ox){
                for (auto oy {-intRadius}; oy <= intRadius; ++oy){
                    float distSq = ox * ox + oy;
                    if(distSq <= radius * radius){
                        int digX = x + ox;
                        int digY = y + oy;

                        auto b = gameMap.getBlockSafe(digX, digY);

                        if(b){
                            b->type = Block::air;
                        }
                    }
                }
            }

            changeDirectionTime--;

            if (changeDirectionTime <= 0){
                changeDirectionTime = getRandomInt(rng, 5, 20);

                if (getRandomChance(rng, 0.7)){
                    float keepFactor = 0.8;

                    //big chance we keep a very similar direction
                    dirX = dirX * keepFactor + (getRandomFloat(rng, -1, 1)) * (1.f - keepFactor);
                    dirY = dirY * keepFactor + (getRandomFloat(rng, -1, 1)) * (1.f - keepFactor);
                }
                else{
                    float keepFactor = 0.2;

                    //big chance we keep a very similar direction
                    dirX = dirX * keepFactor + (getRandomFloat(rng, -1, 1)) * (1.f - keepFactor);
                    dirY = dirY * keepFactor + (getRandomFloat(rng, -1, 1)) * (1.f - keepFactor);
                }
            }
        }

    }*/ 
}
