#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "raylib/clay_renderer_raylib.h"
#include "raylib/clay_renderer_raylib.c"
#include <string.h>

const int FONT_60_REGULAR = 0;
const int FONT_60_BOLD = 1;
const int FONT_32_REGULAR = 2;
const int FONT_32_BOLD = 3;

const Clay_Color COLOURS[] = {
    { 0, 0, 0, 255 }, // background
    { 255, 101, 0, 255 }, // primary
    { 30, 62, 98, 255 }, // secondary
    { 11, 25, 44, 255 }, // tertiary
    { 255, 255, 255, 255 }, // text
};

const int BTN_COLS = 5;
const int BTN_ROWS = 4;

typedef struct {
    intptr_t id;
    Clay_String label;
    uint8_t colour;
} Button;

typedef struct {
    Button *buttons;
} ButtonRow;

typedef struct {
    ButtonRow *rows;
} ButtonGrid;

ButtonGrid buttonGrid = {
    .rows = (ButtonRow[]) {
        {
            .buttons = (Button[]) {
                { .id = 16, .label = CLAY_STRING("|x|"), .colour = 3 },
                { .id = 17, .label = CLAY_STRING("1/x"), .colour = 3 },
                { .id = 18, .label = CLAY_STRING("C"), .colour = 3 },
                { .id = 19, .label = CLAY_STRING("<"), .colour = 3 },
            },
        },
        {
            .buttons = (Button[]) {
                { .id = 7, .label = CLAY_STRING("7"), .colour = 2 },
                { .id = 8, .label = CLAY_STRING("8"), .colour = 2 },
                { .id = 9, .label = CLAY_STRING("9"), .colour = 2 },
                { .id = 15, .label = CLAY_STRING("/"), .colour = 3 },
            },
        },
        {
            .buttons = (Button[]) {
                { .id = 4, .label = CLAY_STRING("4"), .colour = 2 },
                { .id = 5, .label = CLAY_STRING("5"), .colour = 2 },
                { .id = 6, .label = CLAY_STRING("6"), .colour = 2 },
                { .id = 14, .label = CLAY_STRING("*"), .colour = 3 },
            },
        },
        {
            .buttons = (Button[]) {        
                { .id = 1, .label = CLAY_STRING("1"), .colour = 2 },
                { .id = 2, .label = CLAY_STRING("2"), .colour = 2 },
                { .id = 3, .label = CLAY_STRING("3"), .colour = 2 },
                { .id = 13, .label = CLAY_STRING("-"), .colour = 3 },
            },
        },
        {
            .buttons = (Button[]) {        
                { .id = 10, .label = CLAY_STRING("."), .colour = 2 },
                { .id = 0, .label = CLAY_STRING("0"), .colour = 2 },
                { .id = 11, .label = CLAY_STRING("="), .colour = 1 },
                { .id = 12, .label = CLAY_STRING("+"), .colour = 3 },
            },
        }
    },
};

double num1 = 0;
double num2 = 0;
bool result = false;
char display[100] = "0";
bool hasPoint = false;
int curOp = -1;

void HandleNumberButtonInteraction(intptr_t id) {
    char tmp[10];
    itoa(id, tmp, 10);

    if (strlen(display) < 9) {
        if (strcmp(display, "0") == 0) {
            if (id == 10) {
                strcpy(display, "0.");
                hasPoint = true;
            } else {
                strcpy(display, tmp);
            }
        } else {
            if (id == 10) {
                if (!hasPoint) {
                    strcat(display, ".");
                    hasPoint = true;
                }
            } else {
                strcat(display, tmp);
            }
        }
    }
}

void HandleEqButtonInteraction() {
    if (curOp != -1) {
        switch (curOp) {
            case 0:
                num1 += num2;
                break;
            case 1:
                num1 -= num2;
                break;
            case 2:
                num1 *= num2;
                break;
            case 3:
                num1 /= num2;
                break;
        }
        snprintf(display, sizeof display, "%g", num1);
        result = true;
    }
    printf("%f %f\n", num1, num2);
}

