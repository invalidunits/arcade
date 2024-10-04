#include "highscores.hxx"
#include <bit>
#include <filesystem>
#include <SDL2/SDL_endian.h>
#include <SDL2/SDL_rwops.h>
#include <graphics/graphics.hxx>

namespace Runtime
{
    void HighscoreScene::draw() {
        Runtime::display_coins = false;
        Runtime::drawCounter();

        Graphics::drawText({ARCADE_LOGIC_WIDTH/2, 50, 0, 0}, banner, Graphics::renderer, Math::color8a{
            255, 255, 255, 255}, true);

        int i = 0;
        for (auto it = high_scores.begin(); it != high_scores.end() && i < display_amount; it++, i++) {

            auto x_axis_raw = Controls::axis_inputs[Controls::AXIS_X].load();
            auto y_axis_raw = Controls::axis_inputs[Controls::AXIS_Y].load();
            auto x_axis = std::abs(x_axis_raw) > Controls::deadzone? (0 < x_axis_raw) - (x_axis_raw < 0) : 0;
            auto y_axis = std::abs(y_axis_raw) > Controls::deadzone? (0 < y_axis_raw) - (y_axis_raw < 0) : 0; 
            
            std::string name = it->first;
            if (it == change_it)
            if (y_axis == 0)
            if ((Runtime::current_tick /16)%2 == 0 || x_axis != 0) {
                name[change_char] = '_';
            }

            Graphics::drawText({55, 100 + int(i)*20, 0, 0}, name, Graphics::renderer);
            Graphics::drawText({150, 100 + int(i)*20, 0, 0}, std::to_string(it->second), Graphics::renderer);
        }
        if (change_it != high_scores.end()) {
            Graphics::drawText({0, ARCADE_LOGIC_HEIGHT-50, 0, 0}, "Press Z to confirm your name.", Graphics::renderer);
        } else {
            Graphics::drawText({0, ARCADE_LOGIC_HEIGHT-20, 0, 0}, "Press B to return to the Main Menu.", Graphics::renderer);
        }
    }


    void loadHighScores() {
        if (!high_scores.empty()) {
            return;
        }

        std::filesystem::path file_path = "highscore.dat";
        if (const char * env = getenv("ARCADE_HIGHSCORE_FILE")) {
            file_path = env;
        }

        
        auto rwops = SDL_RWFromFile(file_path.c_str(), "r"); // We only wish to read the highscore when checking them.
        if (rwops == NULL) {
            printf("Failed to recieve highscore from %s", file_path.c_str());
            return;
        }

        

        int i = 0;
        ptrdiff_t size = SDL_RWsize(rwops);
        high_scores = {};
        while (i++ < 50 || size <= 0) {
            size -= high_score_name_size + sizeof(uint32_t);
            if (size < 0) {
                break;
            }
            

            char name[high_score_name_size + 1] = { 0 };
            highscore_t score = 0;
            memset(name, 0, sizeof(name));  

            if (SDL_RWread(rwops, name, high_score_name_size, 1) != 1) break; // We finnished the file
            score = SDL_ReadBE32(rwops);
            high_scores.push_back({std::string(name), score});
        }

        if (!high_scores.empty()) {
              // Sort the results we did get.
            high_scores.sort([](
                std::pair<std::string_view, uint32_t> a, std::pair<std::string_view, uint32_t> b) {
                return a.second > b.second;
            });
            high_score = high_scores.begin()->second;
        }
        SDL_RWclose(rwops);
    }

    void saveHighScores() {
        if (high_scores.empty()) {
            return;
        }

        std::filesystem::path file_path = "highscore.dat";
        if (const char * env = getenv("ARCADE_HIGHSCORE_FILE")) {
            file_path = env;
        }

        
        auto rwops = SDL_RWFromFile(file_path.c_str(), "w"); // We only wish to read the highscore when checking them.
        if (rwops == NULL) {
            printf("Failed to save highscore to %s", file_path.c_str());
            return;
        }

        for (auto score : high_scores) {
            char name[high_score_name_size] = { 0 };
            memset(name, 0, sizeof(name));  
            memcpy(name, score.first.c_str(), SDL_min(high_score_name_size, score.first.length()));

            SDL_RWwrite(rwops, name, high_score_name_size, 1);
            SDL_WriteBE32(rwops, score.second);
        }
        SDL_RWclose(rwops);
    }

    void HighscoreScene::update_fixed() {
        if (Runtime::current_tick % 12 != 0) return;

        
        if (change_it != high_scores.end()) {
            if (Controls::button_inputs[Controls::BUTTON_A]) {
                change_it = high_scores.end();
                saveHighScores();
                change_char = 0;
                return;
            }

            auto x_axis_raw = Controls::axis_inputs[Controls::AXIS_X].load();
            auto y_axis_raw = Controls::axis_inputs[Controls::AXIS_Y].load();
            auto x_axis = std::abs(x_axis_raw) > Controls::deadzone? (0 < x_axis_raw) - (x_axis_raw < 0) : 0;
            auto y_axis = std::abs(y_axis_raw) > Controls::deadzone? (0 < y_axis_raw) - (y_axis_raw < 0) : 0;

            change_char += x_axis;
            while (change_char < 0) change_char += high_score_name_size;
            while (change_char >= high_score_name_size) change_char -= high_score_name_size;

            if (y_axis != 0) {
                auto it = std::find(name_characters.begin(), name_characters.end(), 
                    change_it->first[change_char]);

                if (it == name_characters.end()) {
                    it = name_characters.begin();
                }
                int character_index = int(it - name_characters.begin());
                character_index -= y_axis;
                while (character_index < 0) character_index += name_characters.size();
                while (character_index >= name_characters.size()) character_index -= name_characters.size();
                change_it->first[change_char] = name_characters[character_index];

            }
        } else {
            if (Controls::button_inputs[Controls::BUTTON_B]) {
                // Save then go to main menu
                SceneManager::popScene();
            }
        }
    }

    void HighscoreScene::setup() {
        banner = "These are the\nhighscores of other players.\nScore high to place here!";
        loadHighScores();
        change_it = high_scores.end();
        change_char = 0;
    }
}
