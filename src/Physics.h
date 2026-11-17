#ifndef PHYSICS_H
#define PHYSICS_H

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "GameState.h"

// ============================================================================
// Physics.h - Ball Motion, Collision Detection, Scoring & Game State Logic
// Lead Module: Yousuf Bin Nazib (ID: 231-115-064)
// ============================================================================

inline void clearEffects() {
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

inline void spawnBrickParticles(float cx, float cy, const float color[3]) {
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

inline void spawnScorePopup(float x, float y, int pts) {
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

inline void clampPaddle() {
    if (paddleX < Config::FIELD_LEFT) {
        paddleX = Config::FIELD_LEFT;
    }
    if (paddleX + Config::PADDLE_WIDTH > Config::FIELD_RIGHT) {
        paddleX = Config::FIELD_RIGHT - Config::PADDLE_WIDTH;
    }
    if (gameState == STATE_READY) {
        ballX = paddleX + Config::PADDLE_WIDTH / 2.0f;
        ballY = Config::PADDLE_Y + Config::PADDLE_HEIGHT + Config::BALL_RADIUS + 1.0f;
    }
}

inline void resetBallOnPaddle() {
    ballVX = 0.0f;
    ballVY = 0.0f;
    comboStreak = 0;
    ballX = paddleX + Config::PADDLE_WIDTH / 2.0f;
    ballY = Config::PADDLE_Y + Config::PADDLE_HEIGHT + Config::BALL_RADIUS + 1.0f;
    for (int i = 0; i < Config::BALL_TRAIL_LENGTH; ++i) {
        ballTrail[i].valid = false;
    }
    trailHead = 0;
    trailSampleTimer = 0.0f;
}

inline void initBricks() {
    for (int r = 0; r < Config::BRICK_ROWS; ++r) {
        for (int c = 0; c < Config::BRICK_COLS; ++c) {
            bricks[r][c].x = Config::BRICK_START_X + c * (Config::BRICK_WIDTH + Config::BRICK_PADDING_X);
            bricks[r][c].y = Config::BRICK_START_Y - r * (Config::BRICK_HEIGHT + Config::BRICK_PADDING_Y);
            bricks[r][c].active = true;
        }
    }
    remainingBricks = Config::BRICK_ROWS * Config::BRICK_COLS;
}

inline void resetGame() {
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

inline void launchBall() {
    if (gameState == STATE_READY) {
        gameState = STATE_PLAYING;
        ballVX = Config::BALL_SPEED * std::sin(Config::INITIAL_LAUNCH_ANGLE);
        ballVY = Config::BALL_SPEED * std::cos(Config::INITIAL_LAUNCH_ANGLE);
    }
}

inline void togglePause() {
    if (gameState == STATE_PLAYING || gameState == STATE_READY) {
        stateBeforePause = gameState;
        gameState = STATE_PAUSED;
    } else if (gameState == STATE_PAUSED) {
        gameState = stateBeforePause;
    }
}

// Circle-vs-Axis-Aligned-Rectangle (AABB) intersection test
inline bool checkCircleRectCollision(float rx, float ry, float rw, float rh, float& closestX, float& closestY) {
    closestX = std::max(rx, std::min(ballX, rx + rw));
    closestY = std::max(ry, std::min(ballY, ry + rh));
    float dx = ballX - closestX;
    float dy = ballY - closestY;
    float r = static_cast<float>(Config::BALL_RADIUS);
    return (dx * dx + dy * dy) <= (r * r);
}

inline void updateEffects(float dt) {
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
        particles[i].vy -= 180.0f * dt;
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

inline void stepBallPhysics(float subDt) {
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

                float centerBrickX = bx + Config::BRICK_WIDTH * 0.5f;
                float centerBrickY = by + Config::BRICK_HEIGHT * 0.5f;
                spawnBrickParticles(centerBrickX, centerBrickY, Config::ROW_COLORS[row]);
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

inline void updateGame(float dt) {
    updateEffects(dt);

    if (gameState == STATE_PAUSED || gameState == STATE_WON || gameState == STATE_GAME_OVER) {
        return;
    }

    if (moveLeft) {
        paddleX -= Config::PADDLE_SPEED * dt;
    }
    if (moveRight) {
        paddleX += Config::PADDLE_SPEED * dt;
    }

    clampPaddle();

    if (gameState == STATE_PLAYING) {
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

#endif
