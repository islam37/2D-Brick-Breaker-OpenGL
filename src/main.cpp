#include <GL/glut.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include "Config.h"

enum GameState {
    STATE_READY,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_WON,
    STATE_GAME_OVER
};

struct Brick {
    float x, y;
    bool active;
};

// Game state variables
GameState gameState = STATE_READY;
GameState stateBeforePause = STATE_READY;
int score = 0;
int lives = Config::INITIAL_LIVES;
int remainingBricks = Config::BRICK_ROWS * Config::BRICK_COLS;

// Paddle and ball state
float paddleX = (Config::WINDOW_WIDTH - Config::PADDLE_WIDTH) / 2.0f;
float ballX = Config::WINDOW_WIDTH / 2.0f;
float ballY = Config::PADDLE_Y + Config::PADDLE_HEIGHT + Config::BALL_RADIUS + 1.0f;
float ballVX = 0.0f;
float ballVY = 0.0f;

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

void resetBallOnPaddle() {
    ballVX = 0.0f;
    ballVY = 0.0f;
    ballX = paddleX + Config::PADDLE_WIDTH / 2.0f;
    ballY = Config::PADDLE_Y + Config::PADDLE_HEIGHT + Config::BALL_RADIUS + 1.0f;
}

void initBricks() {
    for (int r = 0; r < Config::BRICK_ROWS; ++r) {
        for (int c = 0; c < Config::BRICK_COLS; ++c) {
            bricks[r][c].x = Config::BRICK_START_X + c * (Config::BRICK_WIDTH + Config::BRICK_PADDING_X);
            bricks[r][c].y = Config::BRICK_START_Y - r * (Config::BRICK_HEIGHT + Config::BRICK_PADDING_Y);
            bricks[r][c].active = true;
        }
    }
    remainingBricks = Config::BRICK_ROWS * Config::BRICK_COLS;
}

void resetGame() {
    score = 0;
    lives = Config::INITIAL_LIVES;
    paddleX = (Config::WINDOW_WIDTH - Config::PADDLE_WIDTH) / 2.0f;
    initBricks();
    resetBallOnPaddle();
    gameState = STATE_READY;
    stateBeforePause = STATE_READY;
}

void launchBall() {
    if (gameState == STATE_READY) {
        gameState = STATE_PLAYING;
        float initialAngle = 0.35f; // ~20 degrees right of vertical
        ballVX = Config::BALL_SPEED * std::sin(initialAngle);
        ballVY = Config::BALL_SPEED * std::cos(initialAngle);
    }
}

void togglePause() {
    if (gameState == STATE_PLAYING || gameState == STATE_READY) {
        stateBeforePause = gameState;
        gameState = STATE_PAUSED;
    } else if (gameState == STATE_PAUSED) {
        gameState = stateBeforePause;
    }
}

