#include "../headers/GamePanel.h"
#include "../headers/SDL_Helpers.h"
#include "../headers/AudioPlayer.h"
#include <stdio.h>

bool *GamePanel::isRunningControl;
int GamePanel::WINDOW_WIDTH;
int GamePanel::WINDOW_HEIGHT;
SDL_Joystick *joystick;

GamePanel::GamePanel()
{
	window_name = "Window Name";
	isRunning = false;
	FPS = 60;
	//frameTime = 0;
	prevTime = 0;
	currTime = 0;
	deltaTime = 0;
	window = NULL;
	renderTarget = NULL;
	gsm = NULL;
	GamePanel::isRunningControl = getRunningPointer();
}
GamePanel::~GamePanel()
{
	if (window)
		SDL_DestroyWindow(window);
	if (renderTarget)
		SDL_DestroyRenderer(renderTarget);
	if (gsm)
		delete gsm;
	gsm = NULL;
	window = NULL;
	renderTarget = NULL;

	SDL_Quit();
}
int GamePanel::init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Video init error: %s\n", SDL_GetError());
        return 1;
    }
	
	//window = SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_CENTERED,
    //                            SDL_WINDOWPOS_CENTERED, (int)(WIDTH*3), (int)(HEIGHT*3), SDL_WINDOW_SHOWN /*| SDL_WINDOW_FULLSCREEN_DESKTOP*/);
	window = SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
	if (window == NULL)
    {
        printf("Window creation error: %s\n", SDL_GetError());
        return 1;
    }

	SDL_GetWindowSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);

	printf("Window width = %d, window height = %d\n", WINDOW_WIDTH, WINDOW_HEIGHT);


	renderTarget = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderTarget == NULL)
	{
		printf("Failed to init render target. Error: %s\n", SDL_GetError());
		return 1;
	}
	if (AudioPlayer::initAudioSystem())
    {
        return 1;
    }
	gsm = new GameStateManager(renderTarget);
	if (gsm == NULL)
	{
		printf("Failed to create game state manager\n");
		return 1;
	}
	
	//SDL_SetRenderDrawColor(renderTarget, 0xFF, 0, 0, 0xFF);
	SDL_SetRenderDrawColor(renderTarget, 0, 0, 0, 0xFF);
	SDL_ShowCursor(SDL_DISABLE);
	return 0;	
}
int GamePanel::run()
{
	int init_check = init();
	if (init_check)
		return init_check;

	isRunning = true;
	while(isRunning)
	{
		
		prevTime = currTime;
        currTime = SDL_GetTicks();
        deltaTime = (currTime - prevTime)/1000.0f;

    	SDL_JoystickEventState(SDL_ENABLE);
    	joystick = SDL_JoystickOpen(0); //joystick declared globally

		handleInputs(); //Init gamepad here?
		update();
		draw();
		drawToScreen();

		while (SDL_GetTicks() - currTime < 16);	
	}
	close();
	return 0;
}
void GamePanel::handleInputs()
{
	while (SDL_PollEvent(&event) != 0)
	{
		printf("event type %d\n", event.type);
		printf("button %d\n", event.jbutton.button);

		if (event.type == SDL_QUIT)
		{
			isRunning = false;
		}
		else if (event.type == 1539) //button press
		{
			//int keyCode = event.key.keysym.sym;
			//gsm->keyPressed(keyCode);

			int button = event.jbutton.button;
			printf("key_down: %d", button);

			gsm->buttonPressed(button);
		}
		else if (event.type == 1540) //button up
		{
			//int keyCode = event.key.keysym.sym;
			//gsm->keyReleased(keyCode);

			int button = event.jbutton.button;
			printf("key_up: %d", button);
			
			gsm->buttonReleased(button);
		}
		else if (event.type == SDL_TEXTINPUT)
		{
			char *text = event.text.text;
		}
		else if (event.type == 1538) //hat
		{
			//int keyCode = event.key.keysym.sym;
			//gsm->keyReleased(keyCode);

			int button = event.jhat.value;
			printf("HAT\n");
			printf("hat: %d\nwhich: %d\nvalue %d\n", event.jhat.hat, event.jhat.which, event.jhat.value);
			
			gsm->buttonReleased(button);
		}
	}
	keyState = SDL_GetKeyboardState(NULL);
}
void GamePanel::update()
{
	gsm->update();	
}
void GamePanel::draw()
{
	SDL_RenderClear(renderTarget);

	//Draw all game objects/graphics
	gsm->draw();
}
void GamePanel::drawToScreen()
{
	SDL_RenderPresent(renderTarget);
}
void GamePanel::close()
{
	AudioPlayer::closeAudioSystem();
	SDL_Quit();
}
