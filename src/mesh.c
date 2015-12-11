#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>

#include "mesh.h"
#include "shapes.h"
#include "proj_api.h"

int* mesh_triangles = (int*)NULL;
int  mesh_triangles_count = 0;
int  mesh_points_count = 0;

double *mesh_X    = (double *) NULL;
double *mesh_Y    = (double *) NULL;
double *mesh_pr_X = (double *) NULL;
double *mesh_pr_Y = (double *) NULL;

void mesh_file(int shape_id, double zoom) {
    
    FILE* f = fopen("mesh.poly", "w");
    
    if(f == NULL) { printf("Can't open file\n"); return; }
    
    int l = shapes_length[shape_id];
    // First line: <# of vertices> <dimension (must be 2)> <# of attributes> <# of boundary markers (0 or 1)>
    fprintf(f, "%d 2 0 0\n", l);
    for(int i=0; i<l; i++) {
        // Following lines: <vertex #> <x> <y> [attributes] [boundary marker]
        fprintf(f, "%d %f %f\n", i, shapes_prX[shape_id][i]/zoom, shapes_prY[shape_id][i]/zoom);
    }
    
    // One line: <# of segments> <# of boundary markers (0 or 1)>
    fprintf(f, "%d 0\n", shapes_length[shape_id]);
    int sp = 1;
    int close_id = 0;
    for(int i=0; i<shapes_length[shape_id]; i++) {
        // Following lines: <segment #> <endpoint> <endpoint> [boundary marker]
        if(i == l-1 || shapes_parts[shape_id][sp] == i+1) {
            fprintf(f, "%d %d %d\n", i, i, close_id); // close part
            close_id = i+1;                           // new start point id
            sp++;
        } else {
            fprintf(f, "%d %d %d\n", i, i, i+1);
        }
    }
    // One line: <# of holes>
    fprintf(f, "0\n");
    // Following lines: <hole #> <x> <y>
    
    // Optional line: <# of regional attributes and/or area constraints>
    // Optional following lines: <region #> <x> <y> <attribute> <maximum area>
    fclose(f);

    system("./triangle -pzq32.5a.001 mesh.poly");

    mesh_load();
}

void mesh_load() {

    printf("LOADING MESH:\n");
    FILE* f;

    // READ RESULTS BACK //

    char*   line = NULL;
    size_t  len  = 0;
    ssize_t read = 0;

    // POINTS //

    printf("\tLoad points: ");
    f = fopen("mesh.1.node","r");
    if(f == NULL) { printf("Can't open elements file\n"); return; }
    
    // get header    
    read = getline(&line, &len, f);
    mesh_points_count = atoi(line);
    printf("count: %d\n", mesh_points_count);
    mesh_X = malloc(mesh_points_count * sizeof(double));
    mesh_Y = malloc(mesh_points_count * sizeof(double));
    int index = -1;
    int i = 0;
    while(i++ < mesh_points_count && (read = getline(&line, &len, f)) != -1) {
        char* t;
        // get index
        t = strtok(line, " ");
        index = atoi(t);
        // get x
        t = strtok(NULL, " ");
        mesh_X[index] = atof(t);
        // get y
        t = strtok(NULL, " ");
        mesh_Y[index] = atof(t);
    }
    fclose(f);
    free(line);
    line = NULL;

    // TRIANGLES //
    
    printf("\tLoad triangles: ");
    f = fopen("mesh.1.ele","r");
    if(f == NULL) { printf("Can't open elements file\n"); return; }
    
    // get header    
    read = getline(&line, &len, f);
    mesh_triangles_count = atoi(line);
    printf("count: %d\n", mesh_triangles_count);
    mesh_triangles = malloc(mesh_triangles_count * 3 * sizeof(int));
    index = -1;
    i = 0;
    while(i++ < mesh_triangles_count && (read = getline(&line, &len, f)) != -1) {
        char* t;
        // get index
        t = strtok(line, " ");
        index = atoi(t);
        t = strtok(NULL, " "); mesh_triangles[index*3  ] = atoi(t);
        t = strtok(NULL, " "); mesh_triangles[index*3+1] = atoi(t);
        t = strtok(NULL, " "); mesh_triangles[index*3+2] = atoi(t);

        // printf(
        //     "%d %d %d %d\n", 
        //     index, 
        //     mesh_triangles[index*3  ],
        //     mesh_triangles[index*3+1],
        //     mesh_triangles[index*3+2]);
    }
    fclose(f);
    
    // cleanup    
    free(line);
}

void mesh_save_ply(double lng, double lat, double zoom) {
        
    if(mesh_triangles == NULL) return;

    FILE* f = fopen("mesh.ply", "w");
    
    if(f == NULL) { printf("Can't open file\n"); return; }
    
    fprintf(f, "ply\n");
    fprintf(f, "format ascii 1.0\n");
    fprintf(f, "comment PROJ: +proj=laea +lat_0=%f +lon_0=%f +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs\n",lat,lng);
    fprintf(f, "comment ZOOM: %f\n", zoom);
    fprintf(f, "element vertex %d\n", mesh_points_count);
    fprintf(f, "property float x\n");
    fprintf(f, "property float y\n");
    fprintf(f, "property float z\n");
    fprintf(f, "element face %d\n", mesh_triangles_count);
    fprintf(f, "property list uchar int vertex_indices\n");
    fprintf(f, "end_header\n");

    for(int i=0; i<mesh_points_count   ; i++) 
        fprintf(f, 
            "%f %f %f\n", 
            mesh_X[i], 
            mesh_Y[i], 
            0.0);

    for(int i=0; i<mesh_triangles_count; i++) 
        fprintf(f, "3 %d %d %d\n", 
            mesh_triangles[i*3  ], 
            mesh_triangles[i*3+1],
            mesh_triangles[i*3+2]);

    fclose(f);    
}

