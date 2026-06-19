#include <cstddef>
#include <raylib.h>
#include "gameMain.hpp"
#include "helpers.hpp"
#include <asserts.hpp>
#include <assetManager.hpp>
#include <gameMap.hpp>

struct GameData
{
    GameMap gameMap;
    Camera2D camera;

}gameData;

AssetManager assetManager;

bool initGame()
{

	assetManager.loadAll();

    gameData.gameMap.create(30, 10);

    gameData.gameMap.getBlock(0, 0).type = Block::dirt;
    gameData.gameMap.getBlock(1, 1).type = Block::grass;
    gameData.gameMap.getBlock(2, 2).type = Block::goldBlock;
    gameData.gameMap.getBlock(3, 3).type = Block::glass;
    gameData.gameMap.getBlock(4, 4).type = Block::platform;

    gameData.camera.target = {0, 0};
    gameData.camera.rotation = 0.0f;
    gameData.camera.zoom = 100.0f;

	return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime();
	if (deltaTime > 1.f / 5) { deltaTime = 1 / 5.f; }

    gameData.camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};

    ClearBackground({75, 75, 150, 255});

#pragma region camera movement

    if(IsKeyDown(KEY_LEFT)) gameData.camera.target.x -= 7.f * deltaTime;
    if(IsKeyDown(KEY_RIGHT)) gameData.camera.target.x += 7.f * deltaTime;
    if(IsKeyDown(KEY_UP)) gameData.camera.target.y -= 7.f * deltaTime;
    if(IsKeyDown(KEY_DOWN)) gameData.camera.target.y += 7.f * deltaTime;

#pragma endregion 

    BeginMode2D(gameData.camera);

    for(size_t y{}; y < gameData.gameMap.h; ++y){
        for (size_t x{}; x < gameData.gameMap.w; ++x){
            
            auto& b = gameData.gameMap.getBlock(x,  y);

            if (b.type != Block::air){
                Rectangle texturesUV;
                texturesUV.width = 32;
                texturesUV.height = 32;
                texturesUV.x = b.type * 32;
                texturesUV.y = 0;

                DrawTexturePro(
                    assetManager.textures ,
                    getTextureAtlas(b.type, 0, 32, 32),
                    {(float)x, (float)y, 1, 1},
                    {0, 0},
                    0.0f,
                    WHITE
                );
            }
        }
    }
    EndMode2D();

	return true;
}

void closeGame()
{
}
