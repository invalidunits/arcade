#include <extern/incbin.h>





extern "C" {
	INCBIN(EmuLogic, "rom/emulogic.ttf");



	// Main Menu
	INCBIN(IMGmainMenu, "rom/img/main_menu.png");
	INCBIN(IMGmainMenuCoin, "rom/img/main_menuic.png");
	INCBIN(IMGmainMenuStart, "rom/img/main_menupsb.png");
	INCBIN(IMGmainMenuGhost, "rom/img/main_menug.png");

	// Pacman
	INCBIN(IMGpacmanCutscene, "rom/img/pacman/pacman_cutscene.png");
	INCBIN(IMGpacman, "rom/img/pacman/pacman.png");

	// Gameplay

	INCBIN(IMGmaze, "rom/img/maze.png");
	INCBIN(IMGmazeCollision, "rom/img/maze_collision.png");
	INCBIN(IMGmazeCollectables, "rom/img/maze_collectables.png");
	
	INCBIN(IMGghostBlinky, "rom/img/ghost/blinky.png");
	INCBIN(IMGghostInky, "rom/img/ghost/inky.png");
	INCBIN(IMGghostPinky, "rom/img/ghost/pinky.png");
	INCBIN(IMGghostClyde, "rom/img/ghost/clyde.png");	
}

