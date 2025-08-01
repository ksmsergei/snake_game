#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "snake.h"

#define CELL_WIDTH 50
#define CELL_HEIGHT 50

#define MAX_GAME_FIELD_SIZE 10
#define MIN_GAME_FIELD_SIZE 2

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

Point game_field_size = {10, 10};
SnakeGame game;

Uint32 last_update = 0;

const int UPDATE_INTERVAL = 500;

bool init_SDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(
        "Snake Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        game_field_size.x * CELL_WIDTH,
        game_field_size.y * CELL_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void close_SDL() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void draw_scene() {
    //Clear screen
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    //Draw vertical lines
    for (int i = 0; i < game.game_field_size.x; i++) {
        SDL_RenderDrawLine(renderer, i * CELL_WIDTH, 0, i * CELL_HEIGHT, game_field_size.y * CELL_HEIGHT);
    }

    //Draw horizontal lines
    for (int i = 0; i < game.game_field_size.y; i++) {
        SDL_RenderDrawLine(renderer, 0, i * CELL_HEIGHT, game_field_size.x * CELL_WIDTH, i * CELL_HEIGHT);
    }

    SDL_Rect fill_rect;

    //Draw fruit
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    fill_rect = (SDL_Rect) {game.fruit_pos.x * CELL_WIDTH + 1, game.fruit_pos.y * CELL_HEIGHT + 1, CELL_WIDTH - 1, CELL_HEIGHT - 1};        
    SDL_RenderFillRect(renderer, &fill_rect);

    //Draw snake's head
    SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
    fill_rect = (SDL_Rect) {game.snake_pos[0].x * CELL_WIDTH + 1, game.snake_pos[0].y * CELL_HEIGHT + 1, CELL_WIDTH - 1, CELL_HEIGHT - 1};
    SDL_RenderFillRect(renderer, &fill_rect);

    //Draw snake parts
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (int i = 1; i < game.snake_length; i++) {
        fill_rect = (SDL_Rect) {game.snake_pos[i].x * CELL_WIDTH + 1, game.snake_pos[i].y * CELL_HEIGHT + 1, CELL_WIDTH - 1, CELL_HEIGHT - 1};        
        SDL_RenderFillRect(renderer, &fill_rect);
    }
    
    SDL_RenderPresent(renderer);
}

void update_window_title() {
    char title[100];

    int score = game.snake_length - 2;
    switch (game.game_state) {
        case PLAYING:
            sprintf(title, "PLAYING (Score: %d) %dx%d", score, game_field_size.x, game_field_size.y);    
            break;

        case VICTORY:
            sprintf(title, "VICTORY (Score: %d)", score);
            break;

        case DEFEAT:
            sprintf(title, "DEFEAT (Score: %d)", score);
            break;
    }

    SDL_SetWindowTitle(window, title);    
}

void resize_game_field(bool increase) {
    bool should_resize = false;

    if (increase && game_field_size.x < MAX_GAME_FIELD_SIZE) {
        game_field_size.x++;
        game_field_size.y++;
        should_resize = true;
    } else if (!increase && game_field_size.x > MIN_GAME_FIELD_SIZE) {
        game_field_size.x--;
        game_field_size.y--;
        should_resize = true;
    }

    if (!should_resize) {
        return;
    }
    
    free_game(&game);
    init_game(&game, game_field_size);

    update_window_title();
    SDL_SetWindowSize(window, game_field_size.x * CELL_WIDTH, game_field_size.y * CELL_HEIGHT);

    draw_scene();

    last_update = SDL_GetTicks();
}

int main(int argc, char* argv[]) {
    if (!init_SDL()) {
        printf("Failed to initialize SDL.\n");
        return 1;
    }

    SDL_Event event;
    bool quit = false;    

    srand(time(NULL));

    init_game(&game, game_field_size);
    draw_scene(renderer, &game);

    MoveDirection move_direction = game.snake_move_direction;

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN) { //Key press event
                switch (event.key.keysym.sym) {
                    //Move snake
                    case SDLK_UP:
                    case SDLK_w:
                        move_direction = MOVE_UP;
                        break;

                    case SDLK_RIGHT:
                    case SDLK_d:
                        move_direction = MOVE_RIGHT;
                        break;

                    case SDLK_DOWN:
                    case SDLK_s:
                        move_direction = MOVE_DOWN;
                        break;

                    case SDLK_LEFT:
                    case SDLK_a:
                        move_direction = MOVE_LEFT;
                        break;

                    //Reset game
                    case SDLK_r:
                        reset_game(&game);
                        move_direction = game.snake_move_direction;
                        draw_scene(renderer, &game);
                        update_window_title(window, &game);
                        last_update = SDL_GetTicks();
                        break;

                    case SDLK_EQUALS:
                        resize_game_field(true);
                        break;

                    case SDLK_MINUS:
                        resize_game_field(false);
                        break;
                }
            }
        }

        if (SDL_GetTicks() > last_update + UPDATE_INTERVAL) {

            last_update = SDL_GetTicks();

            move_snake(&game, move_direction);
            draw_scene(renderer, &game);
            
            update_window_title(window, &game);
        }

        SDL_Delay(10);
    }

    free_game(&game);
    close_SDL(window, renderer);
    return 0;
}
