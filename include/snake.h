#ifndef SNAKE_H
#define SNAKE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t x, y;
} Point;

typedef enum {
    MOVE_UP,
    MOVE_RIGHT,
    MOVE_DOWN,
    MOVE_LEFT
} MoveDirection;

typedef enum {
    PLAYING,
    VICTORY,
    DEFEAT
} GameState;

typedef struct {
    Point game_field_size;

    uint8_t snake_length;
    MoveDirection snake_move_direction;
    Point *snake_pos;

    Point fruit_pos;

    GameState game_state;
} SnakeGame;

void init_game(SnakeGame *game, Point game_field_size);
void reset_game(SnakeGame *game);
void free_game(SnakeGame *game);

typedef enum {
    TURN_LEFT,
    TURN_RIGHT,
    TURN_NONE
} TurnDirection;

void turn_snake(SnakeGame *game, TurnDirection turn_direction);
void move_snake(SnakeGame *game, MoveDirection move_direction);

#endif