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

    // Paddle settings
    const float PADDLE_WIDTH = 110.0f;
    const float PADDLE_HEIGHT = 14.0f;
    const float PADDLE_Y = 40.0f;
    const float PADDLE_SPEED = 450.0f;

    // Ball settings
    const int BALL_RADIUS = 8;

    // Brick grid settings
    const int BRICK_ROWS = 5;
    const int BRICK_COLS = 10;
    const float BRICK_WIDTH = 66.0f;
    const float BRICK_HEIGHT = 22.0f;
    const float BRICK_PADDING_X = 6.0f;
    const float BRICK_PADDING_Y = 6.0f;
    const float BRICK_START_X = 43.0f;
    const float BRICK_START_Y = 490.0f;

    // Paddle settings
    const float PADDLE_WIDTH = 110.0f;
    const float PADDLE_HEIGHT = 14.0f;
    const float PADDLE_Y = 40.0f;
    const float PADDLE_SPEED = 450.0f;

    // Ball settings
    const int BALL_RADIUS = 8;

    // Brick grid settings
    const int BRICK_ROWS = 5;
    const int BRICK_COLS = 10;
    const float BRICK_WIDTH = 66.0f;
    const float BRICK_HEIGHT = 22.0f;
    const float BRICK_PADDING_X = 6.0f;
    const float BRICK_PADDING_Y = 6.0f;
    const float BRICK_START_X = 43.0f;
    const float BRICK_START_Y = 490.0f;
}

#endif
