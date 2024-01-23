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
        void setup() {
            auto tilemap =  addEntity<Runtime::Pac::Tilemap>();
            tilemap->position = {0, 18};

            auto pacman =   addEntity<Runtime::Pac::PacMan>();
            pacman->getComponent<Runtime::Pac::PacComponent>()->m_position = tilemap->position + Math::pointi{112, 188};

            ghosts = {
                            addEntity<Runtime::Pac::Ghost::Blinky>(),
                            addEntity<Runtime::Pac::Ghost::Pinky>(),
                            addEntity<Runtime::Pac::Ghost::Clyde>(),
            };

            ghosts[0]->getComponent<Runtime::Pac::PacComponent>()->m_position = tilemap->position + Math::pointi{112, 92};
            ghosts[0]->getComponent<Runtime::Pac::Ghost::GhostComponent>()->state = Runtime::Pac::Ghost::STATE_SCATTER;
            inactive_ghosts = ghosts;
            inactive_ghosts.erase(inactive_ghosts.begin());


            inactive_ghosts[0]->getComponent<Runtime::Pac::PacComponent>()->m_position = tilemap->position + Math::pointi{112, 120};
            inactive_ghosts[1]->getComponent<Runtime::Pac::PacComponent>()->m_position = tilemap->position + Math::pointi{128, 120};
            // inactive_ghosts[2]->getComponent<Runtime::Pac::PacComponent>()->m_position = {96, 120};


            flags[0] = 1;
            clock_delay = std::chrono::duration_cast<decltype(clock_delay)>(
                std::chrono::seconds(10)
            );
        }


        void update() {
            if (clock_delay.count() > 0) {
                clock_delay -= Runtime::delta_time;
                return;
            }
            
            flags[0] = 0;
            EntityManager::update();
        }

        void update_fixed() {
            if (clock_delay.count() > 0) {
                return;
            }

            auto tilemap = (Runtime::Pac::Tilemap *)getEntitysFromID("Tilemap")[0];
            auto amount = std::count(tilemap->pellets.begin(), tilemap->pellets.end(), Runtime::Pac::PACPellet::regular);
            


            if (inactive_ghosts.size() > (amount/120)) {
                if ((Runtime::current_tick % 3) != 0) goto release_yield_frame;
                const Math::pointi target_active_position = tilemap->position + Math::pointi{112, 92};
                auto pac = inactive_ghosts[0]->getComponent<Runtime::Pac::PacComponent>();

                #define PAC_GOTO(axis, pdir, ndir) if (pac->m_position.axis != target_active_position.axis) { \
                    unsigned int vec = std::signbit(target_active_position.axis - pac->m_position.axis)? -1 : 1; \
                    auto         dir = std::signbit(target_active_position.axis - pac->m_position.axis)? ndir : pdir;  \
                    pac->m_position.axis += vec; \
                    pac->m_direction = dir; \
                    goto release_yield_frame; \
                }
                

                PAC_GOTO(x, Runtime::Pac::PACDirection::LEFT,   Runtime::Pac::PACDirection::RIGHT);
                PAC_GOTO(y, Runtime::Pac::PACDirection::DOWN,   Runtime::Pac::PACDirection::UP);

                #undef PAC_GOTO
                
                inactive_ghosts[0]->getComponent<Runtime::Pac::Ghost::GhostComponent>()->state = Runtime::Pac::Ghost::STATE_SCATTER;
                inactive_ghosts.erase(inactive_ghosts.begin());
                pac->m_direction = (std::rand() % 2)? Runtime::Pac::PACDirection::LEFT : Runtime::Pac::PACDirection::RIGHT;
            }

            release_yield_frame:

            


            EntityManager::update_fixed();
            fixedupdateCounter();
        }

        void draw() {
            auto tilemap = (Runtime::Pac::Tilemap *)getEntitysFromID("Tilemap")[0];
            EntityManager::draw();
            Runtime::display_coins = false;
            Runtime::drawCounter();

            if (flags[0]) {
                const auto rect = SDL_Rect{tilemap->position.x + 80, tilemap->position.y + 136, 64, 8};
                SDL_SetRenderDrawColor(Graphics::renderer, 0, 0, 0, 255);
                SDL_RenderFillRect(Graphics::renderer, &rect);
                Graphics::drawText({tilemap->position.x + 80, tilemap->position.y + 136, 0, 0}, "Ready P1", Graphics::renderer);
            }
        }

        void cleanup() {
            EntityManager::cleanup();
            ghosts.clear();
            inactive_ghosts.clear();
            flags = 0;

            clock_delay = decltype(clock_delay)::zero();
        }
        

        std::bitset<8> flags = 0;
        bool release_ghost = true;
        std::vector<Runtime::Pac::Ghost::Ghost*> ghosts = {};
        std::vector<Runtime::Pac::Ghost::Ghost*> inactive_ghosts = {};
        Runtime::duration clock_delay = decltype(clock_delay)::zero();
    };
}


#endif