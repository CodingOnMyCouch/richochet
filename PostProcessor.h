#ifndef POSTPROCESSOR_H
#define POSTPROCESSOR_H

#if !defined(_ANDROID_)
#include <GL/glew.h>
#else
#include <jni.h>
#include <android/log.h>

#include <KHR/khrplatform.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include "GLFunctionLoader.h"

#endif
#include<glm/glm.hpp>

#include "Texture.h"
#include "SpriteRenderer.h"
#include "Shader.h"

// PostProcessor hosts all PostProcessing effects for the Breakout
// Game. It renders the game on a textured quad after which one can
// enable specific effects by enabling either the Confuse, Chaos or 
// Shake boolean. 
// It is required to call BeginRender() before rendering the game
// and EndRender() after rendering the game for the class to work.
class PostProcessor
{
public: 
		//state
	Shader PostProcessingShader;
	Texture Texture;
	GLuint Width;
	GLuint Height;

	//options
	GLboolean Confuse, Chaos, Shake;

	//constructor
	PostProcessor(Shader shader, GLuint width, GLuint height);

	//prepares the postprocessor's framebuffer operations before rendering the game
	void BeginRender();

	//should be called after rendering the game, so it stores all the rendered data into a texture object
	void EndRender();

	//renders the PostProcessor texture quad, as a screen encompassing large sprite
	void Render(GLfloat time);

private: 
		 //Render state
	GLuint MSFBO, FBO;  //MSFBO - multisampled FBO. FBO is regular, used for blittingMS colour-buffer to texture
	GLuint RBO;         //RBO is used for multisampled colour buffer
	GLuint VAO;

	//Initialise quad for rendering postprocessing texture
	void initRenderData();
};

#endif //POSTPROCESSOR_H


