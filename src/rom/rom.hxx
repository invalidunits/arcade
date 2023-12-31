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
        INCBIN_EXTERN(IMGmainMenuCounter);

        INCBIN_EXTERN(IMGpacmanCutscene);
        INCBIN_EXTERN(IMGpacman);

        INCBIN_EXTERN(IMGmaze);
        INCBIN_EXTERN(IMGmazeCollectables);
        INCBIN_EXTERN(IMGmazeCollision);
        
        INCBIN_EXTERN(IMGghostInky);
	    INCBIN_EXTERN(IMGghostBlinky);
	    INCBIN_EXTERN(IMGghostPinky);
	    INCBIN_EXTERN(IMGghostClyde);  
    }
}





#endif //#ifndef _ARCADE_ROM