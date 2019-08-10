//Texture.h
#ifndef TEXTURE_H
#define TEXTURE_H

#if !defined(_ANDROID_)
#include <GL/glew.h>
#else
#include <jni.h>
#include <android/log.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#endif


//Texture2D is able to store and configure a texture in OpenGL
//It also hosts untility functions for easy management
class Texture
{
public: 
		//Holds the ID of the current texture, used for texture operations to reference this particular texture
	GLuint ID;

	//Texture image dimensions
	GLuint Width, Height; //Width and height of loaded image in pixels

						  //Texture format
	GLuint Internal_Format; //Format of texture object
	GLuint Image_Format;    //Format of loaded image

							//Texture configuration
	GLuint Wrap_S;     //Wrapping mode on S axis
	GLuint Wrap_T;     //Wrapping mode on T axis
	GLuint Filter_Max; //Filtering mode if texture pixels < screen pixels
	GLuint Filter_Min; //Filtering mode if texture pixels > screen pixels

					   //Constructor - sets default texture modes
	Texture();

	//generate texture from image data
	void Generate(GLuint width, GLuint height, unsigned char* data);

	//Binds the texture as the current GL_ACTIVE_2D texture object
	void Bind() const;
};





#endif //TEXTURE_H

