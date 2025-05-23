#ifndef HAMBURGER_ENGINE_H
#define HAMBURGER_ENGINE_H

#include <SDL2/SDL_main.h>
#include <SDL2/SDL.h>
#include <cstdint>

class HamburgerEngine {
public:
    HamburgerEngine() {}
    ~HamburgerEngine() {}

    bool initialized = false;
    bool gameRunning = false;
    bool useBinFile = true;
    
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    uint8_t *frameBuffer = nullptr;
    
    bool isFullScreen = false;
    bool startFullScreen = false;
    bool borderless = false;
    bool vsync = false;
    int windowScale = 2;
    int refreshRate = 60;
    
    char gameTitle[0x40];
    char gameDescription[0x100];
    
    SDL_Texture* screenTexture = nullptr;
    
    int targetFPS = 60;
    int screenWidth = 320;
    int screenHeight = 240;
    bool showFPS = false;
    
    float currentFPS = 0.0f;
    uint64_t lastFPSUpdate = 0;
    int frameCount = 0;
    
    void Init();
    void Run();
    bool LoadGameConfig(const char *filepath);
    bool InitDataFile();
    void Release();

private:
    void ProcessInput();
    void Update();
    void Render();
};

extern HamburgerEngine Engine;

#endif // HAMBURGER_ENGINE_H 