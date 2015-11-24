
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

#include "shapes.h"

static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void cursorpos_callback(GLFWwindow* window, double x, double y);
static void scroll_callback(GLFWwindow* window, double x, double y);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

static void draw_shapes();
static void draw_grid();

double zoom = 1.3e7;

double lat = 90.0, lng = 90.0;


int main(void) {

    GLFWwindow* window;

    // load_shapes("../data/russia_110m");
    load_shapes("../data/earth_110m");
    init_grid(10, 10);
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
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

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

    static int cnt = 0;

    for(int i=0; i<shapesCount; i++) {
        
        if (i!=135) continue;

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
            glVertex3f(pr_shapesX[i][j]/zoom,
                       pr_shapesY[i][j]/zoom,
                       pr_shapesZ[i][j]);

        }
        glEnd();
    }
}

static void draw_grid(){

    for (int i=0; i<horizontalCount; i++) {
        glBegin(GL_LINE_STRIP);
        for (int j=1; j<verticalCount; j++) {
            
            if(i%2) 
                glColor3f(0.20f, 0.20f, 0.20f);
            else    
                glColor3f(0.10f, 0.10f, 0.10f);
            
            glVertex3f(
                vertical_gridX[i][j]/zoom,
                vertical_gridY[i][j]/zoom,
                0);
        }
        glEnd();
    }

    for (int i=0; i<verticalCount; i++) {
        glBegin(GL_LINE_LOOP);
        for (int j=0; j<horizontalCount; j++) {
            if(i%2) 
                glColor3f(0.20f, 0.20f, 0.20f);
            else    
                glColor3f(0.10f, 0.10f, 0.10f);
            glVertex3f(
                horizontal_gridX[i][j]/zoom,
                horizontal_gridY[i][j]/zoom,
                0);
        }
        glEnd();
    }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_KP_ADD      && action == GLFW_PRESS) zoom/=1.2;
    if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS) zoom*=1.2;
    
    if (key == GLFW_KEY_KP_6 && action == GLFW_PRESS) lng+=5.0f;
    if (key == GLFW_KEY_KP_4 && action == GLFW_PRESS) lng-=5.0f;
    if (key == GLFW_KEY_KP_8 && action == GLFW_PRESS) lat+=5.0f;
    if (key == GLFW_KEY_KP_2 && action == GLFW_PRESS) lat-=5.0f;

    printf("lng: %f\tlat:%f\n", lng, lat);
    project_shapes(lng, lat);
    project_grid(lng, lat);

}

static void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

double start_x, start_y;
char dragging=0;

static void cursorpos_callback(GLFWwindow* window, double x, double y) {

    if (dragging) {
        double dx = x-start_x;
        double dy = y-start_y;

        lng -= dx/10.0;
        lat += dy/10.0;
        
        if(lat> 90) lat =  90;
        if(lat<-90) lat = -90;

        start_x = x;
        start_y = y;

        project_shapes(lng, lat);
        project_grid(lng, lat);
        // printf("lng: %f\tlat:%f\n", lng, lat);

    }
}

static void scroll_callback(GLFWwindow* window, double x, double y) {
    // printf("%f, %f\n", x, y); 
    if (y>0) zoom/=1.2; else zoom*=1.2;
    // if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS) zoom*=1.2;
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)   {
        glfwGetCursorPos(window, &start_x, &start_y);
        dragging = 1;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) dragging = 0;
}