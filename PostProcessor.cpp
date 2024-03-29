#include "PostProcessor.h"
#include <iostream>
#include <cmath>


//defining the constructor with initialiser list
PostProcessor::PostProcessor(Shader shader, GLuint width, GLuint height)
	: PostProcessingShader(shader), Texture(), Width(width), Height(height), Confuse(GL_FALSE), Chaos(GL_FALSE), Shake(GL_FALSE)
{

	//initialise renderbuffer/framebuffer object
	glGenFramebuffers(1, &this->MSFBO);
	glGenFramebuffers(1, &this->FBO);
	glGenRenderbuffers(1, &this->RBO);

	//initialise renderbuffer storage with a multisampled colour buffer (don't need a depth/stencil buffer)
	glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
	//allocate storage for render buffer object
	int samples = 8;
#if defined(_ANDROID_)
    samples = 4;
#endif
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 2, GL_RGB8, width, height);

	//Attach MS render object to framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO);

	int frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (frameBufferStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialise MSFBO" << std::endl;
#if defined(_ANDROID_)
		int error = 0;
		switch (frameBufferStatus)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			error = 1;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
			error = 2;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			error = 3;
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			error = 4;
			break;
		default:
			error = 5;
			break;
		}
            __android_log_print(ANDROID_LOG_ERROR, "POSTPROCESSOR", "ERROR::POSTPROCESSOR: Failed to initialise MSFBO(error=%d. RBO=%d, MSFBO=%d)\n",error, RBO, MSFBO);
#endif
	}

	//also initialise the FBO/texture to blit multisampled colour buffer to;
	//used for shader operations (for postprocessing events)
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	this->Texture.Generate(width, height, NULL);
	//attach texture to framebuffer as its colour attachment
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->Texture.ID, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialise FBO" << std::endl;
#if defined(_ANDROID_)
            __android_log_print(ANDROID_LOG_ERROR, "POSTPROCESSOR", "ERROR::POSTPROCESSOR: Failed to initialise FBO\n");
#endif
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	//initialise render data and uniforms
	this->initRenderData();
	this->PostProcessingShader.SetInteger("scene", 0, GL_TRUE);
	GLfloat offset = 1.0f / 300.0f;
	GLfloat offsets[9][2] =
	{
		{ -offset, offset }, //top-left
		{ 0.0f,   offset }, //top-center
		{ offset, offset }, //top-right
		{ -offset, 0.0f }, //center-left
		{ 0.0f,   0.0f }, //center-center
		{ offset, 0.0f }, //center-right
		{ -offset,-offset }, //bottoem-left
		{ 0.0f,  -offset }, //bottom-center
		{ offset,-offset }, //bottom-right
	};

	glUniform2fv(glGetUniformLocation(this->PostProcessingShader.ID, "offsets"), 9, (GLfloat*)offsets);

	GLint edge_kernel[9] =
	{
		-1, -1, -1,
		-1,  8, -1,
		-1, -1, -1,
	};

	glUniform1iv(glGetUniformLocation(this->PostProcessingShader.ID, "edge_kernel"), 9, edge_kernel);

	GLfloat blur_kernel[9] =
	{
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16,
	};

	glUniform1fv(glGetUniformLocation(this->PostProcessingShader.ID, "blur_kernel"), 9, blur_kernel);
}


void PostProcessor::BeginRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::EndRender()
{
	//now resolve multisampled colour-buffer into intermediate FBO to store texture
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->MSFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);
	glBlitFramebuffer(0, 0, this->Width, this->Height, 0, 0, this->Width, this->Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);   //Binds both WRITE and READ framebuffer to default framebuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void PostProcessor::Render(GLfloat time)
{
	//set uniforms/options
	this->PostProcessingShader.Use();
	this->PostProcessingShader.SetFloat("time", std::fmod(time, 6.28318530718f));
	this->PostProcessingShader.SetInteger("confuse", this->Confuse);
	this->PostProcessingShader.SetInteger("chaos", this->Chaos);
	this->PostProcessingShader.SetInteger("shake", this->Shake);

#if defined(_ANDROID_)
    __android_log_print(ANDROID_LOG_ERROR, "POSTPROCESSOR", "time=%f\n",time);
#endif


	//render textured quad
	glActiveTexture(GL_TEXTURE0);
	this->Texture.Bind();
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void PostProcessor::initRenderData()
{
	//configure VAO/VBO
	GLuint VBO;
	GLfloat vertices[] =
	{
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
	};

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
