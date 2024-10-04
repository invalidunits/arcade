#include "intermission.hxx"
#include <sfx/sfx.hxx>
namespace Runtime {
    void Intermission::setup() {
        flags = 0;
        pacmen = {};
    }
    void Intermission::update() {
        if(!flags[1]) {
            flags.set(1);
            LiveCutscene::pac_t pac = { 0 };

            std::srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            pac.sprite = LiveCutscene::pac_t::sprite_t(std::rand() % 3);

            pac.time_elapsed = -std::chrono::duration_cast<decltype(pac.time_elapsed)>(std::chrono::seconds(1));
            pac.life_time = Runtime::tick_length*ARCADE_LOGIC_WIDTH;
            pacmen.push_back(pac);

            Runtime::Sound::SoundEffect<ROM::gSFXIntermissionData>::StartSound();

        }
        for (auto it = pacmen.begin(); it != pacmen.end();) {
            auto &pac = *it;
            pac.time_elapsed += Runtime::delta_time;
            if (pac.time_elapsed > pac.life_time) {
                pacmen.erase(it);
            } else {
                ++it;
            }
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
        for (auto &pac: pacmen) {
            SDL_Rect src = { 0, 0, 16, 16 }, dst = { 0 };
            
            unsigned char frame_count = 3;
            int frame = 0;
            unsigned int tick = (pac.time_elapsed/Runtime::tick_length);
            switch (pac.sprite) {
                case LiveCutscene::pac_t::legman:
                    frame_count = 4;
                    src = {0, 48, 16, 16};
                    goto pac_frame_logic;
                case LiveCutscene::pac_t::kidman:
                    src = {0, 64, 16, 16};
                    goto pac_frame_logic;
                case LiveCutscene::pac_t::woman:
                    src = {48, 0, 16, 16};
                    goto pac_frame_logic;
                
                case LiveCutscene::pac_t::blinky:
                    frame_count = 2;
                    src = {112, 64, 16, 16};
                    goto pac_frame_logic;
                
                case LiveCutscene::pac_t::pinky:
                    frame_count = 2;
                    src = {144, 64, 16, 16};
                    goto pac_frame_logic;

                case LiveCutscene::pac_t::inky:
                    frame_count = 2;
                    src = {176, 64, 16, 16};
                    goto pac_frame_logic;
                
                case LiveCutscene::pac_t::clyde:
                    frame_count = 2;
                    src = {208, 64, 16, 16};
                    goto pac_frame_logic;

                case LiveCutscene::pac_t::pacman:
                    pac_frame_logic:
                        frame = (tick/3 + int(pac.random*100) % 4)% frame_count;
                        src.x += frame*src.w;
                    break;
                
                case LiveCutscene::pac_t::smugman: 
                        frame_count = 8;
                        src = {96, 0, 48, 32};
                        frame = (tick/3 + int(pac.random*100) % 4)% frame_count;
                        src.x += (frame % 4)*src.w;
                        src.y += int(frame > 4)*src.h;
                
                    
                    
                    break;

                    
                case LiveCutscene::pac_t::largeman: 
                        src = {0, 16, 32, 32};
                        goto pac_frame_logic;
            }

            int dist = int((pac.time_elapsed.count()/(float)pac.life_time.count())*ARCADE_LOGIC_WIDTH); 
            dst = {dist, ARCADE_LOGIC_HEIGHT/2 + 25 -src.h, src.w, src.h};
            SDL_RenderCopy(Graphics::renderer, pacmen_cutscene.get(), &src, &dst);
        }

        std::string suffix = "";
        switch (level % 10) {
            case 1:     suffix = "st"; break;
            case 2:     suffix = "nd"; break;
            case 3:     suffix = "rd"; break;
            default:    suffix = "th"; break;


        }

        Graphics::drawText(Math::recti(ARCADE_LOGIC_WIDTH/2, ARCADE_LOGIC_HEIGHT/2 - 48, 0, 0), 
            Math::to_string_with_precision(level, 0) + suffix + " Level", 
            Graphics::renderer);

        Runtime::display_coins = false;
        Runtime::drawCounter();
    }
}