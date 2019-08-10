//Resource_Manager.cpp
#include "ResourceManager.h"

#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include <vector>

#include <SDL2/SDL.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#if defined(_ANDROID_)
#include <android/log.h>
#endif


//instantiate static variables
std::map<std::string, Texture> ResourceManager::Textures;
std::map<std::string, Shader>    ResourceManager::Shaders;

namespace
{
    #define LOG_TAG "ResourceManager"
	unsigned int fileSize(SDL_RWops * ops)
	{
		Sint64 currentOffset = SDL_RWtell(ops);
		SDL_RWseek(ops, 0, SEEK_END);
		const unsigned int seekResult = (const unsigned int)SDL_RWtell(ops);
		SDL_RWseek(ops, currentOffset, SEEK_SET);
		return seekResult;
	}

	std::vector<char> loadFile(const std::string & filename)
	{
		std::vector<char> result;
		SDL_RWops *io = SDL_RWFromFile(filename.c_str(), "rb");
		if (io != nullptr) {
			unsigned int fSize = fileSize(io);
			result.resize(fSize);
			SDL_RWread(io, &(result[0]), fSize, 1);
			SDL_RWclose(io);
#if defined(_ANDROID_)
            __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Successfully read file=%s\n", filename.c_str());
#endif
		}
#if defined(_ANDROID_)
		else
			__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Failed to read file=%s\n", filename.c_str());
#endif

		return result;
	}

	std::string loadFileAsString(const std::string & filename)
	{
		std::string result;
		std::vector<char> data = loadFile(filename);
		if (data.size() > 0)
		{
			result.resize(data.size() + 1);
			result[data.size()] = '\0';
			memcpy(&result[0], &data[0], data.size());
		}
		return result;
	}
}


std::vector<char> ResourceManager::LoadFile(std::string name)
{
	return ::loadFile(name);
}


//defining the functions in the static Resource Manager class
Shader ResourceManager::LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name)
{
	Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
	return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
	return Shaders[name];
}

Texture ResourceManager::LoadTexture(const GLchar *file, GLboolean alpha, std::string name)
{
	Textures[name] = loadTextureFromFile(file, alpha);
	return Textures[name];
}

Texture ResourceManager::GetTexture(std::string name)
{
	return Textures[name];
}

void ResourceManager::Clear()
{
	//(properly) delete all shaders
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.ID);
	//(properly) delete all textures
	for (auto iter : Textures)
		glDeleteTextures(1, &iter.second.ID);
}


Shader ResourceManager::loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile)
{
	
	//1. retrieve the vertex/fragment shader from the filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	try
	{
		vertexCode = loadFileAsString(vShaderFile);
		fragmentCode = loadFileAsString(fShaderFile);

		//If geometry shader path is present also load a geometry shader
		if (gShaderFile != nullptr)
		{
			geometryCode = loadFileAsString(gShaderFile);
		}
	}
	catch (std::exception e)
	{
		std::cout << "ERROR::SHADER: Failed to read shader file " << std::endl;
	}

	const GLchar *vShaderCode = vertexCode.c_str();
	const GLchar *fShaderCode = fragmentCode.c_str();
	const GLchar *gShaderCode = geometryCode.c_str();

	//2. Now create shader object from source code
	Shader shader;
	shader.Compile(vShaderCode, fShaderCode, gShaderFile);
	return shader;
}

Texture ResourceManager::loadTextureFromFile(const GLchar *file, GLboolean alpha)
{
	//create texture object
	Texture texture;
	std::vector<char> data = loadFile(file);
	assert(data.size() > 0);


	//Load image
	int width, height, components;
	stbi_uc * uc = stbi_load_from_memory((const stbi_uc*) (&data[0]), data.size(), &width, &height, &components, alpha ? 4: 3);
	if (components == 4)
	{
		texture.Internal_Format = GL_RGBA;
		texture.Image_Format = GL_RGBA;
	}

	assert(uc != nullptr);
	//now generate texture
	texture.Generate(width, height, uc);
	//And finally free image data
	stbi_image_free(uc);
	//stbi_image_free(image);
	return texture;
}