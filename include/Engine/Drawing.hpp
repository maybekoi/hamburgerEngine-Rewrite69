#ifndef HAMBURGER_DRAWING_H
#define HAMBURGER_DRAWING_H

#include <cstdint>

#define SPRITESHEETS_MAX (16)
#define SURFACE_MAX      (24)
#define GFXDATA_MAX     (0x400000)

struct GFXSurface {
    char fileName[0x40];
    int height;
    int width;
    int dataPosition;
};

extern int SCREEN_XSIZE;
extern int SCREEN_CENTERX;
extern int SCREEN_YSIZE;
extern int SCREEN_CENTERY;

extern uint8_t BlendLookupTable[0x100 * 0x100];
extern uint8_t TintLookupTable[4][0x100];

extern int GfxDataPosition;
extern GFXSurface GfxSurface[SURFACE_MAX];
extern uint8_t GraphicData[GFXDATA_MAX];

void InitDrawingSystem();
void ReleaseDrawingSystem();
void ClearScreen(uint8_t index);
void FlipScreen();

void DrawSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int sheetID);
void DrawSpriteScaled(int XPos, int YPos, int pivotX, int pivotY, int scaleX, int scaleY, 
                      int width, int height, int sprX, int sprY, int sheetID);
void DrawSpriteRotated(int XPos, int YPos, int pivotX, int pivotY, int width, int height,
                       int rotation, int sprX, int sprY, int sheetID);
void DrawSpriteBlended(int XPos, int YPos, int width, int height, int sprX, int sprY, 
                      int sheetID, uint8_t alpha);

inline void ClearGraphicsData() {
    for (int i = 0; i < SURFACE_MAX; ++i) {
        GfxSurface[i].fileName[0] = '\0';
    }
    GfxDataPosition = 0;
}

#endif // HAMBURGER_DRAWING_H 