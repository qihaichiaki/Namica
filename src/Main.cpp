#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <iostream>

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    std::cout << "初始化SDL成功!" << std::endl;
    SDL_Quit();
    return 0;
}