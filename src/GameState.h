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

// Global game state variables
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
TrailPoint ballTrail[Config::BALL_TRAIL_LENGTH];
int trailHead = 0;
float trailSampleTimer = 0.0f;

#endif
