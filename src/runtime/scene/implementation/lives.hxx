#ifndef _ARCADE_LIVES_CUTSCENE
#define _ARCADE_LIVES_CUTSCENE

#include <cmath>
#include <rom/rom.hxx>
#include <runtime/scene/scene.hxx>
#include <graphics/graphics.hxx>
#include <runtime/entity/entity.hxx>
#include "gameplay.hxx"

#include <runtime/counters/counters.hxx>
#include "intermission.hxx"

namespace Runtime {
    class PointsEffect;
    class LiveCutscene : public Runtime::IntermissionBase, public Entity::EntityManager {
        constexpr static auto default_life_time = std::chrono::duration_cast<Runtime::duration>(Runtime::tick_length*ARCADE_LOGIC_WIDTH);
        unsigned int _coin_display = 0;
        unsigned int _live_display = 0;
        
        public:

            LiveCutscene();
            void update_pac(pac_t *pac);
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