#include "controls.hxx"

#include <thread>
#include <SDL2/SDL.h>
#include <mutex>

std::thread                     control_thread;
std::atomic<bool>               cancel_controls;
std::mutex control_mutex;

SDL_GameController *controller = nullptr;

SDL_GameControllerButton button_bind[Controls::BUTTON_END] = {
    SDL_CONTROLLER_BUTTON_A, // A
    SDL_CONTROLLER_BUTTON_B, // B

    SDL_CONTROLLER_BUTTON_Y, // Select
};

SDL_Scancode keyboard_button_bind[Controls::BUTTON_END] = {
    SDL_SCANCODE_Z,
    SDL_SCANCODE_X,
    
    SDL_SCANCODE_RETURN,  
};


SDL_GameControllerAxis axis_bind[Controls::AXIS_END] = {
    SDL_CONTROLLER_AXIS_LEFTX, // X
    SDL_CONTROLLER_AXIS_LEFTY, // Y
};

SDL_Scancode keyboard_axis_bind[Controls::AXIS_END*2] = {
    SDL_SCANCODE_RIGHT, SDL_SCANCODE_LEFT, // X
    SDL_SCANCODE_DOWN, SDL_SCANCODE_UP, // Y
};


void controlLoop() {
    cancel_controls.store(false);
    while (!cancel_controls.load(std::memory_order_acquire)) {
        if (!control_mutex.try_lock()) continue;
        for (unsigned char input = 0; input < Controls::AXIS_END; input++) {
            int16_t value = 0.0;
            if (SDL_GameControllerGetAttached(controller)) 
                value += SDL_GameControllerGetAxis(controller, axis_bind[input]);
            
            const Uint8 *key = SDL_GetKeyboardState(nullptr);
            const ptrdiff_t key_input = input * 2;
            value += (key[keyboard_axis_bind[key_input]] - key[keyboard_axis_bind[key_input + 1]])*INT16_MAX;
            Controls::axis_inputs[input].store(value);
        }

        for (unsigned char input = 0; input < Controls::BUTTON_END; input++) {
            bool value = false;
            if (SDL_GameControllerGetAttached(controller)) 
                value = bool(SDL_GameControllerGetButton(controller, button_bind[input]));
            
            const Uint8 *key = SDL_GetKeyboardState(nullptr);
            value = value || key[keyboard_button_bind[input]];
            Controls::button_inputs[input].store(value);
        }



        control_mutex.unlock();
    }            
}

namespace Controls {

    void beginControlThread(void) {
        if (control_thread.joinable()) endControlThread();
        SDL_Init(SDL_INIT_GAMECONTROLLER);
        controller = SDL_GameControllerOpen(0);
        if (controller == nullptr) {
            // Something bad happened
            std::printf("Failed to initialized controller connection... Is this an error?");
        }

        control_thread = std::thread(controlLoop);
    }

    void endControlThread(void) {
        cancel_controls.store(true, std::memory_order_release);
        control_thread.join();

        if (controller) SDL_GameControllerClose(controller);
    }

}