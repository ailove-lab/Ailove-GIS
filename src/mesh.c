#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>

#include "mesh.h"
#include "shapes.h"
#include "proj_api.h"

void mesh_shape(int shape_id) {
    
    if(shape_id>=shapes_count) return;

    int l = shapes_length[shape_id];
    
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
        
        in.pointlist[i*2  ] = shapes_prX[shape_id][i]/5e6;
        in.pointlist[i*2+1] = shapes_prY[shape_id][i]/5e6;
        in.pointmarkerlist[i] = sp;
        in.pointattributelist[i] = 1.0;
        
        in.segmentlist[i*2] = i;             // first point
        in.segmentmarkerlist[i] = sp;
        // TODO: Check if segment has parts
        if(sp == l-1 || shapes_parts[shape_id][sp] == i+1) { // if next point is start one
            in.segmentlist[i*2+1] = close_id;// close path
            close_id = i+1;                  // new start point id
            sp++;
        } else {
          in.segmentlist[i*2+1] = i+1;       // second - next point
        }
    }


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

    // triangulate("pzq30a10", &in, &mesh, (struct triangulateio *) NULL);
    triangulate("pza.0001", &in, &mesh, (struct triangulateio *) NULL);

    meshX    = (double *) malloc(mesh.numberofpoints * sizeof(double));
    meshY    = (double *) malloc(mesh.numberofpoints * sizeof(double));
    mesh_prX = (double *) malloc(mesh.numberofpoints * sizeof(double));
    mesh_prY = (double *) malloc(mesh.numberofpoints * sizeof(double));

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

    memcpy(mesh_prX, meshX, mesh.numberofpoints * sizeof(double));
    memcpy(mesh_prY, meshY, mesh.numberofpoints * sizeof(double));
    
    for (int j=0; j<mesh.numberofpoints; j++) {
        mesh_prX[j] *= DEG_TO_RAD;
        mesh_prY[j] *= DEG_TO_RAD;
    }
    pj_transform(old_prj, new_prj, mesh.numberofpoints, 0, 
                 mesh_prX,
                 mesh_prY, NULL);

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
    free(mesh_prX);
    free(mesh_prY);
}
