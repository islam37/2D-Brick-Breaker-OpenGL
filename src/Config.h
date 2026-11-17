#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// Config.h - Project Constants & Scene Parameters
// Course: CSE 402 (Computer Graphics & Image Processing Lab)
// ============================================================================

namespace Config {
    // Logical window dimensions (4:3 aspect ratio)
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Playfield boundaries
    const float FIELD_LEFT = 20.0f;
    const float FIELD_RIGHT = 780.0f;
    const float FIELD_BOTTOM = 20.0f;
    const float FIELD_TOP = 540.0f;

    // Frame timing & physics sub-stepping
    const int FPS = 60;
    const int TIMER_MS = 1000 / FPS;
    const int SIM_SUBSTEPS = 4;

    // Paddle parameters
    const float PADDLE_WIDTH = 110.0f;
    const float PADDLE_HEIGHT = 14.0f;
    const float PADDLE_Y = 40.0f;
    const float PADDLE_SPEED = 460.0f;

    // Ball parameters
    const int BALL_RADIUS = 8;
    const float BALL_SPEED = 350.0f;
    const float MAX_BOUNCE_ANGLE = 1.05f; // ~60 degrees in radians
    const int BALL_TRAIL_LENGTH = 10;

    // Brick grid parameters (5 rows x 10 columns = 50 bricks)
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

    // Brick row RGB colors
    const float ROW_COLORS[BRICK_ROWS][3] = {
        {0.96f, 0.31f, 0.42f}, // Row 0: Crimson Rose
        {0.98f, 0.58f, 0.22f}, // Row 1: Sunset Orange
        {0.96f, 0.84f, 0.25f}, // Row 2: Golden Amber
        {0.24f, 0.86f, 0.54f}, // Row 3: Emerald Mint
        {0.26f, 0.66f, 0.98f}  // Row 4: Electric Cyan-Blue
    };
}

#endif
