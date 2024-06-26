#include "pacbase.hxx"


void ateSuper(Runtime::Entity::EntityManager *manager);

#include <sfx/sfx.hxx>
#include <rom/rom.hxx>

namespace Runtime {
    namespace Pac {
        bool PacComponent::atIntersection() {
            auto tile = getCurrentTile();
            auto tilemap = getTileMap();


            auto tile_dir = dfromv(tilemap->getTilePosition(tile) - m_position);
            
            if(tile_dir != m_direction) 
                return false;
            



            unsigned char path_count = 0;

            for (unsigned char i = 0; i < (unsigned char)PACDirection::LAST; i++) {
                PACDirection direction = (PACDirection)i;
                if (tilemap->isBlocked(vfromd(direction) + tile)) continue;
                auto opposingdir = dfromv(Math::pointi{0, 0}-vfromd(m_direction));
                if (direction == opposingdir) continue;

                path_count += 1;
            }
            return path_count > 0; 
        }

        void PacComponent::update_fixed() {


            last_tile = getCurrentTile();
            int val = Runtime::current_tick % inverse_speed;
            if (val) {
                return; // skip frames to account for inverse_speed.
            } 

            
            if (!moving) return;
            auto tilemap = getTileMap();
            SDL_assert(tilemap != nullptr);
            auto next_tile = getNextTile();

            // Clamp to grid.
            #define CLAMP_TO_GRID(axis) m_position.axis = (float((m_position.axis - tilemap->position.axis)/Pac::tile_size.axis) + 0.5)*Pac::tile_size.axis + tilemap->position.axis;
            if (m_direction == PACDirection::RIGHT || m_direction == PACDirection::LEFT)  {
                CLAMP_TO_GRID(y);
            } else {
                CLAMP_TO_GRID(x);
            }

            #undef CLAMP_TO_GRID

            m_position.x = Math::wrap_number(m_position.x, -Pac::tile_size.x, Pac::tile_size.x*(tilemap->tilemap_size.x + 1));
            m_position.y = Math::wrap_number(m_position.y, -Pac::tile_size.x, Pac::tile_size.y*(tilemap->tilemap_size.y + 1));
            
            if (tilemap->isBlocked(next_tile)) 
                return;
            m_position = m_position + Runtime::Pac::vfromd(m_direction);
        }

        
        void PacMan::update_fixed() {
            Entity::update_fixed();
            auto pac = getComponent<PacComponent>();
            auto x_axis_raw = Controls::axis_inputs[Controls::AXIS_X].load();
            auto y_axis_raw = Controls::axis_inputs[Controls::AXIS_Y].load();

            pac->moving = !dead;

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


            if (tile_index < tilemap->pellets.size()) {
                switch (tilemap->pellets[tile_index])  {
                    case Pac::PACPellet::regular:
                        Runtime::current_score += 10; 
                        Runtime::Sound::SoundEffect<ROM::gSFXChompData>::StartSound();
                        break;
                    
                    case Pac::PACPellet::super:
                        Runtime::current_score += 100;
                        //TODO: Add ghost eating
                        ateSuper(getManager());
                        break;

                }

                tilemap->pellets[tile_index] = Pac::PACPellet::none;
            }
                
            
        }

        
        void PacMan::update() {
            auto pac = getComponent<PacComponent>();
            if (!pac->getTileMap()->isBlocked(pac->getNextTile()))
                time_elasped += Runtime::delta_time;
        }

        void PacMan::killPacman() {
            if (dead) return;
            
            Runtime::Sound::SoundEffect<ROM::gSFXDeathData>::StartSound();

            dead = true;
            dead_tick = Runtime::current_tick;
            auto pac = getComponent<PacComponent>();
            pac->moving = false;
            getManager()->flags[7] = 1;
        }

        void PacMan::draw() {
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
            if (dead) {
                auto dtick = Runtime::current_tick - dead_tick;
                dtick -= 20;
                dtick /= 6;
                dtick = dtick >= 0? dtick : 0;
                frame = dtick;
            }

            src.x += frame*16;


            SDL_RenderCopyEx(Graphics::renderer, pacman_texture.get(), &src, &dst, 
                angle, NULL, SDL_FLIP_NONE);
        }
    }
}