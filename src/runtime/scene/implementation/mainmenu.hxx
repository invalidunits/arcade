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



namespace Runtime {
    class MainMenu : public Scene {
        Graphics::shared_texture main_menu = nullptr;
        Graphics::shared_texture main_menu_ghost = nullptr;
        Graphics::shared_texture main_menu_coin = nullptr;
        Graphics::shared_texture main_menu_start = nullptr;
        float _coin_display = 0;

        int coin_update_frame = 0;
        bool first_coin_update = false;


        public: 
            MainMenu() {
                main_menu =        ARDCADE_LOADTEXTROM(IMGmainMenu);
                main_menu_coin =   ARDCADE_LOADTEXTROM(IMGmainMenuCoin);
                main_menu_start =  ARDCADE_LOADTEXTROM(IMGmainMenuStart);
                main_menu_ghost =  ARDCADE_LOADTEXTROM(IMGmainMenuGhost);
            }
            
            void setup() {
                Runtime::coin_display = 0;
            }
            
            

            void update_fixed() {
                int frame = Runtime::current_tick;
                const Uint8 *key = SDL_GetKeyboardState(nullptr);
                bool start_button, enter_coin = false;
                start_button = key[SDL_SCANCODE_RETURN] != 0;
                enter_coin = key[SDL_SCANCODE_RIGHT] != 0;

                

                Runtime::coin_display += (float)COM::coin_inserted_value.exchange(0, std::memory_order::memory_order_relaxed)/100.0;
                // TODO: Connect this to coin machine and to play button;
                if (_coin_display != Runtime::coin_display) {
                    if (can_start()) {
                        first_coin_update = _coin_display < Runtime::life_value;
                    }
                    
                    _coin_display = Runtime::coin_display;
                    coin_update_frame = 3;
                }

                if (first_coin_update) first_coin_update = coin_update_frame > 0;

                if ((frame % 6) == 0) 
                    coin_update_frame -= 1;

                if ((frame % 16) == 0) {
                    if (enter_coin) {
                        Runtime::coin_display += 0.05;
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
                
                if (!can_start() || first_coin_update) {
                    SDL_Rect rect = {ARCADE_LOGIC_WIDTH*(coinframe % 4), 0, ARCADE_LOGIC_WIDTH, ARCADE_LOGIC_HEIGHT};
                    if (first_coin_update) {
                        rect.y += cointrans;
                        SDL_SetTextureColorMod(main_menu_coin.get(), 255, 0 , 0);
                    } else {
                        SDL_SetTextureColorMod(main_menu_coin.get(), 255, 255 , 255);
                    }
                    
                    SDL_RenderCopy(Graphics::renderer, main_menu_coin.get(), &rect, nullptr);
                } else SDL_RenderCopy(Graphics::renderer, main_menu_start.get(), nullptr, nullptr);
                
                

                SDL_Rect rect = {ARCADE_LOGIC_WIDTH*(ghostframe % 4), 0, ARCADE_LOGIC_WIDTH, ARCADE_LOGIC_HEIGHT};
                SDL_RenderCopy(Graphics::renderer, main_menu_ghost.get(), &rect, nullptr);

                auto coindraw = Math::recti(80, ARCADE_LOGIC_HEIGHT - cointrans - 12, 0, 0);
                auto color =  (coin_update_frame > 0? 
                                    Math::color8a(~0, ~0, ~0, ~0) : 
                                    Math::color8a(~0, ~0, 0, ~0));
                if (Runtime::coin_display > 120.00) 
                    Graphics::drawText(coindraw, "STOP IT!",
                            Graphics::renderer, color);
                else if (Runtime::coin_display > 100.0) 
                    Graphics::drawText(coindraw, "Too much...",
                            Graphics::renderer, color);
                else Graphics::drawText(coindraw, "$" + Math::to_string_with_precision(Runtime::coin_display, 2),
                            Graphics::renderer, color);
                    
            }
        
        static bool can_start(void) {
            return Runtime::coin_display >= Runtime::life_value;
        }
    };
}




#endif