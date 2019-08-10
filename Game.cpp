#include<algorithm>
#include<sstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_Mixer.h>

#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "BallObject.h"
#include "ParticleGen.h"
#include "PostProcessor.h"
#include "TextRenderer.h"

namespace
{
	//game-related state data - creating a SpriteRenderer class object
	SpriteRenderer *Renderer;
	GameObject *Player;
	BallObject *Ball;
	ParticleGen *Particles;
	PostProcessor *Effects;
	GLfloat ShakeTime = 0.0f;
	TextRenderer *Text;

	Mix_Music *music = nullptr;
	Mix_Chunk *bleep = nullptr;
	Mix_Chunk *bleep2 = nullptr;
	Mix_Chunk *powerup = nullptr;
	Mix_Chunk *solid = nullptr;
}

//defining the constructor
Game::Game() : State(GAME_MENU), Keys(), Width(0), Height(0), Level(0), Lives(3)
{

}

//defining the destructor
Game::~Game()
{
	delete Renderer;
	delete Player;
	delete Ball;
	delete Particles;
	delete Effects;
	Mix_FreeMusic(music);
	Mix_FreeChunk(bleep);
	Mix_FreeChunk(bleep2);
	Mix_FreeChunk(powerup);
	Mix_FreeChunk(solid);
}

//defining the Game Initialising function
void Game::Init(GLuint width, GLuint height)
{
    Width = width;
    Height = height;

	//load shaders
	ResourceManager::LoadShader("vertexShader.vert", "fragmentShader.frag", nullptr, "sprite");
	ResourceManager::LoadShader("vertexParticle.vert", "fragmentParticle.frag", nullptr, "particle");
	ResourceManager::LoadShader("PP.vert", "PP.frag", nullptr, "postprocessing");

	//configure shaders

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

	//load textures
	ResourceManager::LoadTexture("textures/background.jpg", GL_FALSE, "background");
	ResourceManager::LoadTexture("textures/spikeyface.png", GL_TRUE, "face");
	ResourceManager::LoadTexture("textures/block.png", GL_FALSE, "block");
	ResourceManager::LoadTexture("textures/block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::LoadTexture("textures/paddle.png", GL_TRUE, "paddle");
	ResourceManager::LoadTexture("textures/particle.png", GL_TRUE, "particle");
	ResourceManager::LoadTexture("textures/powerup_speed.png", GL_TRUE, "powerup_speed");
	ResourceManager::LoadTexture("textures/powerup_sticky.png", GL_TRUE, "powerup_sticky");
	ResourceManager::LoadTexture("textures/powerup_increase.png", GL_TRUE, "powerup_increase");
	ResourceManager::LoadTexture("textures/powerup_confuse.png", GL_TRUE, "powerup_confuse");
	ResourceManager::LoadTexture("textures/powerup_chaos.png", GL_TRUE, "powerup_chaos");
	ResourceManager::LoadTexture("textures/powerup_passthrough.png", GL_TRUE, "powerup_passthrough");


	//set render-specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	Particles = new ParticleGen(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
	Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);
	Text = new TextRenderer(this->Width, this->Height);
	Text->Load("fonts/OCRAEXT.TTF", 24);

	//Load levels
	GameLevel one;
	one.Load("Levels/one.lvl", this->Width, this->Height * 0.5);
	GameLevel two;
	two.Load("Levels/two.lvl", this->Width, this->Height * 0.5);
	GameLevel three;
	three.Load("Levels/three.lvl", this->Width, this->Height * 0.5);
	GameLevel four;
	four.Load("Levels/four.lvl", this->Width, this->Height * 0.5);

	//populating the vector with the levels
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);
	this->Level = 0;

	//configure game object
	glm::vec2 playerPos = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	//configure ball object
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));

	music = Mix_LoadMUS("audio/breakout.wav");
	bleep = Mix_LoadWAV("audio/bleep.wav");
	bleep2 = Mix_LoadWAV("audio/bleep2.wav");
	solid = Mix_LoadWAV("audio/solid.wav");
	powerup = Mix_LoadWAV("audio/powerup.wav");
	assert(music != nullptr);
	assert(bleep != nullptr);
	assert(bleep2 != nullptr);
	assert(solid != nullptr);
	assert(powerup != nullptr);

	//audio
	Mix_PlayMusic(music, -1);
}

