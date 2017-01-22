#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "display.h"
#include "resmanager.h"
#include "game.h"
#include "macros.h"

int main(int argc, char *argv[])
{
	int return_code = 0;

	// Init SDL2
	return_code = SDL_Init(SDL_INIT_VIDEO);
	if (return_code != 0)
	{
		ERR("Main: SDL_Init Error: " << SDL_GetError());
		return return_code;
	}
	LOG("Main: SDL2 Initialized successfully.");

	// Init SDL2_ttf
	return_code = TTF_Init();
	if (return_code != 0)
	{
		ERR("Main: TTF_Init Error: " << TTF_GetError());
		return return_code;
	}
	LOG("Main: SDL2_ttf Initialized successfully.");

	// Init SDL2_mixer
	return_code = Mix_Init(MIX_INIT_MOD);
	if (return_code != 0)
	{
		ERR("Main: Mix_Init Error: " << Mix_GetError());
		return return_code;
	}
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	LOG("Main: SDL2_mixer Initialized successfully.");

	// Init display
	Display display("#GGJ17 #FGJ17 @Harha", 640, 480, 3);
	//display.setState(SDL_WINDOW_FULLSCREEN);

	// Init & run game
	Game * game = new Game(&display);
	return_code = game->run();
	DELETE_SP(game);

	// Quit SDL2, TTF, Mixer
	Mix_Quit();
	TTF_Quit();
	SDL_Quit();

	return return_code;
}
