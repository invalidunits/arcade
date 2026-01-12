#include "highscores.hxx"
#include <bit>
#include <filesystem>
#include <SDL2/SDL_endian.h>
#include <SDL2/SDL_rwops.h>
#include <graphics/graphics.hxx>
#include <sfx/sfx.hxx>
#include <rom/rom.hxx>

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

            bool red = red_ticks > 0 && it == change_it;

            Graphics::drawText({55, 100 + int(i)*20, 0, 0}, name, Graphics::renderer, red? Math::color8a(~0, 0, 0, ~0) : Math::color8a(~0, ~0, ~0, ~0) );
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

    bool usernameBanned(std::string_view str)
    {
        std::string lower = std::string(str);
        std::transform(lower.begin(), lower.end(), lower.begin(), tolower);

        printf("Checking username: ");
        printf(str.data());
        printf("\n");
        const char *awful_words = "ahole anus ass bitch c0ck c0cks c0k cawk cawks Clit cnts cntz cock cocks crap cum cunt cunts cuntz dick dild0 dildo dyke enema fag fag1t faget fagit fags fagz faig faigs fart fuck fucks fuk Fukah Fuken fuker Fukin Fukk g00k gay gays gayz h00r h0ar h0re hells hoar hoor hoore jap japs jisim jiss jizm jizz knob knobs knobz kunt kunts kuntz n1gr nastt packy paki pakie paky pen1s penas penis penus Phuc Phuck Phuk polac polak pr1c pr1ck pr1k pusse pussy puuke queer qweir scank semen sex sexy sh1t sh1ts sh1tz shit shits Shity shitz Shyt Shyte Shyty skank slut sluts slutz tit turd vulva w0p wh00r wh0re whore xxx bitch clit fuck shit ass b17ch b1tch c0ck cawk chink cipa clits cock cum cunt dildo dirsa fcuk fuk fux0r hoer hore jism kawk mofo nazi nig phuck pusse pussy slut smut teets tits boobs b00bs teez titt w00se wank whoar whore amcik ayir bi7ch cazzo chraa chuj d4mn daygo dego dupa Ekto faen fanny feces feg Fotze gay gook h0r h4x0r hell hui injun jizz kike kraut kuk Kurac kurwa lesbo mibun muie nazis perse picka pizda poop porn p0rn pr0n pula pule puta puto screw shiz spic suka twat vittu yed";
        while (*awful_words != '\0')
        {
            const char *end_of_word = awful_words;
            while (*end_of_word != '\0' && *end_of_word != ' ') 
                end_of_word++;
            

            // printf("Checking ");
            // fwrite(awful_words, end_of_word-awful_words, 1, stdout);
            // printf("\n");;
            if (lower.find(awful_words, 0, (end_of_word-awful_words)/sizeof(char)) != std::string::npos)
            {
                printf("Found ");
                fwrite(awful_words, end_of_word-awful_words, 1, stdout);
                printf(" in ");
                printf(str.data());
                printf("\n");
                return true;
            }

            end_of_word++;
            awful_words = end_of_word;
        }


        return false;
    }

    void HighscoreScene::update_fixed() {
        if (Runtime::current_tick % 12 != 0) return;

        
        if (change_it != high_scores.end()) {
            if (Controls::button_inputs[Controls::BUTTON_A]) {
                if (!enter_locked)
                {
                    if (usernameBanned(change_it->first))
                    {
                        red_ticks = 4;
                    }
                    else
                    {
                        Runtime::Sound::SoundEffect<ROM::gSFXextraPacData>::StartSound();
                        change_it = high_scores.end();
                        saveHighScores();
                        change_char = 0;
                        return;
                    }
                }
                enter_locked = true;
            }
            else 
            {
                enter_locked = false;
            }

            if (red_ticks > 0)
            {
                red_ticks--;
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
