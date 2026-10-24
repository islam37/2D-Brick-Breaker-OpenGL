@echo off
echo ========================================================
echo   Building 2D Brick Breaker Game (OpenGL / FreeGLUT)
echo   Metropolitan University - CSE 402 Lab
echo ========================================================

if not exist bin mkdir bin

g++ -O2 -Wall -std=c++11 -DFREEGLUT_STATIC -I./include -L./lib src\main.cpp -lfreeglut_static -lopengl32 -lglu32 -lgdi32 -lwinmm -o bin\BrickBreaker.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [SUCCESS] Build completed successfully: bin\BrickBreaker.exe
    echo [INFO] Self-contained standalone executable created!
    echo Run the game with: run.bat
) else (
    echo.
    echo [ERROR] Build failed with error code %ERRORLEVEL%
)
