#ifndef _ARCADE_CONTROLS
#define _ARCADE_CONTROLS


#include <system/math.hxx>
#include <atomic>
#include <cstdint>
#include <array>

namespace Controls {
    enum BTN_INPUT : unsigned char {
        BUTTON_A = 0,
        BUTTON_B,
        
        BUTTON_START,
        BUTTON_SELECT,

        // 

        BUTTON_END
    };
    enum AXS_INPUT : unsigned char {
        AXIS_X = 0,
        AXIS_Y,


        // 
        AXIS_END
    };
    
    inline std::atomic<bool>    button_inputs[BUTTON_END] = {};
    inline std::atomic<int16_t>   axis_inputs[AXIS_END] = {};

    void beginControlThread(void);
    void endControlThread(void);
} // namespace Runtime





#endif