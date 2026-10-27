@echo off
setlocal
cd /d "%~dp0"
set "PYTHONPATH=%~dp0.python-deps;%PYTHONPATH%"
if exist "%USERPROFILE%\miniconda3\python.exe" (
    "%USERPROFILE%\miniconda3\python.exe" main.py
) else (
    where py >nul 2>nul
    if errorlevel 1 (
        python main.py
    ) else (
        py main.py
    )
)
if errorlevel 1 pause
endlocal
