//main.cpp
#if !defined(_ANDROID_)
#define GLEW_STATIC
#include <GL/glew.h>
#else
#include <jni.h>
#include <android/log.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_Mixer.h>

#include "Game.h"
#include "ResourceManager.h"

namespace
{
	//The Width of the screen
#if defined(_ANDROID_)
	GLuint SCREEN_WIDTH = 1280;
	GLuint SCREEN_HEIGHT = 800;
#else
	const GLuint SCREEN_WIDTH = 800;
	const GLuint SCREEN_HEIGHT = 600;
#endif

	Game Ricochet;
	std::string gameName("Ricochet - A Demo Game by IK Nwoke");

	SDL_Window * window;
	SDL_GLContext context;
	bool shouldQuit = false;

	void key_callback(SDL_Window *window, SDL_Keycode key, unsigned int action)
	{
		//when the user presses the escape key, we set shouldQuit to true, closing the application
		if (key == SDLK_ESCAPE && action == SDL_KEYDOWN) {
			shouldQuit = true;
		}
		if (key >= 0 && key < 1024)
		{
			if (action == SDL_KEYDOWN)
				Ricochet.Keys[key] = GL_TRUE;
			else if (action == SDL_KEYUP)
				Ricochet.Keys[key] = GL_FALSE;
			Ricochet.KeysProcessed[key] = GL_FALSE;
		}
	}

	void touch_callback(SDL_Window *window, float x, float y, unsigned int action)
	{
		//when the user presses the escape key, we set shouldQuit to true, closing the application
		if (action == SDL_FINGERDOWN)
		{
		    if(Ricochet.State == GAME_MENU)
		    {
    		    Ricochet.State = GAME_ACTIVE;
		    }
		}

		{
		    const int key = x<0.5f ? SDLK_a : SDLK_d;
			if (action == SDL_FINGERDOWN)
			{
				Ricochet.Keys[key] = GL_TRUE;
				Ricochet.Keys[SDLK_SPACE] = GL_TRUE;
			}
			else if (action == SDL_FINGERUP)
			{
				Ricochet.Keys[key] = GL_FALSE;
				Ricochet.Keys[SDLK_SPACE] = GL_FALSE;
			}
			Ricochet.KeysProcessed[key] = GL_FALSE;
			Ricochet.KeysProcessed[SDLK_SPACE] = GL_FALSE;
		}
	}


}

#if defined(_ANDROID_)
int SDL_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		return false;

#ifdef _ANDROID_
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

#if defined(_ANDROID_)
	SDL_DisplayMode currentDisplayMode;
	int success = SDL_GetCurrentDisplayMode(0, &currentDisplayMode);
	assert(success == 0);
	SCREEN_WIDTH = currentDisplayMode.w;
    SCREEN_HEIGHT = currentDisplayMode.h;
#endif

	window = SDL_CreateWindow(gameName.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
	assert(window != nullptr);

	SDL_SetHint("SDL_HINT_ORIENTATIONS","LandscapeRight LandscapeLeft");

	context = SDL_GL_CreateContext(window);
	assert(context != nullptr);

#ifndef _ANDROID_
	SDL_GL_SetSwapInterval(1); // enable vsync
#endif

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

#ifndef _ANDROID_
	glewExperimental = GL_TRUE;
	glewInit();
#endif
	glGetError();   //Call it once to catch glewInit() bug, all other errors are now form our application

//	glfwSetKeyCallback(window, key_callback);

	//OpenGL configuration
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Initialise game
	Ricochet.Init(SCREEN_WIDTH, SCREEN_HEIGHT);

	//Deltatime variables

	//start game within menu state
	Ricochet.State = GAME_MENU;

	shouldQuit = false;
	unsigned int lastFrame = SDL_GetTicks();
	while (!shouldQuit)
	{
		//Calculate delta time
		unsigned int currentFrame = SDL_GetTicks();
		unsigned int deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				key_callback(window, event.key.keysym.sym, event.type);
				break;
			case SDL_FINGERDOWN:
			case SDL_FINGERUP:
			    touch_callback(window, event.tfinger.x, event.tfinger.y, event.type);
			    break;
			    default:
    			break;

			}

		}

		//deltaTime = 0.001f
		//Manage User Input
		const float deltaSeconds = ((float)deltaTime) / 1000.0f;
		Ricochet.ProcessInput(deltaSeconds);

		//Update Game state
		Ricochet.Update(deltaSeconds);

		//Render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		Ricochet.Render();

		SDL_GL_SwapWindow(window);

	}

	//Delete all resources as loaded using resource manager
	ResourceManager::Clear();

	SDL_Quit();
	return 0;
}

