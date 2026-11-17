#ifndef INPUT_H
#define INPUT_H

#include <GL/glut.h>
#include <cstdlib>
#include "Physics.h"

// ============================================================================
// Input.h - Keyboard, Special Keys & Mouse/Touchscreen Interaction
// Lead Module: Islam Uddin (ID: 231-115-063)
// ============================================================================

// Convert window pixel coordinates (mx, my) into logical scene coordinates (wx, wy)
inline bool windowToWorld(int mx, int my, float& wx, float& wy) {
    int glY = currentWinH - my;
    if (currentVpW <= 0 || currentVpH <= 0) return false;
    wx = static_cast<float>(mx - currentVpX) / currentVpW * Config::WINDOW_WIDTH;
    wy = static_cast<float>(glY - currentVpY) / currentVpH * Config::WINDOW_HEIGHT;
    return (wx >= 0.0f && wx <= Config::WINDOW_WIDTH && wy >= 0.0f && wy <= Config::WINDOW_HEIGHT);
}

inline bool pointInRect(float px, float py, float rx, float ry, float rw, float rh) {
    return (px >= rx && px <= rx + rw && py >= ry && py <= ry + rh);
}

inline void specialDown(int key, int x, int y) {
    (void)x; (void)y;
    if (key == GLUT_KEY_LEFT) moveLeft = true;
    if (key == GLUT_KEY_RIGHT) moveRight = true;
}

inline void specialUp(int key, int x, int y) {
    (void)x; (void)y;
    if (key == GLUT_KEY_LEFT) moveLeft = false;
    if (key == GLUT_KEY_RIGHT) moveRight = false;
}

inline void keyboardDown(unsigned char key, int x, int y) {
    (void)x; (void)y;
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
    if (key == 27) std::exit(0);
}

inline void keyboardUp(unsigned char key, int x, int y) {
    (void)x; (void)y;
    if (key == 'a' || key == 'A') moveLeft = false;
    if (key == 'd' || key == 'D') moveRight = false;
}

inline void mouseClick(int button, int state, int x, int y) {
    if (button != GLUT_LEFT_BUTTON) return;

    if (state == GLUT_UP) {
        isDraggingPaddle = false;
        return;
    }

    float wx = 0.0f, wy = 0.0f;
    if (!windowToWorld(x, y, wx, wy)) return;

    if (state == GLUT_DOWN) {
        // 1. Check HUD Pause / Restart buttons
        if (pointInRect(wx, wy, Config::BTN_PAUSE_X, Config::BTN_Y, Config::BTN_W, Config::BTN_H)) {
            togglePause();
            return;
        }
        if (pointInRect(wx, wy, Config::BTN_RESTART_X, Config::BTN_Y, Config::BTN_W, Config::BTN_H)) {
            resetGame();
            return;
        }

        // 2. If Won or Game Over, clicking inside playfield restarts
        if (gameState == STATE_WON || gameState == STATE_GAME_OVER) {
            resetGame();
            return;
        }

        // 3. Inside playfield: drag paddle and/or launch ball
        if (wy <= Config::FIELD_TOP && (gameState == STATE_READY || gameState == STATE_PLAYING)) {
            isDraggingPaddle = true;
            paddleX = wx - Config::PADDLE_WIDTH * 0.5f;
            clampPaddle();
            if (gameState == STATE_READY && wy > Config::PADDLE_Y + Config::PADDLE_HEIGHT + 20.0f) {
                launchBall();
            }
        }
    }
}

inline void mouseMotion(int x, int y) {
    (void)y;
    if (!isDraggingPaddle || (gameState != STATE_READY && gameState != STATE_PLAYING)) return;
    if (currentVpW <= 0) return;

    float wx = static_cast<float>(x - currentVpX) / currentVpW * Config::WINDOW_WIDTH;
    paddleX = wx - Config::PADDLE_WIDTH * 0.5f;
    clampPaddle();
}

inline void mousePassiveMotion(int x, int y) {
    float wx = 0.0f, wy = 0.0f;
    if (!windowToWorld(x, y, wx, wy)) {
        hoverPauseBtn = false;
        hoverRestartBtn = false;
        return;
    }
    hoverPauseBtn = pointInRect(wx, wy, Config::BTN_PAUSE_X, Config::BTN_Y, Config::BTN_W, Config::BTN_H);
    hoverRestartBtn = pointInRect(wx, wy, Config::BTN_RESTART_X, Config::BTN_Y, Config::BTN_W, Config::BTN_H);
}

#endif
