#include <stdlib.h>
#include <SDL2/SDL.h>
#include "Client.h"

typedef struct Rectangle {
    SDL_Rect rect;
    int id;
    int x_pos_old;
    int y_pos_old;
    int x_pos_new;
    int y_pos_new;
    int move_up;
    int move_right;
    int move_left;
    int move_down;
    int r;
    int g;
    int b;
    int a;
} Rect;

#define TITLE "Game"
#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 900
#define DISP 10

void init_System(SDL_Window **window, SDL_Renderer **renderer) {
    SDL_Init(SDL_INIT_VIDEO);
    *window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    *renderer = SDL_CreateRenderer(*window, -1, render_flags);
    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);
}

void del_System(SDL_Window **window, SDL_Renderer **renderer) {
    SDL_DestroyRenderer(*renderer);
    SDL_DestroyWindow(*window);
    SDL_Quit();
}

void init_rect(Rect *rect, int w, int h, int start_x, int start_y, int r, int g, int b, int a, int id) {
    rect->rect.w = w;
    rect->rect.h = h;
    rect->rect.x = start_x;
    rect->rect.y = start_y;
    rect->x_pos_old = start_x;
    rect->x_pos_new = start_x;
    rect->y_pos_old = start_y;
    rect->y_pos_new = start_y;
    rect->move_up = 0;
    rect->move_down = 0;
    rect->move_left = 0;
    rect->move_right = 0;
    rect->id = id;
    rect->r = r;
    rect->g = g;
    rect->b = b;
    rect->a = a;
}

void handle_events_menu(int *close, int *start) {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_QUIT:
            *close = 1;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_SPACE:
                    *start = 1;
                    break;
                case SDL_SCANCODE_ESCAPE:
                    *close = 1;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void handle_events_game(Rect *rect1, int *close) {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_QUIT:
            *close = 1;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_W:
                    rect1->move_up = 1;
                    break;
                case SDL_SCANCODE_A:
                    rect1->move_left = 1;
                    break;
                case SDL_SCANCODE_S:
                    rect1->move_down = 1;
                    break;
                case SDL_SCANCODE_D:
                    rect1->move_right = 1;
                    break;
                case SDL_SCANCODE_ESCAPE:
                    *close = 1;
                default:
                    break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_W:
                    rect1->move_up = 0;
                    break;
                case SDL_SCANCODE_A:
                    rect1->move_left = 0;
                    break;
                case SDL_SCANCODE_S:
                    rect1->move_down = 0;
                    break;
                case SDL_SCANCODE_D:
                    rect1->move_right = 0;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;

    }
}

int handle_collision(Rect *rect, SDL_Rect wall) {
    int w = rect->rect.w;
    int h = rect->rect.w;

    int x1 = rect->x_pos_new;
    int y1 = rect->y_pos_new;
    int x2 = x1 + w;
    int y2 = y1 + h;

    int lower_y = wall.y - (h);
    int upper_y = (wall.y + wall.h) + (h);
    int lower_x = wall.x - (w);
    int upper_x = (wall.x + wall.w) + (w);

    int collision = 0;

    if ((x2 > wall.x) && (x1 < wall.x) && (y1 > lower_y) && (y2 < upper_y) && (rect->move_right)) {
        rect->x_pos_new = wall.x - w;
        collision = 1;
    }
    if ((x1 < wall.x + wall.w) && (x2 > wall.x + wall.w) && (y1 > lower_y) && (y2 < upper_y) && (rect->move_left)) {
        rect->x_pos_new = wall.x + wall.w;
        collision = 1;
    }
    if ((y2 > wall.y) && (y1 < wall.y) && (x1 > lower_x) && (x2 < upper_x) && (rect->move_down)) {
        rect->y_pos_new = wall.y - h;
        collision = 1;
    }
    if ((y1 < wall.y + wall.h) && (y2 > wall.y + wall.h) && (x1 > lower_x) && (x2 < upper_x) && (rect->move_up)) {
        rect->y_pos_new = wall.y + wall.h;
        collision = 1;
    }

    return collision;
}

