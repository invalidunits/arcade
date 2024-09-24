#ifndef _ARCADE_COUNTER
#define _ARCADE_COUNTER

#include <graphics/graphics.hxx>
#include <runtime/entity/entity.hxx>

namespace Runtime {
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
                if (m_elapsed_time > life_time) 
                    m_manager->removeEntity(this);
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


    inline unsigned short high_score = 0;
    inline unsigned short current_score = 0;
    inline unsigned short level = 0;

    inline unsigned int live_count = 0;
    inline unsigned int coin_display = 0;

    constexpr unsigned int life_value = 25;
    constexpr float coin_display_multiplier     = 0.01;
    constexpr unsigned int extra_coin_divisor   = 5;
    constexpr unsigned int extra_coin_points    = 100;
    

    inline bool display_coins = true;
    
    void setupCounter();
    void fixedupdateCounter();
    void drawCounter();
}


#endif