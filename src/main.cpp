#include <GL/glut.h>
#include "Config.h"

void init() {
    glClearColor(0.08f, 0.09f, 0.13f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, Config::WINDOW_WIDTH, 0.0, Config::WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawBoundaries() {
    glColor3f(0.35f, 0.45f, 0.65f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(Config::FIELD_LEFT, Config::FIELD_BOTTOM);
        glVertex2f(Config::FIELD_RIGHT, Config::FIELD_BOTTOM);
        glVertex2f(Config::FIELD_RIGHT, Config::FIELD_TOP);
        glVertex2f(Config::FIELD_LEFT, Config::FIELD_TOP);
    glEnd();

    // top bar separator
    glColor3f(0.2f, 0.25f, 0.35f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
        glVertex2f(0.0f, Config::FIELD_TOP + 4.0f);
        glVertex2f(Config::WINDOW_WIDTH, Config::FIELD_TOP + 4.0f);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawBoundaries();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, Config::WINDOW_WIDTH, 0.0, Config::WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void timer(int val) {
    glutPostRedisplay();
    glutTimerFunc(Config::TIMER_MS, timer, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("2D Brick Breaker - CSE 402");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(Config::TIMER_MS, timer, 0);

    glutMainLoop();
    return 0;
}
