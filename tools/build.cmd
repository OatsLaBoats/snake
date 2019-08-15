@ECHO OFF

SETLOCAL ENABLEEXTENSIONS

CLS

SET startingTime=%TIME%
SET buildMessage=[92mSucceeded[0m

CD ..\build

SET commonCompilerDebugFlags=-Od -MT -Oi -FC -W4 -Z7 -Wno-unused-parameter -Wno-unused-function -DSNAKE_INTERNAL=1 -DSNAKE_SLOW=1 -DSNAKE_WIN32=1
SET commonCompilerOptimizationFlags=-O2 -MT -Oi -FC -W4 -Wno-unused-parameter -Wno-unused-function -DSNAKE_INTERNAL=0 -DSNAKE_SLOW=0 -DSNAKE_WIN32=1
SET commonaLinkerFlags=-incremental:no -opt:ref -subsystem:console Winmm.lib

CALL clang-cl %commonCompilerOptimizationFlags% ..\win32_snake.c -o win32_snake.exe /link %commonaLinkerFlags%

SET /A clangReturn=%ERRORLEVEL%

SET endTime=%TIME%

IF %clangReturn% NEQ 0 (
    SET buildMessage=[91mFailed[0m
)

CD ..\tools

ECHO(
ECHO Build %buildMessage%
ECHO Staring time:%startingTime%  /  End time:%endTime%

ENDLOCAL