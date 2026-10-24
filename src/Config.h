#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// Metropolitan University, Sylhet
// Department of Computer Science & Engineering
// Course: CSE 402 - Computer Graphics & Image Processing Lab
// Project: 2D Brick Breaker Game Using OpenGL
//
// Team Members:
//   Islam Uddin       (ID: 231-115-063) - Rendering, Paddle & Interface
//   Yousuf Bin Nazib  (ID: 231-115-064) - Ball Dynamics, Collision & Game Logic
//
// Supervised by:
//   Shrabanti Chowdhury, Lecturer, Department of CSE
// ============================================================================

namespace Config {
    // Window settings
    const int WINDOW_WIDTH  = 800;
    const int WINDOW_HEIGHT = 600;
    const char* const WINDOW_TITLE = "2D Brick Breaker Game - OpenGL (CSE 402 Lab)";

    // Playfield boundaries (leave space at top for HUD)
    const float FIELD_LEFT   = 20.0f;
    const float FIELD_RIGHT  = 780.0f;
    const float FIELD_BOTTOM = 20.0f;
    const float FIELD_TOP    = 540.0f;

    // Target FPS & timer update rate (60 FPS -> ~16 ms)
    const int FPS = 60;
    const int TIMER_INTERVAL_MS = 1000 / FPS;

    // Color definitions (R, G, B floats 0.0 - 1.0)
    struct Color {
        float r, g, b;
    };

    const Color COLOR_BG           = {0.08f, 0.09f, 0.13f}; // Deep sleek navy/slate
    const Color COLOR_WALL         = {0.25f, 0.32f, 0.45f}; // Boundary wall slate blue
    const Color COLOR_WALL_ACCENT  = {0.40f, 0.55f, 0.75f};
    const Color COLOR_TEXT_WHITE   = {0.95f, 0.95f, 0.98f};
    const Color COLOR_TEXT_MUTED   = {0.60f, 0.65f, 0.75f};
    const Color COLOR_TEXT_ACCENT  = {0.30f, 0.85f, 0.95f}; // Electric cyan
}

#endif // CONFIG_H