void mesh_shape(int shape_id) {
    
    struct triangulateio in, out;

    if(shape_id>=shapes_count) return;

    int l = shapes_length[shape_id];
    
    //////////////
    // IN MEHSH //
    //////////////

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

    // REGIONS

    in.numberofregions = shapes_parts_count[shape_id];
    in.regionlist = (REAL *) malloc((in.numberofregions+1) * 4 * sizeof(REAL));
    
    in.regionlist[(sp-1)*4  ] = 0.0;
    in.regionlist[(sp-1)*4+1] = 0.0;
    REAL avr_c = 0.0;

    for(int i=0; i<l; i++) {
        
        in.pointlist[i*2  ] = shapesX[shape_id][i];
        in.pointlist[i*2+1] = shapesY[shape_id][i];
        
        // integrate for average 
        in.regionlist[(sp-1)*4  ] += shapesX[shape_id][i];
        in.regionlist[(sp-1)*4+1] += shapesY[shape_id][i];
        avr_c += 1.0;

        in.pointmarkerlist[i] = sp;
        in.pointattributelist[i] = 1.0;
        
        in.segmentlist[i*2] = i; // first point
        in.segmentmarkerlist[i] = sp;
        // if last point of part
        if(i == l-1 || shapes_parts[shape_id][sp] == i+1) { // if next point is start one
            // average
            in.regionlist[(sp-1)*4  ] /= avr_c;
            in.regionlist[(sp-1)*4+1] /= avr_c;
            in.regionlist[(sp-1)*4+2]  = 1.0;    // regional attribute
            in.regionlist[(sp-1)*4+3]  = 1.0;    // area contraint
            
            in.segmentlist[i*2+1] = close_id;    // close path
            close_id = i+1;                      // new start point id
            sp++;
            in.regionlist[(sp-1)*4  ] = 0.0;
            in.regionlist[(sp-1)*4+1] = 0.0;
            avr_c = 0.0;
        } else {
          in.segmentlist[i*2+1] = i+1;                      // second - next point
        }
    }

    // HOLES

    //////////////
    // OUT MESH //
    //////////////

    out.pointlist             = (REAL *) NULL; /* Not needed if -N switch used. */
    out.pointattributelist    = (REAL *) NULL;
    out.trianglelist          = (int  *) NULL; /* Not needed if -E switch used. */
    out.triangleattributelist = (REAL *) NULL;
    
    out.segmentlist           = (int  *) NULL; 
    out.segmentmarkerlist     = (int  *) NULL;

    /* Triangulate the points.  Switches are chosen to read and write a  */
    /*   PSLG (p), preserve the convex hull (c), number everything from  */
    /*   zero (z), assign a regional attribute to each element (A), and  */
    /*   produce an edge list (e), a Voronoi diagram (v), and a triangle */
    /*   neighbor list (n).                                              */

    // triangulate("pczAevn", &in, &mid, &vorout);

    triangulate("pza1ABOV", &in, &out, (struct triangulateio *) NULL);

    mesh_triangles_count = out.numberoftriangles;
    mesh_triangles = malloc(mesh_triangles_count * 3 * sizeof(int));
    memcpy(mesh_triangles, out.trianglelist, mesh_triangles_count * 3 * sizeof(int));


    mesh_points_count = out.numberofpoints;
    mesh_X    = (double *)malloc(mesh_points_count * sizeof(double));
    mesh_Y    = (double *)malloc(mesh_points_count * sizeof(double));
    
    mesh_pr_X = (double *)malloc(mesh_points_count * sizeof(double));
    mesh_pr_Y = (double *)malloc(mesh_points_count * sizeof(double));

    for (int i = 0; i < mesh_points_count; i++) {
        mesh_X[i] = out.pointlist[i * 2    ];
        mesh_Y[i] = out.pointlist[i * 2 + 1];
    }

    /////////////
    // CLEANUP //
    /////////////
    
    // in
    free(in.pointlist);
    free(in.pointattributelist);
    free(in.pointmarkerlist);

    free(in.segmentlist);
    free(in.segmentmarkerlist);

    free(in.regionlist);

    //out
    free(out.pointlist);
    free(out.pointattributelist);
    // free(out.pointmarkerlist);
    
    free(out.trianglelist);
    
    free(out.segmentlist);
    free(out.segmentmarkerlist);
}

void mesh_project(double lng, double lat) {

    if(mesh_pr_X == NULL) return;

    char* pj_old_str = "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs";
    char pj_new_str[256];
    sprintf(pj_new_str, "+proj=laea +lat_0=%f +lon_0=%f +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs",lat,lng);
    
    projPJ old_prj = pj_init_plus(pj_old_str);
    projPJ new_prj = pj_init_plus(pj_new_str);

    memcpy(mesh_pr_X, mesh_X, mesh_points_count * sizeof(double));
    memcpy(mesh_pr_Y, mesh_Y, mesh_points_count * sizeof(double));
    
    for (int j=0; j<mesh_points_count; j++) {
        mesh_pr_X[j] *= DEG_TO_RAD;
        mesh_pr_Y[j] *= DEG_TO_RAD;
    }
    pj_transform(old_prj, new_prj, mesh_points_count, 0, 
                 mesh_pr_X,
                 mesh_pr_Y, NULL);

    pj_free(new_prj);
    pj_free(old_prj);
}

void mesh_free() {

    free(mesh_X); mesh_X = NULL;
    free(mesh_Y); mesh_Y = NULL;

    free(mesh_pr_X); mesh_pr_X = NULL; 
    free(mesh_pr_Y); mesh_pr_Y = NULL;

    free(mesh_triangles); mesh_triangles = NULL;
}
