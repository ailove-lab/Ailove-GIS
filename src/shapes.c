#include "shapes.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <GLFW/glfw3.h>

#include "shapefil.h"
#include "shpgeo.h"
#include "proj_api.h"

SHPHandle hSHP;
double  adfMinBound[4], adfMaxBound[4];

// Shape loader
static int lng_step, lat_step;

void grid_init(int lng_s, int lat_s) {
    
    lng_step = lng_s;
    lat_step = lat_s;
    horizontalCount = 360/lng_step;
    verticalCount   = 180/lat_step;

    grid_verticalX = (double **) calloc (horizontalCount, sizeof(double *));
    grid_verticalY = (double **) calloc (horizontalCount, sizeof(double *));
    assert(grid_verticalX!=NULL && grid_verticalY!=NULL);
    for (int lng=-180, i=0; lng<180; lng+=lng_step, i++) {
        grid_verticalX[i] = (double *) calloc(verticalCount, sizeof(double));
        grid_verticalY[i] = (double *) calloc(verticalCount, sizeof(double));
        for (int lat=-90, j=0;lat<90; lat+=lat_step, j++) {
            grid_verticalX[i][j] = lng;
            grid_verticalY[i][j] = lat;
        }
    }

    grid_horizontalX = (double **) calloc (verticalCount, sizeof(double *));
    grid_horizontalY = (double **) calloc (verticalCount, sizeof(double *));
    for (int lat=-90, i=0; lat<90; lat+=lat_step, i++) {
        grid_horizontalX[i] = (double *) calloc(horizontalCount, sizeof(double));
        grid_horizontalY[i] = (double *) calloc(horizontalCount, sizeof(double));
        for (int lng=-180, j=0; lng<180; lng+=lng_step, j++) {
            grid_horizontalX[i][j] = lng;
            grid_horizontalY[i][j] = lat;
        }
    }
}

void grid_project(double lng, double lat) {


    char* pj_old_str = "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs";
    char pj_new_str[256];
    sprintf(pj_new_str, "+proj=laea +lat_0=%f +lon_0=%f +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs",lat,lng);
    
    projPJ old_prj = pj_init_plus(pj_old_str);
    projPJ new_prj = pj_init_plus(pj_new_str);

    for (int lng=-180, i=0; lng<180; lng+=lng_step, i++) {
        for (int lat=-90, j=0;lat<90; lat+=lat_step, j++) {
            grid_verticalX[i][j] = lng*DEG_TO_RAD;
            grid_verticalY[i][j] = lat*DEG_TO_RAD;
        }
        pj_transform(old_prj, new_prj, verticalCount, 0, 
             grid_verticalX[i],
             grid_verticalY[i], NULL);
    }

    for (int lat=-90, i=0; lat<90; lat+=lat_step, i++) {
        for (int lng=-180, j=0; lng<180; lng+=lng_step, j++) {
            grid_horizontalX[i][j] = lng*DEG_TO_RAD;
            grid_horizontalY[i][j] = lat*DEG_TO_RAD;
        }
        pj_transform(old_prj, new_prj, horizontalCount, 0, 
             grid_horizontalX[i],
             grid_horizontalY[i], NULL);
    }
    pj_free(new_prj);
    pj_free(old_prj);
}

void grid_free() {
    for (int i=0; i<horizontalCount; i++) {
        free(grid_verticalX[i]);
        free(grid_verticalY[i]);
    }
    free(grid_verticalX);
    free(grid_verticalY);

    for (int i=0; i<verticalCount; i++) {
        free(grid_horizontalX[i]);
        free(grid_horizontalY[i]);
    }
    free(grid_horizontalX);
    free(grid_horizontalY);
}

