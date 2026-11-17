#include <GL/glut.h>
#include <cmath>
#include "Config.h"

struct Brick {
    float x, y;
    bool active;
};

// Scene state for Phase 1
float paddleX = (Config::WINDOW_WIDTH - Config::PADDLE_WIDTH) / 2.0f;
float ballX = Config::WINDOW_WIDTH / 2.0f;
float ballY = Config::PADDLE_Y + Config::PADDLE_HEIGHT + Config::BALL_RADIUS + 1.0f;

bool moveLeft = false;
bool moveRight = false;
int lastTimeMs = 0;

Brick bricks[Config::BRICK_ROWS][Config::BRICK_COLS];

// Row colors for bricks (R, G, B)
const float ROW_COLORS[Config::BRICK_ROWS][3] = {
    {0.92f, 0.34f, 0.34f}, // Row 0: Red
    {0.95f, 0.60f, 0.24f}, // Row 1: Orange
    {0.95f, 0.82f, 0.28f}, // Row 2: Yellow
    {0.34f, 0.78f, 0.47f}, // Row 3: Green
    {0.33f, 0.63f, 0.92f}  // Row 4: Blue
};

void initBricks() {
    for (int r = 0; r < Config::BRICK_ROWS; ++r) {
        for (int c = 0; c < Config::BRICK_COLS; ++c) {
            bricks[r][c].x = Config::BRICK_START_X + c * (Config::BRICK_WIDTH + Config::BRICK_PADDING_X);
            bricks[r][c].y = Config::BRICK_START_Y - r * (Config::BRICK_HEIGHT + Config::BRICK_PADDING_Y);
            bricks[r][c].active = true;
        }
    }
}

