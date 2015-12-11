#ifndef SHAPES_H_INCLUDED
#define SHAPES_H_INCLUDED

#define DEG_TO_RAD

#define GRID_LNG 36
#define GRID_LAT 18

int shapes_count;

double ** shapesX;
double ** shapesY;
double ** shapesZ;

double ** shapes_prX;
double ** shapes_prY;
double ** shapes_prZ;

double* shape_centers_X;
double* shape_centers_Y;

int *  shapes_length;
int *  shapes_parts_count;
int ** shapes_parts;

// char** shape_continent;
// char** shape_subregion;
char** shape_name_long;
// int*   shape_pop_est;

void shapes_load(char* filename);
void shapes_load_shp(char* filename);
void shapes_load_dbf(char* filename);
void shapes_project(double lng, double lat);
void shapes_project_shape(int shape_id, double lng, double lat);
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
