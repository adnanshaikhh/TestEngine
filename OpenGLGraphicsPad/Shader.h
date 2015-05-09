#pragma once
#include "GL\glew.h"
#include<glm.hpp>
#include<string>
#include<vector>


class Shader
{
public:
	Shader();
	~Shader();
	void addUniform3f(const GLchar * uniformName, glm::vec3 value);
	void addUniformMatrix4fv(const GLchar * uniformName, glm::mat4 matrix);



	void installShaders(const char* VertexShader, const char * FragmentShader);
	std::string  readShaderCode(const char* fileName);
	bool checkShaderStatus(GLuint shaderID);
	bool checkProgramStatus(GLuint programID);

	GLuint getProgramID();

private:
	GLuint m_uniformLocation;
	GLuint m_vertexShaderID, m_fragmentShaderID;
	GLuint m_programID;
	GLint m_compileStatus;
	GLint m_linkStatus;
	
};

