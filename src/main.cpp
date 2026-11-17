#include <GL/glut.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
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

struct Particle {
    float x, y;
    float vx, vy;
    float r, g, b;
    float life;
    float maxLife;
    float size;
    bool active;
};

struct ScorePopup {
    float x, y;
    int points;
    float life;
    bool active;
};

struct TrailPoint {
    float x, y;
    bool valid;
};

// Game state variables
GameState gameState = STATE_READY;
GameState stateBeforePause = STATE_READY;
int score = 0;
int highScore = 0;
int comboStreak = 0;
int lives = Config::INITIAL_LIVES;
int remainingBricks = Config::BRICK_ROWS * Config::BRICK_COLS;

// Paddle and ball state
float paddleX = (Config::WINDOW_WIDTH - Config::PADDLE_WIDTH) / 2.0f;
float ballX = Config::WINDOW_WIDTH / 2.0f;
float ballY = Config::PADDLE_Y + Config::PADDLE_HEIGHT + Config::BALL_RADIUS + 1.0f;
float ballVX = 0.0f;
float ballVY = 0.0f;
float paddleFlashTimer = 0.0f;
float screenShakeTimer = 0.0f;

bool moveLeft = false;
bool moveRight = false;
int lastTimeMs = 0;

Brick bricks[Config::BRICK_ROWS][Config::BRICK_COLS];
Particle particles[Config::MAX_PARTICLES];
ScorePopup popups[Config::MAX_POPUPS];
TrailPoint ballTrail[Config::BALL_TRAIL_LENGTH];
int trailHead = 0;
float trailSampleTimer = 0.0f;

// Premium vibrant row colors (R, G, B)
const float ROW_COLORS[Config::BRICK_ROWS][3] = {
    {0.96f, 0.31f, 0.42f}, // Row 0: Neon Rose / Crimson
    {0.98f, 0.58f, 0.22f}, // Row 1: Vibrant Sunset Orange
    {0.96f, 0.84f, 0.25f}, // Row 2: Golden Amber
    {0.24f, 0.86f, 0.54f}, // Row 3: Emerald Mint
    {0.26f, 0.66f, 0.98f}  // Row 4: Electric Cyan-Blue
};

void clearEffects() {
    for (int i = 0; i < Config::MAX_PARTICLES; ++i) {
        particles[i].active = false;
    }
    for (int i = 0; i < Config::MAX_POPUPS; ++i) {
        popups[i].active = false;
    }
    for (int i = 0; i < Config::BALL_TRAIL_LENGTH; ++i) {
        ballTrail[i].valid = false;
    }
    trailHead = 0;
    trailSampleTimer = 0.0f;
    paddleFlashTimer = 0.0f;
    screenShakeTimer = 0.0f;
}

void spawnBrickParticles(float cx, float cy, const float color[3]) {
    int spawned = 0;
    for (int i = 0; i < Config::MAX_PARTICLES && spawned < 18; ++i) {
        if (!particles[i].active) {
            float angle = (static_cast<float>(std::rand() % 360) * 3.14159265f) / 180.0f;
            float speed = 50.0f + static_cast<float>(std::rand() % 140);
            particles[i].x = cx + static_cast<float>((std::rand() % 24) - 12);
            particles[i].y = cy + static_cast<float>((std::rand() % 10) - 5);
            particles[i].vx = std::cos(angle) * speed;
            particles[i].vy = std::sin(angle) * speed;
            particles[i].r = color[0];
            particles[i].g = color[1];
            particles[i].b = color[2];
            particles[i].maxLife = 0.35f + (std::rand() % 25) * 0.01f;
            particles[i].life = particles[i].maxLife;
            particles[i].size = 2.5f + static_cast<float>(std::rand() % 3);
            particles[i].active = true;
            spawned++;
        }
    }
}

void spawnScorePopup(float x, float y, int pts) {
    for (int i = 0; i < Config::MAX_POPUPS; ++i) {
        if (!popups[i].active) {
            popups[i].x = x;
            popups[i].y = y;
            popups[i].points = pts;
            popups[i].life = 0.75f;
            popups[i].active = true;
            break;
        }
    }
}

