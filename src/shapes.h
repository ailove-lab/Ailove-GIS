#ifndef SHAPES_H_INCLUDED
#define SHAPES_H_INCLUDED

#define DEG_TO_RAD

#define GRID_LNG 36
#define GRID_LAT 18

int shapesCount;

double ** shapesX;
double ** shapesY;
double ** shapesZ;

double ** shapes_prX;
double ** shapes_prY;
double ** shapes_prZ;

int * shapeLengths;
int ** shapeParts;

void shapes_load(char*);
void shapes_project(double lng, double lat);
void shapes_free();

int verticalCount, horizontalCount;
double ** grid_verticalX;
double ** grid_verticalY;
double ** grid_horizontalX;
double ** grid_horizontalY;

void grid_init(int lng_step, int lat_step);
void grid_project(double lng, double lat);
void grid_free();

#endif
