#include <raylib.h>

#include "block.h"
#include "world.h"
#include "draw.h"

int HOTBAR_SELECTED = 4;
struct Block HOTBAR_BLOCKS[9] = {
    (struct Block){1, 0, 0, false},
    (struct Block){2, 9, 0, true},
    (struct Block){3, 0, 15, true},
    (struct Block){4, 0, 0, false},
    (struct Block){5, 0, 0, false},
    (struct Block){6, 9, 0b100000010000, false},
    (struct Block){0},
    (struct Block){0},
    (struct Block){0},
};

float CAMERA_POS_X = -1;
float CAMERA_POS_Y = -1;

Vector2 moveVecTo(Vector2 source, Vector2 target, float drift) {
    float distx = source.x - target.x;
    float disty = source.y - target.y;
    return (Vector2){source.x - distx/drift, source.y - disty/drift};
}

void updateCamera(Camera2D *camera) {
    if (CAMERA_POS_X == -1 && CAMERA_POS_Y == -1) {
        CAMERA_POS_X = WINDOW_WIDTH/2.0+((WRLDWidth/2.0-7)*WRLDTileSize);
        CAMERA_POS_Y = WINDOW_HEIGHT/2.0+((WRLDHeight/2.0-3)*WRLDTileSize);

        camera -> target = (Vector2){CAMERA_POS_X, CAMERA_POS_Y};
        camera -> rotation = 0.0;
        camera -> zoom = 1.0;
    }

    camera -> offset = (Vector2){WINDOW_WIDTH/2.0, WINDOW_HEIGHT/2.0};

    camera -> target = moveVecTo(
        camera -> target,
        (Vector2){CAMERA_POS_X, CAMERA_POS_Y},
        8
    );

    CAMERA_POS_X += -GetMouseWheelMoveV().x * (2500 + (1000 / camera -> zoom)) * GetFrameTime();
    CAMERA_POS_Y += -GetMouseWheelMoveV().y * (2500 + (1000 / camera -> zoom)) * GetFrameTime();

    if (IsKeyDown(KEY_LEFT_CONTROL) == true) {
        if (IsKeyDown(KEY_EQUAL))
            camera -> zoom += camera -> zoom * 0.9 * GetFrameTime();
        if (IsKeyDown(KEY_MINUS))
            camera -> zoom -= camera -> zoom * 0.9 * GetFrameTime();
        
        if (camera -> zoom > 4) camera -> zoom = 4;
    }
}

void getHotbarInput() {
    int numkeys[9] = {KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE};

    for (int i = 0; i < 9; i++) {
        if (IsKeyPressed(numkeys[i]))
            HOTBAR_SELECTED = i;
    }
}

void getInteractionInput(struct Block *world, Camera2D camera) {
    int hoveridx = getWorldHoverIdx(camera);

    if (hoveridx != -1) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
            if (world[hoveridx].blockId == BLOCK_ID_AIR)
                setBlock(world, hoveridx % WRLDWidth, hoveridx / WRLDWidth, HOTBAR_BLOCKS[HOTBAR_SELECTED]);
            else if (world[hoveridx].blockId == BLOCK_ID_REPEATER && IsKeyDown(KEY_LEFT_SHIFT)) {
                if (getBit(world[hoveridx].state, 11)) {
                    world[hoveridx].state = setBit(world[hoveridx].state, 11, 0); world[hoveridx].state = setBit(world[hoveridx].state, 10, 1);
                } else if (getBit(world[hoveridx].state, 10)) {
                    world[hoveridx].state = setBit(world[hoveridx].state, 10, 0); world[hoveridx].state = setBit(world[hoveridx].state, 9, 1);
                } else if (getBit(world[hoveridx].state, 9)) {
                    world[hoveridx].state = setBit(world[hoveridx].state, 9, 0); world[hoveridx].state = setBit(world[hoveridx].state, 8, 1);
                } else if (getBit(world[hoveridx].state, 8)) {
                    world[hoveridx].state = setBit(world[hoveridx].state, 8, 0); world[hoveridx].state = setBit(world[hoveridx].state, 11, 1);
                } 
            } else {
                if (world[hoveridx].blockId == BLOCK_ID_SWITCH) {
                    world[hoveridx].active = !world[hoveridx].active;
                    world[hoveridx].data = 0;
                }
            }
        else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            setBlock(world, hoveridx % WRLDWidth, hoveridx / WRLDWidth, (struct Block){BLOCK_ID_AIR});
    }
}

int getWorldHoverIdx(Camera2D camera) {
    float relMouseX = GetMousePosition().x;
    float relMouseY = GetMousePosition().y;
    
    Vector2 mouseWorldPos = GetScreenToWorld2D((Vector2){relMouseX, relMouseY}, camera);
    
    int mouseX = (int)(mouseWorldPos.x / WRLDTileSize);
    int mouseY = (int)(mouseWorldPos.y / WRLDTileSize);
    
    if (mouseX >= 0 && mouseX < WRLDWidth && mouseY >= 0 && mouseY < WRLDHeight)
        return (mouseY * WRLDWidth) + mouseX;
    else
        return -1;
}
