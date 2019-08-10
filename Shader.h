//The Shader class
#ifndef SHADER_H
#define SHADER_H

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
#include<glm/gtc/type_ptr.hpp>

class Shader {
public: 
		//state
	GLuint ID;

	//defualt constructor
	Shader() {};

	//Set the current shader as active
	Shader &Use();

	//Compiles the Shader from a given source code
	//Geomtry source is optional
	void Compile(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource = nullptr);

	//utility
	void SetFloat(const GLchar *name, GLfloat value, GLboolean useShader = false);
	void SetInteger(const GLchar *name, GLint value, GLboolean useShader = false);
	void SetVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader = false);
	void SetVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader = false);
	void SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false);
	void SetVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader = false);
	void SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false);
	void SetVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader = false);
	void SetMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader = false);

private: 
		 //checks to see if compilation or linking failed and if so, print the error logs
	void checkCompileErrors(GLuint object, std::string type);
};


#endif //SHADER_H
