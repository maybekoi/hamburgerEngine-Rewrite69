#include "Engine/Drawing.hpp"
#include "Engine/HamburgerEngine.hpp"
#include <cstring>
#include <cstdio>

int SCREEN_XSIZE = 320;
int SCREEN_CENTERX = SCREEN_XSIZE / 2;
int SCREEN_YSIZE = 240;
int SCREEN_CENTERY = SCREEN_YSIZE / 2;

uint8_t BlendLookupTable[0x100 * 0x100];
uint8_t TintLookupTable[4][0x100];

int GfxDataPosition = 0;
GFXSurface GfxSurface[SURFACE_MAX];
uint8_t GraphicData[GFXDATA_MAX];

void InitDrawingSystem() 
{
    Engine.screenTexture = SDL_CreateTexture(
        Engine.renderer,
        SDL_PIXELFORMAT_RGB565,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_XSIZE,
        SCREEN_YSIZE
    );

    if (!Engine.screenTexture) {
        printf("Failed to create screen texture: %s\n", SDL_GetError());
        return;
    }

    for (int y = 0; y < 0x100; y++) {
        for (int x = 0; x < 0x100; x++) {
            BlendLookupTable[x + (y << 8)] = (x + y) >> 1;
        }
    }

    ClearGraphicsData();
}

void ReleaseDrawingSystem()
{
    if (Engine.screenTexture) {
        SDL_DestroyTexture(Engine.screenTexture);
        Engine.screenTexture = nullptr;
    }
}

void ClearScreen(uint8_t index)
{
    memset(Engine.frameBuffer, index, SCREEN_XSIZE * SCREEN_YSIZE);
}

void FlipScreen()
{
    void* pixels;
    int pitch;

    if (SDL_LockTexture(Engine.screenTexture, NULL, &pixels, &pitch) < 0) {
        printf("Failed to lock texture: %s\n", SDL_GetError());
        return;
    }

    uint16_t* output = (uint16_t*)pixels;
    for (int i = 0; i < SCREEN_XSIZE * SCREEN_YSIZE; i++) {
        uint8_t color = Engine.frameBuffer[i];
        output[i] = (color << 8) | color;
    }

    SDL_UnlockTexture(Engine.screenTexture);

    SDL_RenderClear(Engine.renderer);
    SDL_RenderCopy(Engine.renderer, Engine.screenTexture, NULL, NULL);
    SDL_RenderPresent(Engine.renderer);
}

void DrawSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int sheetID)
{
    if (width + XPos > SCREEN_XSIZE)
        width = SCREEN_XSIZE - XPos;
    if (XPos < 0) {
        sprX -= XPos;
        width += XPos;
        XPos = 0;
    }
    if (height + YPos > SCREEN_YSIZE)
        height = SCREEN_YSIZE - YPos;
    if (YPos < 0) {
        sprY -= YPos;
        height += YPos;
        YPos = 0;
    }
    if (width <= 0 || height <= 0)
        return;

    GFXSurface* surface = &GfxSurface[sheetID];
    int pitch = SCREEN_XSIZE - width;
    int gfxPitch = surface->width - width;
    uint8_t* gfxData = &GraphicData[sprX + surface->width * sprY + surface->dataPosition];
    uint8_t* screenPtr = &Engine.frameBuffer[XPos + SCREEN_XSIZE * YPos];

    while (height--) {
        int w = width;
        while (w--) {
            *screenPtr = *gfxData;
            ++gfxData;
            ++screenPtr;
        }
        screenPtr += pitch;
        gfxData += gfxPitch;
    }
} 