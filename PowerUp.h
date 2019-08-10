#ifndef POWER_UP_H
#define POWER_UP_H

#include<string>

#if !defined(_ANDROID_)
#include <GL/glew.h>
#else
#include <jni.h>
#include <android/log.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#endif
#include<glm/glm.hpp>

#include "GameObject.h"

//the size of a PowerUp block
const glm::vec2 SIZE(60, 20);

//Velocity a PowerUp block has stop
const glm::vec2 VELOCITY(0.0f, 150.0f);

//the class Power_Up inherits from the GameObject class
class PowerUp : public GameObject
{
public: 
		//PowerUp state
	std::string Type;
	GLfloat Duration;
	GLboolean Activated;

	//constructor with an intialiser list
	PowerUp(std::string type, glm::vec3 colour, GLfloat duration, glm::vec2 position, Texture texture)
		: GameObject(position, SIZE, texture, colour, VELOCITY), Type(type), Duration(duration), Activated() {}

};


#endif //POWER_UP_H
