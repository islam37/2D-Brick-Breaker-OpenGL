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
A single-player 2D Brick Breaker desktop game built in C++ using OpenGL, GLU, and FreeGLUT. The project demonstrates core 2D computer graphics concepts from CSE 402:
- **OpenGL Primitives:** `GL_QUADS`, `GL_LINES`, `GL_LINE_LOOP`, and `GL_POINTS` for rendering the playfield, paddle, and $5 \times 10$ brick layout.
- **Bresenham's Circle Drawing Algorithm:** Reusable integer midpoint/Bresenham circle routine (`drawBresenhamCircle`) with 8-way symmetry used to render the ball and life indicators.
- **2D Coordinate Translation & Animation:** Time-based position updates (`glutTimerFunc` at 60 FPS with sub-stepped collision physics) and aspect-ratio-preserving viewport reshaping (`gluOrtho2D` + `glViewport`).
- **Keyboard Interaction & Game States:** Smooth paddle control, ball launch, pause/resume, restart, win condition, and 3-life game-over state with GLUT bitmap text (`glutBitmapCharacter`).

---

## 2. Controls

| Key | Action |
| :--- | :--- |
| **Left / Right Arrow** (or **A / D**) | Move the paddle horizontally within the playfield |
| **Space** | Launch the ball from the paddle (`READY` state) |
| **P** | Pause / Resume gameplay |
| **R** | Restart the game (resets score, 3 lives, and all bricks) |
| **ESC** | Exit the application |

---

## 3. Gameplay Rules
- **Brick Grid:** 5 rows $\times$ 10 columns (50 bricks total). Each destroyed brick awards **10 points** (maximum score: **500**).
- **Lives:** The player starts with **3 lives**. Missing the ball at the bottom boundary costs 1 life and resets the ball onto the paddle in the `READY` state.
- **Winning & Losing:** Clearing all 50 bricks triggers the **YOU WIN!** screen; losing all 3 lives triggers the **GAME OVER** screen.

---

## 4. Project Structure

```text
2D-Brick-Breaker-OpenGL/
├── src/
│   ├── Config.h        # Window, boundary, paddle, ball, brick, and gameplay constants
│   └── main.cpp        # Rendering (Bresenham circle, primitives, HUD), input, and collision logic
├── build.bat           # Windows batch script to compile bin\BrickBreaker.exe
├── run.bat             # Builds (if needed) and launches bin\BrickBreaker.exe
├── Makefile            # Make build configuration
└── README.md           # Setup, controls, and project documentation
```

---

## 5. How to Build and Run (Windows / MinGW + FreeGLUT)

### Prerequisites
- **MinGW (`g++`)** installed and added to your system `PATH` with FreeGLUT (`libfreeglut_static.a` and `<GL/glut.h>`).

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

### Option C: Using `make`
```bat
mingw32-make
bin\BrickBreaker.exe
```
