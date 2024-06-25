#include "lives.hxx"

#include <sfx/sfx.hxx>

namespace Runtime {
    LiveCutscene::LiveCutscene() {
        pacmen_cutscene =  ARCADE_LOADTEXTROM(IMGpacmanCutscene);
        
    }

    void LiveCutscene::setup() {

        Runtime::display_coins = false;
        Runtime::Entity::EntityManager::setup();
        Runtime::current_score = 0;
        pacmen = {};
        live_count = coin_display/life_value;
        int next_special_char = 4;
        m_state = 0;

        
        Runtime::Sound::SoundEffect<ROM::gSFXIntermissionData>::StartSoundMS(-1, live_count*500 + 5000);
        for (int i = 0; i < live_count; i++) {
            std::srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            pacmen.push_back(pac_t());
            pac_t &pac = pacmen.back();
            pac.sprite = pac_t::pacman;
            pac.random = ((float)std::rand()) / RAND_MAX;

            pac.time_elapsed = -i*std::chrono::milliseconds(500);
            if (i > 4) {
                pac.time_elapsed -= std::chrono::duration_cast<Runtime::duration>(std::chrono::milliseconds(
                    250 + int(250*((float)std::rand()) / RAND_MAX)
                ));
            }

            pac.life_time = default_life_time;

            if (i > 4) {
                pac.time_elapsed -= std::chrono::duration_cast<Runtime::duration>(std::chrono::milliseconds(int(250*pac.random)));
                pac.life_time -= std::chrono::duration_cast<Runtime::duration>(std::chrono::milliseconds(int(2500*((float)std::rand()) / RAND_MAX)));
            }

            
            if (next_special_char <= 0) {
                next_special_char = std::rand() % 4;
                pac.sprite = pac_t::sprite_t(1 + (std::rand() % (char)pac_t::endman - 1));
            }
            next_special_char -= 1;

        }
        m_do_stuff_timer = std::chrono::duration_cast<Runtime::duration>(std::chrono::seconds(1));
        
    }

    void LiveCutscene::update() {
        
        Runtime::Entity::EntityManager::update();
        for (auto it = pacmen.begin(); it != pacmen.end();) {
            auto &pac = *it;
            pac.time_elapsed += Runtime::delta_time;
            if (!pac.evaluated) {
                if ((pac.time_elapsed.count()/(float)pac.life_time.count()) > 0.5) {
                    coin_display -= life_value;
                    pac.evaluated = true;
                    
                    std::printf("%d", int(pac.sprite));
                }
            }

            if (pac.time_elapsed > pac.life_time) {
                pacmen.erase(it);
            } else {
                ++it;
            }
        }


        if (pacmen.size() == 0) {
            m_do_stuff_timer -= Runtime::delta_time;
            if (m_do_stuff_timer.count() < 0) {
                
                if (coin_display > 0) {
                    m_do_stuff_timer = std::chrono::duration_cast<Runtime::duration>(std::chrono::milliseconds(500));
                    coin_display -= 5;

                    constexpr auto coin_value = 100U;
                    Runtime::current_score += coin_value;
                    addEntity<PointsEffect>(Math::pointi((ARCADE_LOGIC_WIDTH/2) - 8, 90), 100);

                    if (coin_display <= 0) {
                        coin_display = 0;
                        m_do_stuff_timer = std::chrono::duration_cast<Runtime::duration>(std::chrono::seconds(2));
                    }
                } else {
                    m_do_stuff_timer = std::chrono::duration_cast<Runtime::duration>(std::chrono::seconds(2));
                    m_state += 1;
                    if (m_state >= 3) {
                        Runtime::SceneManager::gotoScene<Runtime::Gameplay>();
                    }
                }
                
                //TODO: A point particle effect
            }


        }
    }

    void LiveCutscene::update_fixed() {
        Runtime::fixedupdateCounter();
        Runtime::Entity::EntityManager::update_fixed();
        if (coin_display != _coin_display)
            coin_update_frame = 3;
            _coin_display = coin_display;
            
        if ((Runtime::current_tick % 6) == 0) 
            coin_update_frame -= 1;
        if (coin_update_frame < 0)
            coin_update_frame = 0;
    }

    void LiveCutscene::draw() {
        Runtime::Entity::EntityManager::draw();
        int cointrans = 0;
        switch (coin_update_frame) {
            case 3: cointrans = 1; break;
            case 2: cointrans = 3; break;
            case 1: cointrans = 1; break;
        }

        auto coindraw = Math::recti(ARCADE_LOGIC_WIDTH/2 - 25, ARCADE_LOGIC_HEIGHT/2 - 30, 0, 0);
        auto color =  (coin_update_frame < 0? 
                            Math::color8a(~0, ~0, ~0, ~0) : 
                            Math::color8a(~0, ~0, 0, ~0));
        coindraw.y -= cointrans;   

        Graphics::drawText(coindraw, 
            "$" + Math::to_string_with_precision(float(coin_display*coin_display_multiplier), 2), 
            Graphics::renderer, color);
        
        if (m_state >= 1) {
            Graphics::drawText(coindraw + Math::recti(0, 32, 0, 0), 
                Math::to_string_with_precision(live_count, 0) + " Lives Total", 
                Graphics::renderer, color);
        }
        for (auto &pac: pacmen) {
            SDL_Rect src = { 0, 0, 16, 16 }, dst = { 0 };
            
            unsigned char frame_count = 3;
            int frame = 0;
            unsigned int tick = (pac.time_elapsed/Runtime::tick_length);
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
            }

            int dist = int((pac.time_elapsed.count()/(float)pac.life_time.count())*ARCADE_LOGIC_WIDTH); 
            dst = {dist, ARCADE_LOGIC_HEIGHT/2 + 25 -src.h, src.w, src.h};
            SDL_RenderCopy(Graphics::renderer, pacmen_cutscene.get(), &src, &dst);



        }
        

        Runtime::drawCounter();
    }

    void LiveCutscene::cleanup() {
        Runtime::display_coins = false;
    }

    constexpr inline int lerp(int a, int b, int t) {
        return a + t * (b - a);
    }
}