


#include <SDL2/SDL.h>
#include <graphics/graphics.hxx>
#include <rom/rom.hxx>
#include <runtime/scene/scene.hxx>

using namespace Graphics;


#include <system/clock.hxx>
#include <runtime/scene/scene.hxx>
#include <runtime/scene/implementation/mainmenu.hxx>
#include <system/com.hxx>

Runtime::duration accumalation = Runtime::duration::zero();
int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    
    int err = SDL_CreateWindowAndRenderer(ARCADE_WINDOW_WIDTH, ARCADE_WINDOW_HEIGHT, ARCADE_WINDOW_PROPERTIES,
        &window, &renderer);

    SDL_RenderSetLogicalSize(renderer, ARCADE_LOGIC_WIDTH, ARCADE_LOGIC_HEIGHT);
    if (err) {
        printf("The application crashed due to the following error: %s", SDL_GetError());
        exit(err);
    }


    const auto font_data = SDL_RWFromConstMem(ROM::gEmuLogicData, ROM::gEmuLogicSize);
    Graphics::default_font = TTF_OpenFontRW(font_data, 1, 8);
    if (Graphics::default_font == nullptr) {
        printf("The application crashed due to the following error: %s", SDL_GetError());
        exit(1);
    }

    
    COM::beginCOMThread();
    Runtime::SceneManager::pushScene<Runtime::MainMenu>();


    
    for (;;) {

        SDL_Event event = {};
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                COM::endCOMThread();
                return 0;
            }
        }

        auto scene = Runtime::SceneManager::getCurrentScene();

        auto time = Runtime::clock::now();
        Runtime::delta_time = std::chrono::duration_cast<decltype(Runtime::delta_time)>(time - Runtime::current_time);
        Runtime::current_time = time;
        accumalation += Runtime::delta_time;
        

        while (accumalation > Runtime::tick_length) {
            accumalation -= Runtime::tick_length;
            Runtime::current_tick += 1;
            scene->update_fixed();
        }

        scene->update();

        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderFillRect(NULL, NULL);


        scene->draw();
        SDL_RenderPresent(renderer);
    }

    return 0;
}