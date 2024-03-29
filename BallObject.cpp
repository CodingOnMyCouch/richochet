#include "BallObject.h"


BallObject::BallObject() : GameObject(), Radius(12.5f), Stuck(GL_TRUE), Sticky(GL_FALSE), PassThrough(GL_FALSE)
{
}


BallObject::BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture sprite) :
	GameObject(pos, glm::vec2(radius * 2, radius * 2), sprite, glm::vec3(1.0f), velocity), Radius(radius), Stuck(GL_TRUE), Sticky(GL_FALSE), PassThrough(GL_FALSE)
{
}


glm::vec2 BallObject::Move(GLfloat dt, GLuint window_width)
{
	//if not stuck to player board
	if (!this->Stuck)
	{
		//move the ball
		this->Position += this->Velocity * dt;
		// Then check if outside window bounds and if so, reverse velocity and restore at current position
		if (this->Position.x <= 0.0f)
		{
			this->Velocity.x = -this->Velocity.x;
			this->Position.x = 0.0f;
		}
		else if (this->Position.x + this->Size.x >= window_width)
		{
			this->Velocity.x = -this->Velocity.x;
			this->Position.x = window_width - this->Size.x;
		}
		if (this->Position.y <= 0.0f)
		{
			this->Velocity.y = -this->Velocity.y;
			this->Position.y = 0.0f;
		}
	}
	return this->Position;
}

//Resets the ball to initial Stuck Position (if ball is outside window bounds)
void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
	this->Position = position;
	this->Velocity = velocity;
	this->Stuck = GL_TRUE;
	this->Sticky = GL_TRUE;
	this->PassThrough = GL_TRUE;
}

