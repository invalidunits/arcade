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


                pac->moving = true;
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
                            target_type = TARGET_TILE;
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
                            pacman->killPacman();
                        else if (state == STATE_SCARED)
                            state = STATE_RETREAT;
                    }
                }        

                if (pac->atIntersection()) {
                    if (m_behaviors.at(state)) target_tile = m_behaviors[state](this);
                    
                    auto position = pac->m_position;
                    auto tile = pac->getCurrentTile();
                    PACDirection opposing_direction = dfromv(Math::pointi{0, 0}-vfromd(pac->m_direction));
                    PACDirection direction = (PACDirection)pac->m_direction;
                        
                    unsigned int priority = ~0;

                    PACDirection x = (PACDirection)(std::rand() % (int) PACDirection::LAST);
                    for (unsigned char i = 0; i < (int)PACDirection::LAST; i++) {
                        direction = (PACDirection)i;
                        if (direction == opposing_direction) continue; // Ghosts never backtrack.
                        if (tilemap->isBlocked(vfromd(direction) + tile)) continue; // Ghosts go where they can move.

                        switch (target_type) {
                            case TARGET_RANDOM:
                            {
                                int p = (unsigned int)x - i;
                                if (p < priority) {
                                    priority = p;
                                    pac->m_direction = direction;
                                }
                            }
                                break;
                            
                            default:
                            case TARGET_TILE:
                                auto displacement = (vfromd(direction) + tile) - target_tile;
                                auto distance = displacement.x*displacement.x + displacement.y*displacement.y; 

                                if (distance <= priority) {
                                    pac->m_direction = direction;
                                    priority = distance;
                                }

                                break;
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
                if (ghost->state != STATE_RETREAT) 
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
            movement_tile scaredBehavior(Runtime::Pac::Ghost::GhostComponent *ghostcomponent) {
                auto pac = ghostcomponent->getEntity()->getComponent<Runtime::Pac::PacComponent>();

                return {0, 0};
            }
            movement_tile retreatBehavior(Runtime::Pac::Ghost::GhostComponent *ghostcomponent) {
                return {0, 0};
            }
        }
    }
}



void ateSuper(Runtime::Entity::EntityManager *manager) {
    for (auto&& entity : manager->getEntitysFromGroups("Ghost")) {
        Runtime::Pac::Ghost::GhostComponent *ghost = entity->getComponent<Runtime::Pac::Ghost::GhostComponent>();
        if (ghost->state == Runtime::Pac::Ghost::STATE_INACTIVE) continue;

        ghost->state = Runtime::Pac::Ghost::STATE_SCARED;
        ghost->target_type = Runtime::Pac::Ghost::TARGET_RANDOM;
    }
}