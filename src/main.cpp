#include "Bresenham.h"
#include "Config.h"
#include "GameState.h"
#include "Input.h"
#include "Physics.h"
#include "Renderer.h"
#include <GL/glut.h>
#include <ctime>

// ============================================================================
// main.cpp - OpenGL / FreeGLUT Application Entry Point & Callbacks
// Course: CSE 402 (Computer Graphics & Image Processing Lab)
// Authors: Islam Uddin (231-115-063) & Yousuf Bin Nazib (231-115-064)
// ============================================================================

void init() {
  glClearColor(0.05f, 0.06f, 0.10f, 1.0f);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, Config::WINDOW_WIDTH, 0.0, Config::WINDOW_HEIGHT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  std::srand(static_cast<unsigned int>(std::time(nullptr)));
  resetGame();
  lastTimeMs = glutGet(GLUT_ELAPSED_TIME);
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();

  if (screenShakeTimer > 0.0f) {
    float shakeX = static_cast<float>((std::rand() % 7) - 3);
    float shakeY = static_cast<float>((std::rand() % 7) - 3);
    glTranslatef(shakeX, shakeY, 0.0f);
  }

  drawBackgroundAndBoundaries();
  drawHUD();
  drawBricks();
  drawAimGuide();
  drawPaddle();
  drawBallAndTrail();
  drawParticlesAndPopups();
  drawStatusOverlay();

  glutSwapBuffers();
}

void reshape(int w, int h) {
  if (h == 0)
    h = 1;
  currentWinH = h;

  float targetAspect =
      static_cast<float>(Config::WINDOW_WIDTH) / Config::WINDOW_HEIGHT;
  float windowAspect = static_cast<float>(w) / h;

  currentVpX = 0;
  currentVpY = 0;
  currentVpW = w;
  currentVpH = h;
  if (windowAspect > targetAspect) {
    currentVpW = static_cast<int>(h * targetAspect);
    currentVpX = (w - currentVpW) / 2;
  } else {
    currentVpH = static_cast<int>(w / targetAspect);
    currentVpY = (h - currentVpH) / 2;
  }

  glViewport(currentVpX, currentVpY, currentVpW, currentVpH);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, Config::WINDOW_WIDTH, 0.0, Config::WINDOW_HEIGHT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void timer(int val) {
  (void)val;
  int nowMs = glutGet(GLUT_ELAPSED_TIME);
  float dt = (nowMs - lastTimeMs) / 1000.0f;
  lastTimeMs = nowMs;
  if (dt < 0.0f)
    dt = 0.0f;
  if (dt > 0.05f)
    dt = 0.05f;

  updateGame(dt);
  glutPostRedisplay();
  glutTimerFunc(Config::TIMER_MS, timer, 0);
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("2D Brick Breaker - CSE 402");

  init();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutSpecialFunc(specialDown);
  glutSpecialUpFunc(specialUp);
  glutKeyboardFunc(keyboardDown);
  glutKeyboardUpFunc(keyboardUp);
  glutMouseFunc(mouseClick);
  glutMotionFunc(mouseMotion);
  glutPassiveMotionFunc(mousePassiveMotion);
  glutTimerFunc(Config::TIMER_MS, timer, 0);

  glutMainLoop();
  return 0;
}
