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

    InitDataFile();
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
        SCREEN_XSIZE * windowScale,
        SCREEN_YSIZE * windowScale,
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

    frameBuffer = new uint8_t[SCREEN_XSIZE * SCREEN_YSIZE];
    InitDrawingSystem();
    
    initialized = true;
}

void HamburgerEngine::Run()
{
    if (!initialized)
        return;

    gameRunning = true;
    uint64_t targetFreq = SDL_GetPerformanceFrequency() / refreshRate;
    uint64_t curTicks = 0;

    while (gameRunning) {
        if (!vsync) {
            if (SDL_GetPerformanceCounter() < curTicks + targetFreq)
                continue;
            curTicks = SDL_GetPerformanceCounter();
        }

        ProcessInput();
        Update();
        Render();
    }
}

void HamburgerEngine::Update()
{
}

void HamburgerEngine::Render()
{
    ClearScreen(0);
    
    frameCount++;
    uint64_t currentTime = SDL_GetPerformanceCounter();
    
    if (currentTime > lastFPSUpdate + SDL_GetPerformanceFrequency()) {
        currentFPS = frameCount * (float)SDL_GetPerformanceFrequency() / 
                    (currentTime - lastFPSUpdate);
        lastFPSUpdate = currentTime;
        frameCount = 0;
    }
    
    if (showFPS) {
        char fpsText[32];
        sprintf(fpsText, "FPS: %.1f", currentFPS);
        printf("%s\r", fpsText);
    }
    
    FlipScreen();
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
    printf("Loading game config...\n");
    
    FileInfo info = {};
    if (!LoadFile(filepath, &info)) {
        printf("Failed to open config file: %s from Data.bin\n", filepath);
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
    printf("Loaded description: %s\n", gameDescription);

    ReadFileData(&info, &windowScale, sizeof(int));
    ReadFileData(&info, &refreshRate, sizeof(int));
    ReadFileData(&info, &startFullScreen, sizeof(bool));
    ReadFileData(&info, &borderless, sizeof(bool));
    ReadFileData(&info, &vsync, sizeof(bool));
    
    // Read new settings
    ReadFileData(&info, &targetFPS, sizeof(int));
    ReadFileData(&info, &screenWidth, sizeof(int));
    ReadFileData(&info, &screenHeight, sizeof(int));
    ReadFileData(&info, &showFPS, sizeof(bool));

    printf("Config loaded successfully\n");
    printf("Screen size: %dx%d\n", screenWidth, screenHeight);
    printf("Target FPS: %d\n", targetFPS);
    printf("Show FPS: %s\n", showFPS ? "true" : "false");
    
    CloseFile(&info);
    return true;
}

void HamburgerEngine::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                gameRunning = false;
                break;
                
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    gameRunning = false;
                break;
        }
    }
}

int SDL_main(int argc, char* argv[])
{
    Engine.Init();
    Engine.Run();
    Engine.Release();
    return 0;
}
