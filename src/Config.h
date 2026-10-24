#ifndef CONFIG_H
#define CONFIG_H

namespace Config {
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // boundaries
    const float FIELD_LEFT = 20.0f;
    const float FIELD_RIGHT = 780.0f;
    const float FIELD_BOTTOM = 20.0f;
    const float FIELD_TOP = 540.0f;

    const int FPS = 60;
    const int TIMER_MS = 1000 / FPS;
}

#endif
