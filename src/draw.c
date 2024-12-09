#include <raylib.h>
#include <stdio.h>
#include <string.h>

#include "block.h"
#include "world.h"
#include "control.h"

int WINDOW_WIDTH = 1024;
int WINDOW_HEIGHT = 512;

bool DEBUG_MODE = false;

void drawBlock(struct Block block, int x, int y) {
    if (block.blockId == BLOCK_ID_POWER) {
        DrawRectangleRounded(
            (Rectangle){x, y, WRLDTileSize, WRLDTileSize},
            0.2, 9, (Color){245, 28, 28, 255}
        );
    } else if (block.blockId == BLOCK_ID_WALL) {
        DrawRectangleRounded(
            (Rectangle){x, y, WRLDTileSize, WRLDTileSize},
            0.2, 9, (Color){193, 193, 193, 255}
        );
    } else if (block.blockId == BLOCK_ID_WIRE) {
        Color color = (Color){52, 19, 19, 255};
        if (block.active == true)
            color = (Color){52 + (12 * block.data), 26, 26, 255};
        
        unsigned int state = block.state;
        if (getBit(state, 3))
            DrawRectangle(x+22, y, 19, 41, color);
        if (getBit(state, 2))
            DrawRectangle(x+22, y+22, 42, 19, color);
        if (getBit(state, 1))
            DrawRectangle(x+22, y+22, 19, 42, color);
        if (getBit(state, 0))
            DrawRectangle(x, y+22, 41, 19, color);
        if (state == 0)
            DrawRectangle(x+22, y+22, 19, 19, color);
    } else if (block.blockId == BLOCK_ID_LAMP) {
        Color primary = (Color){104, 80, 48, 255};
        Color secondary = (Color){104, 80, 48, 255};;
        if (block.active == true) {
            primary = (Color){227, 177, 109, 255};
            secondary = (Color){231, 204, 169, 255};
        }

        DrawRectangleRounded(
            (Rectangle){x, y, WRLDTileSize, WRLDTileSize},
            0.4, 9, (Color){58, 32, 21, 255}
        );
        DrawRectangleRounded(
            (Rectangle){x+6, y+6, WRLDTileSize-12, WRLDTileSize-12},
            0.4, 9, primary
        );
        DrawRectangleRounded(
            (Rectangle){x+17, y+17, WRLDTileSize-34, WRLDTileSize-34},
            0.9, 9, (Color){139, 98, 46, 255}
        );
        DrawRectangleRounded(
            (Rectangle){x+21, y+21, WRLDTileSize-42, WRLDTileSize-42},
            0.9, 9, secondary
        );
    } else if (block.blockId == BLOCK_ID_SWITCH) {
        DrawRectangleRounded(
            (Rectangle){x, y, WRLDTileSize, WRLDTileSize},
            0.4, 9, (Color){56, 56, 56, 255}
        );
        DrawRectangleRounded(
            (Rectangle){x+6, y+6, WRLDTileSize-12, WRLDTileSize-12},
            0.4, 9, (Color){140, 140, 140, 255}
        );
        DrawRectangleRounded(
            (Rectangle){x+21, y+9, WRLDTileSize-42, WRLDTileSize-18},
            0.4, 9, (Color){53, 26, 26, 255}
        );

        if (block.active == true)
            DrawRectangleRounded(
                (Rectangle){x+21, y+9, WRLDTileSize-42, WRLDTileSize-42},
                0.4, 9, (Color){255, 0, 0, 255}
            );
        else
            DrawRectangleRounded(
                (Rectangle){x+21, y+34, WRLDTileSize-42, WRLDTileSize-42},
                0.4, 9, (Color){110, 0, 0, 255}
            );
    } else if (block.blockId == BLOCK_ID_REPEATER) {
        Color color = (Color){110, 0, 0, 255};
        if (block.active == true) color = (Color){255, 0, 0, 255};

        int rotation = getBitRange(block.state, 11, 8);
        int delay = getBitRange(block.state, 7, 4);

        Rectangle slitRect = (Rectangle){x+23, y+25, WRLDTileSize-47, WRLDTileSize-31};
        Rectangle ind1Rect = (Rectangle){x+23, y+5, WRLDTileSize-47, WRLDTileSize-47};
        Rectangle ind2Rect = (Rectangle){x+23, y+25, WRLDTileSize-47, WRLDTileSize-47};

        if (getBit(rotation, 2)) {
            slitRect = (Rectangle){x+7, y+25, WRLDTileSize-31, WRLDTileSize-47};
            ind1Rect.x = x + 42; ind1Rect.y = y + 25;
            ind2Rect.x = x + 23; ind2Rect.y = y + 25;
        } else if (getBit(rotation, 1)) {
            slitRect = (Rectangle){x+23, y+6, WRLDTileSize-47, WRLDTileSize-31};
            ind1Rect.x = x + 23; ind1Rect.y = y + 42;
            ind2Rect.x = x + 23; ind2Rect.y = y + 22;
        } else if (getBit(rotation, 0)) {
            slitRect = (Rectangle){x+25, y+25, WRLDTileSize-31, WRLDTileSize-47};
            ind1Rect.x = x + 5; ind1Rect.y = y + 25;
            ind2Rect.x = x + 25; ind2Rect.y = y + 25;
        }

        DrawRectangleRounded(
            (Rectangle){x, y, WRLDTileSize, WRLDTileSize},
            0.4, 9, (Color){56, 56, 56, 255}
        );
        DrawRectangleRounded(
            (Rectangle){x+3, y+3, WRLDTileSize-6, WRLDTileSize-6},
            0.4, 9, (Color){140, 140, 140, 255}
        );
        DrawRectangleRounded(slitRect, 0.4, 9, (Color){53, 26, 26, 255});
        DrawRectangleRounded(ind1Rect, 0.4, 9, color);
        DrawRectangleRounded(ind2Rect, 0.4, 9, color);
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

    int hoveridx = getWorldHoverIdx(camera);
    if (hoveridx != -1)
        DrawRectangle(
            (hoveridx % WRLDWidth) * WRLDTileSize,
            (hoveridx / WRLDWidth) * WRLDTileSize,
            WRLDTileSize, WRLDTileSize, (Color){255, 255, 255, 32}
        );
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

void drawVHotbarUI() {
    int cellCount = 9;
    int cellMargin = 10;
    int cellPadding = 5;
    int cellSize = WRLDTileSize + (cellPadding * 2);

    int bgWidth = (cellMargin * 2) + cellSize;
    int bgHeight = (cellSize * cellCount) + (cellMargin * cellCount) + cellMargin;

    int bgX = WINDOW_WIDTH - bgWidth - 20;
    int bgY = WINDOW_HEIGHT/2 - bgHeight/2;

    for (int i = 0; i < cellCount; i++) {
        if (HOTBAR_SELECTED == i) {
            DrawRectangleRounded(
                (Rectangle){
                    bgX + cellMargin - 3,
                    bgY + (i * cellSize) + (i * cellMargin) + cellMargin - 3,
                    cellSize + 6, cellSize + 6
                },
                0.2, 9, (Color){128, 128, 128, 128}
            );
        } else {
            DrawRectangleRounded(
                (Rectangle){
                    bgX + cellMargin,
                    bgY + (i * cellSize) + (i * cellMargin) + cellMargin,
                    cellSize, cellSize
                },
                0.2, 9, (Color){64, 64, 64, 128}
            );
        }
        if (HOTBAR_BLOCKS[i].blockId != 0) {
            drawBlock(
                HOTBAR_BLOCKS[i],
                bgX + cellMargin + cellPadding,
                bgY + (i * cellSize) + (i * cellMargin) + cellMargin + cellPadding
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
