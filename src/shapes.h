#ifndef SHAPES_H_INCLUDED
#define SHAPES_H_INCLUDED

#define DEG_TO_RAD

#define GRID_LNG 36
#define GRID_LAT 18

int shapesCount;

double ** shapesX;
double ** shapesY;
double ** shapesZ;

double ** pr_shapesX;
double ** pr_shapesY;
double ** pr_shapesZ;

int * shapeLengths;
int ** shapeParts;

void load_shapes(char*);
void project_shapes(double lng, double lat);
void free_shapes();


int verticalCount, horizontalCount;
double ** vertical_gridX;
double ** vertical_gridY;
double ** horizontal_gridX;
double ** horizontal_gridY;

void init_grid(int lng_step, int lat_step);
void project_grid(double lng, double lat);
void free_grid();

#endif
