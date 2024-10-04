#include "mainmenu.hxx"

namespace Runtime {
    MainMenu::MainMenu() {
        main_menu =        ARCADE_LOADTEXTROM(IMGmainMenu);
        main_menu_coin =   ARCADE_LOADTEXTROM(IMGmainMenuCoin);
        main_menu_start =  ARCADE_LOADTEXTROM(IMGmainMenuStart);
        main_menu_ghost =  ARCADE_LOADTEXTROM(IMGmainMenuGhost);
    }

    void MainMenu::setup() {
        Runtime::current_score = 0;
        Runtime::coin_display = 0;
    }



    void MainMenu::update_fixed() {
        Runtime::fixedupdateCounter();
        auto frame = Runtime::current_tick;
        bool start_button = (Controls::button_inputs[Controls::BUTTON_A]);
        const Uint8 *key = SDL_GetKeyboardState(nullptr);
        bool enter_coin = false;
        #ifdef DEBUG
            enter_coin = key[SDL_SCANCODE_RIGHT] != 0; // TODO: Connect enter_coin debug param to a debug macro.
        #endif

        if (can_start() && !first_ready) {
            first_ready = true;
            coin_update_frame = 3;
        }
 

        if ((frame % 6) == 0) 
            coin_update_frame -= 1;
        
        if ((frame % 16) == 0) {
            if (Controls::button_inputs[Controls::BUTTON_SELECT]) {
                Runtime::SceneManager::pushScene<Runtime::HighscoreScene>();
                return;
            } 
        }
      

        if ((frame % 16) == 0) {
            if (enter_coin) {
                Runtime::coin_display += 5;
            }

            if (start_button && can_start()) {
                Runtime::SceneManager::pushScene<Runtime::LiveCutscene>();
            }
        }

        
    }

    void MainMenu::draw() {
        
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

        Runtime::display_coins = true;
        Runtime::drawCounter();
    }

    bool MainMenu::can_start(void) {
        return Runtime::coin_display >= Runtime::life_value;
    }
}