void init() {
    glClearColor(0.08f, 0.09f, 0.13f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, Config::WINDOW_WIDTH, 0.0, Config::WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    initBricks();
    lastTimeMs = glutGet(GLUT_ELAPSED_TIME);
}

// Helper for 8-way symmetric points in Bresenham's circle algorithm
void plotCirclePoints(int xc, int yc, int x, int y) {
    glVertex2i(xc + x, yc + y);
    glVertex2i(xc - x, yc + y);
    glVertex2i(xc + x, yc - y);
    glVertex2i(xc - x, yc - y);
    glVertex2i(xc + y, yc + x);
    glVertex2i(xc - y, yc + x);
    glVertex2i(xc + y, yc - x);
    glVertex2i(xc - y, yc - x);
}

// Reusable Bresenham circle-drawing routine for the ball outline
void drawBresenhamCircle(int xc, int yc, int r) {
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

void drawBoundaries() {
    glColor3f(0.35f, 0.45f, 0.65f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(Config::FIELD_LEFT, Config::FIELD_BOTTOM);
        glVertex2f(Config::FIELD_RIGHT, Config::FIELD_BOTTOM);
        glVertex2f(Config::FIELD_RIGHT, Config::FIELD_TOP);
        glVertex2f(Config::FIELD_LEFT, Config::FIELD_TOP);
    glEnd();

    // Top bar separator
    glColor3f(0.2f, 0.25f, 0.35f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
        glVertex2f(0.0f, Config::FIELD_TOP + 4.0f);
        glVertex2f(Config::WINDOW_WIDTH, Config::FIELD_TOP + 4.0f);
    glEnd();
}

void drawPaddle() {
    // Paddle fill
    glColor3f(0.25f, 0.78f, 0.95f);
    glBegin(GL_QUADS);
        glVertex2f(paddleX, Config::PADDLE_Y);
        glVertex2f(paddleX + Config::PADDLE_WIDTH, Config::PADDLE_Y);
        glVertex2f(paddleX + Config::PADDLE_WIDTH, Config::PADDLE_Y + Config::PADDLE_HEIGHT);
        glVertex2f(paddleX, Config::PADDLE_Y + Config::PADDLE_HEIGHT);
    glEnd();

    // Paddle outline
    glColor3f(0.75f, 0.93f, 1.0f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(paddleX, Config::PADDLE_Y);
        glVertex2f(paddleX + Config::PADDLE_WIDTH, Config::PADDLE_Y);
        glVertex2f(paddleX + Config::PADDLE_WIDTH, Config::PADDLE_Y + Config::PADDLE_HEIGHT);
        glVertex2f(paddleX, Config::PADDLE_Y + Config::PADDLE_HEIGHT);
    glEnd();
}

void drawBricks() {
    for (int r = 0; r < Config::BRICK_ROWS; ++r) {
        for (int c = 0; c < Config::BRICK_COLS; ++c) {
            if (!bricks[r][c].active) continue;

            float bx = bricks[r][c].x;
            float by = bricks[r][c].y;

            // Brick rectangle fill
            glColor3fv(ROW_COLORS[r]);
            glBegin(GL_QUADS);
                glVertex2f(bx, by);
                glVertex2f(bx + Config::BRICK_WIDTH, by);
                glVertex2f(bx + Config::BRICK_WIDTH, by + Config::BRICK_HEIGHT);
                glVertex2f(bx, by + Config::BRICK_HEIGHT);
            glEnd();

            // Brick border
            glColor3f(0.12f, 0.14f, 0.20f);
            glLineWidth(1.0f);
            glBegin(GL_LINE_LOOP);
                glVertex2f(bx, by);
                glVertex2f(bx + Config::BRICK_WIDTH, by);
                glVertex2f(bx + Config::BRICK_WIDTH, by + Config::BRICK_HEIGHT);
                glVertex2f(bx, by + Config::BRICK_HEIGHT);
            glEnd();
        }
    }
}

void drawBall() {
    int cx = static_cast<int>(std::round(ballX));
    int cy = static_cast<int>(std::round(ballY));

    // Inner circle fill using concentric Bresenham circles
    glPointSize(2.0f);
    glColor3f(0.92f, 0.95f, 1.0f);
    for (int r = 1; r < Config::BALL_RADIUS; ++r) {
        drawBresenhamCircle(cx, cy, r);
    }

    // Crisp outer ball outline using Bresenham's circle algorithm
    glColor3f(1.0f, 1.0f, 1.0f);
    drawBresenhamCircle(cx, cy, Config::BALL_RADIUS);
    glPointSize(1.0f);
}

void update(float dt) {
    if (moveLeft) {
        paddleX -= Config::PADDLE_SPEED * dt;
    }
    if (moveRight) {
        paddleX += Config::PADDLE_SPEED * dt;
    }

    // Clamp paddle inside playfield boundaries
    if (paddleX < Config::FIELD_LEFT) {
        paddleX = Config::FIELD_LEFT;
    }
    if (paddleX + Config::PADDLE_WIDTH > Config::FIELD_RIGHT) {
        paddleX = Config::FIELD_RIGHT - Config::PADDLE_WIDTH;
    }

    // Keep ball centered on paddle in Phase 1 (ready state)
    ballX = paddleX + Config::PADDLE_WIDTH / 2.0f;
    ballY = Config::PADDLE_Y + Config::PADDLE_HEIGHT + Config::BALL_RADIUS + 1.0f;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawBoundaries();
    drawBricks();
    drawPaddle();
    drawBall();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;

    // Preserve 4:3 scene aspect ratio during window resizing
    float targetAspect = static_cast<float>(Config::WINDOW_WIDTH) / Config::WINDOW_HEIGHT;
    float windowAspect = static_cast<float>(w) / h;

    int vpX = 0, vpY = 0, vpW = w, vpH = h;
    if (windowAspect > targetAspect) {
        vpW = static_cast<int>(h * targetAspect);
        vpX = (w - vpW) / 2;
    } else {
        vpH = static_cast<int>(w / targetAspect);
        vpY = (h - vpH) / 2;
    }

    glViewport(vpX, vpY, vpW, vpH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, Config::WINDOW_WIDTH, 0.0, Config::WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void timer(int val) {
    int nowMs = glutGet(GLUT_ELAPSED_TIME);
    float dt = (nowMs - lastTimeMs) / 1000.0f;
    lastTimeMs = nowMs;
    if (dt > 0.05f) dt = 0.05f;

    update(dt);
    glutPostRedisplay();
    glutTimerFunc(Config::TIMER_MS, timer, 0);
}

void specialDown(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT) moveLeft = true;
    if (key == GLUT_KEY_RIGHT) moveRight = true;
}

void specialUp(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT) moveLeft = false;
    if (key == GLUT_KEY_RIGHT) moveRight = false;
}

void keyboardDown(unsigned char key, int x, int y) {
    if (key == 'a' || key == 'A') moveLeft = true;
    if (key == 'd' || key == 'D') moveRight = true;
    if (key == 27) exit(0); // ESC to quit
}

void keyboardUp(unsigned char key, int x, int y) {
    if (key == 'a' || key == 'A') moveLeft = false;
    if (key == 'd' || key == 'D') moveRight = false;
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
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutTimerFunc(Config::TIMER_MS, timer, 0);

    glutMainLoop();
    return 0;
}
