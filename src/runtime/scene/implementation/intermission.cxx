#include "intermission.hxx"
#include <rom/rom.hxx>
#include "gameplay.hxx"
#include <sfx/sfx.hxx>
namespace Runtime {
    void IntermissionBase::setup()
    {
        pacmen = {};
        pacmen_cutscene_texture =  ARCADE_LOADTEXTROM(IMGpacmanCutscene);
        ghost_textures[0] = ARCADE_LOADTEXTROM(IMGghostBlinky);
        ghost_textures[1] = ARCADE_LOADTEXTROM(IMGghostPinky);
        ghost_textures[2] = ARCADE_LOADTEXTROM(IMGghostInky);
        ghost_textures[3] = ARCADE_LOADTEXTROM(IMGghostClyde);
    }

    void IntermissionBase::update()
    {
        if (!pacmen.empty())
        {
            for (auto it = pacmen.begin(); it != pacmen.end();) {
                update_pac(it.base());
                if (it->time_elapsed > it->life_time) {
                    pacmen.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }

    void IntermissionBase::update_pac(pac_t *pac)
    {
        pac->time_elapsed += Runtime::delta_time;
    }

    void IntermissionBase::draw()
    {
        for (auto &pac: pacmen) {
            SDL_Rect src = { 0, 0, 16, 16 }, dst = { 0 };
            
            unsigned char frame_count = 3;
            int frame = 0;
            unsigned int tick = (pac.time_elapsed/Runtime::tick_length);

            int ghost = -1;
            switch (pac.sprite) {
                
                case pac_t::legman:
                    frame_count = 4;
                    src = {0, 48, 16, 16};
                    goto pac_frame_logic;
                case pac_t::kidman:
                    src = {0, 64, 16, 16};
                    goto pac_frame_logic;
                case pac_t::woman:
                    src = {48, 0, 16, 16};
                    goto pac_frame_logic;
                case pac_t::pacman:
                    pac_frame_logic:
                        frame = (tick/3 + int(pac.random*100) % 4)% frame_count;
                        src.x += frame*src.w;
                    break;
                case pac_t::smugman: 
                        frame_count = 8;
                        src = {96, 0, 48, 32};
                        frame = (tick/3 + int(pac.random*100) % 4)% frame_count;
                        src.x += (frame % 4)*src.w;
                        src.y += int(frame > 4)*src.h;
                    break;
                case pac_t::largeman: 
                        src = {0, 16, 32, 32};
                        goto pac_frame_logic;
                case pac_t::blinky:
                    ghost = 0;
                case pac_t::pinky:
                    ghost = 1;
                case pac_t::inky:
                    ghost = 2;
                case pac_t::clyde:
                    ghost = 3;
                
                default:
                    break;
            }

            int dist = int((pac.time_elapsed.count()/(float)pac.life_time.count())*ARCADE_LOGIC_WIDTH); 
            if (flipped) dist = ARCADE_LOGIC_WIDTH - dist - src.w;
            dst = {dist, ARCADE_LOGIC_HEIGHT/2 + 25 -src.h, src.w, src.h};

            if (ghost >= 0)
            {

                int ghostframe = Runtime::current_tick/6;
                src = {Pac::Ghost::ghost_width*(ghostframe % 2), 0, Pac::Ghost::ghost_width, Pac::Ghost::ghost_height};
                int scared_frame = Runtime::current_tick/12;
                if (flipped) src.x += (1 + (scared_frame % 2))*Pac::Ghost::ghost_width*2;
                SDL_RenderCopy(Graphics::renderer, ghost_textures[ghost].get(), &src, &dst); 
                SDL_RendererFlip eyefliped = flipped? SDL_RendererFlip::SDL_FLIP_HORIZONTAL : SDL_RendererFlip::SDL_FLIP_NONE;

                if (flipped) 
                {  
                    src = {(9 + (scared_frame % 2))*Pac::Ghost::ghost_width, 0, Pac::Ghost::ghost_width, Pac::Ghost::ghost_height};
                    SDL_RenderCopy(Graphics::renderer, ghost_textures[ghost].get(), &src, &dst);    
                } 
                else
                {
                    src = {Pac::Ghost::ghost_width*6, 0, Pac::Ghost::ghost_width, Pac::Ghost::ghost_height};
                    SDL_RenderCopyEx(Graphics::renderer, ghost_textures[ghost].get(), &src, &dst, 0, NULL, eyefliped);
                }
            }
            else 
            {
                if (flipped)
                {
                    SDL_Point p = {0, 0};
                    SDL_RenderCopyEx(Graphics::renderer, pacmen_cutscene_texture.get(), &src, &dst, 0, &p, SDL_RendererFlip::SDL_FLIP_HORIZONTAL);
                }
                else
                {
                    SDL_RenderCopy(Graphics::renderer, pacmen_cutscene_texture.get(), &src, &dst);
                }
            }

            
        }
    }

    
    void Intermission::setup() {
        IntermissionBase::setup();
        flags = 0;
        pacmen = {};
        Runtime::Sound::SoundEffect<ROM::gSFXIntermissionData>::StartSound();
    }

    void Intermission::update() {
        IntermissionBase::update();
        if(!flags[1]) {
            flags.set(1);
            pac_t pac = { 0 };
            std::srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            pac.sprite = pac_t::sprite_t(std::rand() % 3);
            pac.time_elapsed = -std::chrono::duration_cast<decltype(pac.time_elapsed)>(std::chrono::seconds(1));
            pac.life_time = Runtime::tick_length*ARCADE_LOGIC_WIDTH;
            

            pac_t pac2 = { 0 };
            std::srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            pac2.sprite = pac_t::sprite_t((std::rand() % 3) + pac_t::blinky);
            pac2.time_elapsed = pac.time_elapsed-std::chrono::duration_cast<decltype(pac.time_elapsed)>(std::chrono::milliseconds(1000));
            pac2.life_time = Runtime::tick_length*ARCADE_LOGIC_WIDTH;
            

            flipped = level % 2 == 0;
            if (flipped)
            {
                Runtime::duration temp;
                std::swap(pac.time_elapsed, pac2.time_elapsed);
                pac.sprite = pac_t::sprite_t((std::rand() % 2)+ pac_t::sprite_t::legman);
                pac.life_time -= std::chrono::milliseconds(500);
            }
            else 
            {
                pac2.life_time -= std::chrono::milliseconds(500);
            }

            pacmen.push_back(pac);
            pacmen.push_back(pac2);
        }

        if (pacmen.empty()) {
            if (!flags[0])  {
                flags.set(0);
                m_do_stuff_timer = std::chrono::duration_cast<decltype(m_do_stuff_timer)>(std::chrono::seconds(3));
            }
            m_do_stuff_timer -= Runtime::delta_time;
            if (m_do_stuff_timer.count() <= 0) {
                flags.reset();
                Runtime::SceneManager::gotoScene<Gameplay>();
            }
        }
    }

    void Intermission::draw() {
        std::string suffix = "";
        switch (level % 10) {
            case 1:     suffix = "st"; break;
            case 2:     suffix = "nd"; break;
            case 3:     suffix = "rd"; break;
            default:    suffix = "th"; break;
        }

        if (level >= 11 && level <= 13) suffix = "th"; // teens man

        Graphics::drawText(Math::recti(ARCADE_LOGIC_WIDTH/2, ARCADE_LOGIC_HEIGHT/2 - 48, 0, 0), 
            Math::to_string_with_precision(level, 0) + suffix + " Level", 
            Graphics::renderer, Math::color8a(~0, ~0, ~0, ~0), true);
        IntermissionBase::draw();
        Runtime::display_coins = false;
        Runtime::drawCounter();
    }
}