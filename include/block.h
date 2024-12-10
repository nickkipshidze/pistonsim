#ifndef BLOCK_H
#define BLOCK_H

extern const int BLOCK_ID_AIR;
extern const int BLOCK_ID_WALL;
extern const int BLOCK_ID_POWER;
extern const int BLOCK_ID_WIRE;
extern const int BLOCK_ID_LAMP;
extern const int BLOCK_ID_SWITCH;
extern const int BLOCK_ID_REPEATER;

struct Block {
    int blockId;
    float data;
    float state;
    bool active;
};

int getBit(int byte, int index);
int setBit(int byte, int index, int value);
int getBitRange(int byte, int startIndex, int endIndex);
void setBlock(struct Block *world, int x, int y, struct Block block);
struct Block *getBlock(struct Block *world, int x, int y);
int getBlockId(struct Block *world, int x, int y);

#endif