void Game::Update(GLfloat dt)
{
	//update objects
	Ball->Move(dt, this->Width);

	//check for collisions
	this->DoCollisions();

	//update particles
	Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2));

	//Update powerUps
	this->UpdatePowerUps(dt);

	//reduces shake time
	if (ShakeTime > 0.0f)
	{
		ShakeTime -= dt;
		if (ShakeTime <= 0.0f)
			Effects->Shake = GL_FALSE;
	}

	//check loss condition
	if (Ball->Position.y >= this->Height)  //did the ball reach the bottom edge?
	{
		--this->Lives;
		//did the player lose all of his lives? : Game Over
		if (this->Lives == 0)
		{
			this->ResetLevel();
			this->State = GAME_MENU;
		}
		this->ResetPlayer();
	}
	//check win condition
	if (this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted())
	{
		this->ResetLevel();
		this->ResetPlayer();
		Effects->Chaos = GL_TRUE;
		this->State = GAME_WIN;
	}
}

void Game::ProcessInput(GLfloat dt)
{
	if (this->State == GAME_MENU)
	{
		if (this->Keys[SDLK_RETURN] && !this->KeysProcessed[SDLK_RETURN])
		{
			this->State = GAME_ACTIVE;
			this->KeysProcessed[SDLK_RETURN] = GL_TRUE;
		}
		if (this->Keys[SDLK_w] && !this->KeysProcessed[SDLK_w])
		{
			this->Level = (this->Level + 1) % 4;
			this->KeysProcessed[SDLK_w] = GL_TRUE;
		}
		if (this->Keys[SDLK_s] && !this->KeysProcessed[SDLK_s])
		{
			if (this->Level > 0)
				--this->Level;
			else
				this->Level = 3;
			this->KeysProcessed[SDLK_s] = GL_TRUE;
		}
	}
	if (this->State == GAME_WIN)
	{
		if (this->Keys[SDLK_RETURN])
		{
			this->KeysProcessed[SDLK_RETURN] = GL_TRUE;
			Effects->Chaos = GL_FALSE;
			this->State = GAME_MENU;
		}
	}
	if (this->State == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * dt;
		//move player board
		if (this->Keys[SDLK_a])
		{
		if (Player->Position.x >= 0)
		{
		Player->Position.x -= velocity;
		if (Ball->Stuck)
		Ball->Position.x -= velocity;
		}
		}
		if (this->Keys[SDLK_d])
		{
		if (Player->Position.x <= this->Width - Player->Size.x)
		{
			Player->Position.x += velocity;
			if (Ball->Stuck)
				Ball->Position.x += velocity;
		}
		}
		if (this->Keys[SDLK_SPACE])
		    FireBall();
	}
}

void Game::FireBall()
{
	Ball->Stuck = false;
}


