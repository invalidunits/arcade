#ifndef _ARCADE_PACGHOST
#define _ARCADE_PACGHOST

#include <graphics/graphics.hxx>
#include <rom/rom.hxx>
#include <system/math.hxx>
#include <system/clock.hxx>
#include <runtime/entity/entity.hxx>
#include "pacbase.hxx"
#include "tileset.hxx"

#include <functional>
#include <array>

namespace Runtime {
    namespace Pac {
        namespace Ghost {
            struct GhostComponent;
            using movement_behavior = std::function<movement_tile(GhostComponent*)>;
            enum State {
                STATE_SCATTER = 0,
                STATE_CHASE,
                STATE_SCARED,
                STATE_RETREAT,
                STATE_LAST,
            };

            using movement_behavior_array = std::array<movement_behavior, STATE_LAST>;
            struct GhostComponent : public Runtime::Entity::Component {
                GhostComponent(Entity::Entity *entity,
                    Runtime::duration scatter_time,
                    Runtime::duration chase_time,
                    movement_behavior_array behavior
                    ): Entity::Component(entity), m_behaviors(behavior), m_scatter_time(scatter_time), m_chase_time(chase_time) {
                        state_timer = m_scatter_time;
                    }

                void update_fixed() {
                    PacComponent *pac  = m_entity->getComponent<PacComponent>();
                    SDL_assert(pac != nullptr);
                    pac->moving = true;
                    auto tilemap = pac->getTileMap();
                    state_timer -= Runtime::tick_length;
                    if (state_timer.count() < 0) {
                        switch (state) {
                            default:
                                state = STATE_SCATTER;
                                pac->m_direction = dfromv(Math::pointi{0, 0}-vfromd(pac->m_direction));
                                state_timer = m_scatter_time;
                                break;
                            
                            case STATE_SCATTER:
                                state = STATE_CHASE;
                                pac->m_direction = dfromv(Math::pointi{0, 0}-vfromd(pac->m_direction));
                                state_timer = m_chase_time;
                                break;
                        }
                    }
                    
                    

                    if (pac->atIntersection()) {
                        if (m_behaviors.at(state)) target_tile = m_behaviors[state](this);
                        
                        auto position = pac->m_position;
                        auto tile = pac->getCurrentTile();
                        PACDirection opposing_direction = dfromv(Math::pointi{0, 0}-vfromd(pac->m_direction));
                        PACDirection direction = (PACDirection)pac->m_direction;
                         
                        unsigned int priority = ~0;
                        for (unsigned char i = 0; i < (int)PACDirection::LAST; i++) {
                            direction = (PACDirection)i;
                            if (direction == opposing_direction) continue; // Ghosts never backtrack.
                            if (tilemap->isBlocked(vfromd(direction) + tile)) continue; // Ghosts go where they can move.

                            auto displacement = (vfromd(direction) + tile) - target_tile;
                            auto distance = displacement.x*displacement.x + displacement.y*displacement.y; 

                            if (distance <= priority) {
                                pac->m_direction = direction;
                                priority = distance;
                            }
                        }
                    }

                }


                Runtime::duration state_timer = Runtime::duration::zero();
                State state = State::STATE_SCARED;
                Pac::movement_tile target_tile = {0, 0};

                const movement_behavior_array m_behaviors;
                const Runtime::duration m_scatter_time;
                const Runtime::duration m_chase_time;
            };

            constexpr int ghost_width = 16, ghost_height = 16;

            struct Ghost : Entity::Entity {
                void draw() {
                    Entity::Entity::draw();

                    PacComponent *pac = getComponent<PacComponent>();
                    GhostComponent *ghost = getComponent<GhostComponent>();

                    // Body
                    int ghostframe = Runtime::current_tick/6;
                    SDL_Rect src = {ghost_width*(ghostframe % 2), 0, ghost_width, ghost_height};
                    
                    int scared_frame = Runtime::current_tick/12;
                    if (ghost->state == STATE_SCARED) src.x += (1 + (scared_frame % 2))*ghost_width*2;
                
                    
                    SDL_Rect dst = {pac->m_position.x - ghost_width/2, pac->m_position.y - ghost_height/2, ghost_width, ghost_height};
                    SDL_RenderCopy(Graphics::renderer, texture.get(), &src, &dst);

                    if (ghost->state != STATE_SCARED) {  
                        int eyeframe = 0;
                        SDL_RendererFlip eyefliped = SDL_RendererFlip::SDL_FLIP_NONE;
                        switch (pac->m_direction) {
                            case Pac::PACDirection::LEFT:
                                eyefliped = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
                            case Pac::PACDirection::RIGHT:
                                eyeframe = 0; break;
                            case Pac::PACDirection::UP:
                                eyeframe = 1; break;
                            case Pac::PACDirection::DOWN:
                                eyeframe = 2; break;
                        }
                        src = {ghost_width*(eyeframe + 6), 0, ghost_width, ghost_height};
                        SDL_RenderCopyEx(Graphics::renderer, texture.get(), &src, &dst, 0, NULL, eyefliped);
                    } else if (ghost->state == STATE_SCARED) {
                        src = {(9 + (scared_frame % 2))*ghost_width, 0, ghost_width, ghost_height};
                        SDL_RenderCopy(Graphics::renderer, texture.get(), &src, &dst);
                    }
                }

