//Creating the game class for BreakOut
#ifndef GAME_H
#define GAME_H
#include<vector>
#include<tuple>

#if !defined(_ANDROID_)
#include <GL/glew.h>
#else
#include <jni.h>
#include <android/log.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#endif

#include "GameLevel.h"
#include "GameObject.h"
#include "PowerUp.h"

//represents the current state of the game
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN,
};

//represents the four possible collision directions
enum Direction {
	UP,
	DOWN,
	RIGHT,
	LEFT,
};


//defines a Collision typedef that represents the Collision data
typedef std::tuple<GLboolean, Direction, glm::vec2> Collision; //<collision?, what direction? difference vector center - closest point>

//initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100, 20);

//initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY(500.0f);

//initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);

//radius of the Ball Object
const GLfloat BALL_RADIUS = 12.5f;


class Game {
public: 
		//Game State
	GameState State;
	GLboolean Keys[1024];
	GLboolean KeysProcessed[1024];
	GLuint Width, Height;
	std::vector<GameLevel> Levels;
	std::vector<PowerUp> PowerUps;
	GLuint Level;
	GLuint Lives;

	//Constructor
	Game();

	//Destructor
	~Game();

	//Initialise game state - load all shaders/textures/levels
	void Init(GLuint width, GLuint height);

	//Game Loop
	void ProcessInput(GLfloat dt);
	void Update(GLfloat dt);
	void Render();

	//collision detection
	void DoCollisions();

	//reset
	void ResetLevel();
	void ResetPlayer();

	//PowerUps
	void SpawnPowerUps(GameObject &block);
	void UpdatePowerUps(GLfloat dt);

	void FireBall();
};

#endif GAME_H