void resetBallOnPaddle() {
    ballVX = 0.0f;
    ballVY = 0.0f;
    comboStreak = 0;
    ballX = paddleX + Config::PADDLE_WIDTH / 2.0f;
    ballY = Config::PADDLE_Y + Config::PADDLE_HEIGHT + Config::BALL_RADIUS + 1.0f;
    for (int i = 0; i < Config::BALL_TRAIL_LENGTH; ++i) {
        ballTrail[i].valid = false;
    }
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
    if (score > highScore) highScore = score;
    score = 0;
    comboStreak = 0;
    lives = Config::INITIAL_LIVES;
    paddleX = (Config::WINDOW_WIDTH - Config::PADDLE_WIDTH) / 2.0f;
    initBricks();
    clearEffects();
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
    glClearColor(0.05f, 0.06f, 0.10f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

// Reusable Bresenham circle-drawing routine for the ball, trail, and life indicators
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

void drawBackgroundAndBoundaries() {
    // 1. Playfield smooth vertical gradient background
    glBegin(GL_QUADS);
        glColor3f(0.06f, 0.07f, 0.12f);
        glVertex2f(Config::FIELD_LEFT, Config::FIELD_BOTTOM);
        glVertex2f(Config::FIELD_RIGHT, Config::FIELD_BOTTOM);
        glColor3f(0.10f, 0.13f, 0.22f);
        glVertex2f(Config::FIELD_RIGHT, Config::FIELD_TOP);
        glVertex2f(Config::FIELD_LEFT, Config::FIELD_TOP);
    glEnd();

    // 2. Subtle cyber grid inside playfield
    glColor4f(0.25f, 0.35f, 0.55f, 0.12f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (float x = Config::FIELD_LEFT + 40.0f; x < Config::FIELD_RIGHT; x += 40.0f) {
        glVertex2f(x, Config::FIELD_BOTTOM);
        glVertex2f(x, Config::FIELD_TOP);
    }
    for (float y = Config::FIELD_BOTTOM + 40.0f; y < Config::FIELD_TOP; y += 40.0f) {
        glVertex2f(Config::FIELD_LEFT, y);
        glVertex2f(Config::FIELD_RIGHT, y);
    }
    glEnd();

    // 3. Outer glow border around playfield
    glColor4f(0.22f, 0.62f, 0.95f, 0.25f);
    glLineWidth(5.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(Config::FIELD_LEFT - 1.5f, Config::FIELD_BOTTOM - 1.5f);
        glVertex2f(Config::FIELD_RIGHT + 1.5f, Config::FIELD_BOTTOM - 1.5f);
        glVertex2f(Config::FIELD_RIGHT + 1.5f, Config::FIELD_TOP + 1.5f);
        glVertex2f(Config::FIELD_LEFT - 1.5f, Config::FIELD_TOP + 1.5f);
    glEnd();

    // 4. Crisp inner playfield boundary
    glColor3f(0.40f, 0.65f, 0.92f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(Config::FIELD_LEFT, Config::FIELD_BOTTOM);
        glVertex2f(Config::FIELD_RIGHT, Config::FIELD_BOTTOM);
        glVertex2f(Config::FIELD_RIGHT, Config::FIELD_TOP);
        glVertex2f(Config::FIELD_LEFT, Config::FIELD_TOP);
    glEnd();

    // 5. Bottom danger zone subtle glow line
    glColor4f(0.95f, 0.25f, 0.35f, 0.35f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(Config::FIELD_LEFT + 2.0f, Config::FIELD_BOTTOM + 1.0f);
        glVertex2f(Config::FIELD_RIGHT - 2.0f, Config::FIELD_BOTTOM + 1.0f);
    glEnd();

    // 6. Top HUD bar background and separator
    glColor3f(0.08f, 0.10f, 0.16f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, Config::FIELD_TOP + 4.0f);
        glVertex2f(Config::WINDOW_WIDTH, Config::FIELD_TOP + 4.0f);
        glVertex2f(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT);
        glVertex2f(0.0f, Config::WINDOW_HEIGHT);
    glEnd();

    glColor3f(0.25f, 0.38f, 0.58f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
        glVertex2f(0.0f, Config::FIELD_TOP + 4.0f);
        glVertex2f(Config::WINDOW_WIDTH, Config::FIELD_TOP + 4.0f);
    glEnd();
}

void drawAimGuide() {
    if (gameState != STATE_READY) return;

    float initialAngle = 0.35f;
    float dirX = std::sin(initialAngle);
    float dirY = std::cos(initialAngle);

    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (int i = 1; i <= 7; ++i) {
        float alpha = 0.65f - i * 0.07f;
        glColor4f(0.45f, 0.88f, 1.0f, alpha);
        float px = ballX + dirX * (i * 16.0f);
        float py = ballY + dirY * (i * 16.0f);
        glVertex2f(px, py);
    }
    glEnd();
    glPointSize(1.0f);
}

void drawPaddle() {
    float x = paddleX;
    float y = Config::PADDLE_Y;
    float w = Config::PADDLE_WIDTH;
    float h = Config::PADDLE_HEIGHT;
    float boost = (paddleFlashTimer > 0.0f) ? 0.25f : 0.0f;

    // Soft glow beneath paddle
    glColor4f(0.18f, 0.72f, 0.98f, 0.22f + boost);
    glBegin(GL_QUADS);
        glVertex2f(x - 3.0f, y - 3.0f);
        glVertex2f(x + w + 3.0f, y - 3.0f);
        glVertex2f(x + w + 3.0f, y + h + 3.0f);
        glVertex2f(x - 3.0f, y + h + 3.0f);
    glEnd();

    // Main paddle gradient body
    glBegin(GL_QUADS);
        glColor3f(0.14f + boost, 0.56f + boost, 0.82f);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glColor3f(0.32f + boost, 0.86f + boost, 0.98f);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();

    // Bright top bevel highlight strip
    glColor4f(1.0f, 1.0f, 1.0f, 0.45f);
    glBegin(GL_QUADS);
        glVertex2f(x + 2.0f, y + h - 3.0f);
        glVertex2f(x + w - 2.0f, y + h - 3.0f);
        glVertex2f(x + w - 2.0f, y + h);
        glVertex2f(x + 2.0f, y + h);
    glEnd();

    // Left and right accent bumpers
    glColor3f(0.98f, 0.76f, 0.24f);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + 8.0f, y);
        glVertex2f(x + 8.0f, y + h);
        glVertex2f(x, y + h);

        glVertex2f(x + w - 8.0f, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x + w - 8.0f, y + h);
    glEnd();

    // Crisp outline
    glColor3f(0.80f, 0.96f, 1.0f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();
}

void drawBricks() {
    for (int r = 0; r < Config::BRICK_ROWS; ++r) {
        for (int c = 0; c < Config::BRICK_COLS; ++c) {
            if (!bricks[r][c].active) continue;

            float bx = bricks[r][c].x;
            float by = bricks[r][c].y;
            float bw = Config::BRICK_WIDTH;
            float bh = Config::BRICK_HEIGHT;
            const float* col = ROW_COLORS[r];

            // Base brick body with subtle vertical shading
            glBegin(GL_QUADS);
                glColor3f(col[0] * 0.80f, col[1] * 0.80f, col[2] * 0.80f);
                glVertex2f(bx, by);
                glVertex2f(bx + bw, by);
                glColor3f(col[0], col[1], col[2]);
                glVertex2f(bx + bw, by + bh);
                glVertex2f(bx, by + bh);
            glEnd();

            // Top & left 3D highlight bevel
            glColor4f(1.0f, 1.0f, 1.0f, 0.32f);
            glBegin(GL_QUADS);
                glVertex2f(bx, by + bh - 3.0f);
                glVertex2f(bx + bw, by + bh - 3.0f);
                glVertex2f(bx + bw, by + bh);
                glVertex2f(bx, by + bh);
            glEnd();

            // Bottom shadow bevel
            glColor4f(0.0f, 0.0f, 0.0f, 0.28f);
            glBegin(GL_QUADS);
                glVertex2f(bx, by);
                glVertex2f(bx + bw, by);
                glVertex2f(bx + bw, by + 3.0f);
                glVertex2f(bx, by + 3.0f);
            glEnd();

            // Crisp dark border
            glColor3f(0.08f, 0.10f, 0.15f);
            glLineWidth(1.2f);
            glBegin(GL_LINE_LOOP);
                glVertex2f(bx, by);
                glVertex2f(bx + bw, by);
                glVertex2f(bx + bw, by + bh);
                glVertex2f(bx, by + bh);
            glEnd();
        }
    }
}

void drawBallAndTrail() {
    // 1. Draw fading motion trail using Bresenham circles
    if (gameState == STATE_PLAYING) {
        glPointSize(1.5f);
        for (int i = 0; i < Config::BALL_TRAIL_LENGTH; ++i) {
            int idx = (trailHead + i) % Config::BALL_TRAIL_LENGTH;
            if (!ballTrail[idx].valid) continue;

            float frac = static_cast<float>(i + 1) / Config::BALL_TRAIL_LENGTH;
            int trailR = std::max(2, static_cast<int>(Config::BALL_RADIUS * frac * 0.75f));
            glColor4f(0.35f, 0.82f, 1.0f, frac * 0.32f);
            drawBresenhamCircle(
                static_cast<int>(std::round(ballTrail[idx].x)),
                static_cast<int>(std::round(ballTrail[idx].y)),
                trailR
            );
        }
    }

    int cx = static_cast<int>(std::round(ballX));
    int cy = static_cast<int>(std::round(ballY));

    // 2. Outer soft halo using Bresenham circle
    glPointSize(2.0f);
    glColor4f(0.38f, 0.85f, 1.0f, 0.35f);
    drawBresenhamCircle(cx, cy, Config::BALL_RADIUS + 2);

    // 3. Inner circle fill using concentric Bresenham circles
    glColor3f(0.88f, 0.95f, 1.0f);
    for (int r = 1; r < Config::BALL_RADIUS; ++r) {
        drawBresenhamCircle(cx, cy, r);
    }

    // 4. Crisp outer ball outline using Bresenham's circle algorithm
    glColor3f(1.0f, 1.0f, 1.0f);
    drawBresenhamCircle(cx, cy, Config::BALL_RADIUS);

    // 5. Specular highlight dot
    glPointSize(3.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POINTS);
        glVertex2i(cx - 2, cy + 2);
    glEnd();
    glPointSize(1.0f);
}

void drawParticlesAndPopups() {
    // Brick destruction particles
    for (int i = 0; i < Config::MAX_PARTICLES; ++i) {
        if (!particles[i].active) continue;
        float alpha = particles[i].life / particles[i].maxLife;
        float hs = particles[i].size * 0.5f;
        glColor4f(particles[i].r, particles[i].g, particles[i].b, alpha);
        glBegin(GL_QUADS);
            glVertex2f(particles[i].x - hs, particles[i].y - hs);
            glVertex2f(particles[i].x + hs, particles[i].y - hs);
            glVertex2f(particles[i].x + hs, particles[i].y + hs);
            glVertex2f(particles[i].x - hs, particles[i].y + hs);
        glEnd();
    }

    // Floating "+10" score popups
    for (int i = 0; i < Config::MAX_POPUPS; ++i) {
        if (!popups[i].active) continue;
        float alpha = std::min(1.0f, popups[i].life / 0.75f);
        char buf[16];
        std::snprintf(buf, sizeof(buf), "+%d", popups[i].points);
        glColor4f(1.0f, 0.92f, 0.35f, alpha);
        drawCenteredText(popups[i].x, popups[i].y, GLUT_BITMAP_HELVETICA_12, buf);
    }
}

void drawHUD() {
    // Score & High Score on the left
    char scoreBuf[64];
    if (score > highScore) highScore = score;
    std::snprintf(scoreBuf, sizeof(scoreBuf), "SCORE: %d   BEST: %d", score, highScore);
    glColor3f(0.95f, 0.97f, 1.0f);
    drawText(Config::FIELD_LEFT + 2.0f, 565.0f, GLUT_BITMAP_HELVETICA_18, scoreBuf);

    // Combo badge or controls summary in the center
    if (comboStreak >= 2 && gameState == STATE_PLAYING) {
        char comboBuf[48];
        std::snprintf(comboBuf, sizeof(comboBuf), "STREAK x%d!", comboStreak);
        glColor3f(0.98f, 0.82f, 0.25f);
        drawCenteredText(Config::WINDOW_WIDTH / 2.0f, 566.0f, GLUT_BITMAP_HELVETICA_18, comboBuf);
    } else {
        glColor3f(0.68f, 0.76f, 0.90f);
        drawCenteredText(
            Config::WINDOW_WIDTH / 2.0f + 15.0f,
            567.0f,
            GLUT_BITMAP_HELVETICA_12,
            "Left/Right: Move  |  SPACE: Launch  |  P: Pause  |  R: Restart"
        );
    }

    // Lives text and Bresenham circle indicators on the right
    char livesBuf[32];
    std::snprintf(livesBuf, sizeof(livesBuf), "LIVES:");
    glColor3f(0.95f, 0.97f, 1.0f);
    drawText(Config::FIELD_RIGHT - 122.0f, 565.0f, GLUT_BITMAP_HELVETICA_18, livesBuf);

    glPointSize(1.5f);
    for (int i = 0; i < Config::INITIAL_LIVES; ++i) {
        int lx = static_cast<int>(Config::FIELD_RIGHT - 46.0f + i * 19.0f);
        int ly = 571;
        if (i < lives) {
            glColor3f(0.96f, 0.34f, 0.44f);
            for (int r = 1; r <= 6; ++r) {
                drawBresenhamCircle(lx, ly, r);
            }
            glColor3f(1.0f, 0.82f, 0.86f);
            drawBresenhamCircle(lx - 2, ly + 2, 1);
        } else {
            glColor3f(0.32f, 0.36f, 0.46f);
            drawBresenhamCircle(lx, ly, 6);
        }
    }
    glPointSize(1.0f);
}

void drawStatusBanner(const char* title, const char* subtitle, float r, float g, float b) {
    float boxW = 450.0f;
    float boxH = 118.0f;
    float boxX = (Config::WINDOW_WIDTH - boxW) / 2.0f;
    float boxY = (Config::WINDOW_HEIGHT - boxH) / 2.0f - 25.0f;

    // Drop shadow
    glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
    glBegin(GL_QUADS);
        glVertex2f(boxX + 6.0f, boxY - 6.0f);
        glVertex2f(boxX + boxW + 6.0f, boxY - 6.0f);
        glVertex2f(boxX + boxW + 6.0f, boxY + boxH - 6.0f);
        glVertex2f(boxX + 6.0f, boxY + boxH - 6.0f);
    glEnd();

    // Translucent dark panel
    glColor4f(0.08f, 0.10f, 0.17f, 0.92f);
    glBegin(GL_QUADS);
        glVertex2f(boxX, boxY);
        glVertex2f(boxX + boxW, boxY);
        glVertex2f(boxX + boxW, boxY + boxH);
        glVertex2f(boxX, boxY + boxH);
    glEnd();

    // Top accent bar
    glColor4f(r, g, b, 0.85f);
    glBegin(GL_QUADS);
        glVertex2f(boxX, boxY + boxH - 5.0f);
        glVertex2f(boxX + boxW, boxY + boxH - 5.0f);
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
    drawCenteredText(Config::WINDOW_WIDTH / 2.0f, boxY + 70.0f, GLUT_BITMAP_HELVETICA_18, title);

    glColor3f(0.88f, 0.92f, 0.98f);
    drawCenteredText(Config::WINDOW_WIDTH / 2.0f, boxY + 32.0f, GLUT_BITMAP_9_BY_15, subtitle);
}

void drawStatusOverlay() {
    char subBuf[96];
    if (gameState == STATE_READY) {
        drawStatusBanner(
            "READY TO LAUNCH",
            "Press SPACE to Launch  |  Left/Right to Aim & Move",
            0.35f, 0.82f, 0.98f
        );
    } else if (gameState == STATE_PAUSED) {
        drawStatusBanner(
            "GAME PAUSED",
            "Press P to Resume  |  Press R to Restart",
            0.98f, 0.82f, 0.28f
        );
    } else if (gameState == STATE_WON) {
        std::snprintf(subBuf, sizeof(subBuf), "Final Score: %d  |  Press R or SPACE to Play Again", score);
        drawStatusBanner(
            "YOU WIN! ALL BRICKS CLEARED!",
            subBuf,
            0.32f, 0.90f, 0.56f
        );
    } else if (gameState == STATE_GAME_OVER) {
        std::snprintf(subBuf, sizeof(subBuf), "Final Score: %d  |  Press R or SPACE to Restart", score);
        drawStatusBanner(
            "GAME OVER",
            subBuf,
            0.96f, 0.36f, 0.42f
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

void updateEffects(float dt) {
    if (paddleFlashTimer > 0.0f) {
        paddleFlashTimer = std::max(0.0f, paddleFlashTimer - dt);
    }
    if (screenShakeTimer > 0.0f) {
        screenShakeTimer = std::max(0.0f, screenShakeTimer - dt);
    }

    for (int i = 0; i < Config::MAX_PARTICLES; ++i) {
        if (!particles[i].active) continue;
        particles[i].x += particles[i].vx * dt;
        particles[i].y += particles[i].vy * dt;
        particles[i].vy -= 180.0f * dt; // subtle gravity on debris
        particles[i].life -= dt;
        if (particles[i].life <= 0.0f) {
            particles[i].active = false;
        }
    }

    for (int i = 0; i < Config::MAX_POPUPS; ++i) {
        if (!popups[i].active) continue;
        popups[i].y += 38.0f * dt;
        popups[i].life -= dt;
        if (popups[i].life <= 0.0f) {
            popups[i].active = false;
        }
    }
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
        screenShakeTimer = 0.22f;
        if (lives > 0) {
            gameState = STATE_READY;
            resetBallOnPaddle();
        } else {
            lives = 0;
            if (score > highScore) highScore = score;
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
        paddleFlashTimer = 0.15f;
        comboStreak = 0;

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
                if (score > highScore) highScore = score;
                remainingBricks--;
                comboStreak++;

                // Spawn visual effects
                float centerBrickX = bx + Config::BRICK_WIDTH * 0.5f;
                float centerBrickY = by + Config::BRICK_HEIGHT * 0.5f;
                spawnBrickParticles(centerBrickX, centerBrickY, ROW_COLORS[row]);
                spawnScorePopup(centerBrickX, centerBrickY, Config::POINTS_PER_BRICK);

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
    updateEffects(dt);

    // Prevent gameplay movement while paused, won, or game over
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
        trailSampleTimer += dt;
        if (trailSampleTimer >= 0.014f) {
            trailSampleTimer = 0.0f;
            ballTrail[trailHead].x = ballX;
            ballTrail[trailHead].y = ballY;
            ballTrail[trailHead].valid = true;
            trailHead = (trailHead + 1) % Config::BALL_TRAIL_LENGTH;
        }

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

    if (screenShakeTimer > 0.0f) {
        float shakeX = static_cast<float>((std::rand() % 7) - 3);
        float shakeY = static_cast<float>((std::rand() % 7) - 3);
        glTranslatef(shakeX, shakeY, 0.0f);
    }

    drawBackgroundAndBoundaries();
    drawHUD();
    drawBricks();
    drawAimGuide();
    drawPaddle();
    drawBallAndTrail();
    drawParticlesAndPopups();
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
    if (key == ' ') {
        if (gameState == STATE_WON || gameState == STATE_GAME_OVER) {
            resetGame();
        } else {
            launchBall();
        }
    }
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
