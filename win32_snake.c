#include "snake.c"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "win32_snake.h"

GLOBAL_VAR uint64 globalPerfCountFrequency;

INTERNAL bool32 Win32SetConsoleSettings(win32_console *newConsole, win32_console *oldConsole)
{
    if(!SetConsoleTitleA(newConsole->consoleTitle))
    {
        return true;
    }

    COORD oldFont = oldConsole->fontInfo.dwFontSize;
    COORD newFont = newConsole->fontInfo.dwFontSize;

    int newFontSize = newFont.X + newFont.Y;
    int oldFontSize = oldFont.X + oldFont.Y;

    bool32 fontIsSmaller = (newFontSize < oldFontSize);

    if(fontIsSmaller)
    {
        if(!SetCurrentConsoleFontEx(newConsole->bufferHandle, FALSE, &newConsole->fontInfo))
        {
            return true;
        }
    }

    bool32 XIsSmaller = (oldConsole->windowSize.Right > newConsole->windowSize.Right);
    bool32 YIsSmaller = (oldConsole->windowSize.Bottom > newConsole->windowSize.Bottom);

    if(XIsSmaller || YIsSmaller)
    {
        if(XIsSmaller)
        {
            SMALL_RECT smallerX = {0};
            smallerX.Right = newConsole->windowSize.Right;
            smallerX.Bottom = oldConsole->windowSize.Bottom;

            if(!SetConsoleWindowInfo(newConsole->bufferHandle, TRUE, &smallerX))
            {
                return 1;
            }  
        }
        else if(YIsSmaller)
        {
            SMALL_RECT smallerY = {0};
            smallerY.Bottom = newConsole->windowSize.Bottom;
            smallerY.Right = oldConsole->windowSize.Right;

            if(!SetConsoleWindowInfo(newConsole->bufferHandle, TRUE, &smallerY))
            {
                return 2;
            }  
        }

        if(!SetConsoleScreenBufferSize(newConsole->bufferHandle, newConsole->bufferSize))
        {
            return 3;
        }

        if(!SetConsoleWindowInfo(newConsole->bufferHandle, TRUE, &newConsole->windowSize))
        {
            return 4;
        }   
    }
    else
    {
        if(!SetConsoleWindowInfo(newConsole->bufferHandle, TRUE, &newConsole->windowSize))
        {
            return 5;
        }

        if(!SetConsoleScreenBufferSize(newConsole->bufferHandle, newConsole->bufferSize))
        {
            return 6;
        }
    }

    if(!fontIsSmaller)
    {
        if(!SetCurrentConsoleFontEx(newConsole->bufferHandle, FALSE, &newConsole->fontInfo))
        {
            return true;
        }
    }

    if(!SetConsoleCursorInfo(newConsole->bufferHandle, &newConsole->cursorInfo))
    {
        return true;
    }

    if(!SetConsoleMode(newConsole->inputHandle, newConsole->modeFlags))
    {
        return true;
    }    

    return false;
}

