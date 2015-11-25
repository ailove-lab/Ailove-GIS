#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>

#include "mesh.h"
#include "shapes.h"
#include "proj_api.h"

void mesh_shape(int shape_id) {
    
    if(shape_id>=shapesCount) return;

    int l = shapeLengths[shape_id];
    
    // POINTS
    in.numberofpoints = l;
    in.numberofpointattributes = 1;
    
    in.pointlist = (REAL *) malloc(
        in.numberofpoints * 2 * 
        sizeof(REAL));
    in.pointmarkerlist = (int *) malloc(
        in.numberofpoints * 
        sizeof(int));
    in.pointattributelist = (REAL *) malloc(
        in.numberofpoints * 
        in.numberofpointattributes * 
        sizeof(REAL));

    // SEGMENTS
    in.numberofsegments = l;
    in.segmentlist = (int *) malloc(
        in.numberofsegments * 2 *
        sizeof(int));
    in.segmentmarkerlist = (int *) malloc(
        in.numberofsegments *
        sizeof(int));

    // Fill regions with data
    int close_id = 0;
    int sp = 1;
    for(int i=0; i<l; i++) {
        
        in.pointlist[i*2  ] = shapesX[shape_id][i];
        in.pointlist[i*2+1] = shapesY[shape_id][i];
        in.pointmarkerlist[i] = sp;
        in.pointattributelist[i] = 1.0;
        
        in.segmentlist[i*2] = i;             // first point
        in.segmentmarkerlist[i] = sp;
        // TODO: Check if segment has parts
        if(sp == l-1 || shapeParts[shape_id][sp] == i+1) { // if next point is start one
            in.segmentlist[i*2+1] = close_id;// close path
            close_id = i+1;                  // new start point id
            sp++;
        } else {
          in.segmentlist[i*2+1] = i+1;       // second - next point
        }
    }

    // REGIONS
    // in.numberofregions = sp;
    // in.regionlist = (double *) malloc(sp * 4 * sizeof(double));
    
    // mid.pointlist             = (REAL *) NULL; /* Not needed if -N switch used. */
    // mid.pointattributelist    = (REAL *) NULL;
    // mid.pointmarkerlist       = (int  *) NULL; /* Not needed if -N or -B switch used. */
    // mid.trianglelist          = (int  *) NULL; /* Not needed if -E switch used. */
    // mid.triangleattributelist = (REAL *) NULL;
    // mid.neighborlist          = (int  *) NULL; /* Needed only if -n switch used. */
    // mid.segmentlist           = (int  *) NULL;
    // mid.segmentmarkerlist     = (int  *) NULL;
    // mid.edgelist              = (int  *) NULL; /* Needed only if -e switch used. */
    // mid.edgemarkerlist        = (int  *) NULL; /* Needed if -e used and -B not used. */
    
    // triangulate("pzen", &in , &mid , (struct triangulateio *) NULL);

    // // OUT
    // mid.trianglearealist = (REAL *) malloc(mid.numberoftriangles * sizeof(REAL));
    // mid.trianglearealist[0] = 3.0;
    // mid.trianglearealist[1] = 1.0;

    mesh.pointlist             = (REAL *) NULL; /* Not needed if -N switch used. */
    mesh.pointattributelist    = (REAL *) NULL;
    mesh.trianglelist          = (int  *) NULL; /* Not needed if -E switch used. */
    mesh.triangleattributelist = (REAL *) NULL;
    
    /* Triangulate the points.  Switches are chosen to read and write a  */
    /*   PSLG (p), preserve the convex hull (c), number everything from  */
    /*   zero (z), assign a regional attribute to each element (A), and  */
    /*   produce an edge list (e), a Voronoi diagram (v), and a triangle */
    /*   neighbor list (n).                                              */

    // triangulate("pczAevn", &in, &mid, &vorout);

    triangulate("pzq30a2", &in, &mesh, (struct triangulateio *) NULL);

    meshX    = (double *) malloc(mesh.numberofpoints * sizeof(double));
    meshY    = (double *) malloc(mesh.numberofpoints * sizeof(double));
    pr_meshX = (double *) malloc(mesh.numberofpoints * sizeof(double));
    pr_meshY = (double *) malloc(mesh.numberofpoints * sizeof(double));

    for (int i = 0; i < mesh.numberofpoints; i++) {
        meshX[i] = mesh.pointlist[i * 2    ];
        meshY[i] = mesh.pointlist[i * 2 + 1];
    }

}

void mesh_project(double lng, double lat) {

    char* pj_old_str = "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs";
    char pj_new_str[256];
    sprintf(pj_new_str, "+proj=laea +lat_0=%f +lon_0=%f +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs",lat,lng);
    
    projPJ old_prj = pj_init_plus(pj_old_str);
    projPJ new_prj = pj_init_plus(pj_new_str);

    memcpy(pr_meshX, meshX, mesh.numberofpoints * sizeof(double));
    memcpy(pr_meshY, meshY, mesh.numberofpoints * sizeof(double));
    
    for (int j=0; j<mesh.numberofpoints; j++) {
        pr_meshX[j] *= DEG_TO_RAD;
        pr_meshY[j] *= DEG_TO_RAD;
    }
    pj_transform(old_prj, new_prj, mesh.numberofpoints, 0, 
                 pr_meshX,
                 pr_meshY, NULL);

    pj_free(new_prj);
    pj_free(old_prj);
}

void mesh_free() {
    
    // in
    free(in.pointlist);
    free(in.pointattributelist);
    free(in.pointmarkerlist);
    
    free(in.segmentlist);
    free(in.segmentmarkerlist);

    // free(in.regionlist);

    // free(mid.pointlist);
    // free(mid.pointattributelist);
    // free(mid.pointmarkerlist);
    
    // free(mid.trianglelist);
    // free(mid.triangleattributelist);
    // free(mid.trianglearealist);
    
    // free(mid.neighborlist);
    
    // free(mid.segmentlist);
    // free(mid.segmentmarkerlist);
    // free(mid.edgelist);
    // free(mid.edgemarkerlist);

    // out
    free(mesh.pointlist);
    free(mesh.pointattributelist);
    
    free(mesh.trianglelist);
    free(mesh.triangleattributelist);

    free(meshX);
    free(meshY);    
    free(pr_meshX);
    free(pr_meshY);
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
