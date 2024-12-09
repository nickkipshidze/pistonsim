#ifndef BLOCK_H
#define BLOCK_H

extern const int BLOCK_ID_AIR;
extern const int BLOCK_ID_POWER;
extern const int BLOCK_ID_WIRE;

struct Block {
    int blockId;
    float data;
    float state;
    bool active;
};

void setBlock(struct Block *world, int x, int y, struct Block block);
struct Block *getBlock(struct Block *world, int x, int y);
int getBlockId(struct Block *world, int x, int y);

#endif