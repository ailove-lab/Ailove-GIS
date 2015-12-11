#ifndef MESH_INCLUDE
#define MESH_INCLUDE

#ifdef SINGLE
#define REAL float
#else /* not SINGLE */
#define REAL double
#endif /* not SINGLE */

#include "triangle.h"

double *mesh_X, *mesh_Y, *mesh_pr_X, *mesh_pr_Y;

int  mesh_points_count;
int  mesh_triangles_count;
int* mesh_triangles;

void mesh_shape(int shape_id);
void mesh_file(int shape_id, double zoom);
void mesh_load();
void mesh_project(double lng, double lat);
void mesh_free();
void mesh_save_ply(double lng, double lat, double zoom);
#endif