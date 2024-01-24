#ifndef _ARCADE_PACBASE
#define _ARCADE_PACBASE

#include <graphics/graphics.hxx>
#include <rom/rom.hxx>
#include <system/math.hxx>
#include <system/clock.hxx>
#include <runtime/entity/entity.hxx>
#include "tileset.hxx"

#include <system/controls.hxx>
#include <runtime/counters/counters.hxx>


namespace Runtime {
    namespace Pac {
            // To prevent invalid conversions.


            struct PacComponent : public Entity::Component {   
                PacComponent(Entity::Entity *entity): Entity::Component(entity) {}

                bool atIntersection();

                inline Runtime::Pac::Tilemap *getTileMap() const {
                    auto tilemaps = m_entity->getManager()->getEntitysFromID("Tilemap");
                    if (tilemaps.size() <= 0) return nullptr;
                    return (Runtime::Pac::Tilemap*)tilemaps.front();;
                }


                inline movement_tile getCurrentTile() const {
                    auto tilemap = getTileMap();
                    SDL_assert(tilemap != nullptr);
                    return tilemap->getClosestTile(m_position);
                } 

                inline movement_tile getNextTile() const {
                    auto tilemap = getTileMap();
                    SDL_assert(tilemap != nullptr);
                    Math::pointi offset = Runtime::Pac::vfromd(m_direction)*Runtime::Pac::tile_size/2;
                    Math::pointi nexttilep = m_position + offset;
                    return tilemap->getClosestTile(nexttilep);
                }

                void update_fixed();

                // bool canProceed();
                movement_tile last_tile = {8, 8}; 
                Math::pointi m_position = {8, 8}; 
                bool moving = false;
                int inverse_speed = 1.0;
                PACDirection m_direction = PACDirection::RIGHT;
            };

            constexpr int16_t deadzone = 8192;
    
            struct PacMan : public Entity::Entity {
                PacMan() {
                    pacman_texture = ARCADE_LOADTEXTROM(IMGpacman);
                    registerComponent<PacComponent>();
                }

                const inline std::string_view getIdentity() const { return "PacMan"; }
                void update_fixed();

                void update();

                void draw();

                Graphics::shared_texture pacman_texture = nullptr;
                Runtime::duration time_elasped = Runtime::duration::zero();
                Runtime::Pac::PACDirection m_direction_buffer = PACDirection::LAST;
            };
        
    }
   
}


#endif