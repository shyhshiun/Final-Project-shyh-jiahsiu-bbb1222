#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "utils/fn.h"
#include "utils/toml/toml.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// 初始化SDL和全局变量
int initSDL();
void closeSDL();
SDL_Texture* loadTexture(const char* path);
void displayScene(SDL_Renderer* renderer, SDL_Texture* texture);
void displayText(SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color);
void displayDialogueBox(SDL_Renderer* renderer, int x, int y, int w, int h);
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;

int main() {
    char* path = "story.toml";

    // 解析TOML配置文件
    Item* items = NULL;
    int n_item;
    Scene* scenes = NULL;
    int n_scene;
    Character* characters = NULL;
    int n_character;
    Event* events = NULL;
    int n_event;
    Dialogue* dialogues = NULL;
    int n_dialogue;
    Player player;
    read_toml(&player, &items, &n_item, &scenes, &n_scene, &characters, &n_character, &events, &n_event, &dialogues, &n_dialogue, path);

    if (initSDL() == -1) return -1;  // 初始化SDL

    bool quit = false;
    SDL_Event e;
    Event* event = &events[0];
    Dialogue* dialog = NULL;
    Act_Type type = EVENT;
    int opt_number;
    Option opt;
    SDL_Texture* currentTexture = NULL;
    bool end_scene = false;  // 用于控制结束时暂停

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        if (type == EVENT) {
            currentTexture = loadTexture(find_scene(scenes, n_scene, event->scene)->bg);
            SDL_RenderClear(gRenderer);
            displayScene(gRenderer, currentTexture);
            dialog = find_dialogue(dialogues, n_dialogue, event->dialog);
            type = DIALOGUE;
        } else if (type == DIALOGUE) {
            SDL_RenderClear(gRenderer);
            displayScene(gRenderer, currentTexture); // 重新绘制背景图片
            displayDialogueBox(gRenderer, 0, SCREEN_HEIGHT - 150, SCREEN_WIDTH, 150);
            displayText(gRenderer, dialog->text, 50, SCREEN_HEIGHT - 130, (SDL_Color){255, 255, 255, 255});
            SDL_RenderPresent(gRenderer);

            if (dialog->n_opt > 0) {
                for (int i = 0; i < dialog->n_opt; i++) {
                    printf("Option (%d): %s\n", i + 1, dialog->opts[i].text);
                }
                printf("Choose an option: ");
                scanf("%d", &opt_number);
                opt_number = (opt_number - 1 >= dialog->n_opt) ? dialog->n_opt - 1 : opt_number - 1;
                opt = dialog->opts[opt_number];
                if (opt.event) {
                    event = find_event(events, n_event, opt.event);
                    type = EVENT;
                } else if (opt.next) {
                    dialog = find_dialogue(dialogues, n_dialogue, opt.next);
                    type = DIALOGUE;
                }
                SDL_RenderClear(gRenderer);
            }

            if (dialog && strstr(dialog->alias, "ending")) {
                end_scene = true;
                break;
            }
        }

        SDL_RenderPresent(gRenderer);  // 更新屏幕
    }

    // 显示结束场景并暂停
    if (end_scene) {
        bool pause = true;
        while (pause) {
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    pause = false;
                    quit = true;
                }
            }
        }
    }

    closeSDL();  // 关闭SDL和清理资源
    return 0;
}

int initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    gWindow = SDL_CreateWindow("Interactive Fiction", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!gWindow) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!gRenderer) {
        SDL_DestroyWindow(gWindow);
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    if (TTF_Init() == -1) {
        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        printf("TTF could not initialize! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }

    gFont = TTF_OpenFont("font.ttf", 28);  // 确保font.ttf在项目目录下
    if (!gFont) {
        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }

    return 0;
}

void closeSDL() {
    if (gFont) TTF_CloseFont(gFont);
    if (gRenderer) SDL_DestroyRenderer(gRenderer);
    if (gWindow) SDL_DestroyWindow(gWindow);
    TTF_Quit();
    SDL_Quit();
}

SDL_Texture* loadTexture(const char* path) {
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (!loadedSurface) {
        printf("Unable to load image %s! SDL Error: %s\n", path, IMG_GetError());
    } else {
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (!newTexture) {
            printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
        }
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}

void displayScene(SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_RenderCopy(renderer, texture, NULL, NULL);
}

void displayText(SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color) {
    SDL_Surface* surfaceMessage = TTF_RenderUTF8_Solid(gFont, text, color);
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect message_rect;
    message_rect.x = x;
    message_rect.y = y;
    message_rect.w = surfaceMessage->w;
    message_rect.h = surfaceMessage->h;
    SDL_RenderCopy(renderer, message, NULL, &message_rect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}

void displayDialogueBox(SDL_Renderer* renderer, int x, int y, int w, int h) {
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);  // 设置颜色为半透明黑色
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // 重置颜色为白色
}