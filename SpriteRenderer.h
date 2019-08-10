//SpriteRenderer class
#ifndef SPRITERENDERER_H
#define SPRITERENDERER_H

#if !defined(_ANDROID_)
#include <GL/glew.h>
#else
#include <jni.h>
#include <android/log.h>

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include "GLFunctionLoader.h"

#endif
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include "Texture.h"
#include "Shader.h"

class SpriteRenderer {
public: 

		//constructor which takes a parameter
	SpriteRenderer(Shader shader);

	//destructor
	~SpriteRenderer();

	void DrawSprite(Texture texture, glm::vec2 position,
		glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f,
		glm::vec3 colour = glm::vec3(1.0f));

private: 
		 //render state
	Shader shader;
	GLuint quadVAO;

	//initialises and configures the quad's buffer and vertex attributes
	void initRenderData();
};



#endif //SPRITERENDERER_H
