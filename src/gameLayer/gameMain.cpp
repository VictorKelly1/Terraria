#include <algorithm>
#include <cstddef>
#include <raylib.h>
#include "gameMain.hpp"
#include "blocks.hpp"
#include "helpers.hpp"
#include "raymath.h"

#include <cmath>
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

    gameData.gameMap.create(700, 500);

    for (auto i{0}; i < 700; ++i){
        for (auto j {0}; j < 500; ++j){
            gameData.gameMap.getBlock(i, j).type = Block::stone;
        }
    }

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

    Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);
    int blockX = (int)std::floor(worldPos.x);
    int blockY = (int)std::floor(worldPos.y);

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        auto b = gameData.gameMap.getBlockSafe(blockX, blockY);

        if(b) {
            *b = {};
        }
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        auto b = gameData.gameMap.getBlockSafe(blockX, blockY);

        if(b){
            b->type = Block::gold;
        }
    }

    BeginMode2D(gameData.camera);

    Vector2 topLeftView = GetScreenToWorld2D({0, 0}, gameData.camera);
    Vector2 bottomRightView = GetScreenToWorld2D({(float)GetScreenWidth(), (float)GetScreenHeight()}, gameData.camera);

    int startXView = (int)floorf(topLeftView.x - 1);
    int endXView = (int)ceilf(bottomRightView.x + 1);
    int startYView = (int)floorf(topLeftView.y - 1);
    int endYView = (int)ceilf(bottomRightView.y + 1);

    startXView = Clamp(startXView, 0, gameData.gameMap.w - 1);
    endXView = Clamp(endXView, 0, gameData.gameMap.w - 1);

    startYView = Clamp(startYView, 0, gameData.gameMap.h - 1);
    endYView = Clamp(endYView, 0, gameData.gameMap.h - 1);

    for (size_t y{}; y < endYView; ++y) {
        for (size_t x{}; x < endXView; ++x) {
        
            auto& b = gameData.gameMap.getBlock(x, y);

            if (b.type != Block::air) {
                // Nota: Se asume que getTextureAtlas devuelve el Rectangle de origen correcto
                DrawTexturePro(
                assetManager.textures,
                getTextureAtlas(b.type, 0, 32, 32), 
                { (float)x, (float)y, 1.0f, 1.0f }, // Destino en unidades del mundo
                { 0.0f, 0.0f },                     
                0.0f,                               
                WHITE
                );
            }
        }
    } 

    DrawTexturePro(
        assetManager.frame, 
        {0,0, (float)assetManager.frame.width, (float)assetManager.frame.height}, 
        {(float)blockX, (float)blockY, 1, 1}, 
        {0, 0}, 
        0.0f, 
        WHITE
    );


    EndMode2D();

    DrawFPS(10, 10);

	return true;
}

void closeGame()
{
}