void init() {
    glClearColor(0.08f, 0.09f, 0.13f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, Config::WINDOW_WIDTH, 0.0, Config::WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    resetGame();
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

// Reusable Bresenham circle-drawing routine for the ball and life indicators
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

void drawText(float x, float y, void* font, const char* text) {
    glRasterPos2f(x, y);
    for (const char* p = text; *p != '\0'; ++p) {
        glutBitmapCharacter(font, *p);
    }
}

int getTextWidth(void* font, const char* text) {
    return glutBitmapLength(font, reinterpret_cast<const unsigned char*>(text));
}

void drawCenteredText(float centerX, float y, void* font, const char* text) {
    int width = getTextWidth(font, text);
    drawText(centerX - width / 2.0f, y, font, text);
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
    glColor3f(0.25f, 0.78f, 0.95f);
    glBegin(GL_QUADS);
        glVertex2f(paddleX, Config::PADDLE_Y);
        glVertex2f(paddleX + Config::PADDLE_WIDTH, Config::PADDLE_Y);
        glVertex2f(paddleX + Config::PADDLE_WIDTH, Config::PADDLE_Y + Config::PADDLE_HEIGHT);
        glVertex2f(paddleX, Config::PADDLE_Y + Config::PADDLE_HEIGHT);
    glEnd();

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

            glColor3fv(ROW_COLORS[r]);
            glBegin(GL_QUADS);
                glVertex2f(bx, by);
                glVertex2f(bx + Config::BRICK_WIDTH, by);
                glVertex2f(bx + Config::BRICK_WIDTH, by + Config::BRICK_HEIGHT);
                glVertex2f(bx, by + Config::BRICK_HEIGHT);
            glEnd();

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

    glPointSize(2.0f);
    glColor3f(0.92f, 0.95f, 1.0f);
    for (int r = 1; r < Config::BALL_RADIUS; ++r) {
        drawBresenhamCircle(cx, cy, r);
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    drawBresenhamCircle(cx, cy, Config::BALL_RADIUS);
    glPointSize(1.0f);
}

void drawHUD() {
    // Score on the left
    char scoreBuf[64];
    std::snprintf(scoreBuf, sizeof(scoreBuf), "Score: %d", score);
    glColor3f(0.95f, 0.96f, 1.0f);
    drawText(Config::FIELD_LEFT + 4.0f, 565.0f, GLUT_BITMAP_HELVETICA_18, scoreBuf);

    // Controls summary in the center
    glColor3f(0.68f, 0.75f, 0.88f);
    drawCenteredText(
        Config::WINDOW_WIDTH / 2.0f,
        567.0f,
        GLUT_BITMAP_HELVETICA_12,
        "Left/Right: Move  |  SPACE: Launch  |  P: Pause  |  R: Restart"
    );

    // Lives text and Bresenham circle indicators on the right
    char livesBuf[32];
    std::snprintf(livesBuf, sizeof(livesBuf), "Lives: %d", lives);
    glColor3f(0.95f, 0.96f, 1.0f);
    drawText(Config::FIELD_RIGHT - 135.0f, 565.0f, GLUT_BITMAP_HELVETICA_18, livesBuf);

    glPointSize(1.5f);
    for (int i = 0; i < Config::INITIAL_LIVES; ++i) {
        int lx = static_cast<int>(Config::FIELD_RIGHT - 48.0f + i * 18.0f);
        int ly = 571;
        if (i < lives) {
            glColor3f(0.95f, 0.38f, 0.42f);
            for (int r = 1; r <= 5; ++r) {
                drawBresenhamCircle(lx, ly, r);
            }
        } else {
            glColor3f(0.35f, 0.38f, 0.45f);
            drawBresenhamCircle(lx, ly, 5);
        }
    }
    glPointSize(1.0f);
}

void drawStatusBanner(const char* title, const char* subtitle, float r, float g, float b) {
    float boxW = 420.0f;
    float boxH = 110.0f;
    float boxX = (Config::WINDOW_WIDTH - boxW) / 2.0f;
    float boxY = (Config::WINDOW_HEIGHT - boxH) / 2.0f - 20.0f;

    // Banner background
    glColor3f(0.11f, 0.13f, 0.19f);
    glBegin(GL_QUADS);
        glVertex2f(boxX, boxY);
        glVertex2f(boxX + boxW, boxY);
        glVertex2f(boxX + boxW, boxY + boxH);
        glVertex2f(boxX, boxY + boxH);
    glEnd();

    // Banner border
    glColor3f(r, g, b);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(boxX, boxY);
        glVertex2f(boxX + boxW, boxY);
        glVertex2f(boxX + boxW, boxY + boxH);
        glVertex2f(boxX, boxY + boxH);
    glEnd();

    // Title and subtitle text
    glColor3f(r, g, b);
    drawCenteredText(Config::WINDOW_WIDTH / 2.0f, boxY + 66.0f, GLUT_BITMAP_HELVETICA_18, title);

    glColor3f(0.88f, 0.91f, 0.96f);
    drawCenteredText(Config::WINDOW_WIDTH / 2.0f, boxY + 32.0f, GLUT_BITMAP_9_BY_15, subtitle);
}

void drawStatusOverlay() {
    char subBuf[96];
    if (gameState == STATE_READY) {
        drawStatusBanner(
            "READY",
            "Press SPACE to Launch  |  Left/Right to Move",
            0.35f, 0.80f, 0.98f
        );
    } else if (gameState == STATE_PAUSED) {
        drawStatusBanner(
            "GAME PAUSED",
            "Press P to Resume  |  Press R to Restart",
            0.96f, 0.80f, 0.28f
        );
    } else if (gameState == STATE_WON) {
        std::snprintf(subBuf, sizeof(subBuf), "Final Score: %d  |  Press R to Play Again", score);
        drawStatusBanner(
            "YOU WIN! ALL BRICKS CLEARED!",
            subBuf,
            0.36f, 0.86f, 0.52f
        );
    } else if (gameState == STATE_GAME_OVER) {
        std::snprintf(subBuf, sizeof(subBuf), "Final Score: %d  |  Press R to Restart", score);
        drawStatusBanner(
            "GAME OVER",
            subBuf,
            0.95f, 0.36f, 0.36f
        );
    }
}

bool checkCircleRectCollision(float rx, float ry, float rw, float rh, float& closestX, float& closestY) {
    closestX = std::max(rx, std::min(ballX, rx + rw));
    closestY = std::max(ry, std::min(ballY, ry + rh));
    float dx = ballX - closestX;
    float dy = ballY - closestY;
    float r = static_cast<float>(Config::BALL_RADIUS);
    return (dx * dx + dy * dy) <= (r * r);
}

void stepBallPhysics(float subDt) {
    float r = static_cast<float>(Config::BALL_RADIUS);

    ballX += ballVX * subDt;
    ballY += ballVY * subDt;

    // 1. Wall collisions (left, right, top)
    if (ballX - r <= Config::FIELD_LEFT) {
        ballX = Config::FIELD_LEFT + r;
        ballVX = std::fabs(ballVX);
    } else if (ballX + r >= Config::FIELD_RIGHT) {
        ballX = Config::FIELD_RIGHT - r;
        ballVX = -std::fabs(ballVX);
    }

    if (ballY + r >= Config::FIELD_TOP) {
        ballY = Config::FIELD_TOP - r;
        ballVY = -std::fabs(ballVY);
    }

    // Bottom wall miss -> lose 1 life
    if (ballY - r < Config::FIELD_BOTTOM) {
        lives--;
        if (lives > 0) {
            gameState = STATE_READY;
            resetBallOnPaddle();
        } else {
            lives = 0;
            gameState = STATE_GAME_OVER;
            resetBallOnPaddle();
        }
        return;
    }

    // 2. Paddle collision
    float closestX = 0.0f, closestY = 0.0f;
    if (ballVY < 0.0f &&
        checkCircleRectCollision(paddleX, Config::PADDLE_Y, Config::PADDLE_WIDTH, Config::PADDLE_HEIGHT, closestX, closestY)) {
        ballY = Config::PADDLE_Y + Config::PADDLE_HEIGHT + r + 0.5f;

        float paddleCenter = paddleX + Config::PADDLE_WIDTH / 2.0f;
        float hitOffset = (ballX - paddleCenter) / (Config::PADDLE_WIDTH / 2.0f);
        hitOffset = std::max(-1.0f, std::min(1.0f, hitOffset));

        float bounceAngle = hitOffset * Config::MAX_BOUNCE_ANGLE;
        ballVX = Config::BALL_SPEED * std::sin(bounceAngle);
        ballVY = Config::BALL_SPEED * std::cos(bounceAngle);
    }

    // 3. Brick collisions (each brick is destroyed and scored once)
    for (int row = 0; row < Config::BRICK_ROWS; ++row) {
        bool hitBrick = false;
        for (int col = 0; col < Config::BRICK_COLS; ++col) {
            if (!bricks[row][col].active) continue;

            float bx = bricks[row][col].x;
            float by = bricks[row][col].y;
            if (checkCircleRectCollision(bx, by, Config::BRICK_WIDTH, Config::BRICK_HEIGHT, closestX, closestY)) {
                bricks[row][col].active = false;
                score += Config::POINTS_PER_BRICK;
                remainingBricks--;

                float overlapLeft   = (ballX + r) - bx;
                float overlapRight  = (bx + Config::BRICK_WIDTH) - (ballX - r);
                float overlapBottom = (ballY + r) - by;
                float overlapTop    = (by + Config::BRICK_HEIGHT) - (ballY - r);

                float minOverlapX = std::min(overlapLeft, overlapRight);
                float minOverlapY = std::min(overlapBottom, overlapTop);

                if (minOverlapX < minOverlapY) {
                    if (overlapLeft < overlapRight) {
                        ballX = bx - r - 0.5f;
                        ballVX = -std::fabs(ballVX);
                    } else {
                        ballX = bx + Config::BRICK_WIDTH + r + 0.5f;
                        ballVX = std::fabs(ballVX);
                    }
                } else {
                    if (overlapBottom < overlapTop) {
                        ballY = by - r - 0.5f;
                        ballVY = -std::fabs(ballVY);
                    } else {
                        ballY = by + Config::BRICK_HEIGHT + r + 0.5f;
                        ballVY = std::fabs(ballVY);
                    }
                }

                if (remainingBricks <= 0) {
                    gameState = STATE_WON;
                    ballVX = 0.0f;
                    ballVY = 0.0f;
                }

                hitBrick = true;
                break;
            }
        }
        if (hitBrick) break;
    }
}

void update(float dt) {
    // Prevent all movement while paused, won, or game over
    if (gameState == STATE_PAUSED || gameState == STATE_WON || gameState == STATE_GAME_OVER) {
        return;
    }

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

    if (gameState == STATE_READY) {
        ballX = paddleX + Config::PADDLE_WIDTH / 2.0f;
        ballY = Config::PADDLE_Y + Config::PADDLE_HEIGHT + Config::BALL_RADIUS + 1.0f;
    } else if (gameState == STATE_PLAYING) {
        float subDt = dt / Config::SIM_SUBSTEPS;
        for (int i = 0; i < Config::SIM_SUBSTEPS; ++i) {
            if (gameState != STATE_PLAYING) break;
            stepBallPhysics(subDt);
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawBoundaries();
    drawHUD();
    drawBricks();
    drawPaddle();
    drawBall();
    drawStatusOverlay();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;

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
    if (key == ' ') launchBall();
    if (key == 'p' || key == 'P') togglePause();
    if (key == 'r' || key == 'R') resetGame();
    if (key == 27) exit(0);
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
