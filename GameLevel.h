//GameLevel.h
#ifndef GAMELEVEL_H
#define GAMELEVEL_H

#include<vector>

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
#include "SpriteRenderer.h"
#include "ResourceManager.h"

//GameLevel holds all Tiles as part of a Breakout level and 
//hosts functionality to Load/Render levels from the harddisk
class GameLevel
{
public: 
		//level state
	std::vector<GameObject> Bricks;

	//constructor
	GameLevel() {}

	//Loads level from file
	void Load(const GLchar *file, GLuint levelWidth, GLuint levelHeight);

	//Render Level
	void Draw(SpriteRenderer &renderer);

	//check if the level is completed (all non-solid tiles are destroyed)
	GLboolean IsCompleted();

private: 
		 //initialise level from tile data
	void init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
};



#endif //GAMELEVEL_H

