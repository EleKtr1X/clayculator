#ifndef CLAY_RENDERER_RAYLIB_H
#define CLAY_RENDERER_RAYLIB_H

#include "raylib.h"
#include "raymath.h"
#include "../clay.h"

typedef struct
{
    uint32_t fontId;
    Font font;
} Raylib_Font;

extern uint32_t measureCalls;
extern Raylib_Font Raylib_fonts[10];
extern Camera Raylib_camera;

Ray GetScreenToWorldPointWithZDistance(Vector2 position, Camera camera, int screenWidth, int screenHeight, float zDistance);
void Clay_Raylib_Initialize(int width, int height, const char *title, unsigned int flags);
void Clay_Raylib_Render(Clay_RenderCommandArray renderCommands);
Clay_Dimensions Raylib_MeasureText(Clay_String *text, Clay_TextElementConfig *config);

#endif

