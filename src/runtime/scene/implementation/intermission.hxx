#ifndef _ARCADE_INTERMISSION_MENU
#define _ARCADE_INTERMISSION_MENU
#include "gameplay.hxx"

namespace Runtime {
    class IntermissionBase : public virtual Scene {
        protected:
            bool flipped = false;
            struct pac_t {
                bool evaluated = false;
                Runtime::duration time_elapsed = Runtime::duration::zero();
                Runtime::duration life_time = Runtime::duration::zero();
                enum sprite_t : unsigned char {
                    pacman,
                    woman,
                    kidman,
                    legman,
                    largeman,

                    smugman, // Smugman is "special"

                    endman, // This doesn't have an animation, This just represents the amount of pacman variations in the list.

                    // For Intermissions.
                    inky,
                    blinky,
                    pinky,
                    clyde,
                    
                } sprite;
                float random = 0;
            };
            
            Graphics::shared_texture ghost_textures[4] = {};
            Graphics::shared_texture pacmen_cutscene_texture = nullptr;
            std::vector<pac_t> pacmen = {};
        public:
            virtual void update_pac(pac_t *pac);
            void setup();
            void update();
            void draw();
    };

    class Intermission : public IntermissionBase {
        Runtime::duration m_do_stuff_timer = Runtime::duration::zero();
        public:
            inline Intermission() {}
            void setup();
            void update();
            void draw();

            void scene_finished();
    };
}




#endif