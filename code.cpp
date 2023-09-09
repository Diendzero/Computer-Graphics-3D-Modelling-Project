#define FREEGLUT_STATIC

#include <GL/freeglut.h>
#include <stdlib.h>
#include<math.h>
#include <stdio.h>
#include "vector"

using namespace std;
GLint imagewidth, imageheight, pixellength;
vector<GLubyte*> p;
GLuint texture[19];
struct image {
    GLint imageWidth;
    GLint imageHeight;
    GLint pixelLength;
};
image text_image[19];
float roadTreeDistance;

int intWinWidth = 1200; // Default window size
int intWinHeight = 800;
float fltZoom = 4.0; // Zoom amount
float fltX0 = 0.0; // Camera position
float fltY0 = 70.0;
float fltZ0 = -700.0;
float fltXRef = 0.0; // Look At reference point
float fltYRef = 0.0;
float fltZRef = 0.0;
float fltXUp = 0.0; // Up vector
float fltYUp = 1.0;
float fltZUp = 0.0;
float time_interval_gate = 30;
float gate_rotation_angle = 0;
float gate_rotation_step = 1;
float time_interval_car1 = 30;
float car1_z_position = 2500;
float car1_z_position_step = -20;
float car1_x_position = 3250;
float car1_x_position_step = 0;
float car1_rotate_angle = 270;
float car1_rotate_angle_step = 0;
float time_interval_car2 = 30;
float car2_z_position = 2500;
float car2_z_position_step = -20;
float car2_x_position = -3900;
float car2_x_position_step = 0;
float car2_rotate_angle = 270;
float car2_rotate_angle_step = 0;


void ReadImage(const char path[512], GLint& imagewidth, GLint& imageheight, GLint& pixellength) {
    GLubyte* pixeldata;
    FILE* pfile;
    fopen_s(&pfile, path, "rb");
    if (pfile == 0) exit(0);
    fseek(pfile, 0x0012, SEEK_SET);
    fread(&imagewidth, sizeof(imagewidth), 1, pfile);
    fread(&imageheight, sizeof(imageheight), 1, pfile);
    pixellength = imagewidth * 3;
    while (pixellength % 4 != 0)pixellength++;
    pixellength *= imageheight;
    pixeldata = (GLubyte*)malloc(pixellength);
    if (pixeldata == 0) exit(0);
    fseek(pfile, 54, SEEK_SET);
    fread(pixeldata, pixellength, 1, pfile);
    p.push_back(pixeldata);
    fclose(pfile);
}


void when_in_mainloop() {
    // force OpenGL to redraw the current window
    glutPostRedisplay();
}


void createCube(void) {
    const GLfloat x1 = -0.5, x2 = 0.5;
    const GLfloat y1 = -0.5, y2 = 0.5;
    const GLfloat z1 = -0.5, z2 = 0.5;

    GLfloat point[6][4][3] = {
            {{x1, y1, z1}, {x2, y1, z1}, {x2, y2, z1}, {x1, y2, z1}},
            {{x1, y1, z1}, {x2, y1, z1}, {x2, y1, z2}, {x1, y1, z2}},
            {{x2, y1, z1}, {x2, y2, z1}, {x2, y2, z2}, {x2, y1, z2}},
            {{x1, y1, z1}, {x1, y2, z1}, {x1, y2, z2}, {x1, y1, z2}},
            {{x1, y2, z1}, {x2, y2, z1}, {x2, y2, z2}, {x1, y2, z2}},
            {{x1, y1, z2}, {x2, y1, z2}, {x2, y2, z2}, {x1, y2, z2}}
    };

    int dir[4][2] = { {1, 1},
                     {1, 0},
                     {0, 0},
                     {0, 1} };

    glBegin(GL_QUADS);
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 4; j++) {
            glTexCoord2iv(dir[j]);
            glVertex3fv(point[i][j]);
        }
    }
    glEnd();
}


void createRectangularPyramid(void) {
    const GLfloat x1 = -0.5, x2 = 0.5, x3 = 0;
    const GLfloat z1 = -0.5, z2 = 0.5, z3 = 0;
    const GLfloat y1 = 0, y2 = 0.5;

    GLfloat point[4][3][3] = {
            {{x1, y1, z1}, {x2, y1, z1}, {x3, y2, z3}}, // front
            {{x1, y1, z2}, {x2, y1, z2}, {x3, y2, z3}}, // back
            {{x1, y1, z2}, {x1, y1, z1}, {x3, y2, z3}}, // left
            {{x2, y1, z1}, {x2, y1, z2}, {x3, y2, z3}} // right
    };

    GLfloat dir[3][2] = { {0,   0},
                         {1,   0},
                         {0.5, 1} };

    glBegin(GL_QUADS);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            glTexCoord2fv(dir[j]);
            glVertex3fv(point[i][j]);
        }
    }
    glEnd();

    // The bottom
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(x1, y1, z1);
    glTexCoord2f(1, 0);
    glVertex3f(x2, y1, x1);
    glTexCoord2f(0, 1);
    glVertex3f(x1, y1, z2);
    glTexCoord2f(1, 1);
    glVertex3f(x2, y1, z2);
    glEnd();
    glPopMatrix();
}


void createWireRectangularPyramid(void) {
    const GLfloat x1 = -0.5, x2 = 0.5, x3 = 0;
    const GLfloat z1 = -0.5, z2 = 0.5, z3 = 0;
    const GLfloat y1 = 0, y2 = 0.5;

    GLfloat point[4][3][3] = {
            {{x1, y1, z1}, {x2, y1, z1}, {x3, y2, z3}}, // front
            {{x1, y1, z2}, {x2, y1, z2}, {x3, y2, z3}}, // back
            {{x1, y1, z2}, {x1, y1, z1}, {x3, y2, z3}}, // left
            {{x2, y1, z1}, {x2, y1, z2}, {x3, y2, z3}} // right
    };

    // front
    glBegin(GL_LINE_LOOP);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y1, z1);
    glVertex3f(x3, y2, z3);
    glEnd();

    // back
    glBegin(GL_LINE_LOOP);
    glVertex3f(x1, y1, z2);
    glVertex3f(x2, y1, z2);
    glVertex3f(x3, y2, z3);
    glEnd();

    // left
    glBegin(GL_LINE_LOOP);
    glVertex3f(x1, y1, z2);
    glVertex3f(x1, y1, z1);
    glVertex3f(x3, y2, z3);
    glEnd();

    // right
    glBegin(GL_LINE_LOOP);
    glVertex3f(x2, y1, z1);
    glVertex3f(x2, y1, z2);
    glVertex3f(x3, y2, z3);
    glEnd();

    // The bottom
    glBegin(GL_LINE_LOOP);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y1, x1);
    glVertex3f(x2, y1, z2);
    glVertex3f(x1, y1, z2);
    glEnd();
}


void createHexahedron(void) {
    const GLfloat x1 = -0.5, x2 = 0.5, x3 = -1.0, x4 = 1.0;
    const GLfloat z1 = -0.3, z2 = 0.3, z3 = -1.0, z4 = 1.0;
    const GLfloat y1 = 0, y2 = 0.5;

    GLfloat point[6][4][3] = {
            {{x3, y1, z3}, {x4, y1, z3}, {x3, y2, z1}, {x4, y2, z1}}, // front
            {{x4, y1, z4}, {x3, y1, z4}, {x4, y2, z2}, {x3, y2, z2}}, // back
            {{x3, y1, z4}, {x3, y1, z3}, {x3, y2, z2}, {x3, y2, z1}}, // left
            {{x4, y1, z3}, {x4, y1, z4}, {x4, y2, z1}, {x4, y2, z2}}, // right
            {{x3, y2, z1}, {x4, y2, z1}, {x3, y2, z2}, {x4, y2, z2}}, // top
            {{x3, y1, z3}, {x4, y1, z3}, {x3, y1, z4}, {x4, y1, z4}} // bottom
    };

    int dir[4][2] = { {0, 0},
                     {1, 0},
                     {0, 1},
                     {1, 1} };

    glBegin(GL_QUADS);
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 4; j++) {
            glBindTexture(GL_TEXTURE_2D, texture[9]);
            glTexCoord2iv(dir[j]);
            glVertex3fv(point[i][j]);
        }
    }
    glEnd();
}


void createHexahedron2(void) {
    const GLfloat x1 = -0.5, x2 = 0.5;
    const GLfloat z1 = -0.5, z2 = 0.5;
    const GLfloat y1 = 0, y2 = 0.5, y3 = 0.2;

    GLfloat point[5][4][3] = {
            {{x1, y1, z1}, {x2, y1, z1}, {x2, y3, z1}, {x1, y2, z1}}, // front
            {{x2, y1, z2}, {x1, y1, z2}, {x1, y2, z2}, {x2, y3, z2}}, // back
            {{x1, y1, z2}, {x1, y1, z1}, {x1, y2, z2}, {x1, y2, z1}}, // left
            {{x2, y1, z1}, {x2, y1, z2}, {x2, y3, z2}, {x2, y3, z1}}, // right
            {{x1, y1, z1}, {x2, y1, z1}, {x2, y1, z2}, {x1, y1, z2}} // bottom
    };

    int dir[4][2] = { {0, 0},
                     {1, 0},
                     {1, 1},
                     {0, 1} };

    glBegin(GL_QUADS);
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 4; j++) {
            glTexCoord2iv(dir[j]);
            glVertex3fv(point[i][j]);
        }
    }
    glEnd();

    // top
    GLfloat point2[1][4][3] = {
            {{x1, y2, z1}, {x2, y3, z1}, {x2, y3, z2}, {x1, y2, z2}} };
    glBindTexture(GL_TEXTURE_2D, texture[9]);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 4; j++) {
            glTexCoord2iv(dir[j]);
            glVertex3fv(point2[i][j]);
        }
    }
    glEnd();
}


