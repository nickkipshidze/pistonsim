#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "world.h"
#include "block.h"
#include "draw.h"
#include "control.h"

int WRLDWidth = 128;
int WRLDHeight = 128;
const int WRLDTileSize = 64;

float WRLDLastTick = 0.0;
float WRLDTickRate = 0.05;
bool WRLDTickPaused = false;

struct Stack;

struct Stack WRLDEventStack = {0, NULL};
struct Stack WRLDEventStackNext = {0, NULL};

void stackPrint(struct Stack *stack) {
    printf("[");
    for (int i = 0; i < stack->top; i++) {
        printf("%d", stack->elements[i]);
        if (i < stack->top - 1) printf(", ");
    }
    printf("]\n");
}

void stackPush(struct Stack *stack, int element) {
    stack -> top++;
    if (stack -> elements == NULL) {
        stack -> elements = malloc(sizeof(int) * (stack -> top));
    } else {
        stack -> elements = realloc(stack -> elements, sizeof(int) * (stack -> top));
    }
    stack -> elements[stack -> top - 1] = element;
}

int stackPop(struct Stack *stack) {
    if (stack -> top == 0)
        return -1;
    stack -> top--;
    int popElement = stack -> elements[stack -> top];
    stack -> elements = realloc(stack -> elements, sizeof(int) * (stack -> top));
    return popElement;
}

void initWorld(struct Block **world) {
    *world = malloc(sizeof(struct Block) * WRLDWidth * WRLDHeight);

    for (int i = 0; i < WRLDWidth * WRLDHeight; i++) {
        (*world)[i] = (struct Block){
            BLOCK_ID_AIR, 0, 0, false
        };
    }
}

void printWorld(struct Block *world) {
    for (int y = 0; y < WRLDHeight; y++) {
        for (int x = 0; x < WRLDWidth; x++) {
            printf("%d ", world[y*WRLDWidth+x].blockId);
        }
        printf("\n");
    }
}

void pushSurrBlocks(struct Block *world, int x, int y) {
    int dirOfs[4][2] = {
        {0, -1}, {1, 0}, {0, 1}, {-1, 0}
    };
    int index;
    for (int i = 0; i < 4; i++) {
        index = (dirOfs[i][1] + y) * WRLDWidth + (dirOfs[i][0] + x);
        if (getBlockId(world, dirOfs[i][0] + x, dirOfs[i][1] + y) != BLOCK_ID_AIR)
            stackPush(&WRLDEventStack, index);
    }
}

