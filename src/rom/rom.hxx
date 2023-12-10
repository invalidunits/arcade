#ifndef _ARCADE_ROM
#define _ARCADE_ROM

#include <extern/incbin.h>

namespace ROM {
    extern "C" {
        INCBIN_EXTERN(EmuLogic);
        INCBIN_EXTERN(IMGmainMenu);
	    INCBIN_EXTERN(IMGmainMenuCoin);
	    INCBIN_EXTERN(IMGmainMenuStart);
        INCBIN_EXTERN(IMGmainMenuGhost);

        INCBIN_EXTERN(IMGmaze);
        INCBIN_EXTERN(IMGmazeCollision);
    }
}





#endif //#ifndef _ARCADE_ROM