void shapes_load(char* filename){
    
    int nShapeType;

    // Read file 
    hSHP = SHPOpen( filename, "rb" );

    if( hSHP == NULL ) {
        printf( "Unable to open:%s\n", filename );
        return;
    }
        
    // Print shape bounds
    SHPGetInfo( hSHP, &shapes_count, &nShapeType, adfMinBound, adfMaxBound );
    
    printf( "Shapefile Type: %s   # of Shapes: %d\n\n",
            SHPTypeName( nShapeType ), shapes_count );
    
    printf( "File Bounds: (%.15g,%.15g,%.15g,%.15g)\n"
            "         to  (%.15g,%.15g,%.15g,%.15g)\n",
            adfMinBound[0], 
            adfMinBound[1], 
            adfMinBound[2], 
            adfMinBound[3], 
            adfMaxBound[0], 
            adfMaxBound[1], 
            adfMaxBound[2], 
            adfMaxBound[3] );

    // Iterate through entries
    const char  *pszPlus;

    shapesX      = (double **) calloc(shapes_count, sizeof(double*));
    shapesY      = (double **) calloc(shapes_count, sizeof(double*));
    shapesZ      = (double **) calloc(shapes_count, sizeof(double*));
    shapes_prX   = (double **) calloc(shapes_count, sizeof(double*));
    shapes_prY   = (double **) calloc(shapes_count, sizeof(double*));
    shapes_prZ   = (double **) calloc(shapes_count, sizeof(double*));

    shapes_length = (int*)      calloc(shapes_count, sizeof(int));
    shapes_parts   = (int**)     calloc(shapes_count, sizeof(int*));
    
    for( int i = 0; i < shapes_count; i++ ) {

        int j, iPart;
        SHPObject   *psShape;

        psShape = SHPReadObject( hSHP, i );
        // SHPProject( psShape, old_prj, new_prj );

        if( psShape == NULL ) {
            fprintf( stderr,
                     "Unable to read shape %d, terminating object reading.\n",
                    i );
            break;
        }

        // psShape->nVertices, psShape->nParts,
        // psShape->dfXMin, psShape->dfYMin,
        // psShape->dfZMin,
        // psShape->dfXMax, psShape->dfYMax,
        // psShape->dfZMax );

        if( psShape->nParts > 0 && psShape->panPartStart[0] != 0 ) {
            fprintf( stderr, "panPartStart[0] = %d, not zero as expected.\n",
                     psShape->panPartStart[0] );
        }
        
        shapes_length[i] = psShape->nVertices;

        shapesX[i]    = (double *)calloc(psShape->nVertices, sizeof(double));
        shapesY[i]    = (double *)calloc(psShape->nVertices, sizeof(double));
        shapesZ[i]    = (double *)calloc(psShape->nVertices, sizeof(double));

        shapes_prX[i] = (double *)calloc(psShape->nVertices, sizeof(double));
        shapes_prY[i] = (double *)calloc(psShape->nVertices, sizeof(double));
        shapes_prZ[i] = (double *)calloc(psShape->nVertices, sizeof(double));

        shapes_parts[i] = (int *)calloc(psShape->nParts, sizeof(int));
        
        for (j =0; j< psShape->nParts; j++) shapes_parts[i][j] = psShape->panPartStart[j];
        for( j = 0, iPart = 1; j < psShape->nVertices; j++ ) {
            shapesX[i][j] = psShape->padfX[j];
            shapesY[i][j] = psShape->padfY[j];
            shapesZ[i][j] = psShape->padfZ[j];
        }
        memcpy(shapes_prX[i], shapesX[i], shapes_length[i]*sizeof(double));
        memcpy(shapes_prY[i], shapesY[i], shapes_length[i]*sizeof(double));
        memcpy(shapes_prZ[i], shapesZ[i], shapes_length[i]*sizeof(double));

        SHPDestroyObject( psShape );
    }
    SHPClose( hSHP );
}

void shapes_project(double lng, double lat) {


    char* pj_old_str = "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs";
    char pj_new_str[256];
    sprintf(pj_new_str, "+proj=laea +lat_0=%f +lon_0=%f +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs",lat,lng);
    
    projPJ old_prj = pj_init_plus(pj_old_str);
    projPJ new_prj = pj_init_plus(pj_new_str);

    for(int i=0; i<shapes_count; i++) {
        
        memcpy(shapes_prX[i], shapesX[i], shapes_length[i]*sizeof(double));
        memcpy(shapes_prY[i], shapesY[i], shapes_length[i]*sizeof(double));
        memcpy(shapes_prZ[i], shapesZ[i], shapes_length[i]*sizeof(double));
        
        for (int j=0; j<shapes_length[i]; j++) {
            shapes_prX[i][j] *= DEG_TO_RAD;
            shapes_prY[i][j] *= DEG_TO_RAD;
            shapes_prZ[i][j]  = 0; 
        }
        pj_transform(old_prj, new_prj, shapes_length[i], 0, 
                 shapes_prX[i],
                 shapes_prY[i], NULL);
    }
    // printf("%f, %f\n", shapes_prX[0][0]/4e6, shapes_prY[0][0]/4e6);
    pj_free(new_prj);
    pj_free(old_prj);
}

void shapes_free() {

    for(int i=0; i<shapes_count; i++) {

        free(shapesX[i]);
        free(shapesY[i]);
        free(shapesZ[i]);
        
        free(shapes_prX[i]);
        free(shapes_prY[i]);
        free(shapes_prZ[i]);

        free(shapes_parts[i]);
    }

    free(shapesX);
    free(shapesY);
    free(shapesZ);
    
    free(shapes_prX);
    free(shapes_prY);
    free(shapes_prZ);

    free(shapes_length);
    free(shapes_parts);

}
