#include "../headers/GamePanel.h"
#include "../headers/SDL_Helpers.h"
#include "../headers/AudioPlayer.h"
#include <stdio.h>

bool *GamePanel::isRunningControl;
int GamePanel::WINDOW_WIDTH;
int GamePanel::WINDOW_HEIGHT;
SDL_Joystick *joystick;
SDL_Joystick *joystick1;
SDL_Joystick *joystick2;

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
    	joystick1 = SDL_JoystickOpen(1); //joystick declared globally
    	joystick2 = SDL_JoystickOpen(2); //joystick declared globally
		// printf("Number of joysticks: %i\n", SDL_NumJoysticks());
		// printf("Opening joysticks:\n");
		// printf("    %s\n", SDL_JoystickName(joystick));
		// printf("    %s\n", SDL_JoystickName(joystick1));
		// printf("    %s\n", SDL_JoystickName(joystick2));



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
		else if (event.type == SDL_JOYAXISMOTION) //hat
		{
			//int keyCode = event.key.keysym.sym;
			//gsm->keyReleased(keyCode);
			int val = 0;

			// button = 0 --> L/R
			// button = 1 --> Up/Down
			int button = event.jhat.value;

			// Left
			if (button == 0 && event.jaxis.value < 0){
				val = 8;
			// right
			} else if (button == 0 && event.jaxis.value > 0) {
				val = 2;
			// up
			} else if (button == 1 && event.jaxis.value < 0) {
				val = 1;
			// down
			} else if (button == 1 && event.jaxis.value > 0) {
				val = 4;
			}

			printf("JOYSTICK\n");
			printf("button: %d, jaxis: %d\n", button, event.jaxis.value);
			
			/*
			0 - nothing
			1 - up
			2 - right
			8 - left
			4 - down
			*/
			gsm->hatPressed(val);
		}
		else if (event.type == SDL_JOYBUTTONUP && (event.jbutton.button == 4 || event.jbutton.button == 5 || event.jbutton.button == 8 || event.jbutton.button == 9) && (gsm->getState() == GameStateManager::LEVEL1_STATE)) {
			int button = event.jbutton.button;
			printf("Key release %d, button\n");
			printf("No left or right pressed now");
			gsm->hatPressed(0);
		}
		else if (event.type == SDL_JOYBUTTONDOWN && (event.jbutton.button == 4 || event.jbutton.button == 5 || event.jbutton.button == 8 || event.jbutton.button == 9) && (gsm->getState() == GameStateManager::LEVEL1_STATE)) {
			int button = event.jbutton.button;
			printf("Key press %d, button");

			// go left
			if (button == 4) {
				gsm->hatPressed(8);
			// go right
			} else if (button == 5) {
				gsm->hatPressed(2);
			// go down
			} else if (button == 8) {
				gsm->hatPressed(1);
			}
			// go up
			else if (button == 9) {
				gsm->hatPressed(4);
			}
			else {
				gsm->hatPressed(0);
				printf("Wat da!!!!");
			}
		}
		else if (event.type == SDL_JOYBUTTONDOWN) //button press down
		{
			//int keyCode = event.key.keysym.sym;
			//gsm->keyPressed(keyCode);

			int button = event.jbutton.button;
			printf("key_down: %d", button);

			gsm->buttonPressed(button);
		}
		else if (event.type == SDL_JOYBUTTONUP) //button up
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
