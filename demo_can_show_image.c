#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "utils/fn.h"
#include "utils/toml/toml.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 640

int init();
int loadMedia(char* imagePath1);
void close();

SDL_Surface* loadSurface(const char* path);
SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gHelloWorld = NULL;
SDL_Surface* gOverlayImage = NULL;

int main()
{
    char*      path = "story.toml";

    Item*      items = NULL;
    int        n_item;

    Scene*     scenes = NULL;
    int        n_scene;

    Character* characters = NULL;
    int        n_character;

    Event*     events = NULL;
    int        n_event;

    Dialogue*  dialogues = NULL;
    int        n_dialogue;

    Player player;

    // read all the information in toml
    read_toml(&player, 
              &items, &n_item,
              &scenes, &n_scene,
              &characters, &n_character,
              &events, &n_event,
              &dialogues, &n_dialogue,
              path);

    // 開始劇情
    Event*    event = &(events[0]);
    Dialogue* dialog = NULL;
    Act_Type  type = EVENT;
    int       opt_number;
    Option    opt;
    bool      is_ending = false;
    char*     img_path = NULL;

    while (!is_ending) {
        switch (type) {
            case EVENT:
                img_path = find_scene(scenes, n_scene, event->scene)->bg;
                printf("\033[33m用 SDL 顯示圖:\033[0m %s\n", img_path);
                
                if (!loadMedia(img_path)) {
                    printf("Failed to load media!\n");
                } else {
                    printf("Media loaded successfully.\n");
                }

                if( !init() )
                {
                    printf( "初始化失敗！\n" );
                }
                else
                {
                    // 加載媒體
                    if( !loadMedia(img_path) )
                    {
                        printf( "加載媒體失敗！\n" );
                    }
                    else
                    {
                        // 應用圖像
                        SDL_BlitSurface( gHelloWorld, NULL, gScreenSurface, NULL );

                        //Apply the image stretched
                        SDL_Rect stretchRect;
                        stretchRect.x = 0;
                        stretchRect.y = 0;
                        stretchRect.w = SCREEN_WIDTH;
                        stretchRect.h = SCREEN_HEIGHT;
                        SDL_BlitScaled( gHelloWorld, NULL, gScreenSurface, &stretchRect );

                        // 疊加第二張圖像
                        SDL_Rect overlayRect;
                        overlayRect.x = 280; // 設定第二張圖像的 x 坐標
                        overlayRect.y = 0; // 設定第二張圖像的 y 坐標
                        overlayRect.w = 200; // 設定第二張圖像的寬度
                        overlayRect.h = 200; // 設定第二張圖像的高度
                        SDL_BlitScaled(gOverlayImage, NULL, gScreenSurface, &overlayRect);

                        //Update the surface
                        SDL_UpdateWindowSurface( gWindow );

                        SDL_Event e; 
                        int quit = 0; 
                        while( quit == 0 )
                        { 
                            while( SDL_PollEvent( &e ) )
                            { 
                                if( e.type == SDL_QUIT ) quit = 1; 
                            } 
                        }
                    }
                }

                // 釋放資源並關閉 SDL
                close(); //
                dialog = find_dialogue(dialogues, n_dialogue, event->dialog);
                type = DIALOGUE;
                break;
                
            case DIALOGUE:
                printf("\033[35m用 SDL 顯示對話:\033[0m %s\n", dialog->text);
                if (dialog->character) {
                    printf("\033[36m用 SDL 顯示角色:\033[0m %s\n", find_character(characters, n_character, dialog->character)->avatar);
                }

                if (dialog->n_opt > 0) {
                    printf("\033[32m------------------------ 出現選項 ------------------------\033[0m\n");
                    for (int i = 0; i < dialog->n_opt; i++) {
                        printf("選項(%d): %s\n", i + 1, dialog->opts[i].text);
                    }

                    printf("請選擇一個, 你選擇: ");
                    scanf("%d", &opt_number);
                    opt_number = opt_number - 1 >= dialog->n_opt ? dialog->n_opt - 1 : opt_number - 1;
                    opt = dialog->opts[opt_number];

                    if (opt.event) {
                        event = find_event(events, n_event, opt.event);
                        type = EVENT;
                    }
                    else if (opt.next) {
                        dialog = find_dialogue(dialogues, n_dialogue, opt.next);
                        type = DIALOGUE;
                    }
                    else {
                        printf("\033[31mdialogue's option format is error: '%s'\033[0m\n", dialog->alias);
                    }
                }

                printf("\n");

                if (dialog && strstr(dialog->alias, "ending")) {
                    is_ending = true;
                }

                break;

            default:
                printf("\033[31mUnknow type\033[0m\n");
        }
    }

    printf("\033[35m劇終囉 !!!\033[0m\n");
}

int init() {
    int success = 1;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = 0;
    } else {
        gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL) {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = 0;
        } else {
            gScreenSurface = SDL_GetWindowSurface(gWindow);
        }
    }

    return success;
}

int loadMedia(char* imagePath1)
{
    // 加载成功标志
    int success = 1;

    // 加载第一张图像
    gHelloWorld = SDL_LoadBMP(imagePath1);
    if (gHelloWorld == NULL)
    {
        printf("无法加载图像 %s！SDL 错误：%s\n", imagePath1, SDL_GetError());
        success = 0;
    }

    // 加载第二张图像
    /*gOverlayImage = SDL_LoadBMP(imagePath2);
    if (gOverlayImage == NULL)
    {
        printf("无法加载图像 %s！SDL 错误：%s\n", imagePath2, SDL_GetError());
        success = 0;
    }*/

    return success; // 注意这里应该返回 success 而不是固定的 1
}

void close()
{
    // 釋放表面
    SDL_FreeSurface( gHelloWorld );
    gHelloWorld = NULL;

    SDL_FreeSurface(gOverlayImage);
    gOverlayImage = NULL;

    // 銷毀窗口
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    // 退出 SDL 子系統
    SDL_Quit();
}

SDL_Surface* loadSurface(const char* path)
{
    SDL_Surface* optimizedSurface = NULL; // The final optimized image

    // Load image at specified path
    SDL_Surface* loadedSurface = SDL_LoadBMP(path);
    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL Error: %s\n", path, SDL_GetError());
    }
    else
    {
        // Convert surface to screen format
        optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
        if (optimizedSurface == NULL)
        {
            printf("Unable to optimize image %s! SDL Error: %s\n", path, SDL_GetError());
        }

        // Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    return optimizedSurface;
}