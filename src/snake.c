#include <stdlib.h>
#include <stdio.h>
#include "snake.h"

void spawn_snake(SnakeGame *game);
void generate_fruit(SnakeGame *game);

//Corresponds to MOVE_UP, MOVE_RIGHT, MOVE_DOWN, MOVE_LEFT
static const MoveDirection opposite_directions[] = {MOVE_DOWN, MOVE_LEFT, MOVE_UP, MOVE_RIGHT};

//Constant arrays for coordinate offsets
static const int8_t dx[] = {0, 1, 0, -1}; //Corresponds to MOVE_UP, MOVE_RIGHT, MOVE_DOWN, MOVE_LEFT
static const int8_t dy[] = {-1, 0, 1, 0};

//Compare two points for equality
bool points_equal(Point p1, Point p2) {
    return (p1.x == p2.x && p1.y == p2.y);
}

//Initialize SnakeGame structure
void init_game(SnakeGame *game, Point game_field_size) {
    if (game_field_size.x == 1 && game_field_size.y == 1) {
        return;
    }

    game->game_field_size = game_field_size;
    
    //Dynamically allocate memory for an array of snake body parts
    game->snake_pos = (Point *)malloc(sizeof(Point) * (game_field_size.x * game_field_size.y));

    reset_game(game);
}

//Set game settings to default and start new game
void reset_game(SnakeGame *game) {
    //Initial snake length is 2 and snake moves right
    game->snake_length = 2;
    game->snake_move_direction = MOVE_RIGHT; 

    spawn_snake(game);

    generate_fruit(game);

    game->game_state = PLAYING;   
}

void free_game(SnakeGame *game) {
    if (game->snake_pos != NULL) {
        free(game->snake_pos);
        game->snake_pos = NULL;
    }
}

void spawn_snake(SnakeGame *game) {
    //Randomly generate new snake's head pos
    game->snake_pos[0] = (Point) {rand() % game->game_field_size.x, rand() % game->game_field_size.y};

    //Randomly generate the side from which the second part of the snake will be added
    MoveDirection snake_part_direction = rand() % 4;

    int16_t new_x;
    int16_t new_y;

    //Change the position of the second part of the snake until it fits in the field
    while (true) {
        new_x = game->snake_pos[0].x + dx[snake_part_direction];
        new_y = game->snake_pos[0].y + dy[snake_part_direction];

        if (new_x >= 0 && new_x <= game->game_field_size.x - 1 &&
            new_y >= 0 && new_y <= game->game_field_size.y - 1) {
            break;
        }

        snake_part_direction = (snake_part_direction + 1) % 4;
    }


    //Set the pos of the 2nd part of the snake and set the direction of movement to the opposite side from the 2nd part
    game->snake_pos[1] = (Point) {new_x, new_y};
    game->snake_move_direction = opposite_directions[snake_part_direction];
}

void generate_fruit(SnakeGame *game) {
    bool valid_pos;
    do {
        //Randomly generate new fruit pos
        game->fruit_pos = (Point) {rand() % game->game_field_size.x, rand() % game->game_field_size.y};

        //Generate the coordinates of the fruit until it enters free space
        valid_pos = true;
        for (int i = 0; i < game->snake_length; i++) {
            if (points_equal(game->fruit_pos, game->snake_pos[i])) {
                valid_pos = false;
                break;
            }
        }
    } while (!valid_pos);
}

void turn_snake(SnakeGame *game, TurnDirection turn_direction) {
    if (game->game_state != PLAYING) {
        return;
    }

    //Private constant arrays for turnings
    static const MoveDirection turn_left_map[] = {MOVE_LEFT, MOVE_UP, MOVE_RIGHT, MOVE_DOWN};
    static const MoveDirection turn_right_map[] = {MOVE_RIGHT, MOVE_DOWN, MOVE_LEFT, MOVE_UP};

    //Determine the new direction depending on the turning direction
    if (turn_direction == TURN_LEFT) {
        game->snake_move_direction = turn_left_map[game->snake_move_direction];
    } else if (turn_direction == TURN_RIGHT) {
        game->snake_move_direction = turn_right_map[game->snake_move_direction];
    }

    //A new position for the snake's head
    Point new_pos = (Point) {
        (game->snake_pos[0].x + dx[game->snake_move_direction] + game->game_field_size.x) % game->game_field_size.x,
        (game->snake_pos[0].y + dy[game->snake_move_direction] + game->game_field_size.y) % game->game_field_size.y
    };

    for (int i = 1; i < game->snake_length - 1; i++) {
        if (points_equal(new_pos, game->snake_pos[i])) {
            game->game_state = DEFEAT;
            return;
        }
    }  

    bool fruit_eaten = points_equal(new_pos, game->fruit_pos);
    Point last_snake_pos = game->snake_pos[game->snake_length - 1];

    for (int i = 0; i < game->snake_length; i++) {
        //Save the current coordinates for moving the next element
        Point pos = game->snake_pos[i];

        //Move the current element to the place of the previous element
        game->snake_pos[i] = new_pos;

        //Updating previous coordinates
        new_pos = pos;
    }

	if (fruit_eaten) {
        game->snake_pos[game->snake_length++] = last_snake_pos;

        if (game->snake_length == game->game_field_size.x * game->game_field_size.y) {
            game->game_state = VICTORY;
            return;
        }

        generate_fruit(game);
    }
}

void move_snake(SnakeGame *game, MoveDirection move_direction) {
    if (move_direction != opposite_directions[game->snake_move_direction]) {
        game->snake_move_direction = move_direction;
    }
        
    turn_snake(game, TURN_NONE);
}
