#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include "qoi.h"


int main(int argc, char *argv[])
{
 
    if (argc != 2 && argc != 3) {
        printf("USAGE: decoder [file.qoi]\n");
        exit(EXIT_SUCCESS);
    }

    int SCALE = 1;

    if (argc == 3) {
        SCALE = atoi(argv[2]);
    }


    char *file = argv[1];

    printf("File to be decoded: %s\n", file);

    FILE * qoi_file = fopen(file, "r");

    if (qoi_file == NULL) {
        printf("File does not exist: %s", file);
        exit(EXIT_FAILURE);
    }

    QOI_HEADER header;

    PIXEL* pixels = decode_qoi_file(qoi_file, &header);

    if (pixels == NULL) {
        printf("File could not be decoded, error encountered");
        exit(EXIT_FAILURE);
    }

    printf("Magic: %s, Width: %u, Height: %u, Channels: %u, Colorspace: %u\n", header.magic, header.width, header.height, header.channels, header.colorspace);

    // returns zero on success else non-zero
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_Window* window = SDL_CreateWindow("QOI Decoder",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       header.width * SCALE, header.height * SCALE, 0);

    if (!window) {
        printf("Error initializing a window: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    } 

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); 
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    if (!renderer) {
        printf("Error initializing a renderer: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    } 

    char running = 1;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    for (int i = 0; i < header.width * header.height; i++) {
        SDL_SetRenderDrawColor(renderer, pixels[i].r, pixels[i].g, pixels[i].b, pixels[i].a);
        SDL_RenderFillRect(renderer, &(SDL_Rect){(i%header.width) * SCALE, (i/header.width) * SCALE, SCALE, SCALE});
    }

    SDL_RenderPresent(renderer);

    free_pixels(pixels);

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }
    }
 
    return EXIT_SUCCESS;
}
