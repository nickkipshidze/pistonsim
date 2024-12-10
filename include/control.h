#ifndef CONTROL_H
#define CONTROL_H

#include <raylib.h>

extern int HOTBAR_SELECTED;
extern struct Block HOTBAR_BLOCKS[9];

extern float CAMERA_POS_X;
extern float CAMERA_POS_Y;

void updateCamera(Camera2D *camera);
void getHotbarInput();
void getInteractionInput(struct Block *world, Camera2D camera);
int getWorldHoverIdx(Camera2D camera);

#endif