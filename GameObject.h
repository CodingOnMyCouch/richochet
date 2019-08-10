//GameObject.h
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#if !defined(_ANDROID_)
#include <GL/glew.h>
#else
#include <jni.h>
#include <android/log.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#endif

#include"Texture.h"
#include"SpriteRenderer.h"

//This container object is responsible for holding all states...
//that are relevant for a single game object entity.
//Each object in the game likely needs the minimal state that is described in the state object

class GameObject
{
public: 
		//object state
	glm::vec2 Position, Size, Velocity;
	glm::vec3 Colour;
	GLfloat Rotation;
	GLboolean IsSolid;
	GLboolean Destroyed;

	//Render state
	Texture Sprite;

	//constructor(s)
	GameObject(); //default constructor
	GameObject(glm::vec2 pos, glm::vec2 size, Texture sprite, glm::vec3 colour = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	//draw sprite
	virtual void Draw(SpriteRenderer &renderer);
};





#endif //GAMEOBJECT_H