void HandleButtonInteraction(
    Clay_ElementId elementId,
    Clay_PointerData pointerData,
    intptr_t id
) {
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        if (id >= 0 && id <= (BTN_COLS * BTN_ROWS) - 1) {
            if (id <= 10) {
                if (!result) {
                    HandleNumberButtonInteraction(id);
                }
            }
            
            switch (id) {
                case 11:
                    if (!result) {
                        num2 = strtod(display, NULL);
                    }
                    HandleEqButtonInteraction();
                    break;
                case 12 ... 15:
                    num1 = strtod(display, NULL);
                    result = false;
                    strcpy(display, "0");
                    curOp = id - 12;
                    break;
                case 16:
                    num1 = fabs(strtod(display, NULL));
                    snprintf(display, sizeof display, "%g", num1);
                    break;
                case 17:
                    num1 = 1/strtod(display, NULL);
                    snprintf(display, sizeof display, "%g", num1);
                    break;
                case 18:
                    num1 = 0;
                    num2 = 0;
                    strcpy(display, "0");
                    curOp = -1;
                    result = false;
                    break;
                case 19:
                    if (!result) {
                        size_t lastIndex = strlen(display) - 1;
                        if (display[lastIndex] == '.') hasPoint = false;
                        if (lastIndex == 0) {
                            display[lastIndex] = '0';
                        } else {
                            display[lastIndex] = '\0';
                        }
                    }
            }
        }
    }
}

