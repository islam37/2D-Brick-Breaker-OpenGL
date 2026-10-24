#include <GL/glut.h>
#include <iostream>
#include "Config.h"

// ============================================================================
// Metropolitan University, Sylhet
// Department of Computer Science and Engineering
// Course: CSE 402 - Computer Graphics & Image Processing Lab
// Project: 2D Brick Breaker Game Using OpenGL
//
// Milestone 1: OpenGL Initialization, 2D Orthographic Projection & Window Setup
// ============================================================================

void initOpenGL() {
    // Background color: Deep dark aesthetic
    glClearColor(Config::COLOR_BG.r, Config::COLOR_BG.g, Config::COLOR_BG.b, 1.0f);

    // Setup 2D Orthographic Projection matching window pixels
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, Config::WINDOW_WIDTH, 0.0, Config::WINDOW_HEIGHT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Enable smooth line rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

void drawFieldBoundaries() {
    // Outer border wall
    glLineWidth(3.0f);
    glColor3f(Config::COLOR_WALL_ACCENT.r, Config::COLOR_WALL_ACCENT.g, Config::COLOR_WALL_ACCENT.b);
    glBegin(GL_LINE_LOOP);
        glVertex2f(Config::FIELD_LEFT, Config::FIELD_BOTTOM);
        glVertex2f(Config::FIELD_RIGHT, Config::FIELD_BOTTOM);
        glVertex2f(Config::FIELD_RIGHT, Config::FIELD_TOP);
        glVertex2f(Config::FIELD_LEFT, Config::FIELD_TOP);
    glEnd();

    // Subtle header separator line
    glLineWidth(1.5f);
    glColor3f(Config::COLOR_WALL.r, Config::COLOR_WALL.g, Config::COLOR_WALL.b);
    glBegin(GL_LINES);
        glVertex2f(0.0f, Config::FIELD_TOP + 5.0f);
        glVertex2f(Config::WINDOW_WIDTH, Config::FIELD_TOP + 5.0f);
    glEnd();
}

void displayCallback() {
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Draw field boundary outline
    drawFieldBoundaries();

    glutSwapBuffers();
}

void reshapeCallback(int w, int h) {
    // Prevent division by zero
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, Config::WINDOW_WIDTH, 0.0, Config::WINDOW_HEIGHT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void timerCallback(int value) {
    glutPostRedisplay();
    glutTimerFunc(Config::TIMER_INTERVAL_MS, timerCallback, 0);
}

int main(int argc, char** argv) {
    std::cout << "========================================================\n";
    std::cout << "  2D Brick Breaker Game - Metropolitan University\n";
    std::cout << "  CSE 402: Computer Graphics & Image Processing Lab\n";
    std::cout << "  Students: Islam Uddin & Yousuf Bin Nazib\n";
    std::cout << "  Lecturer: Shrabanti Chowdhury\n";
    std::cout << "========================================================\n";

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(Config::WINDOW_TITLE);

    initOpenGL();

    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutTimerFunc(Config::TIMER_INTERVAL_MS, timerCallback, 0);

    // Start GLUT main loop
    glutMainLoop();
    return 0;
}