void createHouse2Pentahedron(void) {
    const GLfloat x1 = -1.0, x2 = 1.0, x3 = 0.0;
    const GLfloat z1 = -1.0, z2 = 1.0;
    const GLfloat y1 = 0, y2 = 1.0;

    // left
    GLfloat point1[1][4][3] = {
            {{x1, y1, z2}, {x1, y1, z1}, {x3, y2, z1}, {x3, y2, z2}} };
    int dir1[4][2] = { {0, 0},
                      {1, 0},
                      {1, 1},
                      {0, 1} };
    glBindTexture(GL_TEXTURE_2D, texture[9]);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 4; j++) {
            glTexCoord2iv(dir1[j]);
            glVertex3fv(point1[i][j]);
        }
    }
    glEnd();

    // right
    GLfloat point2[1][4][3] = {
            {{x2, y1, z1}, {x2, y1, z2}, {x3, y2, z2}, {x3, y2, z1}} };
    int dir2[4][2] = { {0, 0},
                      {1, 0},
                      {1, 1},
                      {0, 1} };
    glBindTexture(GL_TEXTURE_2D, texture[9]);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 4; j++) {
            glTexCoord2iv(dir2[j]);
            glVertex3fv(point2[i][j]);
        }
    }
    glEnd();

    // bottom
    GLfloat point3[1][4][3] = {
            {{x1, y1, z1}, {x2, y1, z1}, {x2, y1, z2}, {x1, y1, z2}} };
    int dir3[4][2] = { {0, 0},
                      {1, 0},
                      {1, 1},
                      {0, 1} };
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    glBegin(GL_QUADS);
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 4; j++) {
            glTexCoord2iv(dir3[j]);
            glVertex3fv(point3[i][j]);
        }
    }
    glEnd();

    // front
    GLfloat point4[1][3][3] = {
            {{x1, y1, z1}, {x2, y1, z1}, {x3, y2, z1}} };
    GLfloat dir4[3][2] = { {0,   0},
                          {1,   0},
                          {0.5, 1} };
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 3; j++) {
            glTexCoord2fv(dir4[j]);
            glVertex3fv(point4[i][j]);
        }
    }
    glEnd();

    // back
    GLfloat point5[1][3][3] = {
            {{x2, y1, z2}, {x1, y1, z2}, {x3, y2, z2}} };
    GLfloat dir5[3][2] = { {0,   0},
                          {1,   0},
                          {0.5, 1} };
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 3; j++) {
            glTexCoord2fv(dir5[j]);
            glVertex3fv(point5[i][j]);
        }
    }
    glEnd();
}


