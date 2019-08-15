#include "snake.h"
#include "charArt.h"
#include <stdlib.h>
#include <stdio.h>

INTERNAL inline bool32 IsMapPointEmpty(game_map* map, uint32 tileX, uint32 tileY)
{
    bool32 isValid = true;

    if(map->tiles[tileY * map->width + tileX])
    {
        isValid = false;
    }

    return isValid;
}

INTERNAL inline bool32 IsPlayerInMap(int32 mapWidth, int32 mapHeight, int32 tileX, int32 tileY)
{
    bool32 isValid = true;

    if(tileX >= mapWidth)
    {
        isValid = false;
    }

    if(tileY >= mapHeight)
    {
        isValid = false;
    }

    if(tileX < 0)
    {
        isValid = false;
    }

    if(tileY < 0)
    {
        isValid = false;
    }

    return isValid;
}

INTERNAL void DrawRectangle(game_window *window, int minX, int minY, int maxX, int maxY, uint32 color, char character)
{
    for(int Y = minY; Y < maxY; ++Y)
    {
        for(int X = minX; X < maxX; ++X)
        {
            int point = CalculatePoint(X, Y, window->width);
            SetCharacter(window, point, character);
            SetAttribute(window, point, color);
        }
    }
}

INTERNAL void DrawLetter(int *letter, game_window *window, int32 startX, int32 startY, uint32 color)
{
    int32 X = startX;
    int32 Y = startY;

    for(int32 letterIndexY = 0; letterIndexY < LETTER_HEIGHT; ++letterIndexY)
    {
        for(int32 letterIndexX = 0; letterIndexX < LETTER_WIDTH; ++letterIndexX)
        {
            int32 letterPoint = CalculatePoint(letterIndexX, letterIndexY, LETTER_WIDTH);
            int32 bufferPoint = CalculatePoint(X, Y, window->width);

            if(letter[letterPoint] == 1)
            {
                SetAttribute(window, bufferPoint, color);
            }

            ++X;
        }

        X = startX;
        ++Y;
    }
}

INTERNAL void DisplayText(char *text, game_window *window, int32 startX, int32 startY, uint32 bgColor, uint32 fgColor)
{
    int32 point = CalculatePoint(startX, startY, window->width);

    while(*text != '\0')
    {
        SetCharacter(window, point, *text);
        SetAttribute(window, point, (bgColor | fgColor));
        ++text;
        ++point;
    }
}

INTERNAL void RenderGameMap(game_map *map, game_window *window)
{
    int bufferX = 0;
    int bufferY = 0;

    for(uint32 mapY = 0; mapY < map->height; ++mapY)
    {
        for(uint32 mapX = 0; mapX < map->width; ++mapX)
        {
            bufferX = mapX * map->tileSize;
            bufferY = mapY * map->tileSize;
            uint32 color = 0;
            bool32 isFruit = false;

            int mapPoint = CalculatePoint(mapX, mapY, map->width);
            if(map->tiles[mapPoint] == TILE_GROUND)
            {
                color = BG_COLOR_GREY_DARK;
            }
            else if(map->tiles[mapPoint] == TILE_FRUIT)
            {
                color = BG_COLOR_MAGENTA_LIGHT;
                isFruit = true;
            }
            else if(map->tiles[mapPoint] == TILE_HEAD)
            {
                color = BG_COLOR_CYAN;
            }
            else
            {
                color = BG_COLOR_GREEN_LIGHT;
            }

            if(isFruit)
            {
                SetAttribute(window, CalculatePoint(bufferX + 0, bufferY + 0, window->width), BG_COLOR_GREY_DARK);
                SetAttribute(window, CalculatePoint(bufferX + 1, bufferY + 0, window->width), color);
                SetAttribute(window, CalculatePoint(bufferX + 2, bufferY + 0, window->width), BG_COLOR_GREY_DARK);
                SetAttribute(window, CalculatePoint(bufferX + 0, bufferY + 1, window->width), color);
                SetAttribute(window, CalculatePoint(bufferX + 1, bufferY + 1, window->width), color);
                SetAttribute(window, CalculatePoint(bufferX + 2, bufferY + 1, window->width), color);
                SetAttribute(window, CalculatePoint(bufferX + 0, bufferY + 2, window->width), BG_COLOR_GREY_DARK);
                SetAttribute(window, CalculatePoint(bufferX + 1, bufferY + 2, window->width), color);
                SetAttribute(window, CalculatePoint(bufferX + 2, bufferY + 2, window->width), BG_COLOR_GREY_DARK);

                SetCharacter(window, CalculatePoint(bufferX + 0, bufferY + 0, window->width), ' ');
                SetCharacter(window, CalculatePoint(bufferX + 1, bufferY + 0, window->width), ' ');
                SetCharacter(window, CalculatePoint(bufferX + 2, bufferY + 0, window->width), ' ');
                SetCharacter(window, CalculatePoint(bufferX + 0, bufferY + 1, window->width), ' ');
                SetCharacter(window, CalculatePoint(bufferX + 1, bufferY + 1, window->width), ' ');
                SetCharacter(window, CalculatePoint(bufferX + 2, bufferY + 1, window->width), ' ');
                SetCharacter(window, CalculatePoint(bufferX + 0, bufferY + 2, window->width), ' ');
                SetCharacter(window, CalculatePoint(bufferX + 1, bufferY + 2, window->width), ' ');
                SetCharacter(window, CalculatePoint(bufferX + 2, bufferY + 2, window->width), ' ');
            }
            else
            {
                DrawRectangle(window, bufferX, bufferY, bufferX + map->tileSize, bufferY + map->tileSize, color, ' ');
            }
        }
    }
}