                Graphics::shared_texture texture;
            };



            struct Blinky : Ghost {
                Blinky() {
                    texture = ARDCADE_LOADTEXTROM(IMGghostBlinky);
                    registerComponent<PacComponent>();
                    registerComponent<GhostComponent>(
                        std::chrono::duration_cast<Runtime::duration>(std::chrono::seconds(16)),
                        std::chrono::duration_cast<Runtime::duration>(std::chrono::seconds(30)),
                        movement_behavior_array{
                            // Scatter
                            [](GhostComponent *comp) { 
                                return movement_tile{0, 0};
                            },

                            // Chase
                            [](GhostComponent *comp) { 
                                auto pacmen = comp->getEntity()->getManager()->getEntitysFromID("PacMan");
                                if (pacmen.size() > 0) {   
                                    return pacmen[0]->getComponent<PacComponent>()->getCurrentTile();
                                }
                                return movement_tile{0, 0};
                            }
                        }
                    );
                }
            };

            struct Pinky : Ghost {
                Pinky() {
                    texture = ARDCADE_LOADTEXTROM(IMGghostPinky);
                    registerComponent<PacComponent>();
                    registerComponent<GhostComponent>(
                        std::chrono::duration_cast<Runtime::duration>(std::chrono::seconds(16)),
                        std::chrono::duration_cast<Runtime::duration>(std::chrono::seconds(30)),
                        movement_behavior_array{
                            // Scatter
                            [](GhostComponent *comp) { 
                                return movement_tile{ARCADE_LOGIC_WIDTH/tile_size.w, 0};
                            },

                            // Chase
                            [](GhostComponent *comp) { 
                                auto pacmen = comp->getEntity()->getManager()->getEntitysFromID("PacMan");
                                if (pacmen.size() > 0) {   
                                    auto pac = pacmen[0]->getComponent<PacComponent>();
                                    return pac->getCurrentTile() + vfromd(pac->m_direction)*5;
                                }
                                return movement_tile{0, 0};
                            }
                        }
                    );
                }
            };

            struct Clyde : Ghost {
                static constexpr unsigned int pacman_follow_distance = 6;


                Clyde() {
                    texture = ARDCADE_LOADTEXTROM(IMGghostClyde);
                    registerComponent<PacComponent>();
                    registerComponent<GhostComponent>(
                        std::chrono::duration_cast<Runtime::duration>(std::chrono::seconds(16)),
                        std::chrono::duration_cast<Runtime::duration>(std::chrono::seconds(30)),
                        movement_behavior_array{
                            // Scatter
                            [](GhostComponent *comp) { 
                                return movement_tile{0, ARCADE_LOGIC_HEIGHT/tile_size.h};
                            },

                            // Chase
                            [](GhostComponent *comp) { 
                                auto my_pac = comp->getEntity()->getComponent<PacComponent>();
                                auto tilemap = my_pac->getTileMap();
                                auto pacmen = comp->getEntity()->getManager()->getEntitysFromID("PacMan");
                                if (pacmen.size() > 0) {   
                                    auto pac = pacmen[0]->getComponent<PacComponent>();
                                    auto displacement = my_pac->m_position - pac->m_position;
                                    auto distance = displacement.x*displacement.x + displacement.y*displacement.y; 
                                    if (distance <= std::pow(pacman_follow_distance, 4)) 
                                        return pac->getCurrentTile();
                                }
                                return (movement_tile)tilemap->tilemap_size/2;
                            }
                        }
                    );
                }
            };
        }

    }
}


#endif