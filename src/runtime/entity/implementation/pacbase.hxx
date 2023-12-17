#ifndef _ARCADE_PACBASE
#define _ARCADE_PACBASE

#include <graphics/graphics.hxx>
#include <rom/rom.hxx>
#include <system/math.hxx>
#include <system/clock.hxx>
#include <runtime/entity/entity.hxx>
#include "tileset.hxx"


namespace Runtime {
    namespace Pac {
            // To prevent invalid conversions.


            struct PacComponent : public Entity::Component {   
                PacComponent(Entity::Entity *entity): Entity::Component(entity) {}

                bool atIntersection() {
                    

                    auto tile = getCurrentTile();
                    if (last_tile.x == tile.x && last_tile.y == tile.y) return false;
                    auto tilemap = getTileMap();
                    unsigned char path_count = 0;

                    for (unsigned char i = 0; i < (unsigned char)PACDirection::LAST; i++) {
                        PACDirection direction = (PACDirection)i;
                        if (tilemap->isBlocked(vfromd(direction) + tile)) continue;
                        path_count += 1;
                    }
                    return path_count > 1; 
                }

                Runtime::Pac::Tilemap *getTileMap() {
                    auto tilemaps = m_entity->getManager()->getEntitysFromID("Tilemap");
                    if (tilemaps.size() <= 0) return nullptr;
                    return (Runtime::Pac::Tilemap*)tilemaps.front();;
                }


                movement_tile getCurrentTile() {
                    auto tilemap = getTileMap();
                    SDL_assert(tilemap != nullptr);
                    return tilemap->getClosestTile(m_position);
                } 

                movement_tile getNextTile() {
                    auto tilemap = getTileMap();
                    SDL_assert(tilemap != nullptr);
                    Math::pointi offset = Runtime::Pac::vfromd(m_direction)*Runtime::Pac::tile_size/2;
                    Math::pointi nexttilep = m_position + offset;
                    return tilemap->getClosestTile(nexttilep);
                }

                void update_fixed() {
                    last_tile = getCurrentTile();
                    if (!moving) return;
                    auto tilemap = getTileMap();
                    SDL_assert(tilemap != nullptr);
                    auto next_tile = getNextTile();

                    // Clamp to grid.
                    if (m_direction == PACDirection::RIGHT || m_direction == PACDirection::LEFT)  {
                        m_position.y = (float(m_position.y/Pac::tile_size.y) + 0.5)*Pac::tile_size.y;
                    } else {
                        m_position.x = (float(m_position.x/Pac::tile_size.x) + 0.5)*Pac::tile_size.x;
                    }
                    
                    if (tilemap->isBlocked(next_tile)) return;
                    auto dvector = Runtime::Pac::vfromd(m_direction);
                    m_position = m_position + dvector;

   
        



                }

                // bool canProceed();
                movement_tile last_tile = {8, 8}; 
                Math::pointi m_position = {8, 8}; 
                bool moving = true ;
                PACDirection m_direction = PACDirection::RIGHT;
            };


            struct TestMan : public Entity::Entity {
                TestMan() {
                    registerComponent<PacComponent>();
                }

                void update_fixed() {
                    Entity::update_fixed();
                    auto state = SDL_GetKeyboardState(nullptr);
                    Math::pointi input = {
                        state[SDL_SCANCODE_RIGHT] - state[SDL_SCANCODE_LEFT],
                        state[SDL_SCANCODE_DOWN] - state[SDL_SCANCODE_UP]
                    };
                    
                    if (input.x != 0 || input.y != 0) {
                       getComponent<PacComponent>()->m_direction = Runtime::Pac::dfromv(input);
                    }
                }

                const std::string_view getIdentity() const { return "TestMan"; }

                void draw() {
                    Entity::Entity::draw();
                    Graphics::drawText({
                        getComponent<PacComponent>()->m_position.x-4, 
                        getComponent<PacComponent>()->m_position.y-4, 
                        0, 0}, "E", Graphics::renderer);

                }
            };
    }
   
}


#endif