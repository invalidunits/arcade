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

            constexpr int16_t deadzone = 8192;
    
            struct PacMan : public Entity::Entity {
                PacMan() {
                    pacman_texture = ARCADE_LOADTEXTROM(IMGpacman);
                    registerComponent<PacComponent>();
                }

                void update_fixed() {
                    Entity::update_fixed();
                    auto pac = getComponent<PacComponent>();
                    auto x_axis_raw = Controls::axis_inputs[Controls::AXIS_X].load();
                    auto y_axis_raw = Controls::axis_inputs[Controls::AXIS_Y].load();

                    auto x_axis = std::abs(x_axis_raw) > deadzone? (0 < x_axis_raw) - (x_axis_raw < 0) : 0;
                    auto y_axis = std::abs(y_axis_raw) > deadzone? (0 < y_axis_raw) - (y_axis_raw < 0) : 0;
                    Math::pointi input = {
                        x_axis,
                        y_axis
                    };
                            

                    if (input.x != 0 || input.y != 0) {
                       m_direction_buffer = Runtime::Pac::dfromv(input);
                    }
                    if (m_direction_buffer != PACDirection::LAST) {
                        auto dirblocked = (pac->getTileMap()->isBlocked(
                                pac->getCurrentTile() + vfromd(m_direction_buffer)));
                        if (!dirblocked) {
                            pac->m_direction = m_direction_buffer;
                            m_direction_buffer = PACDirection::LAST;
                        }
                    }
                    
                    auto tilemap = pac->getTileMap();
                    auto tile = pac->getCurrentTile();
                    int tile_index = tile.x + tile.y*tilemap->tilemap_size.w;
                    if (tile_index < 0)
                        tile_index = 0;

                    if (tile_index < tilemap->pellets.size())
                        // Allow super pellet.
                        tilemap->pellets[tile_index] = Pac::PACPellet::none;
                    
                }

                const std::string_view getIdentity() const { return "PacMan"; }
                void update() {
                    auto pac = getComponent<PacComponent>();
                    if (!pac->getTileMap()->isBlocked(pac->getNextTile()))
                        time_elasped += Runtime::delta_time;
                }

                void draw() {
                    Entity::Entity::draw();
                    auto pac = getComponent<PacComponent>();
                    SDL_Rect src = {0, 0, 16, 16};
                    SDL_Rect dst = {
                        pac->m_position.x-8, 
                        pac->m_position.y-8, 16, 16};
                    
                    auto direction = Runtime::Pac::vfromd(pac->m_direction);
                    auto angle = atan2(direction.y, direction.x)/(M_PI/180.0);

                    // Animation
                    int frame = ((time_elasped/Runtime::tick_length)/3) % 3;
                    src.x += frame*16;


                    SDL_RenderCopyEx(Graphics::renderer, pacman_texture.get(), &src, &dst, 
                        angle, NULL, SDL_FLIP_NONE);
                }

                Graphics::shared_texture pacman_texture = nullptr;
                Runtime::duration time_elasped = Runtime::duration::zero();
                Runtime::Pac::PACDirection m_direction_buffer = PACDirection::LAST;
            };
        
    }
   
}


#endif