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

        
        INCBIN_EXTERN(SFXBegin);
        INCBIN_EXTERN(SFXChomp);
        INCBIN_EXTERN(SFXDeath);
        INCBIN_EXTERN(SFXeatFruit);
        INCBIN_EXTERN(SFXeatGhost);
        INCBIN_EXTERN(SFXextraPac);
        INCBIN_EXTERN(SFXIntermission);
        INCBIN_EXTERN(SFXYooHOO);
        INCBIN_EXTERN(SFXSirin1);
        INCBIN_EXTERN(SFXSirin2);
        INCBIN_EXTERN(SFXSirin3);
        INCBIN_EXTERN(SFXSirin4);
        INCBIN_EXTERN(SFXSirin5);
    }
}





#endif //#ifndef _ARCADE_ROM