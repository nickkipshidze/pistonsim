#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "block.h"
#include "world.h"
#include "draw.h"
#include "control.h"

void makeStarterWorld(struct Block *world) {
    for (int x = WRLDWidth/2-10; x < WRLDWidth/2+11; x++)
        setBlock(world, x, WRLDWidth/2, (struct Block){BLOCK_ID_WIRE, .state = 0b0101});
    for (int y = WRLDHeight/2-10; y < WRLDHeight/2+11; y++)
        setBlock(world, WRLDWidth/2, y, (struct Block){BLOCK_ID_WIRE, .state = 0b1010});
    setBlock(world, WRLDWidth/2, WRLDHeight/2, (struct Block){.blockId = BLOCK_ID_POWER, .active = true, .data = 9});
}

int main(int argc, char *argv[]) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Pistonsim - By Nick");
    SetTargetFPS(120);

    Camera2D camera = {0};

    struct Block *world = NULL;
    initWorld(&world);
    makeStarterWorld(world);

    while (!WindowShouldClose()) {
        updateCamera(&camera);
        getHotbarInput();
        getInteractionInput(world, camera);
        
        WINDOW_WIDTH = GetScreenWidth();
        WINDOW_HEIGHT = GetScreenHeight();

        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode2D(camera);
                drawBgGrid(
                    0, 0,
                    WRLDWidth*WRLDTileSize, WRLDHeight*WRLDTileSize,
                    (Color){11, 11, 11, 255}
                );
                updateWorld(world, camera);
            EndMode2D();

            if (WINDOW_HEIGHT >= 750) drawVHotbarUI();
            else drawHotbarUI();

            if (DEBUG_MODE) drawDebugUI(camera);
        EndDrawing();
    }

    CloseWindow();
    free(world);

    return 0;
}
