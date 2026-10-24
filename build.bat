@echo off
if not exist bin mkdir bin

g++ src\main.cpp -DFREEGLUT_STATIC -lfreeglut_static -lopengl32 -lglu32 -lgdi32 -lwinmm -o bin\BrickBreaker.exe

if %ERRORLEVEL% EQU 0 (
    echo Build successful: bin\BrickBreaker.exe
) else (
    echo Build failed.
)
