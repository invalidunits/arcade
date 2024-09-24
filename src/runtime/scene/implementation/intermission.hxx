#ifndef _ARCADE_INTERMISSION_MENU
#define _ARCADE_INTERMISSION_MENU

#include "lives.hxx"

namespace Runtime {
    class Intermission : public Scene {
        Graphics::shared_texture pacmen_cutscene = nullptr;
        std::vector<LiveCutscene::pac_t> pacmen = {};
        Runtime::duration m_do_stuff_timer = Runtime::duration::zero();
        public:
            inline Intermission() {
                pacmen_cutscene =  ARCADE_LOADTEXTROM(IMGpacmanCutscene);
            }
            void setup();
            void update();
            void draw();


    };
}




#endif