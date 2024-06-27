#include "gameplay.hxx"
#include "mainmenu.hxx"

#include <sfx/sfx.hxx>
#include <optional>

std::optional<std::vector<Runtime::Pac::PACPellet>> save_pellets = {}; 

namespace Runtime {
    void Gameplay::setup() {
        
        Runtime::Sound::SoundEffect<ROM::gSFXBeginData>::StartSound();
        auto tilemap =  addEntity<Runtime::Pac::Tilemap>();
        tilemap->position = {0, 18};

        auto pacman =   addEntity<Runtime::Pac::PacMan>();
        pacman->getComponent<Runtime::Pac::PacComponent>()->m_position = tilemap->position + Math::pointi{112, 188};

        tilemap->pellets = save_pellets.value_or(tilemap->pellets);
        save_pellets = {};

        ghosts = {
                        addEntity<Runtime::Pac::Ghost::Blinky>(),
                        addEntity<Runtime::Pac::Ghost::Pinky>(),
                        addEntity<Runtime::Pac::Ghost::Clyde>(),
        };

        ghosts[0]->getComponent<Runtime::Pac::PacComponent>()->m_position = tilemap->position + Math::pointi{112, 92};
        ghosts[0]->getComponent<Runtime::Pac::Ghost::GhostComponent>()->state = Runtime::Pac::Ghost::STATE_SCATTER;
        inactive_ghosts = ghosts;
        inactive_ghosts.erase(inactive_ghosts.begin());


        inactive_ghosts[0]->getComponent<Runtime::Pac::PacComponent>()->m_position = tilemap->position + Math::pointi{112, 120};
        inactive_ghosts[1]->getComponent<Runtime::Pac::PacComponent>()->m_position = tilemap->position + Math::pointi{128, 120};
        // inactive_ghosts[2]->getComponent<Runtime::Pac::PacComponent>()->m_position = {96, 120};
        
        flags[0] = 1;
        clock_delay = std::chrono::duration_cast<decltype(clock_delay)>(
            std::chrono::seconds(5)
        );

        game_over = false;
    }


    void Gameplay::update() {
        if (Controls::button_inputs[Controls::BUTTON_A]) 
            clock_delay = decltype(clock_delay)::zero();

        if (clock_delay.count() > 0) {
            clock_delay -= Runtime::delta_time;
            return;
        }

        flags[0] = 0;
        EntityManager::update();
    }

    void Gameplay::update_fixed() {
        if (clock_delay.count() > 0) {
            return;
        }

        auto tilemap = (Runtime::Pac::Tilemap *)getEntitysFromID("Tilemap")[0];
        auto amount = std::count(tilemap->pellets.begin(), tilemap->pellets.end(), Runtime::Pac::PACPellet::regular);

        if (game_over) {
            if (Runtime::live_count <= 1) {
                flags[6] = 1;
                game_over = false;
                clock_delay = std::chrono::duration_cast<decltype(clock_delay)>(
                    std::chrono::seconds(5)
                );

                return;
            }
            save_pellets = tilemap->pellets;
            live_count -= 1;
            SceneManager::popScene();
            SceneManager::pushScene<Gameplay>();
        }

        
        

        if (flags[4]) {
            Runtime::current_score += 1000;
            flags = 0;
            SceneManager::popScene();
            SceneManager::pushScene<Gameplay>();
            return;
        }

        if (flags[5]) {
            flags.set(4);
            Runtime::Sound::SoundEffect<ROM::gSFXYooHOOData>::StartSound(1);
            clock_delay = std::chrono::seconds(10);
            return;
        }


        if (amount <= 0) {
            flags.set(5);
            clock_delay = std::chrono::duration_cast<decltype(clock_delay)>(
                std::chrono::seconds(3)
            );
            return;
        }

        
        



        if (flags.test(7)) {
            flags[7] = 0;
            game_over = true;
            clock_delay = std::chrono::duration_cast<decltype(clock_delay)>(
                std::chrono::seconds(5)
            );

            return;
        }

        if (flags.test(6)) {
            SceneManager::gotoScene<Runtime::MainMenu>();
        }


        if (inactive_ghosts.size() > (amount/120)) {
            if ((Runtime::current_tick % 3) != 0) goto release_yield_frame;
            const Math::pointi target_active_position = tilemap->position + Math::pointi{112, 92};
            auto pac = inactive_ghosts[0]->getComponent<Runtime::Pac::PacComponent>();

            #define PAC_GOTO(axis, pdir, ndir) if (pac->m_position.axis != target_active_position.axis) { \
                unsigned int vec = std::signbit(target_active_position.axis - pac->m_position.axis)? -1 : 1; \
                auto         dir = std::signbit(target_active_position.axis - pac->m_position.axis)? ndir : pdir;  \
                pac->m_position.axis += vec; \
                pac->m_direction = dir; \
                goto release_yield_frame; \
            }
            

            PAC_GOTO(x, Runtime::Pac::PACDirection::LEFT,   Runtime::Pac::PACDirection::RIGHT);
            PAC_GOTO(y, Runtime::Pac::PACDirection::DOWN,   Runtime::Pac::PACDirection::UP);

            #undef PAC_GOTO
            
            inactive_ghosts[0]->getComponent<Runtime::Pac::Ghost::GhostComponent>()->state = Runtime::Pac::Ghost::STATE_SCATTER;
            inactive_ghosts.erase(inactive_ghosts.begin());
            pac->m_direction = (std::rand() % 2)? Runtime::Pac::PACDirection::LEFT : Runtime::Pac::PACDirection::RIGHT;
        }

        release_yield_frame:

        


        EntityManager::update_fixed();
        fixedupdateCounter();
    }

    void Gameplay::draw() {
        auto tilemap = (Runtime::Pac::Tilemap *)getEntitysFromID("Tilemap")[0];
        
        if (!flags[7] && !game_over)
            EntityManager::draw();
        else {
            auto pacman = (Runtime::Pac::Tilemap *)getEntitysFromID("PacMan")[0];
            // Only draw the tilemap and pacman.
            tilemap->draw();
            pacman->draw();
        }

        Runtime::display_coins = false;
        Runtime::drawCounter();

        if (flags[0]) {
            const auto rect = SDL_Rect{tilemap->position.x + 80, tilemap->position.y + 136, 64, 8};
            SDL_SetRenderDrawColor(Graphics::renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(Graphics::renderer, &rect);
            Graphics::drawText({tilemap->position.x + 80, tilemap->position.y + 136, 0, 0}, "Ready P1", Graphics::renderer);
        }

        if (flags[6]) {
            const auto rect = SDL_Rect{tilemap->position.x + 80, tilemap->position.y + 136, 64, 8};
            SDL_SetRenderDrawColor(Graphics::renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(Graphics::renderer, &rect);
            Graphics::drawText({tilemap->position.x + 80, tilemap->position.y + 136, 0, 0}, "Game OVER", Graphics::renderer);
        }
    }

    void Gameplay::cleanup() {
        EntityManager::cleanup();
        ghosts.clear();
        inactive_ghosts.clear();
        flags = 0;

        clock_delay = decltype(clock_delay)::zero();
    }
}