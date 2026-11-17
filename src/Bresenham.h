#ifndef BRESENHAM_H
#define BRESENHAM_H

#include <GL/glut.h>

// ============================================================================
// Bresenham.h - Reusable Bresenham Circle Drawing Algorithm
// Used to render the ball outline, inner core, motion trail, and life icons.
// ============================================================================

// Plots all 8 symmetric octant points around center (xc, yc)
inline void plotCirclePoints(int xc, int yc, int x, int y) {
    glVertex2i(xc + x, yc + y);
    glVertex2i(xc - x, yc + y);
    glVertex2i(xc + x, yc - y);
    glVertex2i(xc - x, yc - y);
    glVertex2i(xc + y, yc + x);
    glVertex2i(xc - y, yc + x);
    glVertex2i(xc + y, yc - x);
    glVertex2i(xc - y, yc - x);
}

// Draws a circle of radius r centered at (xc, yc) using integer decision parameter d = 3 - 2r
inline void drawBresenhamCircle(int xc, int yc, int r) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    glBegin(GL_POINTS);
    while (x <= y) {
        plotCirclePoints(xc, yc, x, y);
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
    glEnd();
}

#endif
