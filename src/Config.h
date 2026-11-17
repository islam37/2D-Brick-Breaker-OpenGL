#ifndef CONFIG_H
#define CONFIG_H

namespace Config {
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Playfield boundaries
    const float FIELD_LEFT = 20.0f;
    const float FIELD_RIGHT = 780.0f;
    const float FIELD_BOTTOM = 20.0f;
    const float FIELD_TOP = 540.0f;

    const int FPS = 60;
    const int TIMER_MS = 1000 / FPS;
    const int SIM_SUBSTEPS = 4;

    // Paddle settings
    const float PADDLE_WIDTH = 110.0f;
    const float PADDLE_HEIGHT = 14.0f;
    const float PADDLE_Y = 40.0f;
    const float PADDLE_SPEED = 460.0f;

    // Ball settings
    const int BALL_RADIUS = 8;
    const float BALL_SPEED = 350.0f;
    const float MAX_BOUNCE_ANGLE = 1.05f; // ~60 degrees in radians
    const int BALL_TRAIL_LENGTH = 10;

    // Brick grid settings
    const int BRICK_ROWS = 5;
    const int BRICK_COLS = 10;
    const float BRICK_WIDTH = 66.0f;
    const float BRICK_HEIGHT = 22.0f;
    const float BRICK_PADDING_X = 6.0f;
    const float BRICK_PADDING_Y = 6.0f;
    const float BRICK_START_X = 43.0f;
    const float BRICK_START_Y = 490.0f;

    // Top HUD interactive buttons (for mouse / touchscreen)
    const float BTN_Y = 556.0f;
    const float BTN_W = 78.0f;
    const float BTN_H = 28.0f;
    const float BTN_PAUSE_X = 460.0f;
    const float BTN_RESTART_X = 546.0f;

    // Visual effects limits
    const int MAX_PARTICLES = 160;
    const int MAX_POPUPS = 16;

    // Gameplay rules
    const int INITIAL_LIVES = 3;
    const int POINTS_PER_BRICK = 10;
}

#endif