INTERNAL void SpawnFruit(game_map *map)
{
    int32 randLimit = map->width * map->height;
    int32 randomTile = rand() % randLimit;

    if(map->tiles[randomTile] == TILE_GROUND)
    {
        map->tiles[randomTile] = TILE_FRUIT;
    }
    else
    {
        bool32 tileFound = false;

        for(int index = randomTile; index < randLimit; ++index)
        {
            if(map->tiles[index] == TILE_GROUND)
            {
                map->tiles[index] = TILE_FRUIT;
                tileFound = true;
                break;
            }
        }

        if(!tileFound)
        {
            for(int index = randomTile; index >= 0; --index)
            {
                if(map->tiles[index] == TILE_GROUND)
                {
                    map->tiles[index] = TILE_FRUIT;
                    break;
                }
            }
        }
    }
}

INTERNAL void UpdateGame(game_map *map, snake *player, game_state *gameState)
{
    int32 lastX = (int32)player->head.lastX;
    int32 lastY = (int32)player->head.lastY;

    int32 X = (int32)player->head.X;
    int32 Y = (int32)player->head.Y;

    int32 tailX = player->tailX;
    int32 tailY = player->tailY;

    bool32 growSnake = false;

    if(!gameState->firstMove)
    {
        if(!IsMapPointEmpty(map, X, Y))
        {
            if(map->tiles[CalculatePoint(X, Y, map->width)] == TILE_FRUIT)
            {
                ++gameState->score;
                growSnake = true;
                gameState->spawnFruit = true;
            }
            else if(map->tiles[CalculatePoint(X, Y, map->width)] != TILE_HEAD)
            {
                gameState->gameIsOver = true;
            }
        }
    }

    map->tiles[CalculatePoint(X, Y, map->width)] = TILE_HEAD;
    
    if((lastX != X) || (lastY != Y))
    {
        player->hasMoved = true;

        map->tiles[CalculatePoint(lastX, lastY, map->width)] = player->headDirection;
        
        if(!growSnake)
        {
            direction d = map->tiles[CalculatePoint(tailX, tailY, map->width)];

            if(d == UP)
            {
                --player->tailY;
            }
            else if(d == DOWN)
            {
                ++player->tailY;
            }
            else if(d == LEFT)
            {
                --player->tailX;
            }
            else if(d == RIGHT)
            {
                ++player->tailX;
            }

            map->tiles[CalculatePoint(tailX, tailY, map->width)] = TILE_GROUND;
        }
    }

    if(gameState->spawnFruit)
    {
        SpawnFruit(map);
        gameState->spawnFruit = false;
    }
}

INTERNAL void DrawText(char *text, game_window *window, int32 startX, int32 startY, uint32 color)
{
    while((*text != '\0') && (startX < window->width))
    {
        int32 *letter;

        switch(*text)
        {
            case '0':
            {
                letter = LETTER_0;

            } break;

            case '1':
            {
                letter = LETTER_1;

            } break;

            case '2':
            {
                letter = LETTER_2;

            } break;

            case '3':
            {
                letter = LETTER_3;

            } break;

            case '4':
            {
                letter = LETTER_4;

            } break;

            case '5':
            {
                letter = LETTER_5;

            } break;

            case '6':
            {
                letter = LETTER_6;

            } break;

            case '7':
            {
                letter = LETTER_7;

            } break;

            case '8':
            {
                letter = LETTER_8;

            } break;

            case '9':
            {
                letter = LETTER_9;

            } break;

            case ':':
            {
                letter = LETTER_COLON;

            } break;

            case 'A':
            {
                letter = LETTER_A;

            } break;

            case 'B':
            {
                letter = LETTER_B;

            } break;

            case 'C':
            {
                letter = LETTER_C;

            } break;

            case 'D':
            {
                letter = LETTER_D;

            } break;

            case 'E':
            {
                letter = LETTER_E;

            } break;

            case 'F':
            {
                letter = LETTER_F;

            } break;

            case 'G':
            {
                letter = LETTER_G;

            } break;

            case 'H':
            {
                letter = LETTER_H;

            } break;

            case 'I':
            {
                letter = LETTER_I;

            } break;

            case 'J':
            {
                letter = LETTER_J;

            } break;

            case 'K':
            {
                letter = LETTER_K;

            } break;

            case 'L':
            {
                letter = LETTER_L;

            } break;
            
            case 'M':
            {
                letter = LETTER_M;

            } break;

            case 'N':
            {
                letter = LETTER_N;

            } break;

            case 'O':
            {
                letter = LETTER_O;

            } break;

            case 'P':
            {
                letter = LETTER_P;

            } break;

            case 'Q':
            {
                letter = LETTER_Q;

            } break;

            case 'R':
            {
                letter = LETTER_R;

            } break;

            case 'S':
            {
                letter = LETTER_S;

            } break;

            case 'T':
            {
                letter = LETTER_T;

            } break;

            case 'U':
            {
                letter = LETTER_U;

            } break;

            case 'V':
            {
                letter = LETTER_V;

            } break;
            
            case 'W':
            {
                letter = LETTER_W;

            } break;

            case 'X':
            {
                letter = LETTER_X;

            } break;

            case 'Y':
            {
                letter = LETTER_Y;

            } break;

            case 'Z':
            {
                letter = LETTER_Z;

            } break;

            default:
            {
                letter = LETTER_SPACE;

            } break;
        }

        DrawLetter(letter, window, startX, startY, color);

        startX += LETTER_WIDTH + 1;
        ++text;
    }
}

