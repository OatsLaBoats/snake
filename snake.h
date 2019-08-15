#ifndef SNAKE_H
#define SNAKE_H

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef float real32;
typedef double real64;

typedef int bool32;
#define true 1
#define false 0

#define INTERNAL static
#define GLOBAL_VAR static
#define PERSIST static

#if SNAKE_SLOW
#define Assert(expression) if(!(expression)) {*(volatile int *)0 = 0;}
#else
#define Assert(expression)
#endif

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

#define BG_COLOR_WHITE          (0xF0)
#define BG_COLOR_GREY_LIGHT     (0x70)
#define BG_COLOR_GREY_DARK      (0x80)
#define BG_COLOR_BLACK          (0x00)

#define BG_COLOR_CYAN_LIGHT     (0xB0)
#define BG_COLOR_CYAN           (0x30)
#define BG_COLOR_BLUE_LIGHT     (0x90)
#define BG_COLOR_BLUE           (0x10)

#define BG_COLOR_RED_LIGHT      (0xC0)
#define BG_COLOR_RED            (0x40)

#define BG_COLOR_GREEN_LIGHT    (0xA0)
#define BG_COLOR_GREEN          (0x20)

#define BG_COLOR_MAGENTA_LIGHT  (0xD0)
#define BG_COLOR_MAGENTA        (0x50)

#define BG_COLOR_YELLOW_LIGHT   (0xE0)
#define BG_COLOR_YELLOW         (0x60)

#define FG_COLOR_WHITE          (0x0F)
#define FG_COLOR_GREY_LIGHT     (0x07)
#define FG_COLOR_GREY_DARK      (0x08)
#define FG_COLOR_BLACK          (0x00)

#define FG_COLOR_CYAN_LIGHT     (0x0B)
#define FG_COLOR_CYAN           (0x03)
#define FG_COLOR_BLUE_LIGHT     (0x09)
#define FG_COLOR_BLUE           (0x01)

#define FG_COLOR_RED_LIGHT      (0x0C)
#define FG_COLOR_RED            (0x04)

#define FG_COLOR_GREEN_LIGHT    (0x0A)
#define FG_COLOR_GREEN          (0x02)

#define FG_COLOR_MAGENTA_LIGHT  (0x0D)
#define FG_COLOR_MAGENTA        (0x05)

#define FG_COLOR_YELLOW_LIGHT   (0x0E)
#define FG_COLOR_YELLOW         (0x06)

#define SetCharacter(structName, index, characterValue) (*(structName->character + ((structName->characterJump) * (index))) = (characterValue))
#define SetAttribute(structName, index, attributeValue) (*(structName->attribute + ((structName->attributeJump) * (index))) = (attributeValue))
#define CalculatePoint(x, y, rowWidth) ((y) * (rowWidth) + (x))

#define TILE_GROUND 0
#define TILE_FRUIT 1
#define TILE_HEAD 2

typedef enum
{
    UP = 3,
    DOWN = 4,
    LEFT = 5,
    RIGHT = 6

} direction;

typedef struct
{
    int32 width;
    int32 height;

    uint32 structSize;
    uint32 characterJump;
    uint32 attributeJump;
    
    int16 *character;
    int16 *attribute;

} game_window;

typedef struct
{
    bool32 start;

    bool32 moveUp;
    bool32 moveDown;
    bool32 moveLeft;
    bool32 moveRight;

} game_input;

typedef struct
{
    real32 lastX;
    real32 lastY;

    real32 X;
    real32 Y;

} map_position;

typedef struct
{
    real32 lastMoveX;
    real32 lastMoveY;

    uint32 partWidth;
    uint32 partHeight;

    map_position head;
    
    int32 tailX;
    int32 tailY;

    direction headDirection;
    direction prevDirection;

    bool32 hasMoved;

} snake;

typedef struct
{
    int32 *tiles;
    
    uint32 width;
    uint32 height;

    int32 tileSize;

} game_map;

typedef struct
{
    bool32 initialize;

    game_map map;

    snake player;

    real32 secondsElapsed;

    bool32 gameIsOver;
    bool32 quitGame;

    bool32 spawnFruit;
    bool32 firstMove;

    int32 score;
    char *scoreBuffer;
    
} game_state;

INTERNAL void GameUpdateAndRender(game_window *gameWindow, game_input *gameInput, game_state *gameState);

#endif