#ifndef MESH_INCLUDE
#define MESH_INCLUDE

#ifdef SINGLE
#define REAL float
#else /* not SINGLE */
#define REAL double
#endif /* not SINGLE */

#include "triangle.h"

typedef struct triangulateio triangulateio;

triangulateio in, mid, mesh;
double *meshX, *meshY, *pr_meshX, *pr_meshY;
void mesh_shape(int shape_id);
void mesh_project(double lat, double lng);
void mesh_free();

#endif