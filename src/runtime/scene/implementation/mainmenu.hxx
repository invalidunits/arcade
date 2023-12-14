#ifndef _ARCADE_MAIN_MENU
#define _ARCADE_MAIN_MENU

#include <graphics/graphics.hxx>
#include <rom/rom.hxx>
#include <system/clock.hxx>
#include "../scene.hxx"
#include "gameplay.hxx"

#include <string>
#include <sstream>
#include <iomanip>

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    std::stringstream out;
    out << std::fixed << std::setprecision(n);
    out << a_value;
    return std::move(out).str();
}

namespace Runtime {
    class MainMenu : public Scene {
        Graphics::shared_texture main_menu = nullptr;
        Graphics::shared_texture main_menu_ghost = nullptr;
        Graphics::shared_texture main_menu_coin = nullptr;
        Graphics::shared_texture main_menu_start = nullptr;
        
        float coin_display = 0;
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

            
            

            void update_fixed() {
                int frame = Runtime::current_tick;
                const Uint8 *key = SDL_GetKeyboardState(nullptr);
                bool start_button, enter_coin = false;
                start_button = key[SDL_SCANCODE_RETURN] != 0;
                enter_coin = key[SDL_SCANCODE_RIGHT] != 0;

                // TODO: Connect this to coin machine and to play button;
                if (_coin_display != coin_display) {
                    first_coin_update = _coin_display == 0;
                    _coin_display = coin_display;
                    coin_update_frame = 3;
                }

                if (first_coin_update) first_coin_update = coin_update_frame > 0;

                if ((frame % 6) == 0) 
                    coin_update_frame -= 1;

                if ((frame % 16) == 0) {
                    coin_update_frame -= 1;
                    if (enter_coin) {
                        coin_display += 0.25;
                    }

                    if (start_button) {
                        Runtime::SceneManager::pushScene<Runtime::Gameplay>();
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
                
                if (coin_display == 0 || first_coin_update) {
                    SDL_Rect rect = {ARCADE_LOGIC_WIDTH*(coinframe % 4), 0, ARCADE_LOGIC_WIDTH, ARCADE_LOGIC_HEIGHT};
                    if (first_coin_update) {
                        // TODO: There is a math solution to this... 
                        rect.y += cointrans;
                        SDL_SetTextureColorMod(main_menu_coin.get(), 255, 0 , 0);
                    } else {
                        SDL_SetTextureColorMod(main_menu_coin.get(), 255, 255 , 255);
                    }
                    
                    SDL_RenderCopy(Graphics::renderer, main_menu_coin.get(), &rect, nullptr);
                } else SDL_RenderCopy(Graphics::renderer, main_menu_start.get(), nullptr, nullptr);
                
                

                SDL_Rect rect = {ARCADE_LOGIC_WIDTH*(ghostframe % 4), 0, ARCADE_LOGIC_WIDTH, ARCADE_LOGIC_HEIGHT};
                SDL_RenderCopy(Graphics::renderer, main_menu_ghost.get(), &rect, nullptr);

                

                Graphics::drawText(
                    Math::recti(80, ARCADE_LOGIC_HEIGHT - cointrans - 12, 0, 0),
                    "$" + to_string_with_precision(coin_display, 2),
                    Graphics::renderer, (coin_update_frame > 0? 
                                    Math::color8a(~0, ~0, ~0, ~0) : 
                                    Math::color8a(~0, ~0, 0, ~0)));
            }
    };
}




#endif