int main(void) {
    Clay_Raylib_Initialize(400, 625, "Clayculator", FLAG_WINDOW_RESIZABLE);
    
    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemory = (Clay_Arena) {
        .memory = malloc(clayRequiredMemory),
        .capacity = clayRequiredMemory
    };

    Clay_Initialize(clayMemory, (Clay_Dimensions) {
        .width = GetScreenWidth(),
        .height = GetScreenHeight(),
    }, (Clay_ErrorHandler) {});

    // font stuff
    Clay_SetMeasureTextFunction(Raylib_MeasureText);
    Raylib_fonts[FONT_60_REGULAR] = (Raylib_Font) {
        .font = LoadFontEx("resources/AtkinsonHyperlegible-Regular.ttf", 72, 0, 0),
        .fontId = FONT_60_REGULAR,
    };
    Raylib_fonts[FONT_60_BOLD] = (Raylib_Font) {
        .font = LoadFontEx("resources/AtkinsonHyperlegible-Bold.ttf", 72, 0, 0),
        .fontId = FONT_60_BOLD,
    };
    Raylib_fonts[FONT_32_REGULAR] = (Raylib_Font) {
        .font = LoadFontEx("resources/AtkinsonHyperlegible-Regular.ttf", 32, 0, 0),
        .fontId = FONT_32_REGULAR,
    };
    Raylib_fonts[FONT_32_BOLD] = (Raylib_Font) {
        .font = LoadFontEx("resources/AtkinsonHyperlegible-Bold.ttf", 32, 0, 0),
        .fontId = FONT_32_BOLD,
    };

    // turn on for debug tools
    // Clay_SetDebugModeEnabled(true);

    while (!WindowShouldClose()) {
        Clay_SetLayoutDimensions((Clay_Dimensions) {
            .width = GetScreenWidth(),
            .height = GetScreenHeight(),
        });
        
        Vector2 mousePosition = GetMousePosition();
        Vector2 scrollDelta = GetMouseDelta();
        Clay_SetPointerState(
            (Clay_Vector2) { mousePosition.x, mousePosition.y },
            IsMouseButtonDown(0)
        );

        Clay_Sizing layoutExpand = {
            .width = CLAY_SIZING_GROW(),
            .height = CLAY_SIZING_GROW(),
        };

        Clay_BeginLayout();
        
        CLAY(
            CLAY_ID("Background"),
            CLAY_RECTANGLE((Clay_RectangleElementConfig) { .color = COLOURS[0] }),
            CLAY_LAYOUT((Clay_LayoutConfig) {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = layoutExpand,
                .padding = { 16, 16 },
                .childGap = 16,
            })
        ) {
            CLAY(
                CLAY_ID("HeaderContainer"),
                CLAY_RECTANGLE((Clay_RectangleElementConfig) {
                    .color = COLOURS[3],
                    .cornerRadius = 16
                }),
                CLAY_LAYOUT((Clay_LayoutConfig) {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = {
                        .height = CLAY_SIZING_FIXED(60),
                        .width = CLAY_SIZING_GROW()
                    },
                    .padding = { 16, 16 },
                    .childAlignment = {
                        .y = CLAY_ALIGN_Y_CENTER
                    }
                })
            ) {
                CLAY_TEXT(CLAY_STRING("Clayculator"), CLAY_TEXT_CONFIG({ 
                    .fontId = FONT_32_BOLD,
                    .fontSize = 32,
                    .textColor = COLOURS[4],
                }));
            }
            CLAY(
                CLAY_ID("CalcResultContainer"),
                CLAY_RECTANGLE((Clay_RectangleElementConfig) {
                    .color = COLOURS[2],
                    .cornerRadius = 16
                }),
                CLAY_LAYOUT((Clay_LayoutConfig) {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = {
                        .height = CLAY_SIZING_FIXED(100),
                        .width = CLAY_SIZING_GROW()
                    },
                    .padding = { 16, 16 },
                    .childAlignment = {
                        .y = CLAY_ALIGN_Y_CENTER
                    }
                })
            ) {
                CLAY_TEXT(CLAY_STRING(display), CLAY_TEXT_CONFIG({ 
                    .fontId = FONT_60_REGULAR,
                    .fontSize = 72,
                    .textColor = COLOURS[4],
                }));
            }
            CLAY(
                CLAY_ID("CalcButtonsContainer"),
                CLAY_LAYOUT((Clay_LayoutConfig) {
                    .sizing = layoutExpand,
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .childGap = 16
                })
            ) {
                for (int i = 0; i < BTN_COLS; i++) {
                    char rowId[10];
                    snprintf(rowId, sizeof rowId, "CalcRow%d", i);
                    CLAY(
                        CLAY_ID(rowId),
                        CLAY_LAYOUT((Clay_LayoutConfig) {
                            .sizing = layoutExpand,
                            .layoutDirection = CLAY_LEFT_TO_RIGHT,
                            .childGap = 16
                        })
                    ) {
                        for (int j = 0; j < BTN_ROWS; j++) {
                            char buttonId[16];
                            snprintf(buttonId, sizeof buttonId, "CalcButton%d_%d", i, j);

                            Button b = buttonGrid.rows[i].buttons[j];
                            Clay_Color c = COLOURS[b.colour];

                            CLAY(
                                CLAY_ID(buttonId),
                                CLAY_LAYOUT((Clay_LayoutConfig) {
                                    .sizing = layoutExpand,
                                    .padding = { 16, 16 },
                                    .childAlignment = {
                                        .x = CLAY_ALIGN_X_CENTER,
                                        .y = CLAY_ALIGN_Y_CENTER
                                    }
                                }),
                                CLAY_RECTANGLE((Clay_RectangleElementConfig) {
                                    .color = c,
                                    .cornerRadius = 16,
                                }),
                                Clay_OnHover(HandleButtonInteraction, b.id)
                            ) {
                                CLAY_TEXT(b.label, CLAY_TEXT_CONFIG((Clay_TextElementConfig) {
                                    .fontId = 2,
                                    .fontSize = 32,
                                    .textColor = COLOURS[4]
                                }));
                            }
                        }
                    }
                }
            }
        }

        Clay_RenderCommandArray renderCommands = Clay_EndLayout();

        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands);
        EndDrawing();
    }
}