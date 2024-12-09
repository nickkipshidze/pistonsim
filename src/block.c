#include <stdbool.h>
#include "block.h"
#include "world.h"

const int BLOCK_ID_AIR = 0x00;
const int BLOCK_ID_POWER = 0x01;
const int BLOCK_ID_WIRE = 0x02;

struct Block;

void setBlock(struct Block *world, int x, int y, struct Block block) {
    world[y*WRLDWidth+x] = block;
}

struct Block *getBlock(struct Block *world, int x, int y) {
    if (x >= 0 && x < WRLDWidth && y >= 0 && y < WRLDHeight)
        return &world[y*WRLDWidth+x];
    else {
        static struct Block air = {.blockId = BLOCK_ID_AIR};
        return &air;
    }
}

int getBlockId(struct Block *world, int x, int y) {
    return getBlock(world, x, y) -> blockId;
}
