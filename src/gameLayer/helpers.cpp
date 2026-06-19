#include "helpers.hpp"
#include "raylib.h"

Rectangle getTextureAtlas(int x, int y, int cellSizePixelX, int cellSizePixelY)
{
    return Rectangle
        {
            (float)x * cellSizePixelX, 
            (float)y * cellSizePixelY, 
            (float)cellSizePixelX, 
            (float)cellSizePixelY
        };
}
