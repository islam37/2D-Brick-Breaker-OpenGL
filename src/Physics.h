#ifndef PHYSICS_H
#define PHYSICS_H

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "GameState.h"

// ============================================================================
// Physics.h - Multi-Ball Motion, Levels, Collision Detection & Scoring
// Lead Module: Yousuf Bin Nazib (ID: 231-115-064)
// ============================================================================

inline int getBallsForLevel(int level) {
    if (level <= 1) return 1;
    if (level == 2) return 2;
    return 3;
}

inline float getBallSpeedForLevel(int level) {
    return Config::BALL_SPEED + (level - 1) * Config::BALL_SPEED_PER_LEVEL;
}

inline const char* getLevelName(int level) {
    switch (level) {
        case 1: return "CLASSIC";
        case 2: return "TWIN STRIKE";
        case 3: return "TRIPLE THREAT";
        case 4: return "CITADEL";
        default: return "SUPERNOVA";
    }
}

inline int countActiveBalls() {
    int count = 0;
    for (int i = 0; i < Config::MAX_BALLS; ++i) {
        if (balls[i].active) count++;
    }
    return count;
}

inline void clearEffects() {
    for (int i = 0; i < Config::MAX_PARTICLES; ++i) {
        particles[i].active = false;
    }
    for (int i = 0; i < Config::MAX_POPUPS; ++i) {
        popups[i].active = false;
    }
    paddleFlashTimer = 0.0f;
    screenShakeTimer = 0.0f;
}

