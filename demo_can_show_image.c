#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "utils/fn.h"
#include "utils/toml/toml.h"

#define SCREEN_WIDTH       800
#define SCREEN_HEIGHT      600
#define DELAY_DISPLAY_TEXT 0.1  // 秒
#define DELAY_DISPLAY_ITEM 1  // 秒


// 初始化SDL和全局变量
int initSDL();
void closeSDL();
SDL_Texture* loadTexture(const char* path);
void displayScene(SDL_Renderer* renderer, SDL_Texture* texture);
void displayText(SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color);
void displayDialogueBox(SDL_Renderer* renderer, int x, int y, int w, int h);
void displayCharacter(SDL_Renderer* renderer, SDL_Texture* texture);
void display_item(SDL_Renderer* renderer, SDL_Texture* texture);
int  display_options(Option* opts, int n_opt);
void play_wav(const char* path);
void close_wav();

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;
SDL_AudioSpec wavSpec;
Uint32 wavLength;
Uint8* wavBuffer;
SDL_AudioDeviceID deviceId;


int main() {
    char* path = "story.toml";

    // 解析TOML配置文件
    Item* items = NULL;
    int n_item;
    Music* musics = NULL;
    int n_music;
    Scene* scenes = NULL;
    int n_scene;
    Character* characters = NULL;
    int n_character;
    Event* events = NULL;
    int n_event;
    Dialogue* dialogues = NULL;
    int n_dialogue;
    Player player;
    read_toml(&player, &items, &n_item, &musics, &n_music, &scenes, &n_scene, &characters, &n_character, &events, &n_event, &dialogues, &n_dialogue, path);

    if (initSDL() == -1) return -1;  // 初始化SDL

    //
    bool quit = false;
    SDL_Event e;
    Event* event = &events[0];
    Dialogue* dialog = NULL;
    Act_Type type = EVENT;
    int opt_number;
    Option opt;
    SDL_Texture* currentTexture = NULL;
    SDL_Texture* eventItem_texture = NULL;
    SDL_Texture* dialogueItem_texture = NULL;
    SDL_Texture* characterTexture = NULL; // 人物纹理
    bool end_scene = false;  // 用于控制结束时暂停
    bool has_event_item  = false;
    bool is_playing = false;

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
            if (event->item) {
                eventItem_texture = loadTexture(find_item(items, n_item, event->item)->icon);
                SDL_Delay(DELAY_DISPLAY_ITEM);
                display_item(gRenderer, eventItem_texture);
                has_event_item = true;
            }
            else {
                has_event_item = false;
            }
            dialog = find_dialogue(dialogues, n_dialogue, event->dialog);
            type = DIALOGUE;
        } else if (type == DIALOGUE) {
            size_t dialog_text_len = strlen(dialog->text);
            char*  pText           = dialog->text;
            char*  pHead           = dialog->text;
            while (pText - pHead < dialog_text_len) {
                if (dialog->wav && !is_playing) {
                    play_wav(find_music(musics, n_music, dialog->wav)->addr);
                    is_playing = true;
                }

                SDL_RenderClear(gRenderer);
                displayScene(gRenderer, currentTexture); // 重新绘制背景图片

                // 如果对话中有角色，则加载并显示角色图像
                if (dialog->character) {
                    characterTexture = loadTexture(find_character(characters, n_character, dialog->character)->avatar);
                    displayCharacter(gRenderer, characterTexture);
                }

                size_t text_len = strcspn(pText, "\n");
                char temp = pText[text_len];
                pText[text_len] = '\0';

                if (has_event_item) {
                    SDL_Delay(DELAY_DISPLAY_ITEM);
                    display_item(gRenderer, eventItem_texture);
                }
                if (dialog->item) {
                    dialogueItem_texture = loadTexture(find_item(items, n_item, dialog->item)->icon);
                    SDL_Delay(DELAY_DISPLAY_ITEM);
                    display_item(gRenderer, dialogueItem_texture);
                }
                displayDialogueBox(gRenderer, 0, SCREEN_HEIGHT - 150, SCREEN_WIDTH, 150);
                displayText(gRenderer, pText, 50, SCREEN_HEIGHT - 130, (SDL_Color){255, 255, 255, 255});
                SDL_RenderPresent(gRenderer);

                SDL_Delay(DELAY_DISPLAY_TEXT * 1000);
                pText[text_len] = temp;
                pText += text_len + 1;
            }

            if (dialog->n_opt > 0) {
                opt_number = display_options(dialog->opts, dialog->n_opt);

                opt = dialog->opts[opt_number];
                if (opt.event) {
                    event = find_event(events, n_event, opt.event);
                    type = EVENT;
                    if (dialog->item) {
                        SDL_Delay(500);
                        bool is_repeat = add_item(&(player.items), &(player.n_item), find_item(items, n_item, dialog->item)->alias);
                        if (!is_repeat) {
                            char msg[500] = {'\0'};
                            sprintf(msg, "恭喜你獲得了 '%s', 現在背包共有 %d 個物品 !!!", find_item(items, n_item, dialog->item)->name, player.n_item);
                            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "恭喜你 !!!", msg, gWindow);
                        }
                    }
                } else if (opt.next) {
                    dialog = find_dialogue(dialogues, n_dialogue, opt.next);
                    type = DIALOGUE;
                }
                SDL_RenderClear(gRenderer);
            }

            if (is_playing) {
                close_wav();
                is_playing = false;
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
        //printf("按 Ctrl+C 结束程序\n");

        char msg[500] = {'\0'};
        int number;
        sscanf(dialog->alias, "%*[^0-9]%d", &number);
        sprintf(msg, "恭喜你完成了結局 %d !!!", number);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "恭喜你 !!!", msg, gWindow);
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

    // audio	
    SDL_Init(SDL_INIT_AUDIO);

    return 0;
}

