
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

#include "shapes.h"
#include "mesh.h"


static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void cursorpos_callback(GLFWwindow* window, double x, double y);
static void scroll_callback(GLFWwindow* window, double x, double y);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

static void draw_shapes();
static void draw_screen_grid();
static void draw_grid();
static void draw_mesh();
static void reproject(double lng, double lat);
static void save_mesh();

double zoom = 1.3e7;
int shape_id = 135;

double lat = 90.0, lng = 90.0;
int dirty = 1;

int main(void) {

    GLFWwindow* window;

    // shapes_load("../data/russia_110m");
    shapes_load("../data/earth_110m");
    grid_init(10, 10);
    
    reproject(90, 90);
    mesh_shape(shape_id);

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

        if (dirty) {
            glfwGetFramebufferSize(window, &width, &height);
            ratio = width / (float) height;

            glViewport(0, 0, width, height);
            glClear(GL_COLOR_BUFFER_BIT);

            glMatrixMode(GL_PROJECTION);
            
            glLoadIdentity();
            glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
            glMatrixMode(GL_MODELVIEW);

            glLoadIdentity();

            draw_screen_grid();
            draw_grid();
            draw_mesh();
            draw_shapes();
            glfwSwapBuffers(window);
            dirty = 0;
        }

        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    // clear shapes
    grid_free();
    shapes_free();
    mesh_free();

    exit(EXIT_SUCCESS);
}

static void save_mesh() {
    
    FILE *f = fopen("mesh.txt","w");
    
    if(f == NULL) {
        printf("Can't open file\n");
        return;
    }

    for (int i = 0; i < mesh.numberoftriangles; i++) {
        int p1i = mesh.trianglelist[i * 3    ];
        int p2i = mesh.trianglelist[i * 3 + 1];
        int p3i = mesh.trianglelist[i * 3 + 2];

        fprintf(f, "%f %f %f\n", mesh_prX[p1i]/zoom, mesh_prY[p1i]/zoom, 0.0f);
        fprintf(f, "%f %f %f\n", mesh_prX[p2i]/zoom, mesh_prY[p2i]/zoom, 0.0f);
        fprintf(f, "%f %f %f\n", mesh_prX[p3i]/zoom, mesh_prY[p3i]/zoom, 0.0f);
    }
    fclose(f);
}

static void draw_screen_grid() {
    float a = 0.1f;
    for (int i=-10; i<=10; i++) {
        glBegin(GL_LINES);
        if(!i) a = 0.4f;
        else   a = 0.1f;
        glColor3f(a, a, a); glVertex3f(   -1.0, i/10.0, 0.0);
        glColor3f(a, a, a); glVertex3f(    1.0, i/10.0, 0.0);
        glColor3f(a, a, a); glVertex3f( i/10.0,    1.0, 0.0);
        glColor3f(a, a, a); glVertex3f( i/10.0,   -1.0, 0.0);

        glEnd();
    }
}

static void draw_mesh() {
   
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < mesh.numberoftriangles; i++) {
        int p1i = mesh.trianglelist[i * 3    ];
        int p2i = mesh.trianglelist[i * 3 + 1];
        int p3i = mesh.trianglelist[i * 3 + 2];

        glColor3f(0.25f,0.25f,0.25f); 
        glVertex3f(meshX[p1i],//zoom,
                   meshY[p1i],//zoom,
                   0);
        glColor3f(0.25f,0.25f,0.25f); 
        glVertex3f(meshX[p2i],//zoom,
                   meshY[p2i],//zoom,
                   0);
        glColor3f(0.25f,0.25f,0.25f); 
        glVertex3f(meshX[p3i],//zoom,
                   meshY[p3i],//zoom,
                   0);
    }
    glEnd();    

    glBegin(GL_POINTS);
    for (int i = 0; i < mesh.numberofpoints; i++) {
        glColor3f(1.0f,1.0f,1.0f); 
        glVertex3f(meshX[i],//zoom,
                   meshY[i],//zoom,
                   0);
    }
    glEnd();
}


static void draw_shapes(){

    static int cnt = 0;
    
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for(int i=0; i<shapes_count; i++) {
        

        glBegin(GL_LINE_LOOP);
        for (int j=0, sp=1; j<shapes_length[i]; j++) {
            if(shapes_parts[i][sp]==j){
                glEnd();
                glBegin(GL_LINE_LOOP);
                sp++;
            }

            // glColor3f(1.0f,1.0f,1.0f); 
            // glVertex3f(shapesX[i][j]/180.0,
            //            shapesY[i][j]/ 90.0,
            //            shapesZ[i][j]);

            if (i==shape_id)
                glColor4f(1.0f,1.0f,1.0f,0.5f); 
            else 
                glColor4f(1.0f,1.0f,1.0f,0.2f); 

            glVertex3f(shapes_prX[i][j]/zoom,
                       shapes_prY[i][j]/zoom,
                       shapes_prZ[i][j]);

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
                grid_verticalX[i][j]/zoom,
                grid_verticalY[i][j]/zoom,
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
                grid_horizontalX[i][j]/zoom,
                grid_horizontalY[i][j]/zoom,
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
    
    if (key == GLFW_KEY_KP_1 && action == GLFW_PRESS) shape_id-=1;
    if (key == GLFW_KEY_KP_3 && action == GLFW_PRESS) shape_id+=1;

    if (key == GLFW_KEY_S && action == GLFW_PRESS) save_mesh();

    printf("lng: %f\tlat:%f\n", lng, lat);
    printf("shape id: %d\n", shape_id);
    

    reproject(lng, lat);
}

static void reproject(double lng, double lat) {
    shapes_project(lng, lat);
    grid_project(lng, lat);
    // mesh_project(lng, lat);
    dirty = 1;
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

        reproject(lng, lat);
        // printf("lng: %f\tlat:%f\n", lng, lat);

    }
}

static void scroll_callback(GLFWwindow* window, double x, double y) {
    // printf("%f, %f\n", x, y); 
    if (y>0) zoom/=1.2; else zoom*=1.2;
    dirty = 1;
    // if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS) zoom*=1.2;
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)   {
        glfwGetCursorPos(window, &start_x, &start_y);
        dragging = 1;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) dragging = 0;
}