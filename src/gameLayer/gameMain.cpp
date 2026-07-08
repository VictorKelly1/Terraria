#include <cstddef>
#include <raylib.h>

#include "gameMain.hpp"
#include "blocks.hpp"
#include "helpers.hpp"
#include "imgui.h"
#include "raymath.h"
#include "saveMap.hpp"
#include "structure.hpp"
#include "worldGenerator.hpp"

#include <cmath>
#include <asserts.hpp>
#include <assetManager.hpp>
#include <gameMap.hpp>

struct GameData
{
    GameMap gameMap;
    Camera2D camera;

    int creativeSelectedBlock = Block::dirt;

    Vector2 selectionStart = {};
    Vector2 selectionEnd = {};
    Structure copyStructure;

    char saveName[100] = {};
}gameData;

AssetManager assetManager;

bool showImgui = false;

bool initGame()
{

	assetManager.loadAll();

    generateWorld(gameData.gameMap);

    gameData.camera.target = {20, 120};
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

    //
    if (IsKeyPressed(KEY_F10)) { showImgui = !showImgui; }

#pragma region camera movement

    if(IsKeyDown(KEY_LEFT)) gameData.camera.target.x -= 7.f * deltaTime;
    if(IsKeyDown(KEY_RIGHT)) gameData.camera.target.x += 7.f * deltaTime;
    if(IsKeyDown(KEY_UP)) gameData.camera.target.y -= 7.f * deltaTime;
    if(IsKeyDown(KEY_DOWN)) gameData.camera.target.y += 7.f * deltaTime;

#pragma endregion 

    Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);
    int blockX = (int)std::floor(worldPos.x);
    int blockY = (int)std::floor(worldPos.y);

    if(gameData.creativeSelectedBlock < 0) {gameData.creativeSelectedBlock = 0; }
    if(gameData.creativeSelectedBlock >= Block::BLOCKS_COUNT) { gameData.creativeSelectedBlock = Block::BLOCKS_COUNT + 1; }

    if(showImgui){
        if(IsKeyPressed(KEY_ONE)) { gameData.selectionStart = Vector2{(float)blockX, (float)blockY}; }
        if(IsKeyPressed(KEY_TWO)) { gameData.selectionEnd = Vector2{(float)blockX, (float)blockY}; }
        if(IsKeyPressed(KEY_THREE)) {
            gameData.copyStructure.pasteIntoMap(gameData.gameMap, Vector2{(float)blockX, (float)blockY});
        }

        if(gameData.selectionStart.x > gameData.selectionEnd.x){
            std::swap(gameData.selectionStart.x, gameData.selectionEnd.x);
        }

        if(gameData.selectionStart.y > gameData.selectionEnd.y){
            std::swap(gameData.selectionStart.y, gameData.selectionEnd.y);
        }

    }

    if(!showImgui){
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            auto b = gameData.gameMap.getBlockSafe(blockX, blockY);

            if(b) {
                *b = {};
            }
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            auto b = gameData.gameMap.getBlockSafe(blockX, blockY);

            if(b){
                b->type = gameData.creativeSelectedBlock;
            }
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

    if(showImgui){
        Rectangle rect;
        rect.x = gameData.selectionStart.x;
        rect.y = gameData.selectionStart.y;
        rect.width = gameData.selectionEnd.x - gameData.selectionStart.x;
        rect.height = gameData.selectionEnd.y - gameData.selectionStart.y;

        rect.width++;
        rect.height++;

        DrawRectangleLinesEx(rect, 0.1, {20, 101, 250, 145});
    }

    EndMode2D();

    if(showImgui){
        ImGui::Begin("Game controll");

        ImGui::SliderFloat("Camera Zoom:", &gameData.camera.zoom, 10, 150);

        ImGui::Separator();

        if(ImGui::Button("Copy")){
            gameData.copyStructure.copyFromMap(gameData.gameMap, gameData.selectionStart, gameData.selectionEnd);
        }

        ImGui::InputText("File name", gameData.saveName, sizeof(gameData.saveName));

        if(ImGui::Button("Safe to file")){
            std::string path = RESOURCES_PATH "structures/";
            path += gameData.saveName;
            path += ".bin";

            saveBlockDataToFile(gameData.copyStructure.mapData, gameData.copyStructure.w, gameData.copyStructure.h, path.c_str());

        } 

        if(ImGui::Button("Load from file")){
            std::string path = RESOURCES_PATH "structures/";
            path += gameData.saveName;
            path += ".bin";


            loadBlockDataFromFile(gameData.copyStructure.mapData, gameData.copyStructure.w, gameData.copyStructure.h, path.c_str());
        }


        for(auto i{0}; i < Block::BLOCKS_COUNT; ++i){
            auto atlas = getTextureAtlas(i, 0, 32, 32);
            atlas.x /= assetManager.textures.width;
            atlas.width /= assetManager.textures.width;
            atlas.y /= assetManager.textures.height;
            atlas.height /= assetManager.textures.height;

            ImGui::PushID(i);

            ImTextureID tex = ( ImTextureID)(intptr_t)assetManager.textures.id;
            if(ImGui::ImageButton(tex, {35, 35}, {atlas.x, atlas.y}, {atlas.x + atlas.width, atlas.y + atlas.height})){
                gameData.creativeSelectedBlock = i;
            }

            ImGui::PopID();
            
            if(i % 10 != 0 ){
                ImGui::SameLine();
            }
        }


        ImGui::End();
    }

    DrawFPS(10, 10);

	return true;
}

void closeGame()
{
}
