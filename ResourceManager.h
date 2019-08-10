//Resource_Manager.h
#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include<map>
#include<string>
#include <vector>

#if !defined(_ANDROID_)
#include <GL/glew.h>
#else
#include <jni.h>
#include <android/log.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#endif

#include "Texture.h"
#include "Shader.h"


// A static singleton ResourceManager class that hosts several
// functions to load Textures and Shaders. Each loaded texture
// and/or shader is also stored for future reference by string
// handles. All functions and resources are static and no 
// public constructor is defined.
class ResourceManager
{
public: 
		//Resource storage
	static std::map<std::string, Shader>    Shaders;
	static std::map<std::string, Texture> Textures;

	//loads and generates a shader program from file loading vertex, fragment (and geometry) shader's source code.
	//If gShaderFile is not nullptr, it also loads a geometry shader
	static Shader LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name);
	//Retrieves a stored shader
	static Shader GetShader(std::string name);

	//Loads and generates a texture from a file
	static Texture LoadTexture(const GLchar *file, GLboolean alpha, std::string name);
	//Retrieves a stored texture
	static Texture GetTexture(std::string name);

	static std::vector<char> LoadFile(std::string name);

	//properly de-allocates all loaded resources
	static void Clear();

private: 
		 //Private constructor - we do not want any Resource manager objects.
		 //Its members and functions should be publicly availiable (static)
	ResourceManager() {};

	//Loads and generates a shader from file
	static Shader loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile = nullptr);

	//Loads a single texture from a file
	static Texture loadTextureFromFile(const GLchar *file, GLboolean alpha);
};




#endif //RESOURCEMANAGER_H