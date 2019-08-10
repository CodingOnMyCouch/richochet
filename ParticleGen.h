//ParticleGen.h

#ifndef PARTICLE_GEN_H
#define PARTICLE_GEN_H

#include<vector>

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

#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"

//represents a single particle and its properties
struct Particle {
	glm::vec2 Position, Velocity;
	glm::vec4 Colour;
	GLfloat Life;

	Particle() : Position(0.0f), Velocity(0.0f), Colour(1.0f), Life(0.0f) {};
};


// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGen
{
public: 
		//constructor
	ParticleGen(Shader shader, Texture texture, GLuint amount);

	//Update all particles
	void Update(GLfloat dt, GameObject &object, GLuint newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));

	//render all particles
	void Draw();

private: 
		 //state
	std::vector<Particle> particles;
	GLuint amount;

	//render state
	Shader shader;
	Texture texture;
	GLuint VAO;

	//Initialises buffer and vertex attributes
	void init();

	//returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0if no particle is currently active
	GLuint firstUnusedParticle();

	//respawns particle
	void respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};


#endif //PARTICLE_GEN_H

