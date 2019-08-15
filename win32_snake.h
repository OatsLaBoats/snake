#ifndef WIN32_SNAKE_H
#define WIN32_SNAKE_H

typedef struct
{
    char *consoleTitle;

    HANDLE inputHandle;
    HANDLE bufferHandle;

    SMALL_RECT windowSize;
    SMALL_RECT writeArea;

    COORD bufferSize;
    COORD writeBufferSize;
    COORD upperLeftCell;

    DWORD modeFlags;

    CONSOLE_CURSOR_INFO cursorInfo;

    CONSOLE_FONT_INFOEX fontInfo;

    CHAR_INFO *charInfoBuffer;

} win32_console;


#endif