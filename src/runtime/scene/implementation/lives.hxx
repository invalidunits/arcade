#ifndef _ARCADE_LIVES_CUTSCENE
#define _ARCADE_LIVES_CUTSCENE

#include <cmath>
#include <rom/rom.hxx>
#include <runtime/scene/scene.hxx>
#include <graphics/graphics.hxx>
#include <runtime/entity/entity.hxx>
#include "gameplay.hxx"

#include <runtime/counters/counters.hxx>

namespace Runtime {

    

    
    class PointsEffect;

    class LiveCutscene : public Entity::EntityManager {
        constexpr static auto default_life_time = std::chrono::duration_cast<Runtime::duration>(Runtime::tick_length*ARCADE_LOGIC_WIDTH);
        struct pac_t {
            bool evaluated = false;
            Runtime::duration time_elapsed = Runtime::duration::zero();
            Runtime::duration life_time = Runtime::duration::zero();
            enum sprite_t : unsigned char {
                pacman,
                woman,
                legman,
                kidman,
                largeman,

                smugman, // Smugman is "special"

                endman
            } sprite;
            float random = 0;
        };

        unsigned int _coin_display = 0;
        Graphics::shared_texture pacmen_cutscene = nullptr;
        std::vector<pac_t> pacmen;
        
        
        public:
            

            LiveCutscene();
            void setup();
            void update();
            void update_fixed();
            void draw();
            void cleanup();

            int m_state = 0;
            int coin_update_frame = 0;
            Runtime::duration m_do_stuff_timer = Runtime::duration::zero();

            
        
    };

    class PointsEffect : public Runtime::Entity::Entity {
        Graphics::shared_texture collectables_texture = nullptr;
        Runtime::duration m_elapsed_time = Runtime::duration::zero();
        constexpr static auto life_time = Runtime::tick_length*11;


        Math::pointi m_position = {0, 0};

        enum PointValue : unsigned short {
            PointValue_100 = 100,
            PointValue_200 = 200,
            PointValue_500 = 500,
            PointValue_700 = 700,
            PointValue_1000 = 1000,
            PointValue_2000 = 2000,
            PointValue_5000 = 5000,
        } m_point_value;

        public:
            PointsEffect(Math::pointi position, unsigned short point_vlaue) : 
                m_position(position),
                m_point_value(PointValue(point_vlaue)) {
                collectables_texture =      ARCADE_LOADTEXTROM(IMGmazeCollectables);
            }

            void update() {
                m_elapsed_time += Runtime::delta_time;
                if (m_elapsed_time > life_time) m_manager->removeEntity(this);
            }

            void draw() {
                
                SDL_Rect src = {0};
                SDL_Rect dst = {m_position.x, 
                                m_position.y, 16, 16};
                switch (m_point_value) {
                    case PointValue_100: src = {0,  40, 16, 16}; break;
                    case PointValue_200: src = {16, 40, 16, 16}; break;
                    case PointValue_500: src = {32, 40, 16, 16}; break;
                    case PointValue_700: src = {48, 40, 16, 16}; break;

                    case PointValue_1000: src = {64, 8,  16, 16}; break;
                    case PointValue_2000: src = {64, 24, 16, 16}; break;
                    case PointValue_5000: src = {64, 40, 16, 16}; break;
                    
                    default:
                        printf("Invalid point value");
                        return;
                }

                auto frame = (m_elapsed_time/(Runtime::tick_length*3));
                if (frame > 3)
                    frame = 3;
                dst.y -= frame;


                SDL_RenderCopy(Graphics::renderer, collectables_texture.get(), &src, &dst);
            }
    };
    
    
} // namespace Runtime



#endif