void closeSDL() {
    if (gFont) TTF_CloseFont(gFont);
    if (gRenderer) SDL_DestroyRenderer(gRenderer);
    if (gWindow) SDL_DestroyWindow(gWindow);
    TTF_Quit();
    SDL_Quit();
}

void play_wav(const char* wav_path)
{
    SDL_LoadWAV(wav_path, &wavSpec, &wavBuffer, &wavLength);
    deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
    int success = SDL_QueueAudio(deviceId, wavBuffer, wavLength);
    SDL_PauseAudioDevice(deviceId, 0);
}

void close_wav()
{
    //SDL_PauseAudioDevice(deviceId, 1);
    SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(wavBuffer);
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
    if (text == NULL || strlen(text) == 0) {
        printf("Warning: Empty or null text passed to displayText\n");
        return;
    }

    SDL_Surface* surfaceMessage = TTF_RenderUTF8_Solid(gFont, text, color);
    if (!surfaceMessage) {
        printf("TTF_RenderUTF8_Solid failed: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    if (!message) {
        printf("SDL_CreateTextureFromSurface failed: %s\n", SDL_GetError());
        SDL_FreeSurface(surfaceMessage);
        return;
    }

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

void displayCharacter(SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_Rect destRect = {SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT - 450, 300, 400};  // 设置人物图片的位置和大小
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
}

void display_item(SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_Rect destRect = {SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT - 450, 200, 300};  // 设置人物图片的位置和大小
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
}

int display_options(Option* opts, int n_opt) {
    SDL_MessageBoxButtonData* buttons = (SDL_MessageBoxButtonData*)calloc(n_opt, sizeof(SDL_MessageBoxButtonData));
    char** texts = (char**)calloc(n_opt, sizeof(char*));
    for (int i = 0; i < n_opt; i++) {
        texts[i] = (char*)calloc(strlen(opts[i].text) + 100, sizeof(char));
        sprintf(texts[i], "選項 %d: ", i + 1);
        sprintf(texts[i] + strlen(texts[i]), "%s", opts[i].text);
        buttons[i] = (SDL_MessageBoxButtonData){SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, i, texts[i]};
    }

    SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION, // 消息框的類型
        NULL, // 父窗口，NULL 表示沒有父窗口
        "注意! 注意! 注意! 很重要所以說 3 次", // 標題
        "請選擇一個選項", // 消息內容
        n_opt, // 按鈕的數量
        buttons, // 按鈕數組
        NULL // 默認的按鈕索引，這裡不設置
    };

    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
        printf("SDL_ShowMessageBox Error: %s\n", SDL_GetError());
        SDL_Quit();
    }

    // free
    for (int i = 0; i < n_opt; i++) {
        free(texts[i]);
    }
    free(texts);
    free(buttons);

    buttonid = buttonid == -1 || buttonid >= n_opt ? 0 : buttonid;

    return buttonid;
}
