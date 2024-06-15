#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>

SDL_Texture* loadTexture(const char *path, SDL_Renderer *renderer, int *width, int *height) {
    SDL_Texture* newTexture = IMG_LoadTexture(renderer, path);
    if (newTexture == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
    } else {
        SDL_QueryTexture(newTexture, NULL, NULL, width, height);
    }
    return newTexture;
}

SDL_Texture* renderText(const char *message, const char *fontFile, SDL_Color color, int fontSize, SDL_Renderer *renderer, int *textWidth, int *textHeight) {
    TTF_Font *font = TTF_OpenFont(fontFile, fontSize);
    if (font == NULL){
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        return NULL;
    }
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, message, color);
    SDL_Texture* textTexture = NULL;
    if (textSurface != NULL) {
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        *textWidth = textSurface->w;  // Get the width of the text
        *textHeight = textSurface->h; // Get the height of the text
        SDL_FreeSurface(textSurface);
    } else {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    TTF_CloseFont(font);
    return textTexture;
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* backgroundTexture = NULL;
    SDL_Texture* imageTexture = NULL;
    SDL_Texture* textTexture = NULL;
    SDL_Texture* nameTexture = NULL;
    int textWidth = 0, textHeight = 0, nameWidth = 0, nameHeight = 0;
    int imgWidth = 0, imgHeight = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    if (TTF_Init() == -1) {
        printf("TTF could not initialize! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }

    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    backgroundTexture = loadTexture("bookstore.bmp", renderer, NULL, NULL);
    imageTexture = loadTexture("oldman.bmp", renderer, &imgWidth, &imgHeight);
    textTexture = renderText("我是圖書館管理者", "font.ttf", (SDL_Color){255, 255, 255, 255}, 24, renderer, &textWidth, &textHeight);
    nameTexture = renderText("Bookstore", "font.ttf", (SDL_Color){255, 255, 255, 255}, 24, renderer, &nameWidth, &nameHeight);

    SDL_Event e;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL); // Render background

        // Position for the image at the bottom-right corner
        SDL_Rect imgRect = {800 - imgWidth - 10, 600 - imgHeight - 110, imgWidth, imgHeight};
        SDL_RenderCopy(renderer, imageTexture, NULL, &imgRect);

        // Background for the text
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Opaque black
        SDL_Rect textBackgroundRect = {0, 600 - 100, 800, 100}; // Full width, 100 pixels tall at the bottom
        SDL_RenderFillRect(renderer, &textBackgroundRect);

        // Position for the name text at the bottom-left corner
        SDL_Rect nameRect = {10, 600 - 90, nameWidth, nameHeight};
        SDL_RenderCopy(renderer, nameTexture, NULL, &nameRect);

        // Position for the dialogue text just above the name
        SDL_Rect textRect = {10, 600 - 50, textWidth, textHeight};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(imageTexture);
    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(nameTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
