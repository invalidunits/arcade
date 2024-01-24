#ifndef _ARCADE_GAMEPLAY
#define _ARCADE_GAMEPLAY

#include <graphics/graphics.hxx>
#include <rom/rom.hxx>
#include <system/clock.hxx>
#include "../scene.hxx"

#include <runtime/entity/implementation/pacbase.hxx>
#include <runtime/entity/implementation/tileset.hxx>
#include <runtime/entity/implementation/ghost.hxx>
#include <runtime/entity/entity.hxx>
#include <runtime/counters/counters.hxx>
#include <bitset>



namespace Runtime {
    struct Gameplay : public Entity::EntityManager { 
        
        void setup();
        void update();
        void update_fixed();
        void draw();
        void cleanup();
        

        std::bitset<8> flags = 0;
        bool release_ghost = true;
        std::vector<Runtime::Pac::Ghost::Ghost*> ghosts = {};
        std::vector<Runtime::Pac::Ghost::Ghost*> inactive_ghosts = {};
        Runtime::duration clock_delay = decltype(clock_delay)::zero();
    };
}


#endif