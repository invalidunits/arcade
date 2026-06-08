


#include <SDL2/SDL.h>
#include <graphics/graphics.hxx>
#include <rom/rom.hxx>
#include <runtime/scene/scene.hxx>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

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

orientation current_orientation = landscape;
SDL_Texture *render_target[orientation_last] = { nullptr, nullptr, nullptr, nullptr };

Runtime::duration accumalation = Runtime::duration::zero();


void iteration(void *arg);
void update_orientation(void);

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048);

    window = SDL_CreateWindow("Arcade Machine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ARCADE_WINDOW_WIDTH, ARCADE_WINDOW_HEIGHT, ARCADE_WINDOW_PROPERTIES);

    if (window == NULL) 
    {
        printf("The application crashed due to the following error: %s", SDL_GetError());
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        printf("The application crashed due to the following error: %s", SDL_GetError());
        exit(1);
    }

    


    

    const auto font_data = SDL_RWFromConstMem(ROM::gArcadePixPlusData, ROM::gArcadePixPlusSize);
    Graphics::default_font = TTF_OpenFontRW(font_data, 1, 16);
    if (Graphics::default_font == nullptr) {
        printf("The application crashed due to the following error: %s", SDL_GetError());
        exit(1);
    }

    #ifndef NO_ARCADE
    COM::beginCOMThread();
    #endif
    Controls::beginControlThread();
    Runtime::setupCounter();

    if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_LSHIFT]) {
        Runtime::live_count = 999;
        Runtime::SceneManager::pushScene<Runtime::Gameplay>();
    } else {
        Runtime::SceneManager::pushScene<Runtime::MainMenu>();
        /*
                    Runtime::SceneManager::pushScene<Runtime::HighscoreScene>();
        ((Runtime::HighscoreScene*)Runtime::SceneManager::getCurrentScene())->newHighScore(0);
        */
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

    Runtime::Sound::SoundEffect<ROM::gSFXCoinData>::InitializeSFX(ROM::gSFXCoinSize);
    Runtime::Sound::SoundEffect<ROM::gSFXSelectData>::InitializeSFX(ROM::gSFXSelectSize);
    Runtime::Sound::SoundEffect<ROM::gSFXErrorData>::InitializeSFX(ROM::gSFXErrorSize);


    update_orientation();
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(iteration, nullptr, 60, 1);
    return 0;
    #endif


    for (;;) iteration(nullptr);
    return 0;
}

int current_logic_width = 0;
int current_logic_height = 0;


void update_orientation()
{
    if (render_target[current_orientation] == nullptr) 
    {
        if (render_target[current_orientation] != nullptr) return;
        render_target[current_orientation] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, ARCADE_LOGIC_WIDTH, ARCADE_LOGIC_HEIGHT);
    }

    current_logic_width = ARCADE_WINDOW_WIDTH;
    current_logic_height = ARCADE_WINDOW_HEIGHT;


    switch (current_orientation) {
        default:
        case upside_down_landscape:
        case landscape:
            break;

        case leftside_portrait:
        case righside_portrait:
            current_logic_height = ARCADE_WINDOW_WIDTH;
            current_logic_width = ARCADE_WINDOW_HEIGHT;
    }

    SDL_SetWindowSize(window, current_logic_width, current_logic_height);
}


void iteration(void *arg)
{
    try {
        #ifdef __EMSCRIPTEN__
        Controls::updateControls();
        #endif

        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_L) {
                #ifndef __EMSCRIPTEN__
                current_orientation = orientation((int)(current_orientation + 1) % int(orientation_last));
                update_orientation();
                #endif
            }
            if (event.type == SDL_QUIT) {
                Controls::endControlThread();
                #ifndef NO_ARCADE
                COM::endCOMThread();
                #endif

                #ifdef __EMSCRIPTEN__
                emscripten_cancel_main_loop();
                #else
                std::exit(0);
                #endif
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
        SDL_RenderFillRect(renderer, NULL);

        scene = Runtime::SceneManager::getCurrentScene();
        scene->draw();
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderClear(renderer);

        SDL_RenderSetLogicalSize(renderer, current_logic_width, current_logic_height);
        SDL_Rect dstrect = {current_logic_width/2 - ARCADE_WINDOW_WIDTH/2, current_logic_height/2 - ARCADE_WINDOW_HEIGHT/2, ARCADE_WINDOW_WIDTH, ARCADE_WINDOW_HEIGHT};
        SDL_Point pivot = {ARCADE_WINDOW_WIDTH/2, ARCADE_WINDOW_HEIGHT/2};
        SDL_RenderCopyEx(renderer, render_target[current_orientation], nullptr, &dstrect, 90*(int)current_orientation, &pivot, SDL_RendererFlip::SDL_FLIP_NONE);
        SDL_RenderPresent(renderer);
    }
    catch (std::exception except)
    {
       printf("%s", except.what());
    }
}