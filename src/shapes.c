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

void init_grid(int lng_s, int lat_s) {
    
    lng_step = lng_s;
    lat_step = lat_s;
    horizontalCount = 360/lng_step;
    verticalCount   = 180/lat_step;

    vertical_gridX = (double **) calloc (horizontalCount, sizeof(double *));
    vertical_gridY = (double **) calloc (horizontalCount, sizeof(double *));
    assert(vertical_gridX!=NULL && vertical_gridY!=NULL);
    for (int lng=-180, i=0; lng<180; lng+=lng_step, i++) {
        vertical_gridX[i] = (double *) calloc(verticalCount, sizeof(double));
        vertical_gridY[i] = (double *) calloc(verticalCount, sizeof(double));
        for (int lat=-90, j=0;lat<90; lat+=lat_step, j++) {
            vertical_gridX[i][j] = lng;
            vertical_gridY[i][j] = lat;
        }
    }

    horizontal_gridX = (double **) calloc (verticalCount, sizeof(double *));
    horizontal_gridY = (double **) calloc (verticalCount, sizeof(double *));
    for (int lat=-90, i=0; lat<90; lat+=lat_step, i++) {
        horizontal_gridX[i] = (double *) calloc(horizontalCount, sizeof(double));
        horizontal_gridY[i] = (double *) calloc(horizontalCount, sizeof(double));
        for (int lng=-180, j=0; lng<180; lng+=lng_step, j++) {
            horizontal_gridX[i][j] = lng;
            horizontal_gridY[i][j] = lat;
        }
    }
}

void project_grid(double lng, double lat) {


    char* pj_old_str = "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs";
    char pj_new_str[256];
    sprintf(pj_new_str, "+proj=laea +lat_0=%f +lon_0=%f +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs",lat,lng);
    
    projPJ old_prj = pj_init_plus(pj_old_str);
    projPJ new_prj = pj_init_plus(pj_new_str);

    for (int lng=-180, i=0; lng<180; lng+=lng_step, i++) {
        for (int lat=-90, j=0;lat<90; lat+=lat_step, j++) {
            vertical_gridX[i][j] = lng*DEG_TO_RAD;
            vertical_gridY[i][j] = lat*DEG_TO_RAD;
        }
        pj_transform(old_prj, new_prj, verticalCount, 0, 
             vertical_gridX[i],
             vertical_gridY[i], NULL);
    }

    for (int lat=-90, i=0; lat<90; lat+=lat_step, i++) {
        for (int lng=-180, j=0; lng<180; lng+=lng_step, j++) {
            horizontal_gridX[i][j] = lng*DEG_TO_RAD;
            horizontal_gridY[i][j] = lat*DEG_TO_RAD;
        }
        pj_transform(old_prj, new_prj, horizontalCount, 0, 
             horizontal_gridX[i],
             horizontal_gridY[i], NULL);
    }
    pj_free(new_prj);
    pj_free(old_prj);
}

void free_grid() {
    for (int i=0; i<horizontalCount; i++) {
        free(vertical_gridX[i]);
        free(vertical_gridY[i]);
    }
    free(vertical_gridX);
    free(vertical_gridY);

    for (int i=0; i<verticalCount; i++) {
        free(horizontal_gridX[i]);
        free(horizontal_gridY[i]);
    }
    free(horizontal_gridX);
    free(horizontal_gridY);
}

