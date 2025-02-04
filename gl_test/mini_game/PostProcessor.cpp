#include "PostProcessor.h"

#include <iostream>

PostProcessor::PostProcessor(Shader shader, GLuint width, GLuint height)
: PostProcessingShader(shader), Texture(), Width(width), Height(height), Confuse(GL_FALSE), Chaos(GL_FALSE), Shake(GL_FALSE)
{
	glGenFramebuffers(1, &this->MSFBO);
	glGenFramebuffers(1, &this->FBO);
	glGenRenderbuffers(1, &this->RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);

	//使用纹理
	//GLuint testS;
	//glGenTextures(1, &testS);
	//glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, testS);
	//glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGB, width, height, GL_TRUE);
	//glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, testS, 0);
	
	//使用rbo
	glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_RGB, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	this->Texture.Generate(width, height, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->Texture.ID, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->initRenderData();
	this->PostProcessingShader.Use();
	this->PostProcessingShader.setInt("scene", 0);
	GLfloat offset = 1.0f / 300.0f;
	GLfloat offsets[9][2] = {
		{ -offset, offset },  // top-left
		{ 0.0f, offset },  // top-center
		{ offset, offset },  // top-right
		{ -offset, 0.0f },  // center-left
		{ 0.0f, 0.0f },  // center-center
		{ offset, 0.0f },  // center - right
		{ -offset, -offset },  // bottom-left
		{ 0.0f, -offset },  // bottom-center
		{ offset, -offset }   // bottom-right    
	};
	glUniform2fv(glGetUniformLocation(this->PostProcessingShader.ID, "offsets"), 9, (GLfloat*)offsets);
	GLint edge_kernel[9] = {
		-1, -1, -1,
		-1, 8, -1,
		-1, -1, -1
	};
	glUniform1iv(glGetUniformLocation(this->PostProcessingShader.ID, "edge_kernel"), 9, edge_kernel);
	GLfloat blur_kernel[9] = {
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16
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
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->MSFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);
	glBlitFramebuffer(0, 0, this->Width, this->Height, 0, 0, this->Width, this->Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessor::Render(GLfloat time)
{
	this->PostProcessingShader.Use();
	this->PostProcessingShader.setFloat("time", time);
	this->PostProcessingShader.setBool("confuse", this->Confuse);
	this->PostProcessingShader.setBool("chaos", this->Chaos);
	this->PostProcessingShader.setBool("shake", this->Shake);
	glActiveTexture(GL_TEXTURE0);
	this->Texture.Bind();
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void PostProcessor::initRenderData()
{
	GLuint VBO;
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
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