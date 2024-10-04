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
#include "highscores.hxx"

namespace Runtime {
    class MainMenu : public Scene {
        Graphics::shared_texture main_menu = nullptr;
        Graphics::shared_texture main_menu_ghost = nullptr;
        Graphics::shared_texture main_menu_coin = nullptr;
        Graphics::shared_texture main_menu_start = nullptr;

        int coin_update_frame = 0;
        bool first_ready = false;


        public: 
            MainMenu();
            void setup();
            void update_fixed();
            void draw();
        
        static bool can_start(void);
    };
}




#endif