INTERNAL void Win32ProcessConsoleEvents(bool32 *gameIsRunning, win32_console *console, INPUT_RECORD *eventBuffer, DWORD bufferLength, game_input *gameInput)
{
    DWORD numOfEvents = 0;
    DWORD numOfEventsRead = 0;

    GetNumberOfConsoleInputEvents(console->inputHandle, &numOfEvents);

    if(numOfEvents)
    {
        ReadConsoleInputA(console->inputHandle, eventBuffer, bufferLength, &numOfEventsRead);
        
        if(numOfEventsRead)
        {
            for(DWORD eventIndex = 0; eventIndex < numOfEventsRead; ++eventIndex)
            {
                if(eventBuffer[eventIndex].EventType == KEY_EVENT)
                {
                    if(eventBuffer[eventIndex].Event.KeyEvent.wVirtualKeyCode == 'W')
                    {
                        gameInput->moveUp = eventBuffer[eventIndex].Event.KeyEvent.bKeyDown;
                    }
                    else if(eventBuffer[eventIndex].Event.KeyEvent.wVirtualKeyCode == 'S')
                    {
                        gameInput->moveDown = eventBuffer[eventIndex].Event.KeyEvent.bKeyDown;
                    }
                    else if(eventBuffer[eventIndex].Event.KeyEvent.wVirtualKeyCode == 'A')
                    {
                        gameInput->moveLeft = eventBuffer[eventIndex].Event.KeyEvent.bKeyDown;
                    }
                    else if(eventBuffer[eventIndex].Event.KeyEvent.wVirtualKeyCode == 'D')
                    {
                        gameInput->moveRight = eventBuffer[eventIndex].Event.KeyEvent.bKeyDown;
                    }
                    else if(eventBuffer[eventIndex].Event.KeyEvent.wVirtualKeyCode == 'P')
                    {
                        gameInput->start = eventBuffer[eventIndex].Event.KeyEvent.bKeyDown;
                    }
                    else if(eventBuffer[eventIndex].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
                    {
                        if(eventBuffer[eventIndex].Event.KeyEvent.bKeyDown)
                        {
                            *gameIsRunning = false;
                        }
                    }
                }
            }
        }
    }

}

INTERNAL inline LARGE_INTEGER Win32GetWallClock(void)
{
    LARGE_INTEGER result;

    QueryPerformanceCounter(&result);

    return result;
}

INTERNAL inline real32 Win32GetSecondsElapsed(LARGE_INTEGER start, LARGE_INTEGER end)
{
    real32 result = ((real32)(end.QuadPart - start.QuadPart) / globalPerfCountFrequency);

    return result;
}

INTERNAL void Win32ClearConsoleBuffer(game_window *window)
{
    int32 height = window->height;
    int32 width = window->width;

    for(int y = 0; y < height; ++y)
    {
        for(int x = 0; x < width; ++x)
        {
            int pos = CalculatePoint(x, y, width);

            SetCharacter(window, pos, ' ');
            SetAttribute(window, pos, BG_COLOR_BLACK);
        }
    }
}

int main(int argumentCount, char **commandLine)
{
    srand(time(0));

    int32 targetFramerate = 60;
    int32 tileSize = 3;

    real32 targetSecondsPerFrame = 1.0f / (real32)targetFramerate;
    
    COORD fontSize;
    fontSize.X = 16;
    fontSize.Y = 16;

    WCHAR fontName[] = L"Lucida Console";

    uint32 windowWidth = 72;
    uint32 windowHeight = 51;
    uint32 bufferWidth = windowWidth;
    uint32 bufferHeight = windowHeight;

    UINT desiredSchedulerMS = 1;
    bool32 sleepIsGranular = (timeBeginPeriod(desiredSchedulerMS) == TIMERR_NOERROR);

    //get the consoles current state so you can reset it after ending
    LARGE_INTEGER perfCountFrequencyResult;
    QueryPerformanceFrequency(&perfCountFrequencyResult);
    globalPerfCountFrequency = perfCountFrequencyResult.QuadPart;

    HANDLE bufferHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);

    win32_console console = {0};
    win32_console oldConsole = {0};

    CONSOLE_SCREEN_BUFFER_INFO oldScreenBufferInfo;
    if(!GetConsoleScreenBufferInfo(bufferHandle, &oldScreenBufferInfo))
    {
        return 1;
    }

    CONSOLE_FONT_INFOEX oldFontInfo;
    oldFontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    if(!GetCurrentConsoleFontEx(bufferHandle, FALSE, &oldFontInfo))
    {
        return 1;
    }

    uint32 currentWindowSizeX = (oldScreenBufferInfo.srWindow.Right - oldScreenBufferInfo.srWindow.Left);
    uint32 currentWindowSizeY = (oldScreenBufferInfo.srWindow.Bottom - oldScreenBufferInfo.srWindow.Top);
    uint32 currentBufferSizeX = (oldScreenBufferInfo.dwSize.X);
    uint32 currentBufferSizeY = (oldScreenBufferInfo.dwSize.Y);

    oldConsole.consoleTitle = "Closing";
    oldConsole.bufferHandle = bufferHandle;
    oldConsole.inputHandle = inputHandle;
    oldConsole.windowSize.Top = 0;
    oldConsole.windowSize.Left = 0;
    oldConsole.windowSize.Right = currentWindowSizeX;
    oldConsole.windowSize.Bottom = currentWindowSizeY;
    oldConsole.bufferSize.X = currentBufferSizeX;
    oldConsole.bufferSize.Y = currentBufferSizeY;
    oldConsole.writeArea.Top = 0;
    oldConsole.writeArea.Left = 0;
    oldConsole.writeArea.Right = currentBufferSizeX;
    oldConsole.writeArea.Bottom = currentBufferSizeY;
    oldConsole.upperLeftCell.X = 0;
    oldConsole.upperLeftCell.Y = 0;
    oldConsole.writeBufferSize.X = currentBufferSizeX;
    oldConsole.writeBufferSize.Y = currentBufferSizeY;
    oldConsole.fontInfo.cbSize = oldFontInfo.cbSize;
    oldConsole.fontInfo.nFont = oldFontInfo.nFont;
    oldConsole.fontInfo.dwFontSize.X = oldFontInfo.dwFontSize.X;
    oldConsole.fontInfo.dwFontSize.Y = oldFontInfo.dwFontSize.Y;
    oldConsole.fontInfo.FontFamily = oldFontInfo.FontFamily;
    oldConsole.fontInfo.FontWeight = oldFontInfo.FontWeight;
    wcsncpy_s(oldConsole.fontInfo.FaceName, sizeof(oldConsole.fontInfo.FaceName), oldFontInfo.FaceName, sizeof(oldFontInfo.FaceName));

    if(!GetConsoleCursorInfo(oldConsole.bufferHandle, &oldConsole.cursorInfo))
    {
        return 1;
    }

    if(!GetConsoleMode(oldConsole.inputHandle, &oldConsole.modeFlags))
    {
        return 1;
    }

    console.consoleTitle = "Snake";
    console.bufferHandle = bufferHandle;
    console.inputHandle = inputHandle;
    console.windowSize.Top = 0;
    console.windowSize.Left = 0;
    console.windowSize.Right = windowWidth - 1;
    console.windowSize.Bottom = windowHeight - 1;
    console.bufferSize.X = bufferWidth;
    console.bufferSize.Y = bufferHeight;
    console.writeArea.Top = 0;
    console.writeArea.Left = 0;
    console.writeArea.Right = windowWidth;
    console.writeArea.Bottom = windowHeight;
    console.upperLeftCell.X = 0;
    console.upperLeftCell.Y = 0;
    console.writeBufferSize.X = windowWidth;
    console.writeBufferSize.Y = windowHeight;
    console.modeFlags = (ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
    console.cursorInfo.bVisible = FALSE;
    console.cursorInfo.dwSize = oldConsole.cursorInfo.dwSize;
    console.fontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    console.fontInfo.nFont = 0;
    console.fontInfo.dwFontSize.X = fontSize.X;
    console.fontInfo.dwFontSize.Y = fontSize.Y;
    console.fontInfo.FontFamily = FF_DONTCARE;
    console.fontInfo.FontWeight = FW_NORMAL;
    wcsncpy_s(console.fontInfo.FaceName, sizeof(console.fontInfo.FaceName), fontName, sizeof(fontName));

    uint64 charInfoBufferSize = (sizeof(CHAR_INFO) * ((uint64)bufferWidth * (uint64)bufferHeight));

    console.charInfoBuffer = calloc(1, charInfoBufferSize);
    if(!console.charInfoBuffer)
    {
        return 1;
    }

    game_window gameWindow = {0};
    gameWindow.width = bufferWidth;
    gameWindow.height = bufferHeight;
    gameWindow.structSize = sizeof(CHAR_INFO);
    gameWindow.attributeJump = 2;
    gameWindow.attribute = (int16 *)&console.charInfoBuffer->Attributes;
    gameWindow.characterJump = 2;
    gameWindow.character = (int16 *)&console.charInfoBuffer->Char.AsciiChar;

    INPUT_RECORD eventBuffer[128] = {{0}};
    game_input gameInput = {0};

    game_state gameState;
    gameState.secondsElapsed = 0;
    gameState.gameIsOver = false;
    gameState.initialize = true;
    gameState.spawnFruit = true;
    gameState.firstMove = true;
    gameState.score = 0;
    gameState.scoreBuffer = (char [20]){0};
    
    snake *player = &gameState.player;
    player->lastMoveX = 0;
    player->lastMoveY = 0;
    player->head.X = 3.0f;
    player->head.Y = 3.0f;
    player->tailX = 2;
    player->tailY = 3;
    player->headDirection = RIGHT;
    player->partWidth = tileSize;
    player->partHeight = tileSize;
    player->hasMoved = true;

    game_map *map = &gameState.map;
    map->tileSize = tileSize;
    map->width = windowWidth / map->tileSize;
    map->height = windowHeight / map->tileSize;

    uint64 mapSize = sizeof(int32) * (map->width * map->height);

    map->tiles = calloc(1, mapSize);
    if(!map->tiles)
    {
        return 1;
    }

    if(Win32SetConsoleSettings(&console, &oldConsole))
    {
        return 1;
    }

    /*
    int framesUntilFPSDisplay = 30;
    real32 FPSSample = 0.0f;
    real32 displayFPS = 0;
    */

    LARGE_INTEGER lastCounter = Win32GetWallClock();
    
    uint64 lastCycleCount = __rdtsc();

    bool32 gameIsRunning = true;
    while(gameIsRunning)
    {
        Win32ProcessConsoleEvents(&gameIsRunning, &console, eventBuffer, 128, &gameInput);
        GameUpdateAndRender(&gameWindow, &gameInput, &gameState);

        if(!WriteConsoleOutputA(console.bufferHandle, console.charInfoBuffer, console.bufferSize, console.upperLeftCell, &console.writeArea))
        {
            gameIsRunning = false;
        }

        LARGE_INTEGER workCounter = Win32GetWallClock();
        real32 secondsElapsedForFrame = Win32GetSecondsElapsed(lastCounter, workCounter);

        if(secondsElapsedForFrame < targetSecondsPerFrame)
        {
            if(sleepIsGranular)
            {
                //converting to ms
                DWORD sleepMS = ((DWORD)(1000.0f * (targetSecondsPerFrame - secondsElapsedForFrame)));
                if(sleepMS > 0)
                {
                    Sleep(sleepMS);
                }
            }

            while(secondsElapsedForFrame < targetSecondsPerFrame)
            {
                secondsElapsedForFrame = Win32GetSecondsElapsed(lastCounter, Win32GetWallClock());
            }
        }
        else
        {
            
        }

        LARGE_INTEGER endCounter = Win32GetWallClock();

        gameState.secondsElapsed = Win32GetSecondsElapsed(lastCounter, endCounter);

        real32 MSPerFrame = 1000.0f * Win32GetSecondsElapsed(lastCounter, endCounter);
        int64 counterElapsed = endCounter.QuadPart - lastCounter.QuadPart;
        lastCounter = endCounter;

        uint64 endCycleCount = __rdtsc();
        uint64 cyclesElapsed = endCycleCount - lastCycleCount;
        lastCycleCount = endCycleCount;
        real32 MCPF = (real32)cyclesElapsed / (1000.0f * 1000.0f);
        
        real32 FPS = (real32)globalPerfCountFrequency / (real32)counterElapsed;

        /*
        FPSSample = FPSSample + FPS;

        if(framesUntilFPSDisplay-- == 0)
        {
           displayFPS = FPSSample / 30.0f;
            FPSSample = 0;
            framesUntilFPSDisplay = 30;
        }
        */
        

        char FPSBuffer[256];
        snprintf(FPSBuffer, sizeof(FPSBuffer), "%s   %.02fms/f   %.02ff/s   %.02fmc/f", console.consoleTitle, MSPerFrame, FPS, MCPF);
        SetConsoleTitleA(FPSBuffer);
    }

    Win32ClearConsoleBuffer(&gameWindow);
    WriteConsoleOutputA(console.bufferHandle, console.charInfoBuffer, console.bufferSize, console.upperLeftCell, &console.writeArea);

    if(Win32SetConsoleSettings(&oldConsole, &console))
    {
        return 1;
    }

    return 0;
}