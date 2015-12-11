
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "shapes.h"
#include "mesh.h"
#include "font.h"

static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void cursorpos_callback(GLFWwindow* window, double x, double y);
static void scroll_callback(GLFWwindow* window, double x, double y);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

static void draw_shapes();
static void draw_screen_grid();
static void draw_text(char* text, float x, float y);
static void draw_grid();
static void draw_mesh();
static void reproject(double lng, double lat);

double zoom = 1.3e7;
// int shape_id = 135;
int shape_id = 1;

double lat = 90.0, lng = 90.0;
int dirty = 1;

int main(void) {

    GLFWwindow* window;

    // shapes_load("../data/russia_110m");
    shapes_load("../data/earth_50m");
    grid_init(10, 10);

    reproject(90, 90);

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
            
            char buf[256];
            float margin = 0.05;
            
            sprintf(buf,"\nLNG:%6.1f\nLAT:%6.1f", lng, lat);
            draw_text(buf, -1.0+margin, 1.0-margin);
            sprintf(buf,"\nPNT: %d\nTRI: %d", mesh_points_count, mesh_triangles_count);
            draw_text(buf, -1.0+margin+0.4, 1.0-margin);

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
    
    if(mesh_triangles == NULL) return;

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < mesh_triangles_count; i++) {
        int p1i = mesh_triangles[i * 3    ];
        int p2i = mesh_triangles[i * 3 + 1];
        int p3i = mesh_triangles[i * 3 + 2];

        glColor3f(0.25f,0.25f,0.25f); 
        glVertex3f(mesh_X[p1i],
                   mesh_Y[p1i],
                   0);
        glColor3f(0.25f,0.25f,0.25f); 
        glVertex3f(mesh_X[p2i],
                   mesh_Y[p2i],
                   0);
        glColor3f(0.25f,0.25f,0.25f); 
        glVertex3f(mesh_X[p3i],
                   mesh_Y[p3i],
                   0);
    }
    glEnd();    

    // glBegin(GL_POINTS);
    // for (int i = 0; i < mesh.numberofpoints; i++) {
    //     glColor3f(1.0f,1.0f,1.0f); 
    //     glVertex3f(mesh_pr_X[i]/zoom,
    //                mesh_pr_Y[i]/zoom,
    //                0);
    // }
    // glEnd();
}

static void draw_text(char* text, float x, float y) {

    float scale = 80.0;
    int x_cnt = 0;
    int y_cnt = 0;
    for (int i=0; i<strlen(text); i++) {

        glBegin(GL_LINES);
        
        unsigned char c = text[i];
        switch(c){
            case '\n':
                x_cnt=0;
                y_cnt++;
                continue;
            case ' ':
                x_cnt++;
                continue;
        }
        // get 0xFF char if char is empty
        if(!font[c].l) c = 0xFF;

        for (int j=0; j<font[c].l; j++) {
            int p1 = font[c].e[j*2  ];
            int p2 = font[c].e[j*2+1];
            
            float x1 = font[c].p[p1*2  ]/scale+(x_cnt/scale*2.5)+x;
            float x2 = font[c].p[p2*2  ]/scale+(x_cnt/scale*2.5)+x;
            float y1 = font[c].p[p1*2+1]/scale-(y_cnt/scale*4.5)+y;
            float y2 = font[c].p[p2*2+1]/scale-(y_cnt/scale*4.5)+y;

            glColor3f(0.50f, 0.50f, 0.50f);
            glVertex3f(x1,y1,0.0);
            glVertex3f(x2,y2,0.0);
        }
        glEnd();
        x_cnt++;
    } 
}

static void draw_shapes(){

    static int cnt = 0;
    
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for(int i=0; i<shapes_count; i++) {
        
        if(shape_id != i) continue;

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

static void incr_shape_id(int v) {
    shape_id = (shape_id +v) % shapes_count;
    if(shape_id<0) shape_id = shapes_count-1;
    lng = shape_centers_X[shape_id];
    lat = shape_centers_Y[shape_id];

    printf("lng lat: %f %f", lng, lat);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    
    if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS) 
        glfwSetWindowShouldClose(window, GL_TRUE);
    
    if (key == GLFW_KEY_KP_ADD      && action == GLFW_PRESS) zoom/=1.2;
    if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS) zoom*=1.2;
    
    if (key == GLFW_KEY_KP_6 && action == GLFW_PRESS) lng+=5.0f;
    if (key == GLFW_KEY_KP_4 && action == GLFW_PRESS) lng-=5.0f;
    if (key == GLFW_KEY_KP_8 && action == GLFW_PRESS) lat+=5.0f;
    if (key == GLFW_KEY_KP_2 && action == GLFW_PRESS) lat-=5.0f;
    
    if (key == GLFW_KEY_KP_9 && action == GLFW_PRESS) incr_shape_id( 1);
    if (key == GLFW_KEY_KP_3 && action == GLFW_PRESS) incr_shape_id(-1);
    
    if (key == GLFW_KEY_S && action == GLFW_PRESS) mesh_save_ply(lng, lat, zoom);
    

    if (key == GLFW_KEY_A && action == GLFW_PRESS) { 
        mesh_free();
        mesh_file(shape_id, zoom); 
        // mesh_project(lng, lat);
    };
    if (key == GLFW_KEY_L && action == GLFW_PRESS) { 
        mesh_load();
    }
    
    printf("lng: %f\tlat:%f\n", lng, lat);
    printf("shape id: %d\n", shape_id);
    

    reproject(lng, lat);
}

static void reproject(double lng, double lat) {
    shapes_project_shape(shape_id, lng, lat);
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