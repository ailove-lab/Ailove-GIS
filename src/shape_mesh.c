
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

#include "shapes.h"

static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void cursorpos_callback(GLFWwindow* window, double x, double y);

static void draw_shapes();
static void draw_grid();

#define SCALE 1.3e7

double lat, lng;

int main(void) {

    GLFWwindow* window;

    // load_shapes("../data/russia_110m");
    load_shapes("../data/earth_110m");
    init_grid(10,10);
    project_shapes(90, 90);
    project_grid(90, 90);

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
    glfwSetCursorPosCallback(window, cursorpos_callback);

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

        draw_grid();
        draw_shapes();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    // clear shapes
    free_grid();
    free_shapes();

    exit(EXIT_SUCCESS);
}

static void draw_shapes(){
    for(int i=0; i<shapesCount; i++) {
        glBegin(GL_LINE_LOOP);
        for (int j=0, sp=1; j<shapeLengths[i]; j++) {
            if(shapeParts[i][sp]==j){
                glEnd();
                glBegin(GL_LINE_LOOP);
                sp++;
            }

            // glColor3f(1.0f,1.0f,1.0f); 
            // glVertex3f(shapesX[i][j]/180.0,
            //            shapesY[i][j]/ 90.0,
            //            shapesZ[i][j]);

            glColor3f(1.0f,1.0f,1.0f); 
            glVertex3f(pr_shapesX[i][j]/SCALE,
                       pr_shapesY[i][j]/SCALE,
                       pr_shapesZ[i][j]);

        }
        glEnd();
    }
}

static void draw_grid(){

    for (int i=0; i<horizontalCount; i++) {
        glBegin(GL_LINE_STRIP);
        for (int j=1; j<verticalCount; j++) {
            
            if(i%2) glColor3f(0.05f, 0.05f, 0.05f);
            else    glColor3f(0.20f, 0.20f, 0.20f);
            
            glVertex3f(
                vertical_gridX[i][j]/SCALE,
                vertical_gridY[i][j]/SCALE,
                0);
        }
        glEnd();
    }

    for (int i=0; i<verticalCount; i++) {
        glBegin(GL_LINE_LOOP);
        for (int j=0; j<horizontalCount; j++) {
            if(i%2) glColor3f(0.05f, 0.05f, 0.05f);
            else    glColor3f(0.20f, 0.20f, 0.20f);
            glVertex3f(
                horizontal_gridX[i][j]/SCALE,
                horizontal_gridY[i][j]/SCALE,
                0);
        }
        glEnd();
    }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

static void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

static void cursorpos_callback(GLFWwindow* window, double x, double y) {
    double lng, lat;
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    lng =-(x/width  - 0.5)*360.0;
    lat =+(y/height - 0.5)*180.0;
    project_shapes(lng, lat);
    project_grid(lng, lat);
}
