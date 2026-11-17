# 2D Brick Breaker Game Using OpenGL

**Course:** Computer Graphics & Image Processing Lab (CSE 402)  
**Department:** Computer Science and Engineering, Metropolitan University, Sylhet  

### Submitted To
- **Shrabanti Chowdhury**, Lecturer, Department of CSE

### Submitted By
- **Islam Uddin** (ID: `231-115-063`)
- **Yousuf Bin Nazib** (ID: `231-115-064`)
- **Batch:** 58 | **Section:** B

---

## 1. Overview
A single-player 2D Brick Breaker game developed in C++ using OpenGL, GLU, and FreeGLUT (with a companion mobile-playable web edition in `docs/index.html`). The project demonstrates core 2D computer graphics concepts from CSE 402:
- **OpenGL Primitives:** `GL_QUADS`, `GL_LINES`, `GL_LINE_LOOP`, and `GL_POINTS` with alpha blending (`GL_BLEND`) for rendering the gradient playfield, 3D-beveled $5 \times 10$ brick layout, glowing paddle, and particle bursts.
- **Bresenham's Circle Drawing Algorithm:** Dedicated integer Bresenham circle module ([src/Bresenham.h](src/Bresenham.h)) using 8-way octant symmetry (`plotCirclePoints`, `drawBresenhamCircle`) to render the ball outline, inner core, 10-point fading motion trail, and life indicator icons.
- **2D Coordinate Translation & Animation:** Time-stepped physics updates (`glutTimerFunc` at 60 FPS with 4 sub-steps per frame) and aspect-ratio-preserving viewport reshaping (`gluOrtho2D` + `glViewport`).
- **Keyboard, Mouse & Touchscreen Interaction:** Smooth paddle movement via keyboard or mouse/touch dragging, clickable top-bar **Pause / Restart** buttons, and GLUT bitmap HUD/overlay text.

---

## 2. Controls (Desktop & Mobile/Touch)

| Input | Action |
| :--- | :--- |
| **Left / Right Arrow** (or **A / D**) | Move the paddle horizontally within the playfield |
| **Mouse / Touch Drag** | Drag inside the playfield to move the paddle smoothly |
| **Space** or **Left Click / Tap** | Launch the ball from the paddle (`READY` state) |
| **P** or **PAUSE Button** | Pause / Resume gameplay |
| **R** or **RESTART Button** | Restart the game (resets score, 3 lives, and all 50 bricks) |
| **ESC** | Exit the desktop application |

---

## 3. Modular Project Structure & Viva Responsibilities

```text
2D-Brick-Breaker-OpenGL/
├── src/
│   ├── main.cpp        # Application entry point, OpenGL init, display, reshape & timer loop
│   ├── Config.h        # Window, boundary, paddle, ball, brick, and color constants
│   ├── Bresenham.h     # Reusable Bresenham circle-drawing algorithm (8-way symmetry)
│   ├── GameState.h     # Structs (Brick, Particle, ScorePopup, TrailPoint) & shared state
│   ├── Renderer.h      # [Islam Uddin] Scene rendering, 3D-beveled bricks, HUD & banners
│   ├── Input.h         # [Islam Uddin] Keyboard, mouse/touch-drag & viewport coordinate mapping
│   └── Physics.h       # [Yousuf Bin Nazib] Ball motion, Circle-AABB collisions, scoring & lives
├── docs/
│   └── index.html      # Mobile & browser playable edition (GitHub Pages)
├── build.bat           # Windows batch script to compile bin\BrickBreaker.exe
├── run.bat             # Builds (if needed) and launches bin\BrickBreaker.exe
├── Makefile            # Make build configuration
└── README.md           # Setup, architecture, controls, and gameplay documentation
```

---

## 4. How to Build and Run (Windows / MinGW + FreeGLUT)

### Option A: Using Batch Scripts (Recommended)
1. **Build the executable:**
   ```bat
   .\build.bat
   ```
2. **Run the game:**
   ```bat
   .\run.bat
   ```

### Option B: Direct `g++` Command
```bat
if not exist bin mkdir bin
g++ src\main.cpp -DFREEGLUT_STATIC -lfreeglut_static -lopengl32 -lglu32 -lgdi32 -lwinmm -o bin\BrickBreaker.exe
bin\BrickBreaker.exe
```

---

## 5. Playing on Mobile Phone
Open the GitHub Pages link on any Android or iPhone browser:
- **Live Mobile / Web Link:** `https://islam37.github.io/2D-Brick-Breaker-OpenGL/`
- Touch and drag anywhere on the arena (or use the on-screen **◀ Left / Right ▶ / Launch / Pause / Restart** buttons) to play.
