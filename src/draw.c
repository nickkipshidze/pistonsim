#include <raylib.h>
#include <stdio.h>
#include <string.h>

#include "block.h"
#include "world.h"

int WINDOW_WIDTH = 1024;
int WINDOW_HEIGHT = 512;

int HOTBAR_SELECTED = 4;
struct Block HOTBAR_BLOCKS[9] = {
    (struct Block){1, 9, 0, true},
    (struct Block){2, 0, 15, true},
    (struct Block){0},
    (struct Block){0},
    (struct Block){0},
    (struct Block){0},
    (struct Block){0},
    (struct Block){0},
    (struct Block){0},
};

bool DEBUG_MODE = false;

void drawBlock(struct Block block, int x, int y) {
    if (block.blockId == BLOCK_ID_POWER) {
        DrawRectangleRounded(
            (Rectangle){x, y, WRLDTileSize, WRLDTileSize},
            0.2, 9, (Color){245, 28, 28, 255}
        );
    } else if (block.blockId == BLOCK_ID_WIRE) {
        Color color = (Color){52, 19, 19, 255};
        if (block.active == true)
            color = (Color){52 + (12 * block.data), 26, 26, 255};
        
        unsigned int state = block.state;
        if (((state >> 3) & 1) == 1)
            DrawRectangle(x+22, y, 19, 41, color);
        if (((state >> 2) & 1) == 1)
            DrawRectangle(x+22, y+22, 42, 19, color);
        if (((state >> 1) & 1) == 1)
            DrawRectangle(x+22, y+22, 19, 42, color);
        if (((state >> 0) & 1) == 1)
            DrawRectangle(x, y+22, 41, 19, color);
        if (state == 0)
            DrawRectangle(x+22, y+22, 19, 19, color);
    }

    if (DEBUG_MODE) {
        char debugText[32];
        sprintf(debugText, "0x%04X\n%.4f\n%.4f\n", block.blockId, block.data, block.state);
        if (block.active) strcat(debugText, "true");
        else strcat(debugText, "false");
        DrawText(debugText, x+5, y+5, 7, (Color){255, 255, 0, 32});
    }
}

void drawWorld(struct Block *world, Camera2D camera) {
    for (int y = 0; y < WRLDHeight; y++) {
        for (int x = 0; x < WRLDWidth; x++) {
            Vector2 screenPos = GetWorldToScreen2D((Vector2){x*WRLDTileSize, y*WRLDTileSize}, camera);
            if (screenPos.x >= -WRLDTileSize*4 && screenPos.x <= WINDOW_WIDTH && screenPos.y >= -WRLDTileSize*4 && screenPos.y <= WINDOW_HEIGHT)
                drawBlock(world[y*WRLDWidth+x], x*WRLDTileSize, y*WRLDTileSize);
        }
    }
}

void drawBgGrid(int x1, int y1, int x2, int y2, Color color) {
    for (int y = y1; y < y2; y += WRLDTileSize)
        for (int x = x1; x < x2; x += WRLDTileSize)
            if ((x / WRLDTileSize + y / WRLDTileSize) % 2 == 0)
                DrawRectangle(x, y, WRLDTileSize, WRLDTileSize, color);
}

void drawHotbarUI() {
    int cellCount = 9;
    int cellMargin = 10;
    int cellPadding = 5;
    int cellSize = WRLDTileSize + (cellPadding * 2);

    int bgWidth = (cellSize * cellCount) + (cellMargin * cellCount) + cellMargin;
    int bgHeight = (cellMargin * 2) + cellSize;

    int bgX = WINDOW_WIDTH/2 - bgWidth/2;
    int bgY = WINDOW_HEIGHT - bgHeight - 20;

    DrawRectangleRounded(
        (Rectangle){bgX, bgY, bgWidth, bgHeight},
        0.2, 9, (Color){64, 64, 64, 128}
    );

    int numkeys[9] = {KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE};
    for (int i = 0; i < 9; i++) {
        if (IsKeyPressed(numkeys[i]))
            HOTBAR_SELECTED = i;
    }

    for (int i = 0; i < cellCount; i++) {
        if (HOTBAR_SELECTED == i) {
            DrawRectangleRounded(
                (Rectangle){
                    bgX + (i * cellSize) + (i * cellMargin) + cellMargin - 3,
                    bgY + cellMargin - 3,
                    cellSize + 6, cellSize + 6
                },
                0.2, 9, (Color){128, 128, 128, 128}
            );
        } else {
            DrawRectangleRounded(
                (Rectangle){
                    bgX + (i * cellSize) + (i * cellMargin) + cellMargin,
                    bgY + cellMargin,
                    cellSize, cellSize
                },
                0.2, 9, (Color){64, 64, 64, 128}
            );
        }
        if (HOTBAR_BLOCKS[i].blockId != 0) {
            drawBlock(
                HOTBAR_BLOCKS[i],
                bgX + (i * cellSize) + (i * cellMargin) + cellMargin + cellPadding,
                bgY + cellMargin + cellPadding
            );
        }
    }
}

void drawDebugUI(Camera2D camera) {
    char debugText[32];

    DrawRectangle(0, 0, MeasureText("Pistonsim - Debug mode enabled", 20) + 20, 20 + 10, (Color){128, 128, 128, 128});
    DrawText("Pistonsim - Debug mode enabled", 10, 5, 20, WHITE);

    sprintf(debugText, "FPS: %.4f", 1 / GetFrameTime());
    DrawRectangle(0, 30, MeasureText(debugText, 20) + 20, 20 + 10, (Color){128, 128, 128, 128});
    DrawText(debugText, 10, 5+30, 20, WHITE);

    sprintf(debugText, "DT: %.4f sec", GetFrameTime());
    DrawRectangle(0, 60, MeasureText(debugText, 20) + 20, 20 + 10, (Color){128, 128, 128, 128});
    DrawText(debugText, 10, 5+60, 20, WHITE);

    sprintf(debugText, "Selection XY: %d / %d", getWorldHoverIdx(camera) % WRLDWidth, getWorldHoverIdx(camera) / WRLDWidth);
    DrawRectangle(0, 90, MeasureText(debugText, 20) + 20, 20 + 10, (Color){128, 128, 128, 128});
    DrawText(debugText, 10, 5+90, 20, WHITE);

    sprintf(debugText, "Cam target XY: %.2f / %.2f", camera.target.x, camera.target.y);
    DrawRectangle(0, 120, MeasureText(debugText, 20) + 20, 20 + 10, (Color){128, 128, 128, 128});
    DrawText(debugText, 10, 5+120, 20, WHITE);

    sprintf(debugText, "Cam offset XY: %.2f / %.2f", camera.offset.x, camera.offset.y);
    DrawRectangle(0, 150, MeasureText(debugText, 20) + 20, 20 + 10, (Color){128, 128, 128, 128});
    DrawText(debugText, 10, 5+150, 20, WHITE);

    sprintf(debugText, "Last tick: %.4f sec", WRLDLastTick);
    DrawRectangle(0, 180, MeasureText(debugText, 20) + 20, 20 + 10, (Color){128, 128, 128, 128});
    DrawText(debugText, 10, 5+180, 20, WHITE);

    if (WRLDTickPaused) strcpy(debugText, "Tick paused: True");
    else strcpy(debugText, "Tick paused: False");
    DrawRectangle(0, 210, MeasureText(debugText, 20) + 20, 20 + 10, (Color){128, 128, 128, 128});
    DrawText(debugText, 10, 5+210, 20, WHITE);
}
