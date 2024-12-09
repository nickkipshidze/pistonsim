#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "block.h"
#include "world.h"
#include "draw.h"

Vector2 moveVecTo(Vector2 source, Vector2 target, float drift) {
    float distx = source.x - target.x;
    float disty = source.y - target.y;
    return (Vector2){source.x - distx/drift, source.y - disty/drift};
}

void updateCamera(Camera2D *camera, float *camPosX, float *camPosY) {
    camera -> offset = (Vector2){WINDOW_WIDTH/2.0, WINDOW_HEIGHT/2.0};

    camera -> target = moveVecTo(
        camera -> target,
        (Vector2){*camPosX, *camPosY},
        8
    );

    *camPosX += -GetMouseWheelMoveV().x * (2500 + (1000 / camera -> zoom)) * GetFrameTime();
    *camPosY += -GetMouseWheelMoveV().y * (2500 + (1000 / camera -> zoom)) * GetFrameTime();

    if (IsKeyDown(KEY_LEFT_CONTROL) == true) {
        if (IsKeyDown(KEY_EQUAL))
            camera -> zoom += camera -> zoom * 0.9 * GetFrameTime();
        if (IsKeyDown(KEY_MINUS))
            camera -> zoom -= camera -> zoom * 0.9 * GetFrameTime();
        
        if (camera -> zoom > 4) camera -> zoom = 4;
    }
}

int main(int argc, char *argv[]) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Pistonsim - By Nick");
    SetTargetFPS(120);

    float camPosX = WINDOW_WIDTH/2.0+((WRLDWidth/2.0-7)*WRLDTileSize);
    float camPosY = WINDOW_HEIGHT/2.0+((WRLDHeight/2.0-3)*WRLDTileSize);

    Camera2D camera = {0};
    camera.target = (Vector2){camPosX, camPosY};
    camera.rotation = 0.0;
    camera.zoom = 1.0;

    struct Block *world = NULL;
    initWorld(&world);

    for (int x = WRLDWidth/2-10; x < WRLDWidth/2+11; x++)
        setBlock(world, x, WRLDWidth/2, (struct Block){BLOCK_ID_WIRE, .state = 0b0101});
    for (int y = WRLDHeight/2-10; y < WRLDHeight/2+11; y++)
        setBlock(world, WRLDWidth/2, y, (struct Block){BLOCK_ID_WIRE, .state = 0b1010});
    setBlock(world, WRLDWidth/2, WRLDHeight/2, (struct Block){.blockId = BLOCK_ID_POWER, .active = true, .data = 9});

    while (!WindowShouldClose()) {
        updateCamera(&camera, &camPosX, &camPosY);

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

            drawHotbarUI();
            if (DEBUG_MODE) drawDebugUI(camera);
        EndDrawing();
    }

    CloseWindow();
    free(world);

    return 0;
}
