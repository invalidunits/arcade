#ifndef _ARCADE_MAIN_MENU
#define _ARCADE_MAIN_MENU

#include <graphics/graphics.hxx>
#include <rom/rom.hxx>
#include <system/clock.hxx>
#include "../scene.hxx"
#include "gameplay.hxx"
#include "lives.hxx"

#include <string>
#include <sstream>
#include <iomanip>

#include <system/com.hxx>
#include <system/controls.hxx>

#include <runtime/counters/counters.hxx>

namespace Runtime {
    class MainMenu : public Scene {
        Graphics::shared_texture main_menu = nullptr;
        Graphics::shared_texture main_menu_ghost = nullptr;
        Graphics::shared_texture main_menu_coin = nullptr;
        Graphics::shared_texture main_menu_start = nullptr;

        int coin_update_frame = 0;
        bool first_ready = false;


        public: 
            MainMenu() {
                main_menu =        ARCADE_LOADTEXTROM(IMGmainMenu);
                main_menu_coin =   ARCADE_LOADTEXTROM(IMGmainMenuCoin);
                main_menu_start =  ARCADE_LOADTEXTROM(IMGmainMenuStart);
                main_menu_ghost =  ARCADE_LOADTEXTROM(IMGmainMenuGhost);
            }
            
            void setup() {
                Runtime::coin_display = 0;
            }
            
            

            void update_fixed() {
                Runtime::fixedupdateCounter();
                int frame = Runtime::current_tick;
                bool start_button = (Controls::button_inputs[Controls::BUTTON_A] || Controls::button_inputs[Controls::BUTTON_START]);
                const Uint8 *key = SDL_GetKeyboardState(nullptr);
                #ifdef DEBUG
                    bool enter_coin = key[SDL_SCANCODE_RIGHT] != 0; // TODO: Connect enter_coin debug param to a debug macro.
                #endif

                if (can_start() && !first_ready) {
                    first_ready = true;
                    coin_update_frame = 3;
                }


                if ((frame % 6) == 0) 
                    coin_update_frame -= 1;

                if ((frame % 16) == 0) {
                    if (enter_coin) {
                        Runtime::coin_display += 5;
                    }

                    if (start_button && can_start()) {
                        Runtime::SceneManager::pushScene<Runtime::LiveCutscene>();
                    }
                }

                
            }

            void draw() {
                
                int coinframe = Runtime::current_tick/6;
                int ghostframe = Runtime::current_tick/6;
                int cointrans = 0;
                switch (coin_update_frame) {
                    case 3: cointrans = 1; break;
                    case 2: cointrans = 3; break;
                    case 1: cointrans = 1; break;
                }
                SDL_RenderCopy(Graphics::renderer, main_menu.get(), nullptr, nullptr);
                
                if (!first_ready || cointrans != 0) {
                    SDL_Rect rect = {ARCADE_LOGIC_WIDTH*(coinframe % 4), 0, ARCADE_LOGIC_WIDTH, ARCADE_LOGIC_HEIGHT};
                    SDL_Rect dstrect = {0, 0, ARCADE_LOGIC_WIDTH, ARCADE_LOGIC_HEIGHT};
                    if (first_ready && cointrans != 0) {
                        dstrect.y -= cointrans;
                        SDL_SetTextureColorMod(main_menu_coin.get(), 255, 0 , 0);
                    } else {
                        SDL_SetTextureColorMod(main_menu_coin.get(), 255, 255 , 255);
                    }
                    
                    SDL_RenderCopy(Graphics::renderer, main_menu_coin.get(), &rect, &dstrect);
                } else SDL_RenderCopy(Graphics::renderer, main_menu_start.get(), nullptr, nullptr);
                
                

                SDL_Rect rect = {ARCADE_LOGIC_WIDTH*(ghostframe % 4), 0, ARCADE_LOGIC_WIDTH, ARCADE_LOGIC_HEIGHT};
                SDL_RenderCopy(Graphics::renderer, main_menu_ghost.get(), &rect, nullptr);
                Runtime::drawCounter();
                

                    
            }
        
        static bool can_start(void) {
            return Runtime::coin_display >= Runtime::life_value;
        }
    };
}




#endif