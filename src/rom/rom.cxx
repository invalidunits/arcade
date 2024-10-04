#include <extern/incbin.h>



extern "C" {
	INCBIN(EmuLogic, "rom/emulogic.ttf");
	INCBIN(ArcadePixPlus, "rom/arcadepix_plus.ttf");

	// Main Menu
	INCBIN(IMGmainMenu, "rom/img/main_menu.png");
	INCBIN(IMGmainMenuCoin, "rom/img/main_menuic.png");
	INCBIN(IMGmainMenuStart, "rom/img/main_menupsb.png");
	INCBIN(IMGmainMenuGhost, "rom/img/main_menug.png");
	INCBIN(IMGmainMenuCounter, "rom/img/main_menu_counters.png");

	// Pacman
	INCBIN(IMGpacmanCutscene, "rom/img/pacman/pacman_cutscene.png");
	INCBIN(IMGpacman, "rom/img/pacman/pacman.png");

	// Gameplay

	INCBIN(IMGmaze, "rom/img/maze.png");
	INCBIN(IMGmazeCollision, "rom/img/maze_collision.png");

	INCBIN(IMGmaze1, "rom/img/maze1.png");
	INCBIN(IMGmaze1Collision, "rom/img/maze1_collision.png");

	INCBIN(IMGmazeCollectables, "rom/img/maze_collectables.png");
	
	INCBIN(IMGghostBlinky, "rom/img/ghost/blinky.png");
	INCBIN(IMGghostInky, "rom/img/ghost/inky.png");
	INCBIN(IMGghostPinky, "rom/img/ghost/pinky.png");
	INCBIN(IMGghostClyde, "rom/img/ghost/clyde.png");	

	// Sound Effects 

	INCBIN(SFXBegin, "rom/sfx/pacman_beginning.wav");
	INCBIN(SFXChomp, "rom/sfx/pacman_chomp.wav");
	INCBIN(SFXDeath, "rom/sfx/pacman_death.wav");
	INCBIN(SFXeatFruit, "rom/sfx/pacman_eatfruit.wav");
	INCBIN(SFXeatGhost, "rom/sfx/pacman_eatghost.wav");
	INCBIN(SFXextraPac, "rom/sfx/pacman_extrapac.wav");
	INCBIN(SFXIntermission, "rom/sfx/pacman_intermission.wav");
	INCBIN(SFXYooHOO, "rom/sfx/pacman_yoohoo.wav");
	INCBIN(SFXSirin1, "rom/sfx/siren_1.wav");
	INCBIN(SFXSirin2, "rom/sfx/siren_2.wav");
	INCBIN(SFXSirin3, "rom/sfx/siren_3.wav");
	INCBIN(SFXSirin4, "rom/sfx/siren_4.wav");
	INCBIN(SFXSirin5, "rom/sfx/siren_5.wav");
}