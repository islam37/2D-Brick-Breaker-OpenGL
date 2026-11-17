#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glut.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include "Bresenham.h"
#include "GameState.h"

// ============================================================================
// Renderer.h - 2D Scene Rendering, OpenGL Primitives, HUD & Status Screens
// Lead Module: Islam Uddin (ID: 231-115-063)
// ============================================================================

inline void drawText(float x, float y, void* font, const char* text) {
    glRasterPos2f(x, y);
    for (const char* p = text; *p != '\0'; ++p) {
        glutBitmapCharacter(font, *p);
    }
}

inline int getTextWidth(void* font, const char* text) {
    return glutBitmapLength(font, reinterpret_cast<const unsigned char*>(text));
}

inline void drawCenteredText(float centerX, float y, void* font, const char* text) {
    int width = getTextWidth(font, text);
    drawText(centerX - width / 2.0f, y, font, text);
}

inline void drawBackgroundAndBoundaries() {
    // 1. Playfield vertical gradient background
    glBegin(GL_QUADS);
        glColor3f(0.06f, 0.07f, 0.12f);
        glVertex2f(Config::FIELD_LEFT, Config::FIELD_BOTTOM);
        glVertex2f(Config::FIELD_RIGHT, Config::FIELD_BOTTOM);
        glColor3f(0.10f, 0.13f, 0.22f);
        glVertex2f(Config::FIELD_RIGHT, Config::FIELD_TOP);
        glVertex2f(Config::FIELD_LEFT, Config::FIELD_TOP);
    glEnd();

    // 2. Subtle grid inside playfield
    glColor4f(0.25f, 0.35f, 0.55f, 0.12f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (float x = Config::FIELD_LEFT + 40.0f; x < Config::FIELD_RIGHT; x += 40.0f) {
        glVertex2f(x, Config::FIELD_BOTTOM);
        glVertex2f(x, Config::FIELD_TOP);
    }
    for (float y = Config::FIELD_BOTTOM + 40.0f; y < Config::FIELD_TOP; y += 40.0f) {
        glVertex2f(Config::FIELD_LEFT, y);
        glVertex2f(Config::FIELD_RIGHT, y);
    }
    glEnd();

    // 3. Outer glow border around playfield
    glColor4f(0.22f, 0.62f, 0.95f, 0.25f);
    glLineWidth(5.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(Config::FIELD_LEFT - 1.5f, Config::FIELD_BOTTOM - 1.5f);
        glVertex2f(Config::FIELD_RIGHT + 1.5f, Config::FIELD_BOTTOM - 1.5f);
        glVertex2f(Config::FIELD_RIGHT + 1.5f, Config::FIELD_TOP + 1.5f);
        glVertex2f(Config::FIELD_LEFT - 1.5f, Config::FIELD_TOP + 1.5f);
    glEnd();

    // 4. Crisp inner playfield boundary
    glColor3f(0.40f, 0.65f, 0.92f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(Config::FIELD_LEFT, Config::FIELD_BOTTOM);
        glVertex2f(Config::FIELD_RIGHT, Config::FIELD_BOTTOM);
        glVertex2f(Config::FIELD_RIGHT, Config::FIELD_TOP);
        glVertex2f(Config::FIELD_LEFT, Config::FIELD_TOP);
    glEnd();

    // 5. Bottom danger-zone indicator line
    glColor4f(0.95f, 0.25f, 0.35f, 0.35f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(Config::FIELD_LEFT + 2.0f, Config::FIELD_BOTTOM + 1.0f);
        glVertex2f(Config::FIELD_RIGHT - 2.0f, Config::FIELD_BOTTOM + 1.0f);
    glEnd();

    // 6. Top HUD bar background and separator
    glColor3f(0.08f, 0.10f, 0.16f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, Config::FIELD_TOP + 4.0f);
        glVertex2f(Config::WINDOW_WIDTH, Config::FIELD_TOP + 4.0f);
        glVertex2f(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT);
        glVertex2f(0.0f, Config::WINDOW_HEIGHT);
    glEnd();

    glColor3f(0.25f, 0.38f, 0.58f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
        glVertex2f(0.0f, Config::FIELD_TOP + 4.0f);
        glVertex2f(Config::WINDOW_WIDTH, Config::FIELD_TOP + 4.0f);
    glEnd();
}

inline void drawAimGuide() {
    if (gameState != STATE_READY) return;

    float dirX = std::sin(Config::INITIAL_LAUNCH_ANGLE);
    float dirY = std::cos(Config::INITIAL_LAUNCH_ANGLE);

    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (int i = 1; i <= 7; ++i) {
        float alpha = 0.65f - i * 0.07f;
        glColor4f(0.45f, 0.88f, 1.0f, alpha);
        float px = ballX + dirX * (i * 16.0f);
        float py = ballY + dirY * (i * 16.0f);
        glVertex2f(px, py);
    }
    glEnd();
    glPointSize(1.0f);
}

inline void drawPaddle() {
    float x = paddleX;
    float y = Config::PADDLE_Y;
    float w = Config::PADDLE_WIDTH;
    float h = Config::PADDLE_HEIGHT;
    float boost = (paddleFlashTimer > 0.0f) ? 0.25f : 0.0f;

    // Soft glow beneath paddle
    glColor4f(0.18f, 0.72f, 0.98f, 0.22f + boost);
    glBegin(GL_QUADS);
        glVertex2f(x - 3.0f, y - 3.0f);
        glVertex2f(x + w + 3.0f, y - 3.0f);
        glVertex2f(x + w + 3.0f, y + h + 3.0f);
        glVertex2f(x - 3.0f, y + h + 3.0f);
    glEnd();

    // Main paddle gradient body
    glBegin(GL_QUADS);
        glColor3f(0.14f + boost, 0.56f + boost, 0.82f);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glColor3f(0.32f + boost, 0.86f + boost, 0.98f);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();

    // Top bevel highlight strip
    glColor4f(1.0f, 1.0f, 1.0f, 0.45f);
    glBegin(GL_QUADS);
        glVertex2f(x + 2.0f, y + h - 3.0f);
        glVertex2f(x + w - 2.0f, y + h - 3.0f);
        glVertex2f(x + w - 2.0f, y + h);
        glVertex2f(x + 2.0f, y + h);
    glEnd();

    // Left and right accent bumpers
    glColor3f(0.98f, 0.76f, 0.24f);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + 8.0f, y);
        glVertex2f(x + 8.0f, y + h);
        glVertex2f(x, y + h);

        glVertex2f(x + w - 8.0f, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x + w - 8.0f, y + h);
    glEnd();

    // Crisp outline
    glColor3f(0.80f, 0.96f, 1.0f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();
}

inline void drawBricks() {
    for (int r = 0; r < Config::BRICK_ROWS; ++r) {
        for (int c = 0; c < Config::BRICK_COLS; ++c) {
            if (!bricks[r][c].active) continue;

            float bx = bricks[r][c].x;
            float by = bricks[r][c].y;
            float bw = Config::BRICK_WIDTH;
            float bh = Config::BRICK_HEIGHT;
            const float* col = Config::ROW_COLORS[r];

            // Base brick body with vertical shading
            glBegin(GL_QUADS);
                glColor3f(col[0] * 0.80f, col[1] * 0.80f, col[2] * 0.80f);
                glVertex2f(bx, by);
                glVertex2f(bx + bw, by);
                glColor3f(col[0], col[1], col[2]);
                glVertex2f(bx + bw, by + bh);
                glVertex2f(bx, by + bh);
            glEnd();

            // Top 3D highlight bevel
            glColor4f(1.0f, 1.0f, 1.0f, 0.32f);
            glBegin(GL_QUADS);
                glVertex2f(bx, by + bh - 3.0f);
                glVertex2f(bx + bw, by + bh - 3.0f);
                glVertex2f(bx + bw, by + bh);
                glVertex2f(bx, by + bh);
            glEnd();

            // Bottom shadow bevel
            glColor4f(0.0f, 0.0f, 0.0f, 0.28f);
            glBegin(GL_QUADS);
                glVertex2f(bx, by);
                glVertex2f(bx + bw, by);
                glVertex2f(bx + bw, by + 3.0f);
                glVertex2f(bx, by + 3.0f);
            glEnd();

            // Crisp dark border
            glColor3f(0.08f, 0.10f, 0.15f);
            glLineWidth(1.2f);
            glBegin(GL_LINE_LOOP);
                glVertex2f(bx, by);
                glVertex2f(bx + bw, by);
                glVertex2f(bx + bw, by + bh);
                glVertex2f(bx, by + bh);
            glEnd();
        }
    }
}

inline void drawBallAndTrail() {
    // 1. Fading motion trail using Bresenham circles
    if (gameState == STATE_PLAYING) {
        glPointSize(1.5f);
        for (int i = 0; i < Config::BALL_TRAIL_LENGTH; ++i) {
            int idx = (trailHead + i) % Config::BALL_TRAIL_LENGTH;
            if (!ballTrail[idx].valid) continue;

            float frac = static_cast<float>(i + 1) / Config::BALL_TRAIL_LENGTH;
            int trailR = std::max(2, static_cast<int>(Config::BALL_RADIUS * frac * 0.75f));
            glColor4f(0.35f, 0.82f, 1.0f, frac * 0.32f);
            drawBresenhamCircle(
                static_cast<int>(std::round(ballTrail[idx].x)),
                static_cast<int>(std::round(ballTrail[idx].y)),
                trailR
            );
        }
    }

    int cx = static_cast<int>(std::round(ballX));
    int cy = static_cast<int>(std::round(ballY));

    // 2. Outer soft halo using Bresenham circle
    glPointSize(2.0f);
    glColor4f(0.38f, 0.85f, 1.0f, 0.35f);
    drawBresenhamCircle(cx, cy, Config::BALL_RADIUS + 2);

    // 3. Inner circle fill using concentric Bresenham circles
    glColor3f(0.88f, 0.95f, 1.0f);
    for (int r = 1; r < Config::BALL_RADIUS; ++r) {
        drawBresenhamCircle(cx, cy, r);
    }

    // 4. Crisp outer ball outline using Bresenham's circle algorithm
    glColor3f(1.0f, 1.0f, 1.0f);
    drawBresenhamCircle(cx, cy, Config::BALL_RADIUS);

    // 5. Specular highlight dot
    glPointSize(3.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POINTS);
        glVertex2i(cx - 2, cy + 2);
    glEnd();
    glPointSize(1.0f);
}

inline void drawParticlesAndPopups() {
    for (int i = 0; i < Config::MAX_PARTICLES; ++i) {
        if (!particles[i].active) continue;
        float alpha = particles[i].life / particles[i].maxLife;
        float hs = particles[i].size * 0.5f;
        glColor4f(particles[i].r, particles[i].g, particles[i].b, alpha);
        glBegin(GL_QUADS);
            glVertex2f(particles[i].x - hs, particles[i].y - hs);
            glVertex2f(particles[i].x + hs, particles[i].y - hs);
            glVertex2f(particles[i].x + hs, particles[i].y + hs);
            glVertex2f(particles[i].x - hs, particles[i].y + hs);
        glEnd();
    }

    for (int i = 0; i < Config::MAX_POPUPS; ++i) {
        if (!popups[i].active) continue;
        float alpha = std::min(1.0f, popups[i].life / 0.75f);
        char buf[16];
        std::snprintf(buf, sizeof(buf), "+%d", popups[i].points);
        glColor4f(1.0f, 0.92f, 0.35f, alpha);
        drawCenteredText(popups[i].x, popups[i].y, GLUT_BITMAP_HELVETICA_12, buf);
    }
}

inline void drawHUDButton(float x, float y, float w, float h, const char* label, bool hovered, bool activeAccent) {
    if (hovered) {
        glColor4f(0.24f, 0.52f, 0.85f, 0.90f);
    } else if (activeAccent) {
        glColor4f(0.85f, 0.62f, 0.15f, 0.85f);
    } else {
        glColor4f(0.14f, 0.18f, 0.28f, 0.90f);
    }
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();

    glColor3f(hovered ? 0.65f : 0.35f, hovered ? 0.88f : 0.52f, 0.95f);
    glLineWidth(1.2f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();

    glColor3f(0.95f, 0.97f, 1.0f);
    drawCenteredText(x + w * 0.5f, y + 9.0f, GLUT_BITMAP_HELVETICA_12, label);
}

inline void drawHUD() {
    // Score & High Score on the left
    char scoreBuf[64];
    if (score > highScore) highScore = score;
    std::snprintf(scoreBuf, sizeof(scoreBuf), "SCORE: %d   BEST: %d", score, highScore);
    glColor3f(0.95f, 0.97f, 1.0f);
    drawText(Config::FIELD_LEFT + 2.0f, 565.0f, GLUT_BITMAP_HELVETICA_18, scoreBuf);

    // Center streak or input guide
    if (comboStreak >= 2 && gameState == STATE_PLAYING) {
        char comboBuf[48];
        std::snprintf(comboBuf, sizeof(comboBuf), "STREAK x%d!", comboStreak);
        glColor3f(0.98f, 0.82f, 0.25f);
        drawCenteredText(344.0f, 566.0f, GLUT_BITMAP_HELVETICA_18, comboBuf);
    } else {
        glColor3f(0.68f, 0.76f, 0.90f);
        drawCenteredText(
            344.0f,
            567.0f,
            GLUT_BITMAP_HELVETICA_12,
            "Arrows/Drag: Move | SPACE: Launch"
        );
    }

    // Clickable Pause/Resume & Restart buttons
    const char* pauseLabel = (gameState == STATE_PAUSED) ? "RESUME (P)" : "PAUSE (P)";
    drawHUDButton(
        Config::BTN_PAUSE_X,
        Config::BTN_Y,
        Config::BTN_W,
        Config::BTN_H,
        pauseLabel,
        hoverPauseBtn,
        gameState == STATE_PAUSED
    );
    drawHUDButton(
        Config::BTN_RESTART_X,
        Config::BTN_Y,
        Config::BTN_W,
        Config::BTN_H,
        "RESTART (R)",
        hoverRestartBtn,
        false
    );

    // Lives text and Bresenham circle indicators on the right
    glColor3f(0.95f, 0.97f, 1.0f);
    drawText(Config::FIELD_RIGHT - 122.0f, 565.0f, GLUT_BITMAP_HELVETICA_18, "LIVES:");

    glPointSize(1.5f);
    for (int i = 0; i < Config::INITIAL_LIVES; ++i) {
        int lx = static_cast<int>(Config::FIELD_RIGHT - 46.0f + i * 19.0f);
        int ly = 571;
        if (i < lives) {
            glColor3f(0.96f, 0.34f, 0.44f);
            for (int r = 1; r <= 6; ++r) {
                drawBresenhamCircle(lx, ly, r);
            }
            glColor3f(1.0f, 0.82f, 0.86f);
            drawBresenhamCircle(lx - 2, ly + 2, 1);
        } else {
            glColor3f(0.32f, 0.36f, 0.46f);
            drawBresenhamCircle(lx, ly, 6);
        }
    }
    glPointSize(1.0f);
}

inline void drawStatusBanner(const char* title, const char* subtitle, float r, float g, float b) {
    float boxW = 460.0f;
    float boxH = 118.0f;
    float boxX = (Config::WINDOW_WIDTH - boxW) / 2.0f;
    float boxY = (Config::WINDOW_HEIGHT - boxH) / 2.0f - 25.0f;

    // Drop shadow
    glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
    glBegin(GL_QUADS);
        glVertex2f(boxX + 6.0f, boxY - 6.0f);
        glVertex2f(boxX + boxW + 6.0f, boxY - 6.0f);
        glVertex2f(boxX + boxW + 6.0f, boxY + boxH - 6.0f);
        glVertex2f(boxX + 6.0f, boxY + boxH - 6.0f);
    glEnd();

    // Translucent dark panel
    glColor4f(0.08f, 0.10f, 0.17f, 0.92f);
    glBegin(GL_QUADS);
        glVertex2f(boxX, boxY);
        glVertex2f(boxX + boxW, boxY);
        glVertex2f(boxX + boxW, boxY + boxH);
        glVertex2f(boxX, boxY + boxH);
    glEnd();

    // Top accent bar
    glColor4f(r, g, b, 0.85f);
    glBegin(GL_QUADS);
        glVertex2f(boxX, boxY + boxH - 5.0f);
        glVertex2f(boxX + boxW, boxY + boxH - 5.0f);
        glVertex2f(boxX + boxW, boxY + boxH);
        glVertex2f(boxX, boxY + boxH);
    glEnd();

    // Banner border
    glColor3f(r, g, b);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(boxX, boxY);
        glVertex2f(boxX + boxW, boxY);
        glVertex2f(boxX + boxW, boxY + boxH);
        glVertex2f(boxX, boxY + boxH);
    glEnd();

    // Title and subtitle text
    glColor3f(r, g, b);
    drawCenteredText(Config::WINDOW_WIDTH / 2.0f, boxY + 70.0f, GLUT_BITMAP_HELVETICA_18, title);

    glColor3f(0.88f, 0.92f, 0.98f);
    drawCenteredText(Config::WINDOW_WIDTH / 2.0f, boxY + 32.0f, GLUT_BITMAP_9_BY_15, subtitle);
}

inline void drawStatusOverlay() {
    char subBuf[96];
    if (gameState == STATE_READY) {
        drawStatusBanner(
            "READY TO LAUNCH",
            "Press SPACE or Click/Tap Playfield to Launch",
            0.35f, 0.82f, 0.98f
        );
    } else if (gameState == STATE_PAUSED) {
        drawStatusBanner(
            "GAME PAUSED",
            "Press P or Click RESUME  |  Press R to Restart",
            0.98f, 0.82f, 0.28f
        );
    } else if (gameState == STATE_WON) {
        std::snprintf(subBuf, sizeof(subBuf), "Final Score: %d  |  Click or Press R / SPACE", score);
        drawStatusBanner(
            "YOU WIN! ALL BRICKS CLEARED!",
            subBuf,
            0.32f, 0.90f, 0.56f
        );
    } else if (gameState == STATE_GAME_OVER) {
        std::snprintf(subBuf, sizeof(subBuf), "Final Score: %d  |  Click or Press R / SPACE", score);
        drawStatusBanner(
            "GAME OVER",
            subBuf,
            0.96f, 0.36f, 0.42f
        );
    }
}

#endif