void createCarTop(void) {
    const GLfloat x1 = -1.0, x2 = 1.0, x3 = -0.5, x4 = 0.5;
    const GLfloat z1 = -1.0, z2 = 1.0, z3 = -0.5, z4 = 0.5;
    const GLfloat y1 = 0, y2 = 0.5;

    // bottom
    glBindTexture(GL_TEXTURE_2D, texture[15]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(x1, y1, z1);
    glTexCoord2f(1, 0);
    glVertex3f(x2, y1, z1);
    glTexCoord2f(1, 1);
    glVertex3f(x2, y1, z2);
    glTexCoord2f(0, 1);
    glVertex3f(x1, y1, z2);
    glEnd();

    // top
    glBindTexture(GL_TEXTURE_2D, texture[16]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(x3, y2, z3);
    glTexCoord2f(1, 0);
    glVertex3f(x4, y2, z3);
    glTexCoord2f(1, 1);
    glVertex3f(x4, y2, z4);
    glTexCoord2f(0, 1);
    glVertex3f(x3, y2, z4);
    glEnd();

    // front
    glBindTexture(GL_TEXTURE_2D, texture[15]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(x1, y1, z1);
    glTexCoord2f(1, 0);
    glVertex3f(x2, y1, z1);
    glTexCoord2f(1, 1);
    glVertex3f(x4, y2, z3);
    glTexCoord2f(0, 1);
    glVertex3f(x3, y2, z3);
    glEnd();

    // back
    glBindTexture(GL_TEXTURE_2D, texture[15]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(x2, y1, z2);
    glTexCoord2f(1, 0);
    glVertex3f(x1, y1, z2);
    glTexCoord2f(1, 1);
    glVertex3f(x3, y2, z4);
    glTexCoord2f(0, 1);
    glVertex3f(x4, y2, z4);
    glEnd();

    // left
    glBindTexture(GL_TEXTURE_2D, texture[15]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(x1, y1, z2);
    glTexCoord2f(1, 0);
    glVertex3f(x1, y1, z1);
    glTexCoord2f(1, 1);
    glVertex3f(x3, y2, z3);
    glTexCoord2f(0, 1);
    glVertex3f(x3, y2, z4);
    glEnd();

    // right
    glBindTexture(GL_TEXTURE_2D, texture[15]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(x2, y1, z1);
    glTexCoord2f(1, 0);
    glVertex3f(x2, y1, z2);
    glTexCoord2f(1, 1);
    glVertex3f(x4, y2, z4);
    glTexCoord2f(0, 1);
    glVertex3f(x4, y2, z3);
    glEnd();
}


void createCarBody(void) {
    const GLfloat x1 = -1.0, x2 = 1.0;
    const GLfloat z1 = -1.0, z2 = 1.0;
    const GLfloat y1 = -1.0, y2 = 1;

    // bottom
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(x1, y1, z1);
    glTexCoord2f(1, 0);
    glVertex3f(x2, y1, z1);
    glTexCoord2f(1, 1);
    glVertex3f(x2, y1, z2);
    glTexCoord2f(0, 1);
    glVertex3f(x1, y1, z2);
    glEnd();

    // top
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(x1, y2, z1);
    glTexCoord2f(1, 0);
    glVertex3f(x2, y2, z1);
    glTexCoord2f(1, 1);
    glVertex3f(x2, y2, z2);
    glTexCoord2f(0, 1);
    glVertex3f(x1, y2, z2);
    glEnd();

    // front
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(x1, y1, z1);
    glTexCoord2f(1, 0);
    glVertex3f(x2, y1, z1);
    glTexCoord2f(1, 1);
    glVertex3f(x2, y2, z1);
    glTexCoord2f(0, 1);
    glVertex3f(x1, y2, z1);
    glEnd();

    // back
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(x1, y1, z2);
    glTexCoord2f(1, 0);
    glVertex3f(x2, y1, z2);
    glTexCoord2f(1, 1);
    glVertex3f(x2, y2, z2);
    glTexCoord2f(0, 1);
    glVertex3f(x1, y2, z2);
    glEnd();

    // left
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(x1, y1, z2);
    glTexCoord2f(1, 0);
    glVertex3f(x1, y1, z1);
    glTexCoord2f(1, 1);
    glVertex3f(x1, y2, z1);
    glTexCoord2f(0, 1);
    glVertex3f(x1, y2, z2);
    glEnd();

    // right
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(x2, y1, z1);
    glTexCoord2f(1, 0);
    glVertex3f(x2, y1, z2);
    glTexCoord2f(1, 1);
    glVertex3f(x2, y2, z2);
    glTexCoord2f(0, 1);
    glVertex3f(x2, y2, z1);
    glEnd();
}


void textureInit(void) {
    const char* image[19] = {
            "skybox_back.bmp",
            "skybox_front.bmp",
            "skybox_top.bmp",
            "skybox_bottom.bmp",
            "skybox_left.bmp",
            "skybox_right.bmp",
            "fencePillar.bmp",
            "board1.bmp",
            "whiteWall.bmp",
            "redTile.bmp",
            "board2.bmp",
            "glass.bmp",
            "door.bmp",
            "stone2.bmp",
            "road.bmp",
            "car1Window.bmp",
            "car1Color1.bmp",
            "car1Color2.bmp",
            "car1Color3.bmp"
    };
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_FLAT);
    glEnable(GL_TEXTURE_2D);
    for (int i = 0; i < 19; i++) {
        ReadImage(image[i], text_image[i].imageWidth, text_image[i].imageHeight, text_image[i].pixelLength);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // set pixel storage modes (in the memory)
        glGenTextures(2, &texture[i]); // number of texture names to be generated and an array of texture names
        glBindTexture(GL_TEXTURE_2D, texture[i]); // target to which texture is bound and name of a texture
        glTexImage2D(GL_TEXTURE_2D, 0, 3, text_image[i].imageWidth, text_image[i].imageHeight, 0, GL_BGR_EXT,
            GL_UNSIGNED_BYTE, p[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
}


void lightInit(void) {
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light0_position[] = { 0.5, 1, -0.5, 0.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE); // Enable glColorMaterial to allow individual
    glEnable(GL_COLOR_MATERIAL); // color setting of each object
    glEnable(GL_NORMALIZE);
}


void OnTimer_gate(int value) {
    gate_rotation_angle += gate_rotation_step;

    if (gate_rotation_angle >= 60) {
        gate_rotation_step = 0;
    }

    if (gate_rotation_angle <= 0) {
        gate_rotation_step = 0;
    }

    glutTimerFunc(time_interval_gate, OnTimer_gate, 1);
}


void OnTimer_car1(int value) {
    car1_x_position += car1_x_position_step;
    car1_z_position += car1_z_position_step;
    car1_rotate_angle += car1_rotate_angle_step;

    if (car1_z_position <= -1150) {
        car1_z_position_step = -5;
        car1_rotate_angle_step = 10;
    }

    if (car1_rotate_angle >= 360) {
        car1_rotate_angle_step = 0;
        car1_z_position_step = 0;
        car1_x_position_step = -20;
    }

    if (car1_x_position <= -3100) {
        car1_z_position_step = -5;
        car1_rotate_angle_step = 10;
    }

    if (car1_rotate_angle >= 450) {
        car1_rotate_angle_step = 0;
        car1_x_position_step = 0;
        car1_z_position_step = 20;
    }

    if (car1_z_position >= 2500) {
        car1_x_position_step = 0;
        car1_z_position_step = 0;
        car1_rotate_angle_step = 0;
    }

    glutTimerFunc(time_interval_car1, OnTimer_car1, 1);
}


void OnTimer_car2(int value) {
    car2_x_position += car2_x_position_step;
    car2_z_position += car2_z_position_step;
    car2_rotate_angle += car2_rotate_angle_step;

    if (car2_z_position <= -1600) {
        car2_z_position_step = -5;
        car2_rotate_angle_step = -10;
    }

    if (car2_rotate_angle <= 180) {
        car2_rotate_angle_step = 0;
        car2_z_position_step = 0;
        car2_x_position_step = 20;
    }

    if (car2_x_position >= 3750) {
        car2_z_position_step = -5;
        car2_rotate_angle_step = -10;
    }

    if (car2_rotate_angle <= 90) {
        car2_rotate_angle_step = 0;
        car2_x_position_step = 0;
        car2_z_position_step = 20;
    }

    if (car2_z_position >= 2500) {
        car2_x_position_step = 0;
        car2_z_position_step = 0;
        car2_rotate_angle_step = 0;
    }

    glutTimerFunc(time_interval_car2, OnTimer_car2, 1);
}


void drawSkybox(void) {
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glPushMatrix();
    glScalef(4500, 4500, 4500);
    //    glScalef(4500, 4500, 10000);
        // The front face
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(1.0, -1.0, 1.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glEnd();
    // The top face
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-1.0, 1.0, -1.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(1.0, 1.0, -1.0);
    glEnd();
    // The bottom face
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(1.0, -1.0, 1.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(1.0, -1.0, -1.0);
    glEnd();
    // The left face
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-1.0, 1.0, -1.0);
    glEnd();
    // The right face
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glBegin(GL_QUADS);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(1.0, 1.0, -1.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(1.0, -1.0, 1.0);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}


void bigFencePillar(void) {
    glEnable(GL_TEXTURE_2D);

    // The body of bigFencePillar
    glPushMatrix();
    glScalef(100, 400, 100);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    createCube();
    glPopMatrix();

    // The top of bigFencePillar
    // The top1
    glPushMatrix();
    glScalef(125, 50, 125);
    glPushMatrix();
    glTranslatef(0, 3, 0);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    // The top2
    glPushMatrix();
    glScalef(160, 50, 160);
    glPushMatrix();
    glTranslatef(0, 3.5, 0);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The bottom of bigFencePillar
    // The bottom1
    glPushMatrix();
    glScalef(125, 50, 125);
    glPushMatrix();
    glTranslatef(0, -3, 0);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    // The bottom2
    glPushMatrix();
    glScalef(160, 50, 160);
    glPushMatrix();
    glTranslatef(0, -3.5, 0);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}


void smallFencePillar(void) {
    glEnable(GL_TEXTURE_2D);

    // The body of bigFencePillar
    glPushMatrix();
    glScalef(90, 200, 90);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    createCube();
    glPopMatrix();

    // The top of bigFencePillar
    glPushMatrix();
    glScalef(120, 50, 120);
    glPushMatrix();
    glTranslatef(0, 2.5, 0);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The bottom of bigFencePillar
    glPushMatrix();
    glScalef(120, 50, 120);
    glPushMatrix();
    glTranslatef(0, -2.5, 0);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}


void gate(void) {
    glEnable(GL_TEXTURE_2D);

    glPushMatrix();
    glScalef(300, 400, 50);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    createCube();
    glPopMatrix();

    glTranslatef(0, 300, 0);
    glPushMatrix();
    glRotatef(140, 0, 0, 1);
    glPushMatrix();
    glScalef(350, 50, 50);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glTranslatef(-130, 0, 0);
    glPushMatrix();
    glScalef(40, 200, 50);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    createCube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}


void fenceWall(void) {
    glEnable(GL_TEXTURE_2D);

    //  The left
    glPushMatrix();
    glTranslatef(350, 0, 0);
    glPushMatrix();
    glScalef(50, 300, 50);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(280, 0, 0);
    glPushMatrix();
    glScalef(40, 300, 40);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(210, 0, 0);
    glPushMatrix();
    glScalef(40, 300, 40);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The middle
    glPushMatrix();
    glTranslatef(175, 70, 0);
    glPushMatrix();
    glRotatef(200, 0, 0, 1);
    glPushMatrix();
    glScalef(30, 150, 30);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(125, 70, 0);
    glPushMatrix();
    glRotatef(160, 0, 0, 1);
    glPushMatrix();
    glScalef(30, 150, 30);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(175, -70, 0);
    glPushMatrix();
    glRotatef(160, 0, 0, 1);
    glPushMatrix();
    glScalef(30, 150, 30);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(125, -70, 0);
    glPushMatrix();
    glRotatef(200, 0, 0, 1);
    glPushMatrix();
    glScalef(30, 150, 30);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The right
    glPushMatrix();
    glTranslatef(90, 0, 0);
    glPushMatrix();
    glScalef(40, 300, 40);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(20, 0, 0);
    glPushMatrix();
    glScalef(40, 300, 40);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-50, 0, 0);
    glPushMatrix();
    glScalef(50, 300, 50);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The top
    glPushMatrix();
    glTranslatef(150, 140, 0);
    glPushMatrix();
    glRotatef(90, 0, 0, 1);
    glPushMatrix();
    glScalef(50, 450, 50);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(50, 210, 0);
    glPushMatrix();
    glRotatef(110, 0, 0, 1);
    glPushMatrix();
    glScalef(50, 250, 50);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(250, 210, 0);
    glPushMatrix();
    glRotatef(250, 0, 0, 1);
    glPushMatrix();
    glScalef(50, 250, 50);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The bottom
    glPushMatrix();
    glTranslatef(150, -140, 0);
    glPushMatrix();
    glRotatef(90, 0, 0, 1);
    glPushMatrix();
    glScalef(50, 450, 50);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}


void window1(void) {
    glPushMatrix();
    glTranslatef(700, 700, 0);

    glPushMatrix();
    glScalef(320, 50, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(700, 150, 0);
    glPushMatrix();
    glScalef(320, 50, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(870, 420, 0);
    glPushMatrix();
    glScalef(40, 600, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(530, 420, 0);
    glPushMatrix();
    glScalef(40, 600, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(700, 430, 0);
    glPushMatrix();
    glScalef(300, 500, 10);
    glBindTexture(GL_TEXTURE_2D, texture[11]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();
}


void window2(void) {
    glPushMatrix();
    glScalef(250, 50, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -450, 0);
    glPushMatrix();
    glScalef(250, 50, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(110, -200, 0);
    glPushMatrix();
    glScalef(30, 450, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-110, -200, 0);
    glPushMatrix();
    glScalef(30, 450, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -225, 0);
    glPushMatrix();
    glScalef(200, 420, 10);
    glBindTexture(GL_TEXTURE_2D, texture[11]);
    createCube();
    glPopMatrix();
    glPopMatrix();
}


void window3(void) {
    glPushMatrix();
    glScalef(150, 50, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -360, 0);
    glPushMatrix();
    glScalef(150, 50, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(70, -180, 0);
    glPushMatrix();
    glScalef(30, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-70, -180, 0);
    glPushMatrix();
    glScalef(30, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -180, 0);
    glPushMatrix();
    glScalef(120, 320, 10);
    glBindTexture(GL_TEXTURE_2D, texture[11]);
    createCube();
    glPopMatrix();
    glPopMatrix();
}


void drawHouseFence(void) {
    glPushMatrix();
    glTranslatef(0, -600, 0);

    // The front left bigFencePillar
    glPushMatrix();
    glTranslatef(400, -1500, -500);
    bigFencePillar();
    glPopMatrix();
    // The front right bigFencePillar
    glPushMatrix();
    glTranslatef(-400, -1500, -500);
    bigFencePillar();
    glPopMatrix();
    // The front left gate
    glPushMatrix();
    glTranslatef(320, -1500, -500);
    glPushMatrix();
    glRotatef(gate_rotation_angle, 0, 1, 0);
    glTranslatef(-140, 0, 0);
    gate();
    glPopMatrix();
    glPopMatrix();
    // The front right gate
    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glPushMatrix();
    glTranslatef(320, -1500, 500);
    glPushMatrix();
    glRotatef(-gate_rotation_angle, 0, 1, 0);
    glTranslatef(-140, 0, 0);
    gate();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    // The front left fenceWall1
    glPushMatrix();
    glTranslatef(550, -1530, -500);
    fenceWall();
    glPopMatrix();
    // The front left smallFencePillar1
    glPushMatrix();
    glTranslatef(990, -1500, -500);
    smallFencePillar();
    glPopMatrix();
    // The front left fenceWall2
    glPushMatrix();
    glTranslatef(1125, -1530, -500);
    fenceWall();
    glPopMatrix();
    // The front left smallFencePillar2
    glPushMatrix();
    glTranslatef(1560, -1500, -500);
    smallFencePillar();
    glPopMatrix();
    // The front left fenceWall3
    glPushMatrix();
    glTranslatef(1700, -1530, -500);
    fenceWall();
    glPopMatrix();
    // The front left smallFencePillar3
    glPushMatrix();
    glTranslatef(2130, -1500, -500);
    smallFencePillar();
    glPopMatrix();
    // The front left fenceWall4
    glPushMatrix();
    glTranslatef(2275, -1530, -500);
    fenceWall();
    glPopMatrix();
    // The front left smallFencePillar4
    glPushMatrix();
    glTranslatef(2700, -1500, -500);
    smallFencePillar();
    glPopMatrix();
    // The front right fenceWall1
    glPushMatrix();
    glTranslatef(-850, -1530, -500);
    fenceWall();
    glPopMatrix();
    // The front right smallFencePillar1
    glPushMatrix();
    glTranslatef(-990, -1500, -500);
    smallFencePillar();
    glPopMatrix();
    // The front right fenceWall2
    glPushMatrix();
    glTranslatef(-1425, -1530, -500);
    fenceWall();
    glPopMatrix();
    // The front right smallFencePillar2
    glPushMatrix();
    glTranslatef(-1560, -1500, -500);
    smallFencePillar();
    glPopMatrix();
    // The front right fenceWall3
    glPushMatrix();
    glTranslatef(-2000, -1530, -500);
    fenceWall();
    glPopMatrix();
    // The front right smallFencePillar3
    glPushMatrix();
    glTranslatef(-2130, -1500, -500);
    smallFencePillar();
    glPopMatrix();
    // The front right fenceWall4
    glPushMatrix();
    glTranslatef(-2575, -1530, -500);
    fenceWall();
    glPopMatrix();
    // The front right smallFencePillar4
    glPushMatrix();
    glTranslatef(-2700, -1500, -500);
    smallFencePillar();
    glPopMatrix();

    // The left side fenceWall1
    glPushMatrix();
    glTranslatef(2700, -1500, -70);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    fenceWall();
    glPopMatrix();
    glPopMatrix();
    // The left side smallFencePillar1
    glPushMatrix();
    glTranslatef(2700, -1500, 60);
    smallFencePillar();
    glPopMatrix();
    // The left side fenceWall2
    glPushMatrix();
    glTranslatef(2700, -1500, 500);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    fenceWall();
    glPopMatrix();
    glPopMatrix();
    // The left side smallFencePillar2
    glPushMatrix();
    glTranslatef(2700, -1500, 640);
    smallFencePillar();
    glPopMatrix();
    // The left side fenceWall3
    glPushMatrix();
    glTranslatef(2700, -1500, 1080);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    fenceWall();
    glPopMatrix();
    glPopMatrix();
    // The left side smallFencePillar3
    glPushMatrix();
    glTranslatef(2700, -1500, 1220);
    smallFencePillar();
    glPopMatrix();
    // The left side fenceWall4
    glPushMatrix();
    glTranslatef(2700, -1500, 1660);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    fenceWall();
    glPopMatrix();
    glPopMatrix();
    // The left side smallFencePillar4
    glPushMatrix();
    glTranslatef(2700, -1500, 1800);
    smallFencePillar();
    glPopMatrix();
    // The left side fenceWall5
    glPushMatrix();
    glTranslatef(2700, -1500, 2240);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    fenceWall();
    glPopMatrix();
    glPopMatrix();
    // The left side smallFencePillar5
    glPushMatrix();
    glTranslatef(2700, -1500, 2380);
    smallFencePillar();
    glPopMatrix();
    // The left side fenceWall6
    glPushMatrix();
    glTranslatef(2700, -1500, 2820);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    fenceWall();
    glPopMatrix();
    glPopMatrix();
    // The left side smallFencePillar6
    glPushMatrix();
    glTranslatef(2700, -1500, 2960);
    smallFencePillar();
    glPopMatrix();
    // The left side fenceWall7
    glPushMatrix();
    glTranslatef(2700, -1500, 3400);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    fenceWall();
    glPopMatrix();
    glPopMatrix();
    // The left side smallFencePillar7
    glPushMatrix();
    glTranslatef(2700, -1500, 3540);
    smallFencePillar();
    glPopMatrix();

    // The right side fenceWall1
    glPushMatrix();
    glTranslatef(-2700, -1500, -70);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    fenceWall();
    glPopMatrix();
    glPopMatrix();
    // The right side smallFencePillar1
    glPushMatrix();
    glTranslatef(-2700, -1500, 60);
    smallFencePillar();
    glPopMatrix();
    // The right side fenceWall2
    glPushMatrix();
    glTranslatef(-2700, -1500, 500);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    fenceWall();
    glPopMatrix();
    glPopMatrix();
    // The right side smallFencePillar2
    glPushMatrix();
    glTranslatef(-2700, -1500, 640);
    smallFencePillar();
    glPopMatrix();
    // The right side fenceWall3
    glPushMatrix();
    glTranslatef(-2700, -1500, 1080);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    fenceWall();
    glPopMatrix();
    glPopMatrix();
    // The right side smallFencePillar3
    glPushMatrix();
    glTranslatef(-2700, -1500, 1220);
    smallFencePillar();
    glPopMatrix();
    // The right side fenceWall4
    glPushMatrix();
    glTranslatef(-2700, -1500, 1660);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    fenceWall();
    glPopMatrix();
    glPopMatrix();
    // The right side smallFencePillar4
    glPushMatrix();
    glTranslatef(-2700, -1500, 1800);
    smallFencePillar();
    glPopMatrix();
    // The right side fenceWall5
    glPushMatrix();
    glTranslatef(-2700, -1500, 2240);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    fenceWall();
    glPopMatrix();
    glPopMatrix();
    // The right side smallFencePillar5
    glPushMatrix();
    glTranslatef(-2700, -1500, 2380);
    smallFencePillar();
    glPopMatrix();
    // The right side fenceWall6
    glPushMatrix();
    glTranslatef(-2700, -1500, 2820);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    fenceWall();
    glPopMatrix();
    glPopMatrix();
    // The right side smallFencePillar6
    glPushMatrix();
    glTranslatef(-2700, -1500, 2960);
    smallFencePillar();
    glPopMatrix();
    // The right side fenceWall7
    glPushMatrix();
    glTranslatef(-2700, -1500, 3400);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    fenceWall();
    glPopMatrix();
    glPopMatrix();
    // The right side smallFencePillar7
    glPushMatrix();
    glTranslatef(-2700, -1500, 3540);
    smallFencePillar();
    glPopMatrix();

    glPopMatrix();
}


void drawHouse1(void) {
    glEnable(GL_TEXTURE_2D);

    // The body1
    glPushMatrix();
    glTranslatef(10, -1500, 4000);
    glPushMatrix();
    glScalef(2500, 2000, 1300);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The front decoration1
    glPushMatrix();
    glTranslatef(10, -1500, 3300);

    glPushMatrix();
    glTranslatef(0, 850, 0);
    glPushMatrix();
    glScalef(2500, 100, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glScalef(2500, 100, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -950, 0);
    glPushMatrix();
    glScalef(2500, 100, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1240, 450, 0);
    glPushMatrix();
    glScalef(50, 890, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1240, -450, 0);
    glPushMatrix();
    glScalef(50, 890, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1270, 450, 0);
    glPushMatrix();
    glScalef(50, 900, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1270, -480, 0);
    glPushMatrix();
    glScalef(50, 950, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The front window1
    window1();

    // The front window2
    glPushMatrix();
    glTranslatef(-680, -1500, 3300);
    window1();
    glPopMatrix();

    // The front window3
    glPushMatrix();
    glTranslatef(-1400, -1500, 3300);
    window1();
    glPopMatrix();

    // The front window4
    glPushMatrix();
    glTranslatef(-350, -2400, 3300);
    window1();
    glPopMatrix();

    // The front door
    glPushMatrix();
    glTranslatef(-350, -2010, 3300);
    glPushMatrix();
    glScalef(400, 800, 0);
    glBindTexture(GL_TEXTURE_2D, texture[12]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();

    // The left decoration1
    glPushMatrix();
    glTranslatef(1260, -1550, 4000);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);

    glPushMatrix();
    glTranslatef(0, 900, 0);
    glPushMatrix();
    glScalef(1300, 100, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glScalef(1300, 100, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(650, 450, 0);
    glPushMatrix();
    glScalef(50, 1000, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(216, 450, 0);
    glPushMatrix();
    glScalef(50, 1000, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-218, 450, 0);
    glPushMatrix();
    glScalef(50, 1000, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-650, 450, 0);
    glPushMatrix();
    glScalef(50, 1000, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(650, -450, 0);
    glPushMatrix();
    glScalef(50, 1000, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -450, 0);
    glPushMatrix();
    glScalef(50, 1000, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-650, -450, 0);
    glPushMatrix();
    glScalef(50, 1000, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -900, 0);
    glPushMatrix();
    glScalef(1300, 100, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();

    // The right decoration1
    glPushMatrix();
    glTranslatef(-1260, -1550, 4000);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);

    glPushMatrix();
    glTranslatef(0, 900, 0);
    glPushMatrix();
    glScalef(1300, 100, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glScalef(1300, 100, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(650, 450, 0);
    glPushMatrix();
    glScalef(50, 1000, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(216, 450, 0);
    glPushMatrix();
    glScalef(50, 1000, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-218, 450, 0);
    glPushMatrix();
    glScalef(50, 1000, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-650, 450, 0);
    glPushMatrix();
    glScalef(50, 1000, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(650, -450, 0);
    glPushMatrix();
    glScalef(50, 1000, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -450, 0);
    glPushMatrix();
    glScalef(50, 1000, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-650, -450, 0);
    glPushMatrix();
    glScalef(50, 1000, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -900, 0);
    glPushMatrix();
    glScalef(1300, 100, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();

    // The top1
    glPushMatrix();
    glTranslatef(10, -500, 3900);
    glPushMatrix();
    glScalef(2900, 2500, 1300);
    glBindTexture(GL_TEXTURE_2D, texture[9]);
    createRectangularPyramid();
    glPopMatrix();
    glPopMatrix();

    // The body2
    glPushMatrix();
    glTranslatef(10, 300, 4000);
    glPushMatrix();
    glScalef(1900, 800, 1300);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The front decoration2
    glPushMatrix();
    glTranslatef(920, 300, 3300);
    glPushMatrix();
    glScalef(60, 800, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-920, 300, 3300);
    glPushMatrix();
    glScalef(60, 800, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 660, 3300);
    glPushMatrix();
    glScalef(1900, 70, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -100, 3300);
    glPushMatrix();
    glScalef(1900, 70, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The left decoration2
    glPushMatrix();
    glTranslatef(960, 300, 3280);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);

    glPushMatrix();
    glScalef(50, 800, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1300, 0, 0);
    glPushMatrix();
    glScalef(50, 800, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-650, 370, 0);
    glPushMatrix();
    glScalef(1350, 60, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-650, -370, 0);
    glPushMatrix();
    glScalef(1350, 60, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();

    // The right decoration2
    glPushMatrix();
    glTranslatef(-950, 300, 3300);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);

    glPushMatrix();
    glScalef(50, 800, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1300, 0, 0);
    glPushMatrix();
    glScalef(50, 800, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-650, 370, 0);
    glPushMatrix();
    glScalef(1350, 60, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-650, -370, 0);
    glPushMatrix();
    glScalef(1350, 60, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();

    // The front window5
    glPushMatrix();
    glTranslatef(-880, -150, 3300);
    window1();
    glPopMatrix();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-510, -150, 3300);
    window1();
    glPopMatrix();
    glPopMatrix();

    // The top2
    glPushMatrix();
    glTranslatef(0, 700, 3900);
    glPushMatrix();
    glScalef(2000, 2800, 1300);
    glBindTexture(GL_TEXTURE_2D, texture[9]);
    createRectangularPyramid();
    glPopMatrix();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}


void drawHouse2_1(void) {
    glEnable(GL_TEXTURE_2D);

    glPushMatrix();
    glTranslatef(2100, -1500, 4000);
    // The body
    glPushMatrix();
    glScalef(1000, 2000, 1300);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    createCube();
    glPopMatrix();
    // The top1
    glPushMatrix();
    glTranslatef(0, 1000, -150);
    glPushMatrix();
    glScalef(600, 1000, 600);
    createHouse2Pentahedron();
    glPopMatrix();
    glPopMatrix();
    // The top2
    glPushMatrix();
    glTranslatef(0, 1000, -150);
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(500, 800, 500);
    createHouse2Pentahedron();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The right decoration1
    glPushMatrix();
    glTranslatef(1595, -90, 3620);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glRotatef(30, 0, 0, 1);
    glPushMatrix();
    glScalef(60, 800, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1595, -90, 4040);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glRotatef(150, 0, 0, 1);
    glPushMatrix();
    glScalef(60, 800, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1595, -340, 3900);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(850, 60, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The right decoration2
    glPushMatrix();
    glTranslatef(1590, -1500, 3400);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(70, 2000, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1590, -1500, 4430);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(70, 2000, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1590, -1500, 4000);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(70, 2000, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1590, -1800, 3890);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(1000, 70, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1590, -1200, 3890);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(1000, 70, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1590, -560, 3890);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(1000, 70, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1590, -2460, 3890);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(1000, 70, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The front decoration1
    glPushMatrix();
    glTranslatef(2100, -2460, 3340);
    glPushMatrix();
    glScalef(1000, 70, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1800, 3340);
    glPushMatrix();
    glScalef(1000, 70, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1200, 3330);
    glPushMatrix();
    glScalef(1000, 70, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -550, 3330);
    glPushMatrix();
    glScalef(1000, 70, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2580, -1420, 3330);
    glPushMatrix();
    glScalef(40, 2000, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1590, -1430, 3330);
    glPushMatrix();
    glScalef(40, 2000, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2350, -1810, 3330);
    glPushMatrix();
    glScalef(40, 1300, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1810, 3330);
    glPushMatrix();
    glScalef(40, 1300, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1850, -1810, 3330);
    glPushMatrix();
    glScalef(40, 1300, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2270, -900, 3330);
    glPushMatrix();
    glScalef(40, 650, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1920, -900, 3330);
    glPushMatrix();
    glScalef(40, 650, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The front decoration2
    glPushMatrix();
    glTranslatef(2380, 0, 3230);
    glPushMatrix();
    glRotatef(31, 0, 0, 1);
    glPushMatrix();
    glScalef(40, 1130, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1810, 0, 3230);
    glPushMatrix();
    glRotatef(149, 0, 0, 1);
    glPushMatrix();
    glScalef(40, 1130, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -470, 3230);
    glPushMatrix();
    glScalef(1175, 60, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(2095, 100, 3230);
    glPushMatrix();
    glScalef(430, 60, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2500, -370, 3230);
    glPushMatrix();
    glRotatef(130, 0, 0, 1);
    glPushMatrix();
    glScalef(40, 200, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2420, -260, 3230);
    glPushMatrix();
    glScalef(40, 390, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2200, -170, 3230);
    glPushMatrix();
    glScalef(40, 550, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1990, -170, 3230);
    glPushMatrix();
    glScalef(40, 550, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1760, -260, 3230);
    glPushMatrix();
    glScalef(40, 390, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1680, -370, 3230);
    glPushMatrix();
    glRotatef(50, 0, 0, 1);
    glPushMatrix();
    glScalef(40, 200, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2060, 180, 3230);
    glPushMatrix();
    glRotatef(31, 0, 0, 1);
    glPushMatrix();
    glScalef(40, 250, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2150, 180, 3230);
    glPushMatrix();
    glRotatef(149, 0, 0, 1);
    glPushMatrix();
    glScalef(40, 250, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -650, 3330);
    window2();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2225, -1320, 3330);
    window3();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}


void drawHouse2_2(void) {
    glEnable(GL_TEXTURE_2D);

    glPushMatrix();
    glTranslatef(-2100, -1500, 4000);
    // The body
    glPushMatrix();
    glScalef(1000, 2000, 1300);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    createCube();
    glPopMatrix();
    // The top1
    glPushMatrix();
    glTranslatef(0, 1000, -150);
    glPushMatrix();
    glScalef(600, 1000, 600);
    createHouse2Pentahedron();
    glPopMatrix();
    glPopMatrix();
    // The top2
    glPushMatrix();
    glTranslatef(0, 1000, -150);
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(500, 800, 500);
    createHouse2Pentahedron();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The right decoration1
    glPushMatrix();
    glTranslatef(-1595, -90, 3620);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glRotatef(30, 0, 0, 1);
    glPushMatrix();
    glScalef(60, 800, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1595, -90, 4040);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glRotatef(150, 0, 0, 1);
    glPushMatrix();
    glScalef(60, 800, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1595, -340, 3900);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(850, 60, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The right decoration2
    glPushMatrix();
    glTranslatef(-1590, -1500, 3400);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(70, 2000, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1590, -1500, 4430);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(70, 2000, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1590, -1500, 4000);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(70, 2000, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1590, -1800, 3890);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(1000, 70, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1590, -1200, 3890);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(1000, 70, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1590, -560, 3890);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(1000, 70, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1590, -2460, 3890);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(1000, 70, 5);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The front decoration1
    glPushMatrix();
    glTranslatef(-2100, -2460, 3340);
    glPushMatrix();
    glScalef(1000, 70, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1800, 3340);
    glPushMatrix();
    glScalef(1000, 70, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1200, 3330);
    glPushMatrix();
    glScalef(1000, 70, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -550, 3330);
    glPushMatrix();
    glScalef(1000, 70, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2580, -1420, 3330);
    glPushMatrix();
    glScalef(40, 2000, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1590, -1430, 3330);
    glPushMatrix();
    glScalef(40, 2000, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2350, -1810, 3330);
    glPushMatrix();
    glScalef(40, 1300, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1810, 3330);
    glPushMatrix();
    glScalef(40, 1300, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1850, -1810, 3330);
    glPushMatrix();
    glScalef(40, 1300, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2270, -900, 3330);
    glPushMatrix();
    glScalef(40, 650, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1920, -900, 3330);
    glPushMatrix();
    glScalef(40, 650, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The front decoration2
    glPushMatrix();
    glTranslatef(-1800, 0, 3230);
    glPushMatrix();
    glRotatef(31, 0, 0, 1);
    glPushMatrix();
    glScalef(40, 1130, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2380, 0, 3230);
    glPushMatrix();
    glRotatef(149, 0, 0, 1);
    glPushMatrix();
    glScalef(40, 1130, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -470, 3230);
    glPushMatrix();
    glScalef(1175, 60, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2095, 100, 3230);
    glPushMatrix();
    glScalef(430, 60, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1690, -370, 3230);
    glPushMatrix();
    glRotatef(130, 0, 0, 1);
    glPushMatrix();
    glScalef(40, 200, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2420, -260, 3230);
    glPushMatrix();
    glScalef(40, 390, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2200, -170, 3230);
    glPushMatrix();
    glScalef(40, 550, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1990, -170, 3230);
    glPushMatrix();
    glScalef(40, 550, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1760, -260, 3230);
    glPushMatrix();
    glScalef(40, 390, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2500, -370, 3230);
    glPushMatrix();
    glRotatef(50, 0, 0, 1);
    glPushMatrix();
    glScalef(40, 200, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2160, 180, 3230);
    glPushMatrix();
    glRotatef(31, 0, 0, 1);
    glPushMatrix();
    glScalef(40, 250, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2050, 180, 3230);
    glPushMatrix();
    glRotatef(149, 0, 0, 1);
    glPushMatrix();
    glScalef(40, 250, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -650, 3330);
    window2();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2225, -1320, 3330);
    window3();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}


void drawHouse3_1(void) {
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(0, -600, -50);

    // The body1
    glPushMatrix();
    glTranslatef(2100, -1700, 600);
    glPushMatrix();
    glScalef(700, 400, 900);
    glBindTexture(GL_TEXTURE_2D, texture[13]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The body1 front decoration
    glPushMatrix();
    glTranslatef(2420, -1700, 150);
    glPushMatrix();
    glScalef(60, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2300, -1700, 150);
    glPushMatrix();
    glScalef(40, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1700, 150);
    glPushMatrix();
    glScalef(40, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1950, -1700, 150);
    glPushMatrix();
    glScalef(40, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1780, -1700, 150);
    glPushMatrix();
    glScalef(60, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1525, 150);
    glPushMatrix();
    glScalef(600, 40, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1680, 150);
    glPushMatrix();
    glScalef(600, 40, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1820, 150);
    glPushMatrix();
    glScalef(600, 40, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2350, -1600, 150);
    glPushMatrix();
    glRotatef(50, 0, 0, 1);
    glPushMatrix();
    glScalef(200, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2350, -1750, 150);
    glPushMatrix();
    glRotatef(130, 0, 0, 1);
    glPushMatrix();
    glScalef(200, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2030, -1600, 150);
    glPushMatrix();
    glRotatef(50, 0, 0, 1);
    glPushMatrix();
    glScalef(200, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1860, -1760, 150);
    glPushMatrix();
    glRotatef(40, 0, 0, 1);
    glPushMatrix();
    glScalef(170, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2030, -1750, 150);
    glPushMatrix();
    glRotatef(140, 0, 0, 1);
    glPushMatrix();
    glScalef(200, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1860, -1600, 150);
    glPushMatrix();
    glRotatef(140, 0, 0, 1);
    glPushMatrix();
    glScalef(170, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body1 right decoration
    glPushMatrix();
    glTranslatef(1745, -1700, 160);
    glPushMatrix();
    glScalef(10, 400, 60);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1745, -1700, 1030);
    glPushMatrix();
    glScalef(10, 400, 60);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1745, -1525, 600);
    glPushMatrix();
    glScalef(10, 50, 900);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1745, -1865, 600);
    glPushMatrix();
    glScalef(10, 50, 900);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The body1 right door
    glPushMatrix();
    glTranslatef(1730, -1710, 600);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(150, 300, 10);
    glBindTexture(GL_TEXTURE_2D, texture[12]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body2
    glPushMatrix();
    glTranslatef(2100, -1480, 600);
    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glPushMatrix();
    glScalef(750, 1000, 1000);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    createHexahedron2();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body2 front decoration
    glPushMatrix();
    glTranslatef(2463, -1230, 90);
    glPushMatrix();
    glScalef(25, 500, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1739, -1380, 90);
    glPushMatrix();
    glScalef(25, 200, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1455, 90);
    glPushMatrix();
    glScalef(700, 50, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1150, 90);
    glPushMatrix();
    glRotatef(22, 0, 0, 1);
    glPushMatrix();
    glScalef(780, 50, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2400, -1245, 90);
    glPushMatrix();
    glScalef(15, 410, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2200, -1300, 90);
    glPushMatrix();
    glScalef(15, 320, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2000, -1325, 90);
    glPushMatrix();
    glScalef(15, 240, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1330, 90);
    glPushMatrix();
    glScalef(700, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1270, 90);
    glPushMatrix();
    glRotatef(65, 0, 0, 1);
    glPushMatrix();
    glScalef(25, 220, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2300, -1230, 90);
    glPushMatrix();
    glRotatef(130, 0, 0, 1);
    glPushMatrix();
    glScalef(20, 260, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1715, -1380, 120);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1715, -1380, 350);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1715, -1380, 580);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1715, -1380, 810);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1715, -1380, 1060);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1710, -1310, 590);
    glPushMatrix();
    glScalef(10, 40, 950);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1710, -1440, 590);
    glPushMatrix();
    glScalef(10, 40, 950);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The body3
    glPushMatrix();
    glTranslatef(2100, -1200, 350);
    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glPushMatrix();
    glScalef(300, 600, 300);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    createHexahedron2();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body3 front decoration
    glPushMatrix();
    glTranslatef(2235, -1000, 200);
    glPushMatrix();
    glScalef(20, 190, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1960, -1130, 200);
    glPushMatrix();
    glScalef(20, 120, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1000, 200);
    glPushMatrix();
    glRotatef(30, 0, 0, 1);
    glPushMatrix();
    glScalef(310, 20, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1120, 200);
    glPushMatrix();
    glRotatef(23, 0, 0, 1);
    glPushMatrix();
    glScalef(310, 20, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1070, 200);
    glPushMatrix();
    glRotatef(190, 0, 0, 1);
    glPushMatrix();
    glScalef(270, 20, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body3 right decoration
    glPushMatrix();
    glTranslatef(1950, -1140, 210);
    glPushMatrix();
    glScalef(10, 120, 30);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1950, -1140, 490);
    glPushMatrix();
    glScalef(10, 120, 30);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1950, -1095, 340);
    glPushMatrix();
    glScalef(10, 30, 270);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1950, -1170, 340);
    glPushMatrix();
    glScalef(10, 30, 270);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The chimney
    glPushMatrix();
    glTranslatef(2300, -900, 800);
    glPushMatrix();
    glScalef(50, 250, 50);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}


void drawHouse3_2(void) {
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(0, -600, -50);

    // The body1
    glPushMatrix();
    glTranslatef(-2100, -1700, 600);
    glPushMatrix();
    glScalef(700, 400, 900);
    glBindTexture(GL_TEXTURE_2D, texture[13]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The body1 front decoration
    glPushMatrix();
    glTranslatef(-2420, -1700, 150);
    glPushMatrix();
    glScalef(60, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2300, -1700, 150);
    glPushMatrix();
    glScalef(40, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1700, 150);
    glPushMatrix();
    glScalef(40, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1950, -1700, 150);
    glPushMatrix();
    glScalef(40, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1780, -1700, 150);
    glPushMatrix();
    glScalef(60, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1525, 150);
    glPushMatrix();
    glScalef(600, 40, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1680, 150);
    glPushMatrix();
    glScalef(600, 40, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1820, 150);
    glPushMatrix();
    glScalef(600, 40, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2350, -1750, 150);
    glPushMatrix();
    glRotatef(50, 0, 0, 1);
    glPushMatrix();
    glScalef(200, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2350, -1600, 150);
    glPushMatrix();
    glRotatef(130, 0, 0, 1);
    glPushMatrix();
    glScalef(200, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2030, -1760, 150);
    glPushMatrix();
    glRotatef(50, 0, 0, 1);
    glPushMatrix();
    glScalef(200, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1860, -1600, 150);
    glPushMatrix();
    glRotatef(40, 0, 0, 1);
    glPushMatrix();
    glScalef(170, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1860, -1750, 150);
    glPushMatrix();
    glRotatef(140, 0, 0, 1);
    glPushMatrix();
    glScalef(200, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2030, -1600, 150);
    glPushMatrix();
    glRotatef(140, 0, 0, 1);
    glPushMatrix();
    glScalef(170, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body1 right decoration
    glPushMatrix();
    glTranslatef(-1745, -1700, 160);
    glPushMatrix();
    glScalef(10, 400, 60);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1745, -1700, 1030);
    glPushMatrix();
    glScalef(10, 400, 60);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1745, -1525, 600);
    glPushMatrix();
    glScalef(10, 50, 900);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1745, -1865, 600);
    glPushMatrix();
    glScalef(10, 50, 900);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The body1 right door
    glPushMatrix();
    glTranslatef(-1730, -1710, 600);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(150, 300, 10);
    glBindTexture(GL_TEXTURE_2D, texture[12]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body2
    glPushMatrix();
    glTranslatef(-2100, -1480, 600);
    glPushMatrix();
    glRotatef(0, 0, 1, 0);
    glPushMatrix();
    glScalef(750, 1000, 1000);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    createHexahedron2();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body2 front decoration
    glPushMatrix();
    glTranslatef(-2463, -1230, 90);
    glPushMatrix();
    glScalef(25, 500, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1739, -1380, 90);
    glPushMatrix();
    glScalef(25, 200, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1455, 90);
    glPushMatrix();
    glScalef(700, 50, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1150, 90);
    glPushMatrix();
    glRotatef(158, 0, 0, 1);
    glPushMatrix();
    glScalef(780, 50, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2400, -1245, 90);
    glPushMatrix();
    glScalef(15, 410, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2200, -1300, 90);
    glPushMatrix();
    glScalef(15, 320, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2000, -1325, 90);
    glPushMatrix();
    glScalef(15, 240, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1330, 90);
    glPushMatrix();
    glScalef(700, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1270, 90);
    glPushMatrix();
    glRotatef(115, 0, 0, 1);
    glPushMatrix();
    glScalef(25, 220, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2300, -1230, 90);
    glPushMatrix();
    glRotatef(50, 0, 0, 1);
    glPushMatrix();
    glScalef(20, 260, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1715, -1380, 120);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1715, -1380, 350);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1715, -1380, 580);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1715, -1380, 810);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1715, -1380, 1060);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1710, -1310, 590);
    glPushMatrix();
    glScalef(10, 40, 950);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1710, -1440, 590);
    glPushMatrix();
    glScalef(10, 40, 950);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The body3
    glPushMatrix();
    glTranslatef(-2100, -1200, 350);
    glPushMatrix();
    glRotatef(0, 0, 1, 0);
    glPushMatrix();
    glScalef(300, 600, 300);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    createHexahedron2();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body3 front decoration
    glPushMatrix();
    glTranslatef(-2235, -1000, 200);
    glPushMatrix();
    glScalef(20, 190, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1960, -1130, 200);
    glPushMatrix();
    glScalef(20, 120, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1000, 200);
    glPushMatrix();
    glRotatef(150, 0, 0, 1);
    glPushMatrix();
    glScalef(310, 20, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1120, 200);
    glPushMatrix();
    glRotatef(157, 0, 0, 1);
    glPushMatrix();
    glScalef(310, 20, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1070, 200);
    glPushMatrix();
    glRotatef(-10, 0, 0, 1);
    glPushMatrix();
    glScalef(270, 20, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body3 right decoration
    glPushMatrix();
    glTranslatef(-1950, -1140, 210);
    glPushMatrix();
    glScalef(10, 120, 30);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1950, -1140, 490);
    glPushMatrix();
    glScalef(10, 120, 30);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1950, -1095, 340);
    glPushMatrix();
    glScalef(10, 30, 270);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1950, -1170, 340);
    glPushMatrix();
    glScalef(10, 30, 270);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The chimney
    glPushMatrix();
    glTranslatef(-2300, -900, 800);
    glPushMatrix();
    glScalef(50, 250, 50);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}


void drawHouse3_3(void) {
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(0, -600, 1400);

    // The body1
    glPushMatrix();
    glTranslatef(2100, -1700, 600);
    glPushMatrix();
    glScalef(700, 400, 900);
    glBindTexture(GL_TEXTURE_2D, texture[13]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The body1 front decoration
    glPushMatrix();
    glTranslatef(2420, -1700, 150);
    glPushMatrix();
    glScalef(60, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2300, -1700, 150);
    glPushMatrix();
    glScalef(40, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1700, 150);
    glPushMatrix();
    glScalef(40, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1950, -1700, 150);
    glPushMatrix();
    glScalef(40, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1780, -1700, 150);
    glPushMatrix();
    glScalef(60, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1525, 150);
    glPushMatrix();
    glScalef(600, 40, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1680, 150);
    glPushMatrix();
    glScalef(600, 40, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1820, 150);
    glPushMatrix();
    glScalef(600, 40, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2350, -1600, 150);
    glPushMatrix();
    glRotatef(50, 0, 0, 1);
    glPushMatrix();
    glScalef(200, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2350, -1750, 150);
    glPushMatrix();
    glRotatef(130, 0, 0, 1);
    glPushMatrix();
    glScalef(200, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2030, -1600, 150);
    glPushMatrix();
    glRotatef(50, 0, 0, 1);
    glPushMatrix();
    glScalef(200, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1860, -1760, 150);
    glPushMatrix();
    glRotatef(40, 0, 0, 1);
    glPushMatrix();
    glScalef(170, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2030, -1750, 150);
    glPushMatrix();
    glRotatef(140, 0, 0, 1);
    glPushMatrix();
    glScalef(200, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1860, -1600, 150);
    glPushMatrix();
    glRotatef(140, 0, 0, 1);
    glPushMatrix();
    glScalef(170, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body1 right decoration
    glPushMatrix();
    glTranslatef(1745, -1700, 160);
    glPushMatrix();
    glScalef(10, 400, 60);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1745, -1700, 1030);
    glPushMatrix();
    glScalef(10, 400, 60);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1745, -1525, 600);
    glPushMatrix();
    glScalef(10, 50, 900);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1745, -1865, 600);
    glPushMatrix();
    glScalef(10, 50, 900);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The body1 right door
    glPushMatrix();
    glTranslatef(1730, -1710, 600);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(150, 300, 10);
    glBindTexture(GL_TEXTURE_2D, texture[12]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body2
    glPushMatrix();
    glTranslatef(2100, -1480, 600);
    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glPushMatrix();
    glScalef(750, 1000, 1000);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    createHexahedron2();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body2 front decoration
    glPushMatrix();
    glTranslatef(2463, -1230, 90);
    glPushMatrix();
    glScalef(25, 500, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1739, -1380, 90);
    glPushMatrix();
    glScalef(25, 200, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1455, 90);
    glPushMatrix();
    glScalef(700, 50, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1150, 90);
    glPushMatrix();
    glRotatef(22, 0, 0, 1);
    glPushMatrix();
    glScalef(780, 50, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2400, -1245, 90);
    glPushMatrix();
    glScalef(15, 410, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2200, -1300, 90);
    glPushMatrix();
    glScalef(15, 320, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2000, -1325, 90);
    glPushMatrix();
    glScalef(15, 240, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1330, 90);
    glPushMatrix();
    glScalef(700, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1270, 90);
    glPushMatrix();
    glRotatef(65, 0, 0, 1);
    glPushMatrix();
    glScalef(25, 220, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2300, -1230, 90);
    glPushMatrix();
    glRotatef(130, 0, 0, 1);
    glPushMatrix();
    glScalef(20, 260, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1715, -1380, 120);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1715, -1380, 350);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1715, -1380, 580);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1715, -1380, 810);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1715, -1380, 1060);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1710, -1310, 590);
    glPushMatrix();
    glScalef(10, 40, 950);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1710, -1440, 590);
    glPushMatrix();
    glScalef(10, 40, 950);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The body3
    glPushMatrix();
    glTranslatef(2100, -1200, 350);
    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glPushMatrix();
    glScalef(300, 600, 300);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    createHexahedron2();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body3 front decoration
    glPushMatrix();
    glTranslatef(2235, -1000, 200);
    glPushMatrix();
    glScalef(20, 190, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1960, -1130, 200);
    glPushMatrix();
    glScalef(20, 120, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1000, 200);
    glPushMatrix();
    glRotatef(30, 0, 0, 1);
    glPushMatrix();
    glScalef(310, 20, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1120, 200);
    glPushMatrix();
    glRotatef(23, 0, 0, 1);
    glPushMatrix();
    glScalef(310, 20, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2100, -1070, 200);
    glPushMatrix();
    glRotatef(190, 0, 0, 1);
    glPushMatrix();
    glScalef(270, 20, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body3 right decoration
    glPushMatrix();
    glTranslatef(1950, -1140, 210);
    glPushMatrix();
    glScalef(10, 120, 30);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1950, -1140, 490);
    glPushMatrix();
    glScalef(10, 120, 30);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1950, -1095, 340);
    glPushMatrix();
    glScalef(10, 30, 270);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1950, -1170, 340);
    glPushMatrix();
    glScalef(10, 30, 270);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The chimney
    glPushMatrix();
    glTranslatef(2300, -900, 800);
    glPushMatrix();
    glScalef(50, 250, 50);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}


void drawHouse3_4(void) {
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(0, -600, 1400);

    // The body1
    glPushMatrix();
    glTranslatef(-2100, -1700, 600);
    glPushMatrix();
    glScalef(700, 400, 900);
    glBindTexture(GL_TEXTURE_2D, texture[13]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The body1 front decoration
    glPushMatrix();
    glTranslatef(-2420, -1700, 150);
    glPushMatrix();
    glScalef(60, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2300, -1700, 150);
    glPushMatrix();
    glScalef(40, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1700, 150);
    glPushMatrix();
    glScalef(40, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1950, -1700, 150);
    glPushMatrix();
    glScalef(40, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1780, -1700, 150);
    glPushMatrix();
    glScalef(60, 400, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1525, 150);
    glPushMatrix();
    glScalef(600, 40, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1680, 150);
    glPushMatrix();
    glScalef(600, 40, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1820, 150);
    glPushMatrix();
    glScalef(600, 40, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2350, -1750, 150);
    glPushMatrix();
    glRotatef(50, 0, 0, 1);
    glPushMatrix();
    glScalef(200, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2350, -1600, 150);
    glPushMatrix();
    glRotatef(130, 0, 0, 1);
    glPushMatrix();
    glScalef(200, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2030, -1760, 150);
    glPushMatrix();
    glRotatef(50, 0, 0, 1);
    glPushMatrix();
    glScalef(200, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1860, -1600, 150);
    glPushMatrix();
    glRotatef(40, 0, 0, 1);
    glPushMatrix();
    glScalef(170, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1860, -1750, 150);
    glPushMatrix();
    glRotatef(140, 0, 0, 1);
    glPushMatrix();
    glScalef(200, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2030, -1600, 150);
    glPushMatrix();
    glRotatef(140, 0, 0, 1);
    glPushMatrix();
    glScalef(170, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body1 right decoration
    glPushMatrix();
    glTranslatef(-1745, -1700, 160);
    glPushMatrix();
    glScalef(10, 400, 60);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1745, -1700, 1030);
    glPushMatrix();
    glScalef(10, 400, 60);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1745, -1525, 600);
    glPushMatrix();
    glScalef(10, 50, 900);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1745, -1865, 600);
    glPushMatrix();
    glScalef(10, 50, 900);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The body1 right door
    glPushMatrix();
    glTranslatef(-1730, -1710, 600);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(150, 300, 10);
    glBindTexture(GL_TEXTURE_2D, texture[12]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body2
    glPushMatrix();
    glTranslatef(-2100, -1480, 600);
    glPushMatrix();
    glRotatef(0, 0, 1, 0);
    glPushMatrix();
    glScalef(750, 1000, 1000);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    createHexahedron2();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body2 front decoration
    glPushMatrix();
    glTranslatef(-2463, -1230, 90);
    glPushMatrix();
    glScalef(25, 500, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1739, -1380, 90);
    glPushMatrix();
    glScalef(25, 200, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1455, 90);
    glPushMatrix();
    glScalef(700, 50, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1150, 90);
    glPushMatrix();
    glRotatef(158, 0, 0, 1);
    glPushMatrix();
    glScalef(780, 50, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2400, -1245, 90);
    glPushMatrix();
    glScalef(15, 410, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2200, -1300, 90);
    glPushMatrix();
    glScalef(15, 320, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2000, -1325, 90);
    glPushMatrix();
    glScalef(15, 240, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1330, 90);
    glPushMatrix();
    glScalef(700, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1270, 90);
    glPushMatrix();
    glRotatef(115, 0, 0, 1);
    glPushMatrix();
    glScalef(25, 220, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2300, -1230, 90);
    glPushMatrix();
    glRotatef(50, 0, 0, 1);
    glPushMatrix();
    glScalef(20, 260, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1715, -1380, 120);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1715, -1380, 350);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1715, -1380, 580);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1715, -1380, 810);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1715, -1380, 1060);
    glPushMatrix();
    glScalef(20, 200, 40);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1710, -1310, 590);
    glPushMatrix();
    glScalef(10, 40, 950);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1710, -1440, 590);
    glPushMatrix();
    glScalef(10, 40, 950);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The body3
    glPushMatrix();
    glTranslatef(-2100, -1200, 350);
    glPushMatrix();
    glRotatef(0, 0, 1, 0);
    glPushMatrix();
    glScalef(300, 600, 300);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    createHexahedron2();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body3 front decoration
    glPushMatrix();
    glTranslatef(-2235, -1000, 200);
    glPushMatrix();
    glScalef(20, 190, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1960, -1130, 200);
    glPushMatrix();
    glScalef(20, 120, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1000, 200);
    glPushMatrix();
    glRotatef(150, 0, 0, 1);
    glPushMatrix();
    glScalef(310, 20, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1120, 200);
    glPushMatrix();
    glRotatef(157, 0, 0, 1);
    glPushMatrix();
    glScalef(310, 20, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2100, -1070, 200);
    glPushMatrix();
    glRotatef(-10, 0, 0, 1);
    glPushMatrix();
    glScalef(270, 20, 10);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The body3 right decoration
    glPushMatrix();
    glTranslatef(-1950, -1140, 210);
    glPushMatrix();
    glScalef(10, 120, 30);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1950, -1140, 490);
    glPushMatrix();
    glScalef(10, 120, 30);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1950, -1095, 340);
    glPushMatrix();
    glScalef(10, 30, 270);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1950, -1170, 340);
    glPushMatrix();
    glScalef(10, 30, 270);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // The chimney
    glPushMatrix();
    glTranslatef(-2300, -900, 800);
    glPushMatrix();
    glScalef(50, 250, 50);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}