void Game::Render()
{
	if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN)
	{
		//begin rendering to post-processing quad
		Effects->BeginRender();

		//Draw background
		Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f);

		//Draw Level
		this->Levels[this->Level].Draw(*Renderer);

		//Draw Player
		Player->Draw(*Renderer);

		//Draw Particles
		Particles->Draw();

		//Draw Ball
		Ball->Draw(*Renderer);

		//Draw PowerUps
		for (PowerUp &powerUp : this->PowerUps)
			if (!powerUp.Destroyed)
				powerUp.Draw(*Renderer);

		//End rendering to postprocessing quad
		Effects->EndRender();

		//Render postprocessing quad
		Effects->Render( ((float)SDL_GetTicks())/1000.0f);

		//Render text (don't include postprocessing)
		std::stringstream ss;
		ss << this->Lives;

		Text->RenderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);
	}
	if (this->State == GAME_MENU)
	{
		Text->RenderText("Press ENTER to start", 250.0f, this->Height / 2, 1.0f);
		Text->RenderText("Press W or S to select level", 245.0f, this->Height / 2 + 20.0f, 0.75f);
	}
	if (this->State == GAME_WIN)
	{
		Text->RenderText("You WON!!!", 320.0f, this->Height / 2 - 20.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		Text->RenderText("Press ENTER to retry or ESC to quit", 130.0f, this->Height / 2, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
	}
}

void Game::ResetLevel()
{
	if (this->Level == 0)
		this->Levels[0].Load("Levels/one.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 1)
		this->Levels[1].Load("Levels/two.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 2)
		this->Levels[2].Load("Levels/three.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 3)
		this->Levels[3].Load("Levels/four.lvl", this->Width, this->Height * 0.5f);

	this->Lives = 3;
}

void Game::ResetPlayer()
{
	//reset player ball stats
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);

	//disable all active PowerUps
	Effects->Chaos = Effects->Confuse = GL_FALSE;
	Ball->PassThrough = Ball->Sticky = GL_FALSE;
	Player->Colour = glm::vec3(1.0f);
	Ball->Colour = glm::vec3(1.0f);
}


//PowerUps forward declaration
GLboolean IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type);

void Game::UpdatePowerUps(GLfloat dt)
{
	for (PowerUp &powerUp : this->PowerUps)
	{
		powerUp.Position += powerUp.Velocity * dt;
		if (powerUp.Activated)
		{
			powerUp.Duration -= dt;

			if (powerUp.Duration <= 0.0f)
			{
				//remove PowerUp from list (will later be removed)
				powerUp.Activated = GL_FALSE;
				//Deactivate effects
				if (powerUp.Type == "sticky")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "sticky"))
					{	//only reset if no ther PowerUp of type sticky is active
						Ball->Sticky = GL_FALSE;
						Player->Colour = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "pass-through")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "pass-through"))
					{	//only reset if no other PowerUp of type pass-through is active
						Ball->PassThrough = GL_FALSE;
						Ball->Colour = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "confuse")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "confuse"))
					{	//only reset if no other PowerUp of type confuse is active
						Effects->Confuse = GL_FALSE;
					}
				}
				else if (powerUp.Type == "chaos")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "chaos"))
					{	//only reset if no other PowerUp of type chaos is active
						Effects->Chaos = GL_FALSE;
					}
				}
			}
		}
	}
	// Remove all PowerUps from vector that are destroyed AND !activated (thus either off the map or finished)
	// Note we use a lambda expression to remove each PowerUp which is destroyed and not activated
	this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
		[](const PowerUp &powerUp) {return powerUp.Destroyed && !powerUp.Activated; }), this->PowerUps.end());
}

GLboolean ShouldSpawn(GLint chance)
{
	GLuint random = rand() % chance;
	return random == 0;
}

void Game::SpawnPowerUps(GameObject &block)
{
	if (ShouldSpawn(75))  //1 in 75
		this->PowerUps.push_back(PowerUp("speed", glm::vec3(0.05f, 0.05f, 1.0f), 0.0f,
			block.Position, ResourceManager::GetTexture("powerup_speed")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f,
			block.Position, ResourceManager::GetTexture("powerup_sticky")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f,
			block.Position, ResourceManager::GetTexture("powerup_passthrough")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4f), 0.0f,
			block.Position, ResourceManager::GetTexture("powerup_increase")));
	if (ShouldSpawn(15))
		this->PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f,
			block.Position, ResourceManager::GetTexture("powerup_confuse")));
	if (ShouldSpawn(15))
		this->PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f,
			block.Position, ResourceManager::GetTexture("powerup_chaos")));
}

void ActivatePowerUp(PowerUp &powerUp)
{	//		initiate a powerup based type of powerup
	if (powerUp.Type == "speed")
	{
		Ball->Velocity *= 1.2;
	}
	else if (powerUp.Type == "sticky")
	{
		Ball->Sticky = GL_TRUE;
		Player->Colour = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.Type == "pass-through")
	{
		Ball->PassThrough = GL_TRUE;
		Ball->Colour = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.Type == "pad-size-increase")
	{
		Player->Size.x += 50;
	}
	else if (powerUp.Type == "confuse")
	{
		if (!Effects->Chaos)
			Effects->Confuse = GL_TRUE;
	}
	else if (powerUp.Type == "chaos")
	{
		if (!Effects->Confuse)
			Effects->Chaos = GL_TRUE;
	}
}



GLboolean IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type)
{
	//check if another PowerUp of the same type is still active
	//in which case we dont disable its effect yet
	for (const PowerUp &powerUp : powerUps)
	{
		if (powerUp.Activated)
			if (powerUp.Type == type)
				return GL_TRUE;
	}
	return GL_FALSE;
}


//collision detection - forward declaration prototyping
GLboolean CheckCollision(GameObject &one, GameObject &two);
Collision CheckCollision(BallObject &one, GameObject &two);
Direction VectorDirection(glm::vec2 closest);


