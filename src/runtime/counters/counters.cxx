#include "counters.hxx"

#include <rom/rom.hxx>
#include <system/com.hxx>
#include <system/clock.hxx>

Graphics::shared_texture counter_texture = nullptr;
Graphics::shared_texture pacman_cutscene_texture = nullptr;

decltype(Runtime::coin_display) _coin_display = 0;
char coin_update_frame = 0;

namespace Runtime {
    void setupCounter() {
        counter_texture = ARCADE_LOADTEXTROM(IMGmainMenuCounter);
        pacman_cutscene_texture = ARCADE_LOADTEXTROM(IMGpacmanCutscene);
    }

    void fixedupdateCounter() {
        if (current_score > high_score) {
            high_score = current_score;
        }


        coin_display += (float)COM::coin_inserted_value.exchange(0, std::memory_order::memory_order_relaxed);
        if (_coin_display != coin_display) {
            _coin_display = coin_display;
            coin_update_frame = 3;
        }
        
        if (coin_update_frame > 0 && (Runtime::current_tick % 6) == 0)
            --coin_update_frame;
    }

    void drawCounter() {
        if (display_coins) {
            SDL_RenderCopy(Graphics::renderer, counter_texture.get(), {}, {});

            char cointrans = 0;
            switch (coin_update_frame) {
                case 3: cointrans = 1; break;
                case 2: cointrans = 3; break;
                case 1: cointrans = 1; break;
            }

            auto coindraw = Math::recti(80, ARCADE_LOGIC_HEIGHT - cointrans - 12, 0, 0);
            auto color =  (coin_update_frame > 0? 
                                    Math::color8a(~0, ~0, ~0, ~0) : 
                                    Math::color8a(~0, ~0, 0, ~0));

            if (Runtime::coin_display > 1000) 
                Graphics::drawText(coindraw, "STOP IT!",
                        Graphics::renderer, color);
            else if (Runtime::coin_display > 8000) 
                Graphics::drawText(coindraw, "Too much...",
                        Graphics::renderer, color);
            else Graphics::drawText(coindraw, std::string("$") + std::string(Math::to_string_with_precision(float(coin_display*coin_display_multiplier), 2)),
                        Graphics::renderer, color);
        } else {
            SDL_Rect rect = {0, 0, 224, 64};
            SDL_RenderCopy(Graphics::renderer, counter_texture.get(), &rect, &rect);
        }
        
        auto livedst = Math::recti(24, 8, 0, 0);
        Graphics::drawText(livedst, "x" + Math::to_string_with_precision(live_count, 0),
                Graphics::renderer); 
        
        Math::color8a highcolor = Math::color8a(~0, ~0, ~0, ~0);
        if (current_score == high_score && high_score != 0) {
            switch ((Runtime::current_tick/6) % 5)
            {
            case 0: highcolor = Math::color8a(~0, 0, 0, ~0); break;
            case 1: highcolor = Math::color8a(~0, ~0, 0, ~0); break;
            case 2: highcolor = Math::color8a(0,   ~0, 0, ~0); break;
            case 3: highcolor = Math::color8a(0, ~0, ~0, ~0); break;
            case 4: highcolor = Math::color8a(0, 0,  ~0, ~0); break;
            case 5: highcolor = Math::color8a(~0, 0,  ~0, ~0); break;

            
            default:
                break;
            }
        }

        auto pointdst = Math::recti(68, 8, 0, 0);
        Graphics::drawText(pointdst, Math::to_string_with_zero_pad(current_score, 5),
                Graphics::renderer, highcolor);

        auto highdst = Math::recti(158, 8, 0, 0);
        Graphics::drawText(highdst, Math::to_string_with_zero_pad(current_score, 5),
                Graphics::renderer);
            
            

    }
}