void load_shapes(char* filename){
    
    int nShapeType;

    // Read file 
    hSHP = SHPOpen( filename, "rb" );

    if( hSHP == NULL ) {
        printf( "Unable to open:%s\n", filename );
        return;
    }
        
    // Print shape bounds
    SHPGetInfo( hSHP, &shapesCount, &nShapeType, adfMinBound, adfMaxBound );
    
    printf( "Shapefile Type: %s   # of Shapes: %d\n\n",
            SHPTypeName( nShapeType ), shapesCount );
    
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

    shapesX      = (double **) calloc(shapesCount, sizeof(double*));
    shapesY      = (double **) calloc(shapesCount, sizeof(double*));
    shapesZ      = (double **) calloc(shapesCount, sizeof(double*));
    pr_shapesX   = (double **) calloc(shapesCount, sizeof(double*));
    pr_shapesY   = (double **) calloc(shapesCount, sizeof(double*));
    pr_shapesZ   = (double **) calloc(shapesCount, sizeof(double*));

    shapeLengths = (int*)      calloc(shapesCount, sizeof(int));
    shapeParts   = (int**)     calloc(shapesCount, sizeof(int*));
    
    for( int i = 0; i < shapesCount; i++ ) {

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
        
        shapeLengths[i] = psShape->nVertices;

        shapesX[i]    = (double *)calloc(psShape->nVertices, sizeof(double));
        shapesY[i]    = (double *)calloc(psShape->nVertices, sizeof(double));
        shapesZ[i]    = (double *)calloc(psShape->nVertices, sizeof(double));

        pr_shapesX[i] = (double *)calloc(psShape->nVertices, sizeof(double));
        pr_shapesY[i] = (double *)calloc(psShape->nVertices, sizeof(double));
        pr_shapesZ[i] = (double *)calloc(psShape->nVertices, sizeof(double));

        shapeParts[i] = (int *)calloc(psShape->nParts, sizeof(int));
        
        for (j =0; j< psShape->nParts; j++) shapeParts[i][j] = psShape->panPartStart[j];
        for( j = 0, iPart = 1; j < psShape->nVertices; j++ ) {
            shapesX[i][j] = psShape->padfX[j];
            shapesY[i][j] = psShape->padfY[j];
            shapesZ[i][j] = psShape->padfZ[j];
        }
        memcpy(pr_shapesX[i], shapesX[i], shapeLengths[i]*sizeof(double));
        memcpy(pr_shapesY[i], shapesY[i], shapeLengths[i]*sizeof(double));
        memcpy(pr_shapesZ[i], shapesZ[i], shapeLengths[i]*sizeof(double));

        SHPDestroyObject( psShape );
    }
    SHPClose( hSHP );
}

void project_shapes(double lng, double lat) {


    char* pj_old_str = "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs";
    char pj_new_str[256];
    sprintf(pj_new_str, "+proj=laea +lat_0=%f +lon_0=%f +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs",lat,lng);
    
    projPJ old_prj = pj_init_plus(pj_old_str);
    projPJ new_prj = pj_init_plus(pj_new_str);

    for(int i=0; i<shapesCount; i++) {
        
        memcpy(pr_shapesX[i], shapesX[i], shapeLengths[i]*sizeof(double));
        memcpy(pr_shapesY[i], shapesY[i], shapeLengths[i]*sizeof(double));
        memcpy(pr_shapesZ[i], shapesZ[i], shapeLengths[i]*sizeof(double));
        
        for (int j=0; j<shapeLengths[i]; j++) {
            pr_shapesX[i][j] *= DEG_TO_RAD;
            pr_shapesY[i][j] *= DEG_TO_RAD;
            pr_shapesZ[i][j]  = 0; 
        }
        pj_transform(old_prj, new_prj, shapeLengths[i], 0, 
                 pr_shapesX[i],
                 pr_shapesY[i], NULL);
    }
    // printf("%f, %f\n", pr_shapesX[0][0]/4e6, pr_shapesY[0][0]/4e6);
    pj_free(new_prj);
    pj_free(old_prj);
}

void free_shapes() {

    for(int i=0; i<shapesCount; i++) {

        free(shapesX[i]);
        free(shapesY[i]);
        free(shapesZ[i]);
        
        free(pr_shapesX[i]);
        free(pr_shapesY[i]);
        free(pr_shapesZ[i]);

        free(shapeParts[i]);
    }

    free(shapesX);
    free(shapesY);
    free(shapesZ);
    
    free(pr_shapesX);
    free(pr_shapesY);
    free(pr_shapesZ);

    free(shapeLengths);
    free(shapeParts);

}
