#include <stdio.h>
#include <stdlib.h>

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

void tickWorld(struct Block *world) {
    int dirOfs[4][2] = {
        {0, -1}, {1, 0}, {0, 1}, {-1, 0}
    };

    struct Block *worldCpy = malloc(sizeof(struct Block) * WRLDHeight * WRLDWidth);
    for (int i = 0; i < WRLDWidth * WRLDHeight; i++) {
        worldCpy[i] = (struct Block){world[i].blockId, world[i].data, world[i].state, world[i].active};
    }

    for (int y = 0; y < WRLDHeight; y++) {
        for (int x = 0; x < WRLDWidth; x++) {
            struct Block *target = getBlock(world, x, y);

            if (target -> blockId == BLOCK_ID_WIRE || target -> blockId == BLOCK_ID_WALL || target -> blockId == BLOCK_ID_LAMP || target -> blockId == BLOCK_ID_SWITCH) {
                int maxSupply = -1;
                unsigned int connDirs = 0b0000;

                for (int i = 0; i < 4; i++) {
                    if (
                        (
                            getBlockId(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_POWER ||
                            getBlockId(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_WIRE ||
                            (
                                getBlockId(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_WALL && target -> blockId != BLOCK_ID_WALL && target -> blockId != BLOCK_ID_LAMP
                            ) || (
                                getBlockId(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_LAMP && target -> blockId != BLOCK_ID_LAMP && target -> blockId != BLOCK_ID_WALL
                            ) || (
                                getBlockId(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_SWITCH
                            )
                        ) &&
                        getBlock(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) -> active == true &&
                        getBlock(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) -> data > 1
                    ) {
                        int supply = getBlock(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) -> data;
                        if (supply >= maxSupply) maxSupply = supply;
                    }
                    
                    if (
                        getBlockId(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_POWER ||
                        getBlockId(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_WIRE ||
                        getBlockId(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_WALL ||
                        getBlockId(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_LAMP ||
                        getBlockId(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_SWITCH
                    ) {
                        connDirs = setBit(connDirs, 3 - i, 1);
                    }

                    if (getBlockId(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_REPEATER) {
                        int rotation = getBitRange(getBlock(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) -> state, 11, 8);
                        int supply = getBlock(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) -> data;

                        if ((getBit(rotation, 3) || getBit(rotation, 1)) && (i == 0 || i == 2)) {
                            connDirs = setBit(connDirs, 3 - i, 1);
                        }
                        if ((getBit(rotation, 2) || getBit(rotation, 0)) && (i == 1 || i == 3)) {
                            connDirs = setBit(connDirs, 3 - i, 1);
                        }

                        if (getBlock(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) -> active == true) {
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
                }

                if (target -> active == true) {
                    if (maxSupply == -1) {
                        if (target -> blockId != BLOCK_ID_SWITCH) target -> active = false;
                        target -> data = 0;
                    } else if (target -> data != maxSupply - 1) {
                        target -> data = maxSupply - 1;
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

                struct Block *input = getBlock(worldCpy, x + dirOfs[inputDir][0], y + dirOfs[inputDir][1]);
                
                if ((input -> blockId == BLOCK_ID_POWER || input -> blockId == BLOCK_ID_WIRE || input -> blockId == BLOCK_ID_LAMP || input -> blockId == BLOCK_ID_WALL || input -> blockId == BLOCK_ID_SWITCH || input -> blockId == BLOCK_ID_REPEATER) && input -> active == true && input -> data != 0) {
                    if (target -> active == false) {
                        if (tickCount < delay)
                            target -> state = setBitRange(target -> state, 3, 0, tickCount + 1);
                        else if (tickCount >= delay) {
                            target -> active = true;
                        }
                    }
                } else {
                    if (target -> active == true && tickCount > 0) {
                        target -> state = setBitRange(target -> state, 3, 0, tickCount - 1);
                    } else if (target -> active == true && tickCount <= 0) {
                        target -> active = false;
                        target -> state = setBitRange(target -> state, 3, 0, 0);
                    } else if (target -> active == true) {
                        target -> state = setBitRange(target -> state, 3, 0, delay);
                    }
                }
            }
        }
    }

    free(worldCpy);
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

