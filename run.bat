@echo off
if not exist bin\BrickBreaker.exe call build.bat
if exist bin\BrickBreaker.exe start bin\BrickBreaker.exe
