#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include<map>

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

#include "Texture.h"
#include "Shader.h"

//Holds all state information relevevant to a character as loaded using FreeType
struct Character {
	GLuint Texture;     //ID handle of the glyph texture
	glm::ivec2 Size;    //Size of glyph
	glm::ivec2 Bearing; //Offest from baseline to left/top glyph
	GLuint Advance;     //Horizontal offset to advance to nect glyph
};


//A renderer class for rendering text displayed by a font loaded using the 
//FreeType library. A single font is loaded, processed into a list of Character
//items form later rendering
class TextRenderer
{
public: 
		//holds a list of pre-compiled Characters
	std::map<GLchar, Character> Characters;

	//Shader used for text rendering
	Shader TextShader;

	//constructor with initialiser list
	TextRenderer(GLuint width, GLuint height);

	//pre-compiles a list of characters from a given font
	void Load(std::string font, GLuint fontSize);

	//renders a string of text using the precompiled list of characters
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 colour = glm::vec3(1.0f));

private:
	GLuint VAO;
	GLuint VBO;
};






#endif //TEXTRENDERER_H
