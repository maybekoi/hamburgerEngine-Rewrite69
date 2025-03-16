#define _CRT_SECURE_NO_WARNINGS
#include <SDL2/SDL_main.h>
#include <SDL2/SDL.h>
#include "Engine/HamburgerEngine.hpp"
#include "Engine/Drawing.hpp"
#include "Engine/Reader.hpp"
#include <stdio.h>
#include <cstdint>


HamburgerEngine Engine;

void HamburgerEngine::Init()
{
    strcpy(gameTitle, "Hamburger Engine Game");
    strcpy(gameDescription, "A game made with Hamburger Engine");

    if (!LoadGameConfig("GameConfig.bin")) {
        printf("Failed to load GameConfig.bin, using defaults\n");
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, vsync ? "1" : "0");
    
    window = SDL_CreateWindow(
        gameTitle,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        320 * windowScale,
        240 * windowScale,
        SDL_WINDOW_SHOWN | (startFullScreen ? SDL_WINDOW_FULLSCREEN : 0)
    );

    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        return;
    }

    frameBuffer = new uint8_t[320 * 240];
    initialized = true;
}

void HamburgerEngine::Run()
{
    if (!initialized)
        return;

    gameRunning = true;
    while (gameRunning) {
        ProcessInput();
        Update();
        Render();
    }
}

void HamburgerEngine::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            gameRunning = false;
    }
}

void HamburgerEngine::Update() {}

void HamburgerEngine::Render()
{
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void HamburgerEngine::Release()
{
    ReleaseDrawingSystem();
    
    if (frameBuffer)
        delete[] frameBuffer;
        
    if (renderer)
        SDL_DestroyRenderer(renderer);
        
    if (window)
        SDL_DestroyWindow(window);
        
    SDL_Quit();
}

bool HamburgerEngine::LoadGameConfig(const char* filepath) 
{
    if (CheckBinFile("Data.bin")) {
        printf("Found Data.bin file\n");
        LoadBinFile("Data.bin");
    }

    FileInfo info = {};
    if (!LoadFile(filepath, &info)) {
        printf("Failed to open config file: %s\n", filepath);
        return false;
    }

    uint8_t len;
    if (ReadFileData(&info, &len, 1) != 1) {
        printf("Failed to read title length\n");
        CloseFile(&info);
        return false;
    }
    
    if (ReadFileData(&info, gameTitle, len) != len) {
        printf("Failed to read title\n");
        CloseFile(&info);
        return false;
    }
    gameTitle[len] = '\0';

    printf("Loaded title: %s\n", gameTitle);

    ReadFileData(&info, &len, 1);
    ReadFileData(&info, gameDescription, len);
    gameDescription[len] = '\0';

    ReadFileData(&info, &windowScale, sizeof(int));
    ReadFileData(&info, &refreshRate, sizeof(int));
    ReadFileData(&info, &startFullScreen, sizeof(bool));
    ReadFileData(&info, &borderless, sizeof(bool));
    ReadFileData(&info, &vsync, sizeof(bool));

    CloseFile(&info);
    return true;
}

int SDL_main(int argc, char* argv[])
{
    Engine.Init();
    Engine.Run();
    Engine.Release();
    return 0;
}