INTERNAL void GameUpdateAndRender(game_window *window, game_input *gameInput, game_state *gameState)
{
    snake *player = &gameState->player;
    game_map *map = &gameState->map;

    if(gameState->initialize)
    {
        gameState->initialize = false;

        map->tiles[CalculatePoint(player->tailX, player->tailY, map->width)] = player->headDirection;
    }

    real32 tilesPerSecond = 4.0f;
    real32 tilesToMove = gameState->secondsElapsed * tilesPerSecond;

    real32 newPlayerX = 0.0f;
    real32 newPlayerY = 0.0f;

    if(!gameState->gameIsOver)
    {
        newPlayerX = player->head.X;
        newPlayerY = player->head.Y;

        player->head.lastX = player->head.X;
        player->head.lastY = player->head.Y;

        if((gameInput->moveUp) && (player->headDirection != DOWN) && (player->hasMoved))
        {
            gameState->firstMove = false;
            newPlayerY -= tilesToMove;
            newPlayerY -= player->lastMoveY;
            player->lastMoveY = -tilesToMove;
            player->lastMoveX = 0.0f;
            player->headDirection = UP;
            player->hasMoved = false;
        }
        else if((gameInput->moveDown) && (player->headDirection != UP) && (player->hasMoved))
        {
            gameState->firstMove = false;
            newPlayerY += tilesToMove;
            newPlayerY += player->lastMoveY;
            player->lastMoveY = tilesToMove;
            player->lastMoveX = 0.0f;
            player->headDirection = DOWN;
            player->hasMoved = false;
        }
        else if((gameInput->moveLeft) && (player->headDirection != RIGHT) && (player->hasMoved))
        {
            gameState->firstMove = false;
            newPlayerX -= tilesToMove;
            newPlayerX -= player->lastMoveX;
            player->lastMoveY = 0.0f;
            player->lastMoveX = -tilesToMove;
            player->headDirection = LEFT;
            player->hasMoved = false;
        }
        else if((gameInput->moveRight) && (player->headDirection != LEFT) && (player->hasMoved))
        {
            gameState->firstMove = false;
            newPlayerX += tilesToMove;
            newPlayerX += player->lastMoveX;
            player->lastMoveY = 0.0f;
            player->lastMoveX = tilesToMove;
            player->headDirection = RIGHT;
            player->hasMoved = false;
        }
        else
        {
            newPlayerX += player->lastMoveX;
            newPlayerY += player->lastMoveY;
        }
    }

    if((IsPlayerInMap(map->width, map->height, (int32)newPlayerX, (int32)newPlayerY)) && (!gameState->gameIsOver))
    {
        player->head.X = newPlayerX;
        player->head.Y = newPlayerY;

        UpdateGame(map, player, gameState);

        RenderGameMap(map, window);

        snprintf(gameState->scoreBuffer, 20, "SCORE: %d", gameState->score);
        DisplayText(gameState->scoreBuffer, window, (window->width / 2) - 6, 0, BG_COLOR_WHITE, FG_COLOR_BLACK);
    }
    else
    {
        gameState->gameIsOver = true;

        DrawRectangle(window, 0, 0, window->width, window->height, BG_COLOR_WHITE, ' ');

        snprintf(gameState->scoreBuffer, 20, "%d", gameState->score);

        DrawText("GAME OVER", window, 9, 10, BG_COLOR_RED);
        DrawText("SCORE:", window, 9, 10 + LETTER_HEIGHT + 10, BG_COLOR_GREEN);
        DrawText(gameState->scoreBuffer, window, 9 + (LETTER_HEIGHT * 4) + 4, 10 + LETTER_HEIGHT + 10, BG_COLOR_YELLOW);
    }
}