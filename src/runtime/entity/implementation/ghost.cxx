#include "ghost.hxx"

namespace Runtime {
    namespace Pac {
        namespace Ghost {
            void GhostComponent::update_fixed() {
                PacComponent *pac  = m_entity->getComponent<PacComponent>();
                SDL_assert(pac != nullptr);
                auto tilemap = pac->getTileMap();
                if (tilemap->slow_moving_tile[tilemap->getTileIndex(pac->getCurrentTile())]) {
                    pac->inverse_speed = int((Runtime::current_tick % 2) == 0) + 1;
                }

                if (state == STATE_INACTIVE){
                    pac->inverse_speed = 1;
                    return;
                }

                #define PAC_GOTO(axis, pdir, ndir) if (pac->m_position.axis != target_active_position.axis) { \
                    unsigned int vec = std::signbit(target_active_position.axis - pac->m_position.axis)? -1 : 1; \
                    auto         dir = std::signbit(target_active_position.axis - pac->m_position.axis)? ndir : pdir;  \
                    pac->m_position.axis += vec; \
                    pac->m_direction = dir; \
                    return; \
                }
            

            

                if (state == STATE_BASE_ENTERING) {
                    tilemap->ghost_hut_busy = true;
                    pac->moving = false;
                    if (Runtime::current_tick % 2 == 0) {
                        auto target_active_position = tilemap->ghost_hut_home + tilemap->position;
                        PAC_GOTO(x, Runtime::Pac::PACDirection::RIGHT,   Runtime::Pac::PACDirection::LEFT);
                        PAC_GOTO(y, Runtime::Pac::PACDirection::DOWN,   Runtime::Pac::PACDirection::UP);
                        state = STATE_BASE_EXITING; 
                    } else return;
                }


                if (state == STATE_BASE_EXITING) {
                    tilemap->ghost_hut_busy = true;
                    pac->moving = false;
                    if (Runtime::current_tick % 2 == 0) {
                        auto target_active_position = tilemap->ghost_hut_entrance + tilemap->position;
                        PAC_GOTO(x, Runtime::Pac::PACDirection::LEFT,   Runtime::Pac::PACDirection::RIGHT);
                        PAC_GOTO(y, Runtime::Pac::PACDirection::DOWN,   Runtime::Pac::PACDirection::UP);
                        tilemap->ghost_hut_busy = false;
                        state = STATE_SCATTER; 
                    } else return;
                }

                if (state == STATE_RETREAT && !tilemap->ghost_hut_busy) {
                    auto diff = pac->m_position - (tilemap->ghost_hut_entrance + tilemap->position);
                    auto difference = sqrt(diff.x*diff.x + diff.y*diff.y);

                    if (difference < 8) {
                        state = STATE_BASE_ENTERING;
                        return;
                    }
                    
                }


                #undef PAC_GOTO

                // Some states are handled specially and don't have behavior functions.
                if (state >= STATE_LAST) {
                    return;
                }

                pac->moving = true;

                if (state != STATE_SCARED)
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

                auto pacmen = getEntity()->getManager()->getEntitysFromID("PacMan");
                

                
                if (pacmen.size() > 0) {                       
                    auto pacman = static_cast<Pac::PacMan*>(pacmen[0]);
                    auto pacman_pac = pacman->getComponent<Pac::PacComponent>();
                    auto dist_squared = std::pow(pacman_pac->m_position.x - pac->m_position.x, 2) + std::pow(pacman_pac->m_position.y - pac->m_position.y, 2);
                    if (dist_squared < std::pow(4, 2)) {
                        if (state == STATE_CHASE || state == STATE_SCATTER)
                            pacman->kill();
                        else if (state == STATE_SCARED) {
                            killGhost();
                            return;
                        }
                            
                    }
                }        

                if (pac->atIntersection() ||  tilemap->isBlocked(pac->getNextTile())) {
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

            void Ghost::draw() {
                PacComponent *pac = getComponent<PacComponent>();
                GhostComponent *ghost = getComponent<GhostComponent>();

                // Body



                int ghostframe = Runtime::current_tick/6;
                SDL_Rect src = {ghost_width*(ghostframe % 2), 0, ghost_width, ghost_height};
                
                int scared_frame = Runtime::current_tick/12;
                if (ghost->state == STATE_SCARED) src.x += (1 + (scared_frame % 2))*ghost_width*2;
            
                
                SDL_Rect dst = {pac->m_position.x - ghost_width/2, pac->m_position.y - ghost_height/2, ghost_width, ghost_height};
                if (ghost->state != STATE_RETREAT && ghost->state != STATE_BASE_ENTERING) 
                    SDL_RenderCopy(Graphics::renderer, texture.get(), &src, &dst);

                dontrenderbody:

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
            movement_tile scaredBehavior(Runtime::Pac::Ghost::GhostComponent *comp) {
                auto pac = comp->getEntity()->getComponent<Runtime::Pac::PacComponent>();
                auto pacmen = comp->getEntity()->getManager()->getEntitysFromID("PacMan");
                if (pacmen.size() > 0) {  
                    auto pacman = pacmen[0]->getComponent<Runtime::Pac::PacComponent>();
                    return pacman->getTileMap()->tilemap_size - pacman->getCurrentTile(); 
                } 
                return {0, 0};
            }
            movement_tile retreatBehavior(Runtime::Pac::Ghost::GhostComponent *comp) {
                const Math::pointi &tilemap_size = comp->getEntity()->getComponent<Runtime::Pac::PacComponent>()->getTileMap()->tilemap_size;
                return {tilemap_size.x/2, tilemap_size.y/2};
            }
        }
    }
}



void ateSuper(Runtime::Entity::EntityManager *manager) {
    manager->addEntityDelay(Runtime::tick_length*20);
    Runtime::Sound::SoundEffect<ROM::gSFXeatFruitData>::StartSound();
    for (auto&& entity : manager->getEntitysFromGroups("Ghost")) {
        Runtime::Pac::Ghost::GhostComponent *ghost = entity->getComponent<Runtime::Pac::Ghost::GhostComponent>();
        if (ghost->state >= Runtime::Pac::Ghost::STATE_LAST) continue;

        ghost->state = Runtime::Pac::Ghost::STATE_SCARED;
        ghost->state_timer = ghost->m_scatter_time;
    }
}