void drawHouseTree(void) {
    // The left house tree
    for (int houseTreeDistance = 250; houseTreeDistance < 2500; houseTreeDistance += 750) {
        // The leaves1
        glPushMatrix();
        glTranslatef(1000, -1700, houseTreeDistance);
        glPushMatrix();
        glScalef(300, 500, 300);
        glColor3f(0.18, 0.76, 0.04);
        createRectangularPyramid();
        glPushMatrix();
        glScalef(301, 501, 301);
        glColor3f(0, 0, 0);
        createWireRectangularPyramid();
        glPopMatrix();
        glPopMatrix();
        glPopMatrix();
        // The leaves2
        glPushMatrix();
        glTranslatef(1000, -1700, houseTreeDistance);
        glPushMatrix();
        glTranslatef(0, -220, 0);
        glPushMatrix();
        glScalef(400, 600, 400);
        glColor3f(0.18, 0.76, 0.04);
        createRectangularPyramid();
        glPushMatrix();
        glScalef(401, 601, 401);
        glColor3f(0, 0, 0);
        createWireRectangularPyramid();
        glPopMatrix();
        glPopMatrix();
        glPopMatrix();
        glPopMatrix();
        // The leaves3
        glPushMatrix();
        glTranslatef(1000, -1700, houseTreeDistance);
        glPushMatrix();
        glTranslatef(0, -440, 0);
        glPushMatrix();
        glScalef(500, 700, 500);
        glColor3f(0.18, 0.76, 0.04);
        createRectangularPyramid();
        glPushMatrix();
        glScalef(501, 701, 501);
        glColor3f(0, 0, 0);
        createWireRectangularPyramid();
        glPopMatrix();
        glPopMatrix();
        glPopMatrix();
        glPopMatrix();
        // The wood
        glPushMatrix();
        glTranslatef(1000, -1700, houseTreeDistance);
        glPushMatrix();
        glTranslatef(0, -500, 0);
        glScalef(100, 500, 100);
        glColor3f(0.47, 0.25, 0.02);
        glutSolidCube(1);
        glColor3f(0, 0, 0);
        glutWireCube(1);
        glPopMatrix();
        glPopMatrix();
    }


    // The right house tree
    for (int houseTreeDistance = 250; houseTreeDistance < 2500; houseTreeDistance += 750) {
        // The leaves1
        glPushMatrix();
        glTranslatef(-1000, -1700, houseTreeDistance);
        glPushMatrix();
        glScalef(300, 500, 300);
        glColor3f(0.18, 0.76, 0.04);
        createRectangularPyramid();
        glPushMatrix();
        glScalef(301, 501, 301);
        glColor3f(0, 0, 0);
        createWireRectangularPyramid();
        glPopMatrix();
        glPopMatrix();
        glPopMatrix();
        // The leaves2
        glPushMatrix();
        glTranslatef(-1000, -1700, houseTreeDistance);
        glPushMatrix();
        glTranslatef(0, -220, 0);
        glPushMatrix();
        glScalef(400, 600, 400);
        glColor3f(0.18, 0.76, 0.04);
        createRectangularPyramid();
        glPushMatrix();
        glScalef(401, 601, 401);
        glColor3f(0, 0, 0);
        createWireRectangularPyramid();
        glPopMatrix();
        glPopMatrix();
        glPopMatrix();
        glPopMatrix();
        // The leaves3
        glPushMatrix();
        glTranslatef(-1000, -1700, houseTreeDistance);
        glPushMatrix();
        glTranslatef(0, -440, 0);
        glPushMatrix();
        glScalef(500, 700, 500);
        glColor3f(0.18, 0.76, 0.04);
        createRectangularPyramid();
        glPushMatrix();
        glScalef(501, 701, 501);
        glColor3f(0, 0, 0);
        createWireRectangularPyramid();
        glPopMatrix();
        glPopMatrix();
        glPopMatrix();
        glPopMatrix();
        // The wood
        glPushMatrix();
        glTranslatef(-1000, -1700, houseTreeDistance);
        glPushMatrix();
        glTranslatef(0, -500, 0);
        glScalef(100, 500, 100);
        glColor3f(0.47, 0.25, 0.02);
        glutSolidCube(1);
        glColor3f(0, 0, 0);
        glutWireCube(1);
        glPopMatrix();
        glPopMatrix();
    }
}