void Game::DoCollisions()
{
	for (GameObject &box : this->Levels[this->Level].Bricks)
	{
		if (!box.Destroyed)
		{
			Collision collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision))   //If collision is true
			{
				//Destroy block if solid
				if (!box.IsSolid)
				{
					box.Destroyed = GL_TRUE;
					this->SpawnPowerUps(box);
					Mix_PlayChannel(-1, bleep2, 0);
				}
				else
				{	//if box is solid enable shake effect
					ShakeTime = 0.05f;
					Effects->Shake = GL_TRUE;
					Mix_PlayChannel(-1, solid, 0);
				}
				//collision resolution
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (dir == LEFT || dir == RIGHT)
				{//Horizontal collision
					Ball->Velocity.x = -Ball->Velocity.x;  //reverse horizontal velocity
														   //reloacte
					GLfloat penetration = Ball->Radius - std::abs(diff_vector.x);
					if (dir == LEFT)
						Ball->Position.x += penetration; //move the ball to the right
					else
						Ball->Position.x -= penetration; //move the ball to the left
				}
				else
				{//Vertical collision
					Ball->Velocity.y = -Ball->Velocity.y; //reverse vertical velocity
														  //relocate
					GLfloat penetration = Ball->Radius - std::abs(diff_vector.y);
					if (dir == RIGHT)
						Ball->Position.y += penetration; //move the ball back up
					else
						Ball->Position.y -= penetration; //move the ball back down
				}
			}
		}
	}

	//also check for collisions on PowerUps and if so, activate them
	for (PowerUp &powerUp : this->PowerUps)
	{
		if (!powerUp.Destroyed)
		{
			//first check if powerup passed bottom edge, if so: keep as inactive and destroy
			if (powerUp.Position.y >= this->Height)
				powerUp.Destroyed = GL_TRUE;

			if (CheckCollision(*Player, powerUp))
			{	//collide with player, now activate powerUp
				ActivatePowerUp(powerUp);
				powerUp.Destroyed = GL_TRUE;
				powerUp.Activated = GL_TRUE;
				Mix_PlayChannel(-1, powerup, 0);
			}
		}
	}

	Collision result = CheckCollision(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(result))
	{
		//check where it hit the board, and change velocity on where it hit the board
		GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
		GLfloat distance = (Ball->Position.x + Ball->Radius) - centerBoard;
		GLfloat percentage = distance / (Player->Size.x / 2);

		//then move accordingly
		GLfloat strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		//keep speed consistent over both axes
		//multiply by length of old velocity, so total strength is not changed
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
		//Fix sticky paddle
		Ball->Velocity.y = -1 * abs(Ball->Velocity.y);

		// If Sticky powerup is activated, also stick ball to paddle once new velocity vectors were calculated
		Ball->Stuck = Ball->Sticky;

		Mix_PlayChannel(-1, bleep, 0);
	}
}

//defining the CheckCollision function
GLboolean CheckCollision(GameObject &one, GameObject &two) //AABB - AABB Collision
{
	//Collision x-axis
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x && two.Position.x + two.Size.x >= one.Position.x;
	//Collision y-axis
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y && two.Position.y + two.Size.y >= one.Position.y;
	//Collision only if on both sides
	return collisionX && collisionY;
}

//defining the CheckCollision function - function overloading
Collision CheckCollision(BallObject &one, GameObject &two)  //AABB - Circle collision
{
	//Get center point circle first
	glm::vec2 center(one.Position + one.Radius);

	//Calculate AABB info (center, half-extents)
	glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
	glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);

	//get difference vector between both centers
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

	//add clamped value to AABB_center and we get the value of box closest to circle
	glm::vec2 closest = aabb_center + clamped;

	//retrieve vector between center circle and closest point AABB and check if length <= radius
	difference = closest - center;
	
	if (glm::length(difference) <= one.Radius)
		return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
	else
		return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
}


Direction VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] =
	{
		glm::vec2(0.0f, 0.0f),  //up
		glm::vec2(1.0f, 0.0f),  //right
		glm::vec2(0.0f, -1.0f), //down
		glm::vec2(-1.0f, 0.0f), //left
	};

	GLfloat max = 0.0f;
	GLuint best_match = -1;

	for (GLuint i = 0; i < 4; i++)
	{
		GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}