int move_rect(Rect *rect, int n, Rect *walls[n]) {
    rect->x_pos_old = rect->x_pos_new;
    rect->y_pos_old = rect->y_pos_new;

    if (rect->move_up)
        rect->y_pos_new = rect->y_pos_old - DISP;
    if (rect->move_left)
        rect->x_pos_new = rect->x_pos_old - DISP;
    if (rect->move_down)
        rect->y_pos_new = rect->y_pos_old + DISP;
    if (rect->move_right)
        rect->x_pos_new = rect->x_pos_old + DISP;

    // window bounds left and right
    if (rect->x_pos_new <= -(rect->rect.w - 1))
        rect->x_pos_new = SCREEN_WIDTH - 1;
    else if (rect->x_pos_new >= SCREEN_WIDTH - 1)
        rect->x_pos_new = -(rect->rect.w - 1);
    // window bounds up and down
    if (rect->y_pos_new <= -(rect->rect.h - 1))
        rect->y_pos_new = SCREEN_HEIGHT - 1;
    else if (rect->y_pos_new >= SCREEN_HEIGHT - 1)
        rect->y_pos_new = -(rect->rect.h - 1);

    // handle collisions
    int collision = 0, flag;
    for (int i = 0; i < n; i++) {
        if (walls[i]->id != rect->id) {
            flag = handle_collision(rect, walls[i]->rect);
            if (flag)
                collision = 1;
        }
    }

    rect->rect.x = rect->x_pos_new;
    rect->rect.y = rect->y_pos_new;
    return collision;
}

void draw_win(SDL_Renderer *renderer, int n, Rect *rects[n], int flag) {

    // draw background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    if (flag)
        SDL_SetRenderDrawColor(renderer, 90, 23, 150, 255);
    SDL_RenderClear(renderer);

    // draw rects
    for (int i = 0; i < n; i++) {
        SDL_SetRenderDrawColor(renderer, rects[i]->r, rects[i]->g, rects[i]->b, rects[i]->a);
        SDL_RenderFillRect(renderer, &(rects[i]->rect));
    }

    // update window
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {

    if (argc != 3)
        return 0;
    int ACTIVE_PLAYER = argv[1][0] - '0';
    int NOT_ACTIVE_PLAYER;

    if (ACTIVE_PLAYER == 1)
        NOT_ACTIVE_PLAYER = 0;
    if (ACTIVE_PLAYER == 0)
        NOT_ACTIVE_PLAYER = 1;

    char *IP = argv[2];

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    init_System(&window, &renderer);

    Rect player_1;
    Rect player_2;
    int n_rects = 2;
    Rect *rects[] = {&player_1, &player_2};

    int close = 0;
    int start = 1;
    int *Ret_Val = NULL;

    while (!close && start) {

        init_rect(&player_1, 30, 30, 370, 285, 255, 0, 0, 255, 0);
        init_rect(&player_2, 30, 30, 500, 285, 0, 0, 255, 255, 1);

        while (start) {
            handle_events_game(rects[ACTIVE_PLAYER], &close);

            int coll = move_rect(rects[ACTIVE_PLAYER], n_rects, rects);

            Ret_Val = get_post(0, 1, rects[ACTIVE_PLAYER]->x_pos_new,
                               rects[ACTIVE_PLAYER]->y_pos_new,
                               ACTIVE_PLAYER + 1, IP);

            rects[NOT_ACTIVE_PLAYER]->rect.x = Ret_Val[0];
            rects[NOT_ACTIVE_PLAYER]->rect.y = Ret_Val[1];

            draw_win(renderer, n_rects, rects, coll);

            if (coll || close) {
                start = 0;
            }

            Ret_Val = get_post(0, 0, close, start, 0, IP);

            close = Ret_Val[0];
            start = Ret_Val[1];
        }

        while (!close && !start) {
            handle_events_menu(&close, &start);

            draw_win(renderer, n_rects, rects, 1);

            Ret_Val = get_post(0, 0, close, start, 1, IP);

            close = Ret_Val[0];
            start = Ret_Val[1];
        }
    }

    del_System(&window, &renderer);

    if (ACTIVE_PLAYER == 0) {
        SDL_Delay(1000);
        get_post(1, 0, 0, 0, 0, IP);
    }
}



