


#include <SDL2/SDL.h>
#include <graphics/graphics.hxx>
#include <rom/rom.hxx>
#include <runtime/scene/scene.hxx>

using namespace Graphics;


#include <system/clock.hxx>
#include <runtime/scene/scene.hxx>
#include <runtime/scene/implementation/mainmenu.hxx>
#include <system/com.hxx>
#include <system/controls.hxx>

#include <sfx/sfx.hxx>

enum orientation {
    landscape,
    leftside_portrait,
    upside_down_landscape,
    righside_portrait,
    orientation_last

};

orientation current_orientation = righside_portrait;
SDL_Texture *render_target[orientation_last] = { nullptr, nullptr, nullptr, nullptr };

Runtime::duration accumalation = Runtime::duration::zero();



void CreateRenderTarget(orientation orient, int width, int height) {
    if (render_target[orient] != nullptr) return;
    render_target[orient] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, width, height);

}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    int err = SDL_CreateWindowAndRenderer(ARCADE_WINDOW_WIDTH, ARCADE_WINDOW_HEIGHT, ARCADE_WINDOW_PROPERTIES,
        &window, &renderer);

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
    Controls::beginControlThread();
    Runtime::setupCounter();

    if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_LSHIFT]) {
        Runtime::live_count = 999;
        Runtime::SceneManager::pushScene<Runtime::Gameplay>();
    } else {
        Runtime::SceneManager::pushScene<Runtime::MainMenu>();
    }

    Runtime::Sound::SoundEffect<ROM::gSFXBeginData>::InitializeSFX(ROM::gSFXBeginSize);
    Runtime::Sound::SoundEffect<ROM::gSFXChompData>::InitializeSFX(ROM::gSFXChompSize);
    Runtime::Sound::SoundEffect<ROM::gSFXDeathData>::InitializeSFX(ROM::gSFXDeathSize);
    Runtime::Sound::SoundEffect<ROM::gSFXeatFruitData>::InitializeSFX(ROM::gSFXeatFruitSize);
    Runtime::Sound::SoundEffect<ROM::gSFXeatGhostData>::InitializeSFX(ROM::gSFXeatGhostSize);
    Runtime::Sound::SoundEffect<ROM::gSFXextraPacData>::InitializeSFX(ROM::gSFXextraPacSize);
    Runtime::Sound::SoundEffect<ROM::gSFXIntermissionData>::InitializeSFX(ROM::gSFXIntermissionSize);
    Runtime::Sound::SoundEffect<ROM::gSFXYooHOOData>::InitializeSFX(ROM::gSFXYooHOOSize);
    Runtime::Sound::SoundEffect<ROM::gSFXSirin1Data>::InitializeSFX(ROM::gSFXSirin1Size);
    Runtime::Sound::SoundEffect<ROM::gSFXSirin2Data>::InitializeSFX(ROM::gSFXSirin2Size);
    Runtime::Sound::SoundEffect<ROM::gSFXSirin3Data>::InitializeSFX(ROM::gSFXSirin3Size);
    Runtime::Sound::SoundEffect<ROM::gSFXSirin4Data>::InitializeSFX(ROM::gSFXSirin4Size);
    Runtime::Sound::SoundEffect<ROM::gSFXSirin5Data>::InitializeSFX(ROM::gSFXSirin5Size);
    
    for (;;) {
        if (render_target[current_orientation] == nullptr) {
            CreateRenderTarget(current_orientation, ARCADE_LOGIC_WIDTH, ARCADE_LOGIC_HEIGHT);
        }

        int width = ARCADE_WINDOW_WIDTH;
        int height = ARCADE_WINDOW_HEIGHT;
        int logic_width = ARCADE_LOGIC_WIDTH;
        int logic_height = ARCADE_LOGIC_HEIGHT;

        switch (current_orientation) {
            default:
            case upside_down_landscape:
            case landscape:
                break;

            case leftside_portrait:
            case righside_portrait:
                height = ARCADE_WINDOW_WIDTH;
                width = ARCADE_WINDOW_HEIGHT;
        }
        SDL_SetWindowSize(window, width, height);


        SDL_Event event = {};
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                Controls::endControlThread();
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
        
        scene = Runtime::SceneManager::getCurrentScene();
        scene->update();


        SDL_RenderSetLogicalSize(renderer, ARCADE_LOGIC_WIDTH, ARCADE_LOGIC_HEIGHT);
        SDL_SetRenderTarget(renderer, render_target[current_orientation]);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderFillRect(NULL, NULL);

        scene = Runtime::SceneManager::getCurrentScene();
        scene->draw();
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderClear(renderer);




        SDL_RenderSetLogicalSize(renderer, width, height);
        SDL_Rect dstrect = {width/2 - ARCADE_WINDOW_WIDTH/2, height/2 - ARCADE_WINDOW_HEIGHT/2, ARCADE_WINDOW_WIDTH, ARCADE_WINDOW_HEIGHT};
        SDL_Point pivot = {ARCADE_WINDOW_WIDTH/2, ARCADE_WINDOW_HEIGHT/2};
        SDL_RenderCopyEx(renderer, render_target[current_orientation], nullptr, &dstrect, 90*(int)current_orientation, &pivot, SDL_RendererFlip::SDL_FLIP_NONE);
        SDL_RenderPresent(renderer);
    }

    return 0;
}

