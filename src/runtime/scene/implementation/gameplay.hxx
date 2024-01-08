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


namespace Runtime {
    struct Gameplay : public Entity::EntityManager { 
        void setup() {
            m_entities.push_back(std::make_unique<Runtime::Pac::Tilemap>());
            m_entities.push_back(std::make_unique<Runtime::Pac::PacMan>());
            m_entities.push_back(std::make_unique<Runtime::Pac::Ghost::Blinky>());
            m_entities.push_back(std::make_unique<Runtime::Pac::Ghost::Pinky>());
            m_entities.push_back(std::make_unique<Runtime::Pac::Ghost::Clyde>());
        }
    };
}


#endif