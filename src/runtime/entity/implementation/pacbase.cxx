#include "pacbase.hxx"
#include "ghost.hxx"

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
            int val = Runtime::current_tick % inverse_speed;
            if (val) {
                return; // skip frames to account for inverse_speed.
            } 

            for (int i = 0; i < (repeat + 1); i++) {
                last_tile = getCurrentTile();
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
        }

        
        void PacMan::update_fixed() {
            Entity::update_fixed();
            auto pac = getComponent<PacComponent>();
            auto tilemap = pac->getTileMap();

            if (fireball_count <= 0)
                fire_man_timer -= Runtime::tick_length;
            bool new_fire_ball_input = Controls::button_inputs[Controls::BUTTON_B].load();
            if (fire_man_timer.count() > 0) {
                if (!new_fire_ball_input && fire_ball_input && fireball_count > 0) {
                    fireball_count -= 1;
                    getManager()->addEntity<FireBall>(
                        tilemap->getTilePosition(pac->getNextTile()),
                        pac->m_direction
                    );
                }
            }
            fire_ball_input = new_fire_ball_input;


            auto x_axis_raw = Controls::axis_inputs[Controls::AXIS_X].load();
            auto y_axis_raw = Controls::axis_inputs[Controls::AXIS_Y].load();

            pac->moving = !dead;

            auto x_axis = std::abs(x_axis_raw) > Controls::deadzone? (0 < x_axis_raw) - (x_axis_raw < 0) : 0;
            auto y_axis = std::abs(y_axis_raw) > Controls::deadzone? (0 < y_axis_raw) - (y_axis_raw < 0) : 0;
            Math::pointi input = {
                x_axis,
                y_axis
            };
                    

            if (input.x != 0 || input.y != 0) {
                m_direction_buffer = Runtime::Pac::dfromv(input);
            }
            if (m_direction_buffer != PACDirection::LAST) {
                auto dirblocked = (tilemap->isBlocked(
                        pac->getCurrentTile() + vfromd(m_direction_buffer)));
                if (!dirblocked) {
                    pac->m_direction = m_direction_buffer;
                    m_direction_buffer = PACDirection::LAST;
                }
            }
            
            
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
                        getManager()->addEntity<PointsEffect>(pac->m_position, 100);
                        //TODO: Add ghost eating
                        ateSuper(getManager());
                        break;
                    
                    case Pac::PACPellet::fire_super:
                        Runtime::current_score += 100;
                        getManager()->addEntity<PointsEffect>(pac->m_position, 100);
                        Runtime::Sound::SoundEffect<ROM::gSFXeatFruitData>::StartSound();
                        fire_man_timer = std::chrono::seconds(2);
                        fireball_count = 3;
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

        void PacMan::kill() {
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

            if (fire_man_timer.count() > 0) {
                SDL_SetTextureColorMod(pacman_texture.get(), 255, 127, 80);
                if (fire_man_timer < std::chrono::seconds(1)) {
                    if ((Runtime::current_tick/8)%2 == 0)
                        SDL_SetTextureColorMod(pacman_texture.get(), 255, 255, 255);
                    
                }
            }
                
            else SDL_SetTextureColorMod(pacman_texture.get(), 255, 255, 0);
            SDL_RenderCopyEx(Graphics::renderer, pacman_texture.get(), &src, &dst, 
                angle, NULL, SDL_FLIP_NONE);
        }
        FireBall::FireBall(Math::pointi pos, Pac::PACDirection dir) {
            collectables_texture = ARCADE_LOADTEXTROM(IMGmazeCollectables);
            auto pac = registerComponent<PacComponent>();
            pac->m_position = pos;
            pac->moving = true;
            pac->m_direction = dir;
            timeout_time = std::chrono::seconds(7);
            pac->repeat = 1;
        }

        void FireBall::draw() {                
            auto pac = getComponent<PacComponent>();
                SDL_Rect src = {24, 0, 8, 8};
                if (timeout_time <  std::chrono::seconds(3) &&
                    (Runtime::current_tick/8)%2 == 0) {
                    src.x = 32;
                }
                SDL_Rect dst = {
                    pac->m_position.x-(src.w/2), 
                    pac->m_position.y-(src.h/2), 8, 8};
                
                double angle = ((Runtime::current_tick/8)%4)*90.0;
                SDL_RenderCopyEx(Graphics::renderer, collectables_texture.get(), 
                    &src, &dst, angle, NULL, SDL_FLIP_NONE);
        }


        void FireBall::update_fixed() {
            timeout_time -= Runtime::tick_length;
            if (timeout_time.count() < 0) {
                queueFree();
            }
            Entity::update_fixed();

            auto pac = getComponent<PacComponent>();
            auto tilemap = pac->getTileMap();


            for (auto &entity : *getManager()) {


                if (auto killable = dynamic_cast<IKillable*>(entity.get()); killable != nullptr) {
                    if (entity->getIdentity() == "PacMan") {
                        auto &pacman = *dynamic_cast<PacMan*>(entity.get());
                        if (pacman.get_fireman_timer().count() > 0) {
                            continue;
                        }
                    }
                    auto their_pac = entity->getComponent<PacComponent>();
                    auto displacement = their_pac->m_position - pac->m_position;

                    auto distance_squared = displacement.x*displacement.x + displacement.y*displacement.y;
                    if (distance_squared < 4*4) killable->kill();
                    continue;
                }
            }


            if (pac->atIntersection() ||  tilemap->isBlocked(pac->getNextTile())) {
                auto position = pac->m_position;
                auto tile = pac->getCurrentTile();
                PACDirection opposing_direction = dfromv(Math::pointi{0, 0}-vfromd(pac->m_direction));
                PACDirection direction = (PACDirection)pac->m_direction;




                if (!tilemap->isBlocked(vfromd(direction) + tile)) goto set_dir;
                
                    

                for (;;) {
                    direction = PACDirection(std::rand() % int(PACDirection::LAST));

                    if (direction == opposing_direction) continue; // Fireballs never backtrack.
                    if (tilemap->isBlocked(vfromd(direction) + tile)) continue; // Fireballs go where they can move.
                    break;
                }
                
                set_dir:
                    pac->m_direction = direction;
            }
        }
    }
}