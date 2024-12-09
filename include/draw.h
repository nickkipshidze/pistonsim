#ifndef DRAW_H
#define DRAW_H

#include <raylib.h>
#include "block.h"

extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

extern int HOTBAR_SELECTED;
extern struct Block HOTBAR_BLOCKS[9];

extern bool DEBUG_MODE;

void drawBlock(struct Block block, int x, int y);
void drawWorld(struct Block *world, Camera2D camera);
void drawBgGrid(int x1, int y1, int x2, int y2, Color color);
void drawHotbarUI();
void drawDebugUI(Camera2D camera);

#endif