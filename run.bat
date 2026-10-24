@echo off
if not exist bin\BrickBreaker.exe (
    echo [INFO] Executable not found. Compiling first...
    call build.bat
)

if exist bin\BrickBreaker.exe (
    echo [INFO] Launching 2D Brick Breaker Game...
    start bin\BrickBreaker.exe
)