void drawRoad(void) {
    glEnable(GL_TEXTURE_2D);

    // The left road
    glPushMatrix();
    glTranslatef(3700, -2100, 7000);
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(20000, 5, 1500);
    glBindTexture(GL_TEXTURE_2D, texture[14]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The right road
    glPushMatrix();
    glTranslatef(-3700, -2100, 7000);
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(20000, 5, 1500);
    glBindTexture(GL_TEXTURE_2D, texture[14]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // The middle road1
    glPushMatrix();
    glTranslatef(0, -2100, -1400);
    glRotatef(180, 0, 1, 0);
    glPushMatrix();
    glScalef(6000, 5, 1000);
    glBindTexture(GL_TEXTURE_2D, texture[14]);
    createCube();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();


    glDisable(GL_TEXTURE_2D);
}


void drawRoadTree(void) {
    // The left road trees
    for (roadTreeDistance = -2400; roadTreeDistance < 4200; roadTreeDistance += 800) {
        glPushMatrix();
        glTranslatef(4500, -2100, roadTreeDistance);
        glPushMatrix();
        glScalef(6, 6, 6);

        // The tree leaves
        glPushMatrix();
        glTranslatef(0, 100, 0);
        glScalef(60, 65, 60);
        glColor3f(0.18, 0.76, 0.04);
        glutSolidIcosahedron();
        glColor3f(0, 0, 0);
        glutWireIcosahedron();
        glPopMatrix();
        // The tree wood
        glPushMatrix();
        glTranslatef(0, 25, 0);
        glScalef(15, 50, 15);
        glColor3f(0.47, 0.25, 0.02);
        glutSolidCube(1);
        glColor3f(0, 0, 0);
        glutWireCube(1);
        glPopMatrix();
        glPopMatrix();

        glPopMatrix();
        glPopMatrix();
    }

    // The right road trees
    for (roadTreeDistance = -2400; roadTreeDistance < 4200; roadTreeDistance += 800) {
        glPushMatrix();
        glTranslatef(-4500, -2100, roadTreeDistance);
        glPushMatrix();
        glScalef(6, 6, 6);

        // The tree leaves
        glPushMatrix();
        glTranslatef(0, 100, 0);
        glScalef(60, 65, 60);
        glColor3f(0.18, 0.76, 0.04);
        glutSolidIcosahedron();
        glColor3f(0, 0, 0);
        glutWireIcosahedron();
        glPopMatrix();
        // The tree wood
        glPushMatrix();
        glTranslatef(0, 25, 0);
        glScalef(15, 50, 15);
        glColor3f(0.47, 0.25, 0.02);
        glutSolidCube(1);
        glColor3f(0, 0, 0);
        glutWireCube(1);
        glPopMatrix();
        glPopMatrix();

        glPopMatrix();
        glPopMatrix();
    }
}


void drawCar1(void) {
    glPushMatrix();
    glTranslatef(car1_x_position, -1800, car1_z_position);
    glPushMatrix();
    glRotatef(car1_rotate_angle, 0, 1, 0);

    glEnable(GL_TEXTURE_2D);

    // Car top
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(200, 400, 200);
    createCarTop();
    glPopMatrix();
    glPopMatrix();

    // Car body
    glPushMatrix();
    glTranslatef(0, -100, 0);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(200, 100, 300);
    glBindTexture(GL_TEXTURE_2D, texture[16]);
    createCarBody();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // front decoration1
    glPushMatrix();
    glTranslatef(285, -100, -205);
    glPushMatrix();
    glScalef(30, 170, 10);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-285, -100, -205);
    glPushMatrix();
    glScalef(30, 170, 10);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -30, -205);
    glPushMatrix();
    glScalef(538, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -170, -205);
    glPushMatrix();
    glScalef(538, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // back decoration1
    glPushMatrix();
    glTranslatef(285, -100, 205);
    glPushMatrix();
    glScalef(30, 170, 10);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-285, -100, 205);
    glPushMatrix();
    glScalef(30, 170, 10);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -30, 205);
    glPushMatrix();
    glScalef(538, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -170, 205);
    glPushMatrix();
    glScalef(538, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // left decoration1
    glPushMatrix();
    glTranslatef(-305, -100, -190);
    glPushMatrix();
    glScalef(10, 170, 30);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-305, -100, 190);
    glPushMatrix();
    glScalef(10, 170, 30);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-305, -40, 0);
    glPushMatrix();
    glScalef(10, 30, 340);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-305, -170, 0);
    glPushMatrix();
    glScalef(10, 30, 340);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-310, -100, 100);
    glPushMatrix();
    glScalef(10, 50, 50);
    glBindTexture(GL_TEXTURE_2D, texture[18]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-310, -100, -100);
    glPushMatrix();
    glScalef(10, 50, 50);
    glBindTexture(GL_TEXTURE_2D, texture[18]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // right decoration1
    glPushMatrix();
    glTranslatef(305, -100, -190);
    glPushMatrix();
    glScalef(10, 170, 30);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(305, -100, 190);
    glPushMatrix();
    glScalef(10, 170, 30);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(305, -40, 0);
    glPushMatrix();
    glScalef(10, 30, 340);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(305, -170, 0);
    glPushMatrix();
    glScalef(10, 30, 340);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(310, -100, 0);
    glPushMatrix();
    glScalef(10, 50, 200);
    glBindTexture(GL_TEXTURE_2D, texture[18]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // front tyres
    glPushMatrix();
    glTranslatef(150, -200, -220);
    glScalef(7, 7, 4);
    glColor3f(0, 0, 0);
    glutSolidCylinder(10, 10, 100, 10);
    glColor3f(0, 0, 0);
    glutWireCylinder(1, 1, 100, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-150, -200, -220);
    glScalef(7, 7, 4);
    glColor3f(0, 0, 0);
    glutSolidCylinder(10, 10, 100, 10);
    glColor3f(0, 0, 0);
    glutWireCylinder(1, 1, 100, 10);
    glPopMatrix();

    // back tyres
    glPushMatrix();
    glTranslatef(150, -200, 180);
    glScalef(7, 7, 4);
    glColor3f(0, 0, 0);
    glutSolidCylinder(10, 10, 100, 10);
    glColor3f(0, 0, 0);
    glutWireCylinder(1, 1, 100, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-150, -200, 180);
    glScalef(7, 7, 4);
    glColor3f(0, 0, 0);
    glutSolidCylinder(10, 10, 100, 10);
    glColor3f(0, 0, 0);
    glutWireCylinder(1, 1, 100, 10);
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();
}


void drawCar2(void) {
    glPushMatrix();
    glTranslatef(car2_x_position, -1800, car2_z_position);
    glPushMatrix();
    glRotatef(car2_rotate_angle, 0, 1, 0);

    glEnable(GL_TEXTURE_2D);

    // Car top
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(200, 400, 200);
    createCarTop();
    glPopMatrix();
    glPopMatrix();

    // Car body
    glPushMatrix();
    glTranslatef(0, -100, 0);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glScalef(200, 100, 300);
    glBindTexture(GL_TEXTURE_2D, texture[16]);
    createCarBody();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // front decoration1
    glPushMatrix();
    glTranslatef(285, -100, -205);
    glPushMatrix();
    glScalef(30, 170, 10);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-285, -100, -205);
    glPushMatrix();
    glScalef(30, 170, 10);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -30, -205);
    glPushMatrix();
    glScalef(538, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -170, -205);
    glPushMatrix();
    glScalef(538, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // back decoration1
    glPushMatrix();
    glTranslatef(285, -100, 205);
    glPushMatrix();
    glScalef(30, 170, 10);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-285, -100, 205);
    glPushMatrix();
    glScalef(30, 170, 10);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -30, 205);
    glPushMatrix();
    glScalef(538, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -170, 205);
    glPushMatrix();
    glScalef(538, 30, 10);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // left decoration1
    glPushMatrix();
    glTranslatef(-305, -100, -190);
    glPushMatrix();
    glScalef(10, 170, 30);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-305, -100, 190);
    glPushMatrix();
    glScalef(10, 170, 30);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-305, -40, 0);
    glPushMatrix();
    glScalef(10, 30, 340);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-305, -170, 0);
    glPushMatrix();
    glScalef(10, 30, 340);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-310, -100, 100);
    glPushMatrix();
    glScalef(10, 50, 50);
    glBindTexture(GL_TEXTURE_2D, texture[18]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-310, -100, -100);
    glPushMatrix();
    glScalef(10, 50, 50);
    glBindTexture(GL_TEXTURE_2D, texture[18]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    // right decoration1
    glPushMatrix();
    glTranslatef(305, -100, -190);
    glPushMatrix();
    glScalef(10, 170, 30);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(305, -100, 190);
    glPushMatrix();
    glScalef(10, 170, 30);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(305, -40, 0);
    glPushMatrix();
    glScalef(10, 30, 340);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(305, -170, 0);
    glPushMatrix();
    glScalef(10, 30, 340);
    glBindTexture(GL_TEXTURE_2D, texture[17]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(310, -100, 0);
    glPushMatrix();
    glScalef(10, 50, 200);
    glBindTexture(GL_TEXTURE_2D, texture[18]);
    createCube();
    glPopMatrix();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // front tyres
    glPushMatrix();
    glTranslatef(150, -200, -220);
    glScalef(7, 7, 4);
    glColor3f(0, 0, 0);
    glutSolidCylinder(10, 10, 100, 10);
    glColor3f(0, 0, 0);
    glutWireCylinder(1, 1, 100, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-150, -200, -220);
    glScalef(7, 7, 4);
    glColor3f(0, 0, 0);
    glutSolidCylinder(10, 10, 100, 10);
    glColor3f(0, 0, 0);
    glutWireCylinder(1, 1, 100, 10);
    glPopMatrix();

    // back tyres
    glPushMatrix();
    glTranslatef(150, -200, 180);
    glScalef(7, 7, 4);
    glColor3f(0, 0, 0);
    glutSolidCylinder(10, 10, 100, 10);
    glColor3f(0, 0, 0);
    glutWireCylinder(1, 1, 100, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-150, -200, 180);
    glScalef(7, 7, 4);
    glColor3f(0, 0, 0);
    glutSolidCylinder(10, 10, 100, 10);
    glColor3f(0, 0, 0);
    glutWireCylinder(1, 1, 100, 10);
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();
}


void keyboard_operation(unsigned char key, int x, int y) {
    // Zoom forward
    if (key == 'w' || key == 'W') {
        if (fltZRef < 100 && fltZ0 < -500) {
            fltZRef += 10;
            fltZ0 += 10;
        }
    }

    // Zoom back
    if (key == 's' || key == 'S') {
        if (fltZRef > -40 && fltZ0 > -750) {
            fltZRef += -10;
            fltZ0 += -10;
        }
    }

    // Rotate left
    if (key == 'a' || key == 'A') {
        fltX0 = -500;
    }

    // Rotate right
    if (key == 'd' || key == 'D') {
        fltX0 = 500;
    }

    // Zoom left
    if (key == 'q' || key == 'Q') {
        if (fltXRef < 80 && fltX0 < 80) {
            fltXRef += 10;
            fltX0 += 10;
        }
    }

    // Zoom right
    if (key == 'e' || key == 'E') {
        if (fltXRef > -70 && fltX0 > -70) {
            fltXRef += -10;
            fltX0 += -10;
        }
    }

    // Camera up
    if (key == 'm' || key == 'M') {
        if (fltY0 <= 90) {
            fltY0 += 5;
        }
    }

    // Camera down
    if (key == 'n' || key == 'N') {
        if (fltY0 >= 60) {
            fltY0 -= 5;
        }
    }

    // Restore
    if (key == 'r' || key == 'R') {
        fltZoom = 4.0;
        fltX0 = 0.0;
        fltY0 = 70.0;
        fltZ0 = -700.0;
        fltXRef = 0.0;
        fltYRef = 0.0;
        fltZRef = 0.0;
        fltXUp = 0.0;
        fltYUp = 1.0;
        fltZUp = 0.0;
    }

    // Exit
    if (key == 't' || key == 'T') {
        exit(0);
    }

    // Car move
    if (key == 'k' || key == 'K') {
        OnTimer_car1(1);
        OnTimer_car2(1);
    }
}


void mouse_input(int button, int state, int x, int y) {
    OnTimer_gate(1);
    if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
        gate_rotation_step = 1;
    }

    if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON) {
        gate_rotation_step = -1;
    }
}


void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gluLookAt(fltX0 * fltZoom, fltY0 * fltZoom, fltZ0 * fltZoom, fltXRef * fltZoom, fltYRef * fltZoom,
        fltZRef * fltZoom, fltXUp, fltYUp, fltZUp);
    glEnable(GL_DEPTH_TEST);
    drawSkybox();
    drawHouseFence();
    drawHouse1();
    drawHouse2_1();
    drawHouse2_2();
    drawHouse3_1();
    drawHouse3_2();
    drawHouse3_3();
    drawHouse3_4();
    drawHouseTree();
    drawRoad();
    drawRoadTree();
    drawCar1();
    drawCar2();
    glLoadIdentity();
    glFlush();
}


void reshape(GLsizei w, GLsizei h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(100, 1, 0.1, 10000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(10, 10);
    glutInitWindowSize(intWinWidth, intWinHeight);
    if (!glutCreateWindow("The 3-dimensional scene"))
        exit(0);
    textureInit();
    lightInit();
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(when_in_mainloop);
    glutKeyboardFunc(keyboard_operation);
    glutMouseFunc(mouse_input);
    glutMainLoop();
    return 0;
}