inline void spawnBrickParticles(float cx, float cy, const float color[3], int count = 16) {
    int spawned = 0;
    for (int i = 0; i < Config::MAX_PARTICLES && spawned < count; ++i) {
        if (!particles[i].active) {
            float angle = (static_cast<float>(std::rand() % 360) * 3.14159265f) / 180.0f;
            float speed = 55.0f + static_cast<float>(std::rand() % 145);
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

inline void spawnTextPopup(float x, float y, const char* msg, float r, float g, float b) {
    for (int i = 0; i < Config::MAX_POPUPS; ++i) {
        if (!popups[i].active) {
            popups[i].x = x;
            popups[i].y = y;
            std::snprintf(popups[i].text, sizeof(popups[i].text), "%s", msg);
            popups[i].r = r;
            popups[i].g = g;
            popups[i].b = b;
            popups[i].life = 0.85f;
            popups[i].active = true;
            break;
        }
    }
}

inline void spawnScorePopup(float x, float y, int pts) {
    char buf[24];
    std::snprintf(buf, sizeof(buf), "+%d", pts);
    spawnTextPopup(x, y, buf, 1.0f, 0.92f, 0.35f);
}

inline void clearBallTrail(Ball& b) {
    for (int i = 0; i < Config::BALL_TRAIL_LENGTH; ++i) {
        b.trail[i].valid = false;
    }
    b.trailHead = 0;
    b.trailSampleTimer = 0.0f;
}

inline void positionReadyBalls() {
    int count = getBallsForLevel(currentLevel);
    float center = paddleX + Config::PADDLE_WIDTH * 0.5f;
    float baseY = Config::PADDLE_Y + Config::PADDLE_HEIGHT + Config::BALL_RADIUS + 1.0f;
    for (int i = 0; i < Config::MAX_BALLS; ++i) {
        if (balls[i].active) {
            float offset = 0.0f;
            if (count == 2) offset = (i == 0) ? -18.0f : 18.0f;
            else if (count == 3) offset = (i - 1) * 22.0f;
            balls[i].x = center + offset;
            balls[i].y = baseY;
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
        positionReadyBalls();
    }
}

inline void resetBallsOnPaddle() {
    int count = getBallsForLevel(currentLevel);
    comboStreak = 0;
    for (int i = 0; i < Config::MAX_BALLS; ++i) {
        balls[i].vx = 0.0f;
        balls[i].vy = 0.0f;
        balls[i].colorIndex = i;
        balls[i].active = (i < count);
        clearBallTrail(balls[i]);
    }
    positionReadyBalls();
}

inline void initBricksForLevel(int level) {
    remainingBricks = 0;
    for (int r = 0; r < Config::BRICK_ROWS; ++r) {
        for (int c = 0; c < Config::BRICK_COLS; ++c) {
            bricks[r][c].x = Config::BRICK_START_X + c * (Config::BRICK_WIDTH + Config::BRICK_PADDING_X);
            bricks[r][c].y = Config::BRICK_START_Y - r * (Config::BRICK_HEIGHT + Config::BRICK_PADDING_Y);
            bricks[r][c].hitFlash = 0.0f;

            int hp = 0;
            if (level == 1) {
                hp = 1;
            } else if (level == 2) {
                // Twin Strike: Fortress pattern with armored bottom row
                if ((r == 0 || r == 4) || ((r + c) % 2 == 0) || (c >= 3 && c <= 6)) {
                    hp = (r == 4) ? 2 : 1;
                }
            } else if (level == 3) {
                // Triple Threat: Diamond + armored wings
                int dist = std::abs(r - 2) + std::abs(c * 2 - 9) / 2;
                if (dist <= 4 || r == 0) {
                    hp = (r == 0 || r == 2) ? 2 : 1;
                }
            } else if (level == 4) {
                // Citadel: Twin towers + reinforced core
                if (c != 2 && c != 7) {
                    hp = (r == 0 || r == 4) ? 2 : ((c >= 4 && c <= 5 && r == 2) ? 3 : 1);
                }
            } else {
                // Supernova: Full armored grid
                hp = (r <= 1) ? 3 : ((r <= 3) ? 2 : 1);
            }

            bricks[r][c].hp = hp;
            bricks[r][c].maxHp = hp;
            bricks[r][c].active = (hp > 0);
            if (bricks[r][c].active) remainingBricks++;
        }
    }
}

inline void startLevel(int level) {
    currentLevel = std::max(1, std::min(Config::MAX_LEVELS, level));
    paddleX = (Config::WINDOW_WIDTH - Config::PADDLE_WIDTH) / 2.0f;
    initBricksForLevel(currentLevel);
    clearEffects();
    resetBallsOnPaddle();
    gameState = STATE_READY;
    stateBeforePause = STATE_READY;
}

inline void resetGame() {
    if (score > highScore) highScore = score;
    score = 0;
    comboStreak = 0;
    lives = Config::INITIAL_LIVES;
    startLevel(1);
}

inline void launchBall() {
    if (gameState == STATE_LEVEL_CLEAR) {
        startLevel(currentLevel + 1);
        return;
    }
    if (gameState != STATE_READY) return;

    gameState = STATE_PLAYING;
    float speed = getBallSpeedForLevel(currentLevel);
    int count = getBallsForLevel(currentLevel);

    for (int i = 0; i < Config::MAX_BALLS; ++i) {
        if (!balls[i].active) continue;
        float angle = Config::INITIAL_LAUNCH_ANGLE;
        if (count == 2) {
            angle = (i == 0) ? -0.34f : 0.34f;
        } else if (count >= 3) {
            if (i == 0) angle = -0.42f;
            else if (i == 1) angle = 0.08f;
            else angle = 0.42f;
        }
        balls[i].vx = speed * std::sin(angle);
        balls[i].vy = speed * std::cos(angle);
    }
}

inline void spawnBonusBall() {
    for (int i = 0; i < Config::MAX_BALLS; ++i) {
        if (!balls[i].active) {
            float speed = getBallSpeedForLevel(currentLevel);
            float angle = (i % 2 == 0) ? -0.28f : 0.28f;
            balls[i].active = true;
            balls[i].colorIndex = i;
            balls[i].x = paddleX + Config::PADDLE_WIDTH * 0.5f;
            balls[i].y = Config::PADDLE_Y + Config::PADDLE_HEIGHT + Config::BALL_RADIUS + 2.0f;
            balls[i].vx = speed * std::sin(angle);
            balls[i].vy = speed * std::cos(angle);
            clearBallTrail(balls[i]);
            spawnTextPopup(balls[i].x, balls[i].y + 20.0f, "EXTRA BALL!", 0.45f, 0.95f, 1.0f);
            break;
        }
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

inline bool checkCircleRectCollision(const Ball& b, float rx, float ry, float rw, float rh, float& closestX, float& closestY) {
    closestX = std::max(rx, std::min(b.x, rx + rw));
    closestY = std::max(ry, std::min(b.y, ry + rh));
    float dx = b.x - closestX;
    float dy = b.y - closestY;
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

    for (int r = 0; r < Config::BRICK_ROWS; ++r) {
        for (int c = 0; c < Config::BRICK_COLS; ++c) {
            if (bricks[r][c].hitFlash > 0.0f) {
                bricks[r][c].hitFlash = std::max(0.0f, bricks[r][c].hitFlash - dt);
            }
        }
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

inline void stepSingleBallPhysics(Ball& b, float subDt) {
    float r = static_cast<float>(Config::BALL_RADIUS);
    float speed = getBallSpeedForLevel(currentLevel);

    b.x += b.vx * subDt;
    b.y += b.vy * subDt;

    // 1. Wall collisions (left, right, top)
    if (b.x - r <= Config::FIELD_LEFT) {
        b.x = Config::FIELD_LEFT + r;
        b.vx = std::fabs(b.vx);
    } else if (b.x + r >= Config::FIELD_RIGHT) {
        b.x = Config::FIELD_RIGHT - r;
        b.vx = -std::fabs(b.vx);
    }

    if (b.y + r >= Config::FIELD_TOP) {
        b.y = Config::FIELD_TOP - r;
        b.vy = -std::fabs(b.vy);
    }

    // Bottom wall miss -> deactivate this ball; lose life only when all balls are gone
    if (b.y - r < Config::FIELD_BOTTOM) {
        b.active = false;
        if (countActiveBalls() == 0) {
            lives--;
            screenShakeTimer = 0.22f;
            if (lives > 0) {
                gameState = STATE_READY;
                resetBallsOnPaddle();
            } else {
                lives = 0;
                if (score > highScore) highScore = score;
                gameState = STATE_GAME_OVER;
                resetBallsOnPaddle();
            }
        } else {
            screenShakeTimer = 0.08f;
        }
        return;
    }

    // 2. Paddle collision
    float closestX = 0.0f, closestY = 0.0f;
    if (b.vy < 0.0f &&
        checkCircleRectCollision(b, paddleX, Config::PADDLE_Y, Config::PADDLE_WIDTH, Config::PADDLE_HEIGHT, closestX, closestY)) {
        b.y = Config::PADDLE_Y + Config::PADDLE_HEIGHT + r + 0.5f;
        paddleFlashTimer = 0.15f;
        comboStreak = 0;

        float paddleCenter = paddleX + Config::PADDLE_WIDTH / 2.0f;
        float hitOffset = (b.x - paddleCenter) / (Config::PADDLE_WIDTH / 2.0f);
        hitOffset = std::max(-1.0f, std::min(1.0f, hitOffset));

        float bounceAngle = hitOffset * Config::MAX_BOUNCE_ANGLE;
        b.vx = speed * std::sin(bounceAngle);
        b.vy = speed * std::cos(bounceAngle);
    }

    // 3. Brick collisions (supports 1/2/3-HP armored bricks)
    for (int row = 0; row < Config::BRICK_ROWS; ++row) {
        bool hitBrick = false;
        for (int col = 0; col < Config::BRICK_COLS; ++col) {
            if (!bricks[row][col].active) continue;

            float bx = bricks[row][col].x;
            float by = bricks[row][col].y;
            if (checkCircleRectCollision(b, bx, by, Config::BRICK_WIDTH, Config::BRICK_HEIGHT, closestX, closestY)) {
                bricks[row][col].hp--;
                bricks[row][col].hitFlash = 0.14f;
                comboStreak++;

                float centerBrickX = bx + Config::BRICK_WIDTH * 0.5f;
                float centerBrickY = by + Config::BRICK_HEIGHT * 0.5f;

                if (bricks[row][col].hp <= 0) {
                    bricks[row][col].active = false;
                    remainingBricks--;
                    int pts = Config::POINTS_PER_BRICK * currentLevel + (comboStreak >= 3 ? 5 : 0);
                    score += pts;
                    if (score > highScore) highScore = score;
                    spawnBrickParticles(centerBrickX, centerBrickY, Config::ROW_COLORS[row], 16);
                    spawnScorePopup(centerBrickX, centerBrickY, pts);
                } else {
                    score += 5;
                    if (score > highScore) highScore = score;
                    spawnBrickParticles(centerBrickX, centerBrickY, Config::ROW_COLORS[row], 6);
                    spawnScorePopup(centerBrickX, centerBrickY, 5);
                }

                if (comboStreak == Config::EXTRA_BALL_STREAK && countActiveBalls() < Config::MAX_BALLS) {
                    spawnBonusBall();
                }

                float overlapLeft   = (b.x + r) - bx;
                float overlapRight  = (bx + Config::BRICK_WIDTH) - (b.x - r);
                float overlapBottom = (b.y + r) - by;
                float overlapTop    = (by + Config::BRICK_HEIGHT) - (b.y - r);

                float minOverlapX = std::min(overlapLeft, overlapRight);
                float minOverlapY = std::min(overlapBottom, overlapTop);

                if (minOverlapX < minOverlapY) {
                    if (overlapLeft < overlapRight) {
                        b.x = bx - r - 0.5f;
                        b.vx = -std::fabs(b.vx);
                    } else {
                        b.x = bx + Config::BRICK_WIDTH + r + 0.5f;
                        b.vx = std::fabs(b.vx);
                    }
                } else {
                    if (overlapBottom < overlapTop) {
                        b.y = by - r - 0.5f;
                        b.vy = -std::fabs(b.vy);
                    } else {
                        b.y = by + Config::BRICK_HEIGHT + r + 0.5f;
                        b.vy = std::fabs(b.vy);
                    }
                }

                if (remainingBricks <= 0) {
                    int bonus = Config::LEVEL_CLEAR_BONUS * currentLevel;
                    score += bonus;
                    if (score > highScore) highScore = score;
                    for (int k = 0; k < Config::MAX_BALLS; ++k) {
                        balls[k].vx = 0.0f;
                        balls[k].vy = 0.0f;
                    }
                    if (currentLevel < Config::MAX_LEVELS) {
                        gameState = STATE_LEVEL_CLEAR;
                    } else {
                        gameState = STATE_WON;
                    }
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

    if (gameState == STATE_PAUSED || gameState == STATE_LEVEL_CLEAR ||
        gameState == STATE_WON || gameState == STATE_GAME_OVER) {
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
        for (int bIdx = 0; bIdx < Config::MAX_BALLS; ++bIdx) {
            if (!balls[bIdx].active) continue;
            balls[bIdx].trailSampleTimer += dt;
            if (balls[bIdx].trailSampleTimer >= 0.014f) {
                balls[bIdx].trailSampleTimer = 0.0f;
                int h = balls[bIdx].trailHead;
                balls[bIdx].trail[h].x = balls[bIdx].x;
                balls[bIdx].trail[h].y = balls[bIdx].y;
                balls[bIdx].trail[h].valid = true;
                balls[bIdx].trailHead = (h + 1) % Config::BALL_TRAIL_LENGTH;
            }
        }

        float subDt = dt / Config::SIM_SUBSTEPS;
        for (int s = 0; s < Config::SIM_SUBSTEPS; ++s) {
            if (gameState != STATE_PLAYING) break;
            for (int bIdx = 0; bIdx < Config::MAX_BALLS; ++bIdx) {
                if (!balls[bIdx].active) continue;
                stepSingleBallPhysics(balls[bIdx], subDt);
                if (gameState != STATE_PLAYING) break;
            }
        }
    }
}

#endif
