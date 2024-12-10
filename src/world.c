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
                                getBlockId(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_WALL && target -> blockId != BLOCK_ID_WALL
                            ) || (
                                getBlockId(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_LAMP && target -> blockId != BLOCK_ID_LAMP
                            ) || (
                                getBlockId(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_SWITCH
                            )
                        ) &&
                        getBlock(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) -> active == true &&
                        getBlock(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) -> data > 1
                    ) {
                        int supply = getBlock(worldCpy, x+dirOfs[i][0], y+dirOfs[i][1]) -> data;
                        if (supply >= maxSupply) {
                            maxSupply = supply;
                        }
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

