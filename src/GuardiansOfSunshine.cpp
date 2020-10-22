#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>

#include "../headers/SDL_Helpers.h"
#include "../headers/GamePanel.h"

#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 240


int main(int nargs, char *args[])
{
	GamePanel gPanel;
	gPanel.setWindowName("Guardians of Sunshine");

	//Initialize Controllers
	if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0)
	{
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	gPanel.run();


	return 0;
}
