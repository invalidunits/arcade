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
                STATE_INACTIVE,
            };

            enum TargetType {
                TARGET_TILE,
                TARGET_RANDOM
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

                void update_fixed();


                Runtime::duration state_timer = Runtime::duration::zero();
                State state = State::STATE_INACTIVE;
                Pac::movement_tile target_tile = {0, 0};
                TargetType target_type = TARGET_TILE;

                const movement_behavior_array m_behaviors;
                const Runtime::duration m_scatter_time;
                const Runtime::duration m_chase_time;
            };

            constexpr int ghost_width = 16, ghost_height = 16;

            struct Ghost : Entity::Entity {
                void draw();
                const inline std::vector<std::string_view> getGroups() const {return {"Ghost"}; }
                Graphics::shared_texture texture;
            };



            struct Blinky : Ghost {
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
                            }
                        }
                    );
                }
            };

            struct Pinky : Ghost {
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
                            }
                        }
                    );
                }
            };

            struct Clyde : Ghost {
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
                            }
                        }
                    );
                }
            };
        }

    }
}


#endif