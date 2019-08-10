#include "ParticleGen.h"
#include <cstdlib>


//defining the constructor
ParticleGen::ParticleGen(Shader shader, Texture texture, GLuint amount)
	: shader(shader), texture(texture), amount(amount)
{
	this->init();
}

void ParticleGen::Update(GLfloat dt, GameObject &object, GLuint newParticles, glm::vec2 offset)
{
	//Add new particles
	for (GLuint i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->particles[unusedParticle], object, offset);
	}
	//update all particles
	for (GLuint i = 0; i < this->amount; ++i)
	{
		Particle &p = this->particles[i];
		p.Life -= dt; //reduce life
		if (p.Life > 0.0f)
		{  //particle is alive, thus update
			p.Position -= p.Velocity * dt;
			p.Colour.a -= dt * 2.5;
		}
	}
}

//render all particles
void ParticleGen::Draw()
{
	//Use additive blending to give it its glow effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	this->shader.Use();

	for (Particle particle : this->particles)
	{
		if (particle.Life > 0.0f)
		{
			this->shader.SetVector2f("offset", particle.Position);
			this->shader.SetVector4f("colour", particle.Colour);
			this->texture.Bind();
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	//don't forget to reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGen::init()
{
	//set up mesh and attribute properties
	GLuint VBO;
	GLfloat particle_quad[] =
	{
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
	};

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);

	//Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

	//set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	//Create this->amount default particle instances
	for (GLuint i = 0; i < this->amount; ++i)
		this->particles.push_back(Particle());
}

//stores the index of the last particle used (for quick access to next dead particle)
GLuint lastUsedParticle = 0;

GLuint ParticleGen::firstUnusedParticle()
{
	//first search from last used particle, this will usually return almost instantly
	for (GLuint i = lastUsedParticle; i < this->amount; ++i)
	{
		if (this->particles[i].Life <= 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}
	//otherwise do a linera search
	for (GLuint i = 0; i < lastUsedParticle; ++i)
	{
		if (this->particles[i].Life <= 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}
	//all particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be
	//reserved)
	lastUsedParticle = 0;
	return 0;
}


void ParticleGen::respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset)
{
	GLfloat random = ((rand() % 100) - 50) / 10.0f;
	GLfloat rColour = 0.5 + ((rand() % 100) / 100.0f);
	particle.Position = object.Position + random + offset;
	particle.Colour = glm::vec4(rColour, rColour, rColour, 1.0f);
	particle.Life = 1.0f;
	particle.Velocity = object.Velocity * 0.1f;
}


