@echo off
REM ================================================================================
REM Install static csalt library (tests OFF)
REM - Default prefix: %ProgramFiles%\csalt
REM - Default build type: Release
REM - Static lib via BUILD_SHARED_LIBS=OFF (csalt target -> libcsalt.lib)
REM
REM Usage:
REM   static.bat
REM   static.bat --prefix "C:\SDK\csalt"
REM   static.bat --relwithdebinfo
REM   static.bat --generator "Visual Studio 17 2022" --arch x64
REM   static.bat --clean
REM ================================================================================

setlocal enabledelayedexpansion

REM --------------------- Defaults ---------------------
set "PREFIX=%ProgramFiles%\csalt"
set "BUILD_DIR=build\static"
set "BUILD_TYPE=Release"
set "GEN="
set "ARCH="
set CLEAN=0

REM --------------------- Parse args -------------------
:parse_args
if "%~1"=="" goto args_done

if "%~1"=="--prefix" (
    set "PREFIX=%~2"
    shift & shift & goto parse_args
)
if "%~1"=="--release" (
    set "BUILD_TYPE=Release"
    shift & goto parse_args
)
if "%~1"=="--relwithdebinfo" (
    set "BUILD_TYPE=RelWithDebInfo"
    shift & goto parse_args
)
if "%~1"=="--rel" (
    set "BUILD_TYPE=RelWithDebInfo"
    shift & goto parse_args
)
if "%~1"=="--debug" (
    set "BUILD_TYPE=Debug"
    shift & goto parse_args
)
if "%~1"=="--generator" (
    set "GEN=%~2"
    shift & shift & goto parse_args
)
if "%~1"=="--arch" (
    set "ARCH=%~2"
    shift & shift & goto parse_args
)
if "%~1"=="--clean" (
    set CLEAN=1
    shift & goto parse_args
)
if "%~1"=="-h"  goto :help
if "%~1"=="--help" goto :help

echo Unknown arg: %~1
exit /b 1

:args_done

REM --------------------- Paths ------------------------
set "SCRIPT_DIR=%~dp0"
pushd "%SCRIPT_DIR%\..\.."
set "PROJ_ROOT=%CD%"
popd

set "SRC_DIR=%PROJ_ROOT%\csalt"
set "OUT_DIR=%PROJ_ROOT%\%BUILD_DIR%"

REM --------------------- Clean if requested -----------
if %CLEAN%==1 (
    echo Cleaning "%OUT_DIR%"
    rmdir /s /q "%OUT_DIR%" 2>nul
)

if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"

REM --------------------- Configure --------------------
REM With BUILD_SHARED_LIBS=OFF, 'csalt' is a static library target.
set CMAKE_ARGS=-S "%SRC_DIR%" -B "%OUT_DIR%" -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DCMAKE_INSTALL_PREFIX="%PREFIX%" -DBUILD_SHARED_LIBS=OFF -DCSALT_BUILD_TESTS=OFF -DCSALT_BUILD_STATIC=OFF

if not "%GEN%"=="" (
    set CMAKE_ARGS=%CMAKE_ARGS% -G "%GEN%"
    if not "%ARCH%"=="" set CMAKE_ARGS=%CMAKE_ARGS% -A %ARCH%
)

echo ==^> Configuring: cmake %CMAKE_ARGS%
cmake %CMAKE_ARGS%
if errorlevel 1 exit /b 1

REM --------------------- Build ------------------------
echo ==^> Building (type: %BUILD_TYPE%)
cmake --build "%OUT_DIR%" --config %BUILD_TYPE%
if errorlevel 1 exit /b 1

REM --------------------- Install ----------------------
echo ==^> Installing to "%PREFIX%"
cmake --install "%OUT_DIR%" --config %BUILD_TYPE%
if errorlevel 1 (
    echo.
    echo ERROR: Install failed. If installing under "Program Files", re-run from an elevated Developer Command Prompt.
    exit /b 2
)

echo.
echo ==^> Static install complete
echo     Headers: %PREFIX%\include\csalt\*.h
echo     Library: %PREFIX%\lib\csalt.lib
exit /b 0

:help
echo Usage: static.bat [--prefix DIR] [--release^|--relwithdebinfo^|--debug] [--generator "NAME"] [--arch x64] [--clean]
exit /b 0

:: ================================================================================
:: ================================================================================
:: eof
