
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include "shapefil.h"

static void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void openShapeFile(char* filename);
void drawShapes();
void freeShapes();


int main(void) {

    GLFWwindow* window;

    openShapeFile("../data/earth_110m");

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    window = glfwCreateWindow(800, 600, "Shape visualizer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetKeyCallback(window, key_callback);

    while (!glfwWindowShouldClose(window)) {
        
        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        
        glLoadIdentity();
        glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        glMatrixMode(GL_MODELVIEW);

        glLoadIdentity();
        float t = (float) glfwGetTime();
        // glRotatef( t*50.f, t*25.0f, t*5.0f, 1.f);
        // glBegin(GL_TRIANGLES);
        
        // float r = 0.1;
        // for(int x=-xs; x<xs; x++) {
        //     for (int y=-ys; y<ys; y++){
        //         createElement(x*r, y*r, r*0.98);
        //     }
        // }
        // glEnd();
        drawShapes();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    freeShapes();

    exit(EXIT_SUCCESS);
}

// shape loader

SHPHandle hSHP;
int nEntities;

void openShapeFile(char* filename){

    double  adfMinBound[4], adfMaxBound[4];
    int nShapeType;
    // Read file 
    hSHP = SHPOpen( filename, "rb" );

    if( hSHP == NULL ) {
        printf( "Unable to open:%s\n", filename );
        return;
    }
    // Print shape bounds
    SHPGetInfo( hSHP, &nEntities, &nShapeType, adfMinBound, adfMaxBound );

    
    printf( "Shapefile Type: %s   # of Shapes: %d\n\n",
            SHPTypeName( nShapeType ), nEntities );
    
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

    return;
}

void drawShapes() {
    // Iterate through entries
    const char  *pszPlus;

    for( int i = 0; i < nEntities; i++ ) {

        int j, iPart;
        SHPObject   *psShape;

        psShape = SHPReadObject( hSHP, i );

        if( psShape == NULL ) {
            fprintf( stderr,
                     "Unable to read shape %d, terminating object reading.\n",
                    i );
            break;
        }

        // if( psShape->bMeasureIsUsed )
        //     printf( "\nShape:%d (%s)  nVertices=%d, nParts=%d\n"
        //             "  Bounds:(%.15g,%.15g, %.15g, %.15g)\n"
        //             "      to (%.15g,%.15g, %.15g, %.15g)\n",
        //             i, SHPTypeName(psShape->nSHPType),
        //             psShape->nVertices, psShape->nParts,
        //             psShape->dfXMin, psShape->dfYMin,
        //             psShape->dfZMin, psShape->dfMMin,
        //             psShape->dfXMax, psShape->dfYMax,
        //             psShape->dfZMax, psShape->dfMMax );
        // else
        //     printf( "\nShape:%d (%s)  nVertices=%d, nParts=%d\n"
        //             "  Bounds:(%.15g,%.15g, %.15g)\n"
        //             "      to (%.15g,%.15g, %.15g)\n",
        //             i, SHPTypeName(psShape->nSHPType),
        //             psShape->nVertices, psShape->nParts,
        //             psShape->dfXMin, psShape->dfYMin,
        //             psShape->dfZMin,
        //             psShape->dfXMax, psShape->dfYMax,
        //             psShape->dfZMax );

        if( psShape->nParts > 0 && psShape->panPartStart[0] != 0 ) {
            fprintf( stderr, "panPartStart[0] = %d, not zero as expected.\n",
                     psShape->panPartStart[0] );
        }
        
        glBegin(GL_LINE_LOOP);

        for( j = 0, iPart = 1; j < psShape->nVertices; j++ ) {
            const char  *pszPartType = "";

            if( j == 0 && psShape->nParts > 0 )
                pszPartType = SHPPartTypeName( psShape->panPartType[0] );
            
            if( iPart < psShape->nParts
                && psShape->panPartStart[iPart] == j ) {
                pszPartType = SHPPartTypeName( psShape->panPartType[iPart] );
                iPart++;
                glEnd();
                glBegin(GL_LINE_LOOP);
                
            } else
                pszPlus = " ";

            if( psShape->bMeasureIsUsed ) {

                printf("   %s (%.15g,%.15g, %.15g, %.15g) %s \n",
                       pszPlus,
                       psShape->padfX[j],
                       psShape->padfY[j],
                       psShape->padfZ[j],
                       psShape->padfM[j],
                       pszPartType );

            } else {
                glColor3f(1.0f,1.0f,1.0f); 
                glVertex3f(psShape->padfX[j]/180.0,
                           psShape->padfY[j]/90.0,
                           psShape->padfZ[j]);

                // printf("   %s (%.15g,%.15g, %.15g) %s \n",
                //        pszPlus,
                //        psShape->padfX[j],
                //        psShape->padfY[j],
                //        psShape->padfZ[j],
                //        pszPartType );
            }
        }
        glEnd();

        SHPDestroyObject( psShape );
    }
}

void freeShapes() {
    SHPClose( hSHP );
}