void tickWorld(struct Block *world) {
    int dirOfs[4][2] = {
        {0, -1}, {1, 0}, {0, 1}, {-1, 0}
    };

    for (int i = 0; i < WRLDEventStackNext.top; i++) {
        stackPush(&WRLDEventStack, stackPop(&WRLDEventStackNext));
    }

    while (true) {
        stackPrint(&WRLDEventStack);

        int index = stackPop(&WRLDEventStack);
        if (index == -1)
            break;

        int x = index % WRLDWidth, y = index / WRLDHeight;
        struct Block *target = getBlock(world, x, y);

        if (target -> blockId == BLOCK_ID_WIRE || target -> blockId == BLOCK_ID_WALL || target -> blockId == BLOCK_ID_LAMP || target -> blockId == BLOCK_ID_SWITCH) {
            int maxSupply = -1;
            unsigned int connDirs = 0b0000;

            for (int i = 0; i < 4; i++) {
                if (
                    (
                        getBlockId(world, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_POWER ||
                        getBlockId(world, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_WIRE ||
                        (
                            getBlockId(world, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_WALL && target -> blockId != BLOCK_ID_WALL && target -> blockId != BLOCK_ID_LAMP
                        ) || (
                            getBlockId(world, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_LAMP && target -> blockId != BLOCK_ID_LAMP && target -> blockId != BLOCK_ID_WALL
                        ) || (
                            getBlockId(world, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_SWITCH
                        )
                    ) &&
                    getBlock(world, x+dirOfs[i][0], y+dirOfs[i][1]) -> active == true &&
                    getBlock(world, x+dirOfs[i][0], y+dirOfs[i][1]) -> data > 1
                ) {
                    int supply = getBlock(world, x+dirOfs[i][0], y+dirOfs[i][1]) -> data;
                    if (supply >= maxSupply) maxSupply = supply;
                }
                
                if (
                    getBlockId(world, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_POWER ||
                    getBlockId(world, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_WIRE ||
                    getBlockId(world, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_WALL ||
                    getBlockId(world, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_LAMP ||
                    getBlockId(world, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_SWITCH
                ) {
                    connDirs = setBit(connDirs, 3 - i, 1);
                }

                if (getBlockId(world, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_REPEATER) {
                    int rotation = getBitRange(getBlock(world, x+dirOfs[i][0], y+dirOfs[i][1]) -> state, 11, 8);
                    int supply = getBlock(world, x+dirOfs[i][0], y+dirOfs[i][1]) -> data;

                    if ((getBit(rotation, 3) || getBit(rotation, 1)) && (i == 0 || i == 2)) {
                        connDirs = setBit(connDirs, 3 - i, 1);
                    }
                    if ((getBit(rotation, 2) || getBit(rotation, 0)) && (i == 1 || i == 3)) {
                        connDirs = setBit(connDirs, 3 - i, 1);
                    }

                    if (getBlock(world, x+dirOfs[i][0], y+dirOfs[i][1]) -> active == true) {
                        if (getBit(rotation, 3) && i == 2 && supply >= maxSupply) maxSupply = supply;
                        if (getBit(rotation, 2) && i == 3 && supply >= maxSupply) maxSupply = supply;
                        if (getBit(rotation, 1) && i == 0 && supply >= maxSupply) maxSupply = supply;
                        if (getBit(rotation, 0) && i == 1 && supply >= maxSupply) maxSupply = supply;
                    }
                }
            }

            target -> state = connDirs;

            if (target -> active == false && maxSupply != -1) {
                if (target -> blockId != BLOCK_ID_SWITCH) target -> active = true;
                target -> data = maxSupply - 1;
                pushSurrBlocks(world, x, y);
            }

            if (target -> active == true) {
                if (maxSupply == -1) {
                    if (target -> blockId != BLOCK_ID_SWITCH) target -> active = false;
                    target -> data = 0;
                    pushSurrBlocks(world, x, y);
                } else if (target -> data != maxSupply - 1) {
                    target -> data = maxSupply - 1;
                    pushSurrBlocks(world, x, y);
                }
            }
        }
        else if (target -> blockId == BLOCK_ID_REPEATER) {
            int rotation = getBitRange(target -> state, 11, 8);
            int delay = getBitRange(target -> state, 7, 4);
            int tickCount = getBitRange(target -> state, 3, 0);

            int inputDir;
            if (getBit(rotation, 3)) inputDir = 2;
            if (getBit(rotation, 2)) inputDir = 3;
            if (getBit(rotation, 1)) inputDir = 0;
            if (getBit(rotation, 0)) inputDir = 1;

            struct Block *input = getBlock(world, x + dirOfs[inputDir][0], y + dirOfs[inputDir][1]);
            
            if ((input -> blockId == BLOCK_ID_POWER || input -> blockId == BLOCK_ID_WIRE || input -> blockId == BLOCK_ID_LAMP || input -> blockId == BLOCK_ID_WALL || input -> blockId == BLOCK_ID_SWITCH || input -> blockId == BLOCK_ID_REPEATER) && input -> active == true && input -> data != 0) {
                if (target -> active == false) {
                    if (tickCount < delay) {
                        target -> state = setBitRange(target -> state, 3, 0, tickCount + 1);
                        stackPush(&WRLDEventStackNext, index);
                    } else if (tickCount >= delay) {
                        target -> active = true;
                    }
                }
            } else {
                if (target -> active == true && tickCount > 0) {
                    target -> state = setBitRange(target -> state, 3, 0, tickCount - 1);
                    stackPush(&WRLDEventStackNext, index);
                } else if (target -> active == true && tickCount <= 0) {
                    target -> active = false;
                    target -> state = setBitRange(target -> state, 3, 0, 0);
                } else if (target -> active == true) {
                    target -> state = setBitRange(target -> state, 3, 0, delay);
                    stackPush(&WRLDEventStackNext, index);
                }
            }
            pushSurrBlocks(world, x, y);
        }
    }
}

void updateWorld(struct Block *world, Camera2D camera) {
    if (IsKeyPressed(KEY_F3)) DEBUG_MODE = !DEBUG_MODE;

    if (DEBUG_MODE) {
        if (IsKeyPressed(KEY_SPACE)) WRLDTickPaused = !WRLDTickPaused;
        if (WRLDTickPaused && IsKeyPressed(KEY_ENTER)) {
            tickWorld(world);
        }
    }

    if (!WRLDTickPaused) {
        WRLDLastTick += GetFrameTime();
        while (WRLDLastTick >= WRLDTickRate) {
            tickWorld(world);
            WRLDLastTick -= WRLDTickRate;
        }
    }

    drawWorld(world, camera);
}

