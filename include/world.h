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

void initWorld(struct Block **world);
void printWorld(struct Block *world);
void tickWorld(struct Block *world);
void updateWorld(struct Block *world, Camera2D camera);
int getWorldHoverIdx(Camera2D camera);

#endif