#ifndef _ARCADE_HIGHSCORE
#define _ARCADE_HIGHSCORE


#include <runtime/scene/scene.hxx>
#include <runtime/counters/counters.hxx>
#include <cstddef>
#include <algorithm>
#include <system/controls.hxx>
#include <list>
namespace Runtime
{
    constexpr std::size_t high_score_name_size = 5; // 5 characters.
    constexpr std::string_view name_characters = "-ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    constexpr std::size_t display_amount = 5; // 

    typedef uint32_t highscore_t;
    constexpr std::size_t highscore_row_size = high_score_name_size + sizeof(highscore_t);
    inline std::list<std::pair<std::string, uint32_t>> high_scores = {};

    class HighscoreScene : public Scene
    {
        public:
            HighscoreScene() {}

            void setup();
            void newHighScore(uint32_t score) {
                std::string name = "";
                for(int i = 0; i < high_score_name_size; i++) name += name_characters[0];

                if (high_scores.empty()) {
                    high_scores.push_back({name, score});
                    change_it = high_scores.begin();
                    change_char = 0;
                    return;
                }
                decltype(high_scores)::iterator highscore_position = high_scores.begin();
                highscore_position = std::find_if(
                    high_scores.begin(), high_scores.end(), [score](
                        std::pair<std::string, uint32_t> a) {
                        return a.second < score;
                    }
                );

                highscore_position = high_scores.emplace(highscore_position, name, score);
                int position = std::distance(high_scores.begin(), highscore_position) + 1;


                banner = "Too Bad.\nYou have no place\non the leaderboard.";
                if (position  <= display_amount) {
                    std::string suffix = "th";
                    switch (position % 10) {
                        case 1: suffix = "st"; break;
                        case 2: suffix = "nd"; break;
                        case 3: suffix = "rd"; break;
                    }
                    banner = "Congratulations!\nYou took " + std::to_string(position) + suffix + " place!\nUse the joystick to select\nyour name.";
                    change_it = highscore_position;
                    change_char = 0;
                }
            }

            void update_fixed();
            void draw();
        
        
        
        char change_char = 0;
        std::string banner;
        decltype(high_scores)::iterator change_it = high_scores.end();
    }; // namespace Highscore
    

} // namespace Runtime












#endif
