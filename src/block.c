#include <stdbool.h>
#include "block.h"
#include "world.h"

const int BLOCK_ID_AIR = 0x00;
// .data: NOT USED
// .state: NOT USED

const int BLOCK_ID_WALL = 0x01;
// .data: electric charge
// .state: NOT USED

const int BLOCK_ID_POWER = 0x02;
// .data: electric charge
// .state: NOT USED

const int BLOCK_ID_WIRE = 0x03;
// .data: electric charge
// .state: 0b0000 | connections top/right/bottom/left

const int BLOCK_ID_LAMP = 0x04;
// .data: electric charge
// .state: NOT USED

const int BLOCK_ID_SWITCH = 0x05;
// .data: electric charge
// .state: NOT USED

const int BLOCK_ID_REPEATER = 0x06;
// .data: electric charge
// .state: 0b1000_0100_0010 |
//     1000 | rotation top/right/bottom/left
//     0100 | delay tick amount number
//     0010 | ticks passed since powered

struct Block;

int getBit(int byte, int index) {
    return (byte >> index) & 1;
}

int setBit(int byte, int index, int value) {
    if (value) {
        return byte | (1 << index);
    } else {
        return byte & ~(1 << index);
    }
}

int getBitRange(int byte, int startIndex, int endIndex) {
    int result = 0;
    for (int i = startIndex; i >= endIndex; i--) {
        result = (result << 1) | getBit(byte, i);
    }
    return result;
}

int setBitRange(int byte, int startIndex, int endIndex, int value) {
    for (int i = endIndex; i <= startIndex; i++) {
        byte = setBit(byte, i, getBit(value, i - endIndex));
    }
    return byte;
}

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
