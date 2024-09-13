#ifndef _ARCADE_LIVES_CUTSCENE
#define _ARCADE_LIVES_CUTSCENE

#include <cmath>
#include <rom/rom.hxx>
#include <runtime/scene/scene.hxx>
#include <graphics/graphics.hxx>
#include <runtime/entity/entity.hxx>
#include "gameplay.hxx"

#include <runtime/counters/counters.hxx>

namespace Runtime {

    

    
    class PointsEffect;

    class LiveCutscene : public Entity::EntityManager {
        constexpr static auto default_life_time = std::chrono::duration_cast<Runtime::duration>(Runtime::tick_length*ARCADE_LOGIC_WIDTH);
        struct pac_t {
            bool evaluated = false;
            Runtime::duration time_elapsed = Runtime::duration::zero();
            Runtime::duration life_time = Runtime::duration::zero();
            enum sprite_t : unsigned char {
                pacman,
                woman,
                legman,
                kidman,
                largeman,

                smugman, // Smugman is "special"

                endman
            } sprite;
            float random = 0;
        };

        unsigned int _coin_display = 0;
        Graphics::shared_texture pacmen_cutscene = nullptr;
        std::vector<pac_t> pacmen;
        
        
        public:
            

            LiveCutscene();
            void setup();
            void update();
            void update_fixed();
            void draw();
            void cleanup();

            int m_state = 0;
            int coin_update_frame = 0;
            Runtime::duration m_do_stuff_timer = Runtime::duration::zero();

            
        
    };
    
    
} // namespace Runtime



#endif