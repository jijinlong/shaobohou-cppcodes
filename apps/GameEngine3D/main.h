#ifndef SDL_TEMPLATE_H
#define SDL_TEMPLATE_H

#include <vector>

#include <GL/glut.h>
#include <vector>
#include "QuatCamera.h"
#include "QuakeCamera.h"
#include "Object.h"


int frame = 0;

double lastTime = -1.0;

QuatCamera first;
QuakeCamera second;
BasicCamera *current;
int windowWidth = 640;
int windowHeight = 480;

bool followCam = false;
Object *testShip1;
Object *testShip2;
Object *currentShip;
Vector3D seed;

void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void motion(int x, int y);
void resize(int height, int width);
void draw_opengl();

void setCamera(double realTime);
void freeMemory();

#endif
