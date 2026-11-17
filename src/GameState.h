#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Config.h"

// ============================================================================
// GameState.h - Data Structures & Shared Game State
// ============================================================================

enum GameState {
    STATE_READY,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_LEVEL_CLEAR,
    STATE_WON,
    STATE_GAME_OVER
};

struct Brick {
    float x, y;
    int hp;
    int maxHp;
    float hitFlash;
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
    char text[24];
    float r, g, b;
    float life;
    bool active;
};

struct TrailPoint {
    float x, y;
    bool valid;
};

struct Ball {
    float x, y;
    float vx, vy;
    bool active;
    int colorIndex;
    TrailPoint trail[Config::BALL_TRAIL_LENGTH];
    int trailHead;
    float trailSampleTimer;
};

// Global game state variables
GameState gameState = STATE_READY;
GameState stateBeforePause = STATE_READY;
int currentLevel = 1;
int score = 0;
int highScore = 0;
int comboStreak = 0;
int lives = Config::INITIAL_LIVES;
int remainingBricks = Config::BRICK_ROWS * Config::BRICK_COLS;

// Paddle and multi-ball state
float paddleX = (Config::WINDOW_WIDTH - Config::PADDLE_WIDTH) / 2.0f;
Ball balls[Config::MAX_BALLS];
float paddleFlashTimer = 0.0f;
float screenShakeTimer = 0.0f;

// Input & UI state
bool moveLeft = false;
bool moveRight = false;
bool isDraggingPaddle = false;
bool hoverPauseBtn = false;
bool hoverRestartBtn = false;
int lastTimeMs = 0;

// Viewport tracking for aspect-ratio-preserving resize & mouse/touch mapping
int currentVpX = 0;
int currentVpY = 0;
int currentVpW = Config::WINDOW_WIDTH;
int currentVpH = Config::WINDOW_HEIGHT;
int currentWinH = Config::WINDOW_HEIGHT;

// Scene objects and effects arrays
Brick bricks[Config::BRICK_ROWS][Config::BRICK_COLS];
Particle particles[Config::MAX_PARTICLES];
ScorePopup popups[Config::MAX_POPUPS];

#endif
