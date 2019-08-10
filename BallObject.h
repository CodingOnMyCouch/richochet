#ifndef BALLOBJECT_H
#define BALLOBJECT_H

#if !defined(_ANDROID_)
#include <GL/glew.h>
#else
#include <jni.h>
#include <android/log.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#endif
#include<glm/glm.hpp>

#include "Texture.h"
#include "SpriteRenderer.h"
#include "GameObject.h"


// BallObject holds the state of the Ball object inheriting
// relevant state data from GameObject. Contains some extra
// functionality specific to Breakout's ball object that
// were too specific for within GameObject alone.

class BallObject : public GameObject //a child class of the GameObject class
{
public: 
		//Ball state - public variables
	GLfloat Radius;
	GLboolean Stuck;
	GLboolean Sticky;
	GLboolean PassThrough;

	//constructors
	BallObject();    
	BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture sprite);  

	//moves the ball, keeping it constrained within the windows bounds (except bottom edge); returns a new position
	glm::vec2 Move(GLfloat dt, GLuint window_width);

	//resets the ball to original state with given position and velocity
	void Reset(glm::vec2 position, glm::vec2 velocity);
};


#endif //BALLOBJECT_H
