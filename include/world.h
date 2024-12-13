#ifndef WORLD_H
#define WORLD_H

#include <raylib.h>
#include "block.h"

extern int WRLDWidth;
extern int WRLDHeight;
extern const int WRLDTileSize;

extern float WRLDLastTick;
extern float WRLDTickRate;
extern bool WRLDTickPaused;

struct Stack {
    int top;
    int *elements;
};

extern struct Stack WRLDEventStack;

void stackPrint(struct Stack *stack);
void stackPush(struct Stack *stack, int element);
int stackPop(struct Stack *stack);

void pushSurrBlocks(struct Block *world, int x, int y);

void initWorld(struct Block **world);
void printWorld(struct Block *world);
void tickWorld(struct Block *world);
void updateWorld(struct Block *world, Camera2D camera);
int getWorldHoverIdx(Camera2D camera);

#endif