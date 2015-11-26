#include <stdlib.h>

#include "mesh.h"
#include "shapes.h"

void mesh_shape(int shape_id) {
    
    if(shape_id>=shapesCount) return;

    int l = shapesLength[shape_id];
    in.numberofpoints = l;
    in.numberofsegments = l;
    in.numberofpointattributes = 1;
    
    in.pointlist = (REAL *) malloc(
        in.numberofpoints * 2 * 
        sizeof(REAL));

    in.pointattributelist = (REAL *) malloc(
        in.numberpoints * 
        in.numberofpointattributes * 
        sizeof(REAL));

    in.segmentlist = (int *) malloc(
        in.numberofsegments * 2);

    int close_id = 0;
    for(int i=0, sp=1; i<l; i++) {
        in.pointlist[i*2  ] = shapesX[shape_id][i];
        in.pointlist[i*2+1] = shapesY[shape_id][i];
        in.pointattributelist[i] = 1.0;
        
        in.segmentlist[i*2  ] = i;

        // next point is start of new contour
        if(shapeParts[shape_id][sp] = i+1) {
            in.segmentlist[i*2+1] = i+1;
            
            sp++;
        }else {
            in.segmentlist[i*2+1] = i+1;
        }

    }

}

void mesh_free() {
    free(in.pointlist);
    free(in.pointattributelist);
    free(in.pointmarkerlist);
    free(in.regionlist);

}

static void report(io, markers, reporttriangles, reportneighbors, reportsegments,
            reportedges, reportnorms)
struct triangulateio *io;
int markers;
int reporttriangles;
int reportneighbors;
int reportsegments;
int reportedges;
int reportnorms;
{
  int i, j;

  for (i = 0; i < io->numberofpoints; i++) {
    printf("Point %4d:", i);
    for (j = 0; j < 2; j++) {
      printf("  %.6g", io->pointlist[i * 2 + j]);
    }
    if (io->numberofpointattributes > 0) {
      printf("   attributes");
    }
    for (j = 0; j < io->numberofpointattributes; j++) {
      printf("  %.6g",
             io->pointattributelist[i * io->numberofpointattributes + j]);
    }
    if (markers) {
      printf("   marker %d\n", io->pointmarkerlist[i]);
    } else {
      printf("\n");
    }
  }
  printf("\n");

  if (reporttriangles || reportneighbors) {
    for (i = 0; i < io->numberoftriangles; i++) {
      if (reporttriangles) {
        printf("Triangle %4d points:", i);
        for (j = 0; j < io->numberofcorners; j++) {
          printf("  %4d", io->trianglelist[i * io->numberofcorners + j]);
        }
        if (io->numberoftriangleattributes > 0) {
          printf("   attributes");
        }
        for (j = 0; j < io->numberoftriangleattributes; j++) {
          printf("  %.6g", io->triangleattributelist[i *
                                         io->numberoftriangleattributes + j]);
        }
        printf("\n");
      }
      if (reportneighbors) {
        printf("Triangle %4d neighbors:", i);
        for (j = 0; j < 3; j++) {
          printf("  %4d", io->neighborlist[i * 3 + j]);
        }
        printf("\n");
      }
    }
    printf("\n");
  }

  if (reportsegments) {
    for (i = 0; i < io->numberofsegments; i++) {
      printf("Segment %4d points:", i);
      for (j = 0; j < 2; j++) {
        printf("  %4d", io->segmentlist[i * 2 + j]);
      }
      if (markers) {
        printf("   marker %d\n", io->segmentmarkerlist[i]);
      } else {
        printf("\n");
      }
    }
    printf("\n");
  }

  if (reportedges) {
    for (i = 0; i < io->numberofedges; i++) {
      printf("Edge %4d points:", i);
      for (j = 0; j < 2; j++) {
        printf("  %4d", io->edgelist[i * 2 + j]);
      }
      if (reportnorms && (io->edgelist[i * 2 + 1] == -1)) {
        for (j = 0; j < 2; j++) {
          printf("  %.6g", io->normlist[i * 2 + j]);
        }
      }
      if (markers) {
        printf("   marker %d\n", io->edgemarkerlist[i]);
      } else {
        printf("\n");
      }
    }
    printf("\n");
  }
}
