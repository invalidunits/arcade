#include "gameplay.hxx"
#include "mainmenu.hxx"

#include <sfx/sfx.hxx>
#include <optional>

#include "intermission.hxx"


std::optional<std::vector<Runtime::Pac::PACPellet>> save_pellets = {}; 

namespace Runtime {
    void Gameplay::setup() {
        fruit_spawner_timer = std::chrono::seconds(30);
        
        Runtime::Sound::SoundEffect<ROM::gSFXBeginData>::StartSound();
        Runtime::Pac::Tilemap *tilemap;
        if (level % 2 == 0) {
            tilemap = addEntity<Runtime::Pac::Tilemap>(
                ARCADE_LOADTEXTROM(IMGmaze1),
                ARCADE_LOADSURFROM(IMGmaze1Collision)
            );
        } else {
            tilemap = addEntity<Runtime::Pac::Tilemap>(
                ARCADE_LOADTEXTROM(IMGmaze),
                ARCADE_LOADSURFROM(IMGmazeCollision)
            );
        }

        // auto tilemap = static_cast<Runtime::Pac::Tilemap*>(getEntitysFromID("Tilemap")[0]);
        tilemap->position = {0, 18};
        fruit_pos = tilemap->position + Math::pointi{112, 188};

        auto pacman =   addEntity<Runtime::Pac::PacMan>();
        pacman->getComponent<Runtime::Pac::PacComponent>()->m_position = tilemap->position + Math::pointi{112, 188};

        tilemap->pellets = save_pellets.value_or(tilemap->pellets);
        save_pellets = {};

        ghosts = {
                        addEntity<Runtime::Pac::Ghost::Blinky>(),
                        addEntity<Runtime::Pac::Ghost::Pinky>(),
                        addEntity<Runtime::Pac::Ghost::Inky>("Blinky"),
                        addEntity<Runtime::Pac::Ghost::Clyde>(),
        };

        ghosts[0]->getComponent<Runtime::Pac::PacComponent>()->m_position = tilemap->position + Math::pointi{112, 92};
        ghosts[0]->getComponent<Runtime::Pac::Ghost::GhostComponent>()->state = Runtime::Pac::Ghost::STATE_SCATTER;
        inactive_ghosts = ghosts;
        inactive_ghosts.erase(inactive_ghosts.begin());
        ghost_released = 0;


        inactive_ghosts[1]->getComponent<Runtime::Pac::PacComponent>()->m_position = tilemap->ghost_hut_home + Math::pointi{-16, 16};
        inactive_ghosts[0]->getComponent<Runtime::Pac::PacComponent>()->m_position = tilemap->ghost_hut_home + Math::pointi{0,  16};
        inactive_ghosts[2]->getComponent<Runtime::Pac::PacComponent>()->m_position = tilemap->ghost_hut_home + Math::pointi{16, 16};
        // inactive_ghosts[2]->getComponent<Runtime::Pac::PacComponent>()->m_position = {96, 120};
        
        flags[0] = 1;
        clock_delay = std::chrono::duration_cast<decltype(clock_delay)>(
            std::chrono::seconds(5)
        );

        game_over = false;
    }


    void Gameplay::update() {
        if (Controls::button_inputs[Controls::BUTTON_A] && flags[0]) {
            clock_delay = decltype(clock_delay)::zero();
        }

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

        auto amount_left = std::count(tilemap->pellets.begin(), tilemap->pellets.end(), Runtime::Pac::PACPellet::regular);
        auto amount = std::count(tilemap->collision.begin(), tilemap->collision.end(), false) - amount_left;
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
            level += 1;
            flags = 0;
            SceneManager::popScene();

            // Second level and every multiple of 4 levels has an intermissions.
            if (level == 2 || level % 4 == 0) 
                SceneManager::pushScene<Intermission>();
            else {
                SceneManager::pushScene<Gameplay>();
            }
            return;
        }

        if (flags[5]) {
            flags.set(4);
            Runtime::Sound::SoundEffect<ROM::gSFXYooHOOData>::StartSound(1);
            clock_delay = std::chrono::seconds(10);
            return;
        }


        if (amount_left <= 0) {
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
            Runtime::live_count = 0;
            SceneManager::gotoScene<Runtime::HighscoreScene>();
            static_cast<Runtime::HighscoreScene*>(Runtime::SceneManager::getCurrentScene())->newHighScore(
                    Runtime::current_score);
        }

       
        // printf("amount %d,", (int)amount);

        
        
        if ((amount - 60 * ghost_released) > 60 && ghost_released < 3) {
            
            if ((Runtime::current_tick % 3) != 0) goto release_yield_frame;
            const Math::pointi target_active_position = tilemap->position + Math::pointi{112, 92};
            auto pac = inactive_ghosts[0]->getComponent<Runtime::Pac::PacComponent>();
            auto ghost = inactive_ghosts[0]->getComponent<Runtime::Pac::Ghost::GhostComponent>();
            if (ghost->state == Runtime::Pac::Ghost::STATE_INACTIVE && !tilemap->ghost_hut_busy)
                ghost->state = Runtime::Pac::Ghost::STATE_BASE_EXITING;


            if (ghost->state != Runtime::Pac::Ghost::STATE_BASE_EXITING && ghost->state != Runtime::Pac::Ghost::STATE_INACTIVE) {
                ghost_released += 1;
                inactive_ghosts[0]->getComponent<Runtime::Pac::Ghost::GhostComponent>()->state = Runtime::Pac::Ghost::STATE_SCATTER;
                inactive_ghosts.erase(inactive_ghosts.begin());
                pac->m_direction = (std::rand() % 2)? Runtime::Pac::PACDirection::LEFT : Runtime::Pac::PACDirection::RIGHT;
            }
        }

        release_yield_frame:

        
        if (has_fruit)
        {
            auto pac = getEntitysFromID("PacMan")[0]->getComponent<Pac::PacComponent>();
            auto dist_to_fruit_squared = std::pow(fruit_pos.x - pac->m_position.x, 2) + std::pow(fruit_pos.y - pac->m_position.y, 2);
            if (dist_to_fruit_squared < 16) {
                
                Runtime::Sound::SoundEffect<ROM::gSFXeatGhostData>::StartSound();
                addEntity<PointsEffect>(pac->m_position, 1000);
                Runtime::current_score += 1000;  
                addEntityDelay(Runtime::tick_length*20);
                has_fruit = false;
                eaten_fruit = true;
            }
        }
        
        if (!eaten_fruit)
        {
            fruit_spawner_timer -= tick_length;
            if (fruit_spawner_timer.count() < 0)
            {
                has_fruit = !has_fruit;
                fruit_spawner_timer += has_fruit? std::chrono::seconds(15) : std::chrono::seconds(30);
            }
        }
        
        
            

        EntityManager::update_fixed();
        fixedupdateCounter();
    }

    void Gameplay::draw() {
        auto tilemap = (Runtime::Pac::Tilemap *)getEntitysFromID("Tilemap")[0];

        bool fruit_blinking = ((Runtime::current_tick / 6) % 2 == 0) && fruit_spawner_timer < std::chrono::seconds(5);
        
        if (has_fruit && !fruit_blinking)
        {
            auto tex = tilemap->collectables_texture;
            const auto pos = fruit_pos;
            const auto dst = SDL_Rect{pos.x - 8, pos.y - 8, 16, 16};
            const auto src = SDL_Rect{0, 8, 16, 16};
            SDL_RenderCopy(Graphics::renderer, tex.get(), &src, &dst);
        }

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