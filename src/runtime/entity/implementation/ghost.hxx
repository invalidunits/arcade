#ifndef _ARCADE_PACGHOST
#define _ARCADE_PACGHOST

#include <graphics/graphics.hxx>
#include <rom/rom.hxx>
#include <system/math.hxx>
#include <system/clock.hxx>
#include <runtime/entity/entity.hxx>
#include "pacbase.hxx"
#include "tileset.hxx"

#include <sfx/sfx.hxx>
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
                STATE_INACTIVE,
                
                STATE_BASE_ENTERING,
                STATE_BASE_EXITING,
            };
            
            movement_tile scaredBehavior(GhostComponent *ghostcomponent);
            movement_tile retreatBehavior(GhostComponent *ghostcomponent);


            using movement_behavior_array = std::array<movement_behavior, STATE_LAST>;
            struct GhostComponent : public Runtime::Entity::Component {
                GhostComponent(Entity::Entity *entity,
                    Runtime::duration scatter_time,
                    Runtime::duration chase_time,
                    movement_behavior_array behavior
                    ): Entity::Component(entity), m_behaviors(behavior), m_scatter_time(scatter_time), m_chase_time(chase_time) {
                        state_timer = m_scatter_time;
                    }

                void killGhost() {
                    if (state <= STATE_SCARED) {
                        PacComponent *pac  = m_entity->getComponent<PacComponent>();
                        Runtime::Sound::SoundEffect<ROM::gSFXeatGhostData>::StartSound();
                        getEntity()->getManager()->addEntity<PointsEffect>(pac->m_position, 200);
                        Runtime::current_score += 200;  
                        getEntity()->getManager()->addEntityDelay(Runtime::tick_length*20);
                        state = STATE_RETREAT;
                        state_timer = m_scatter_time;
                    }
                }
                void update_fixed();


                Runtime::duration state_timer = Runtime::duration::zero();
                State state = State::STATE_INACTIVE;
                Pac::movement_tile target_tile = {0, 0};

                const movement_behavior_array m_behaviors;
                const Runtime::duration m_scatter_time;
                const Runtime::duration m_chase_time;

                Runtime::duration block_time =  10*Runtime::tick_length;
                Runtime::duration block_timer = block_time;
            };

            constexpr int ghost_width = 16, ghost_height = 16;

            struct Ghost : Entity::Entity, IKillable {
                void kill() {
                    getComponent<GhostComponent>()->killGhost();
                }
                void draw();
                const inline std::vector<std::string_view> getGroups() const {return {"Ghost"}; }
                Graphics::shared_texture texture;
            };



            struct Blinky : Ghost {
                const inline std::string_view getIdentity() const { return "Blinky"; }
                Blinky() {
                    texture = ARCADE_LOADTEXTROM(IMGghostBlinky);
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
                            },
                            scaredBehavior,
                            retreatBehavior
                        }
                    );
                }
            };

            struct Inky : Ghost {
                const inline std::string_view getIdentity() const { return "Inky"; }
                // TODO: make this reference wwe
                std::string best_friend;
                Inky(std::string_view _best_friend) {
                    best_friend = _best_friend;
                    texture = ARCADE_LOADTEXTROM(IMGghostInky);
                    registerComponent<PacComponent>();
                    registerComponent<GhostComponent>(
                        // Inky's always chasing when Red's out.
                        std::chrono::duration_cast<Runtime::duration>(std::chrono::seconds(10)),
                        std::chrono::duration_cast<Runtime::duration>(std::chrono::seconds(99999999)),
                        movement_behavior_array{
                            // Scatter
                            [](GhostComponent *comp) {
                                return movement_tile{ARCADE_LOGIC_WIDTH/tile_size.w, ARCADE_LOGIC_HEIGHT/tile_size.h};
                            },

                            // Chase
                            [this](GhostComponent *comp) {
                                auto pacmen = comp->getEntity()->getManager()->getEntitysFromID("PacMan");
                                if (pacmen.size() <= 0) {   
                                    return movement_tile{0, 0};
                                }

                                auto pacs_tile = pacmen[0]->getComponent<PacComponent>()->getCurrentTile();
                                auto friends = comp->getEntity()->getManager()->getEntitysFromID(best_friend);
                                if (friends.size() <= 0) {
                                    return pacs_tile;                                 
                                }

                                auto best_friend = friends[0];

                                if (best_friend->getComponent<GhostComponent>()->state != STATE_CHASE) {
                                    return retreatBehavior(comp);
                                }

                                auto best_friend_tile = best_friend->getComponent<PacComponent>()->getCurrentTile();
                                auto bestys_offset = best_friend_tile - pacs_tile;
                                return pacs_tile - bestys_offset;
                            },
                            scaredBehavior,
                            retreatBehavior
                        }
                    );
                }
            };

            struct Pinky : Ghost {
                const inline std::string_view getIdentity() const { return "Pinky"; }
                Pinky() {
                    texture = ARCADE_LOADTEXTROM(IMGghostPinky);
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
                            },
                            scaredBehavior,
                            retreatBehavior
                        }
                    );
                }
            };

            struct Clyde : Ghost {
                const inline std::string_view getIdentity() const { return "Clyde"; }
                const inline std::vector<std::string_view> getGroups() const { 
                    auto groups = Ghost::getGroups(); groups.push_back("Clyde");  return groups; }
                static constexpr unsigned int pacman_follow_distance = 6;


                Clyde() {
                    texture = ARCADE_LOADTEXTROM(IMGghostClyde);
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
                            },
                            scaredBehavior,
                            retreatBehavior
                        }
                    );
                }
            };
        }

    }
}


#endif