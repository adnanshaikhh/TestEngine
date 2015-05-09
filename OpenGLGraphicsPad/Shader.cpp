#include "Shader.h"
#include<iostream>
#include<fstream>


Shader::Shader()
{
	//m_vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	//m_fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	//m_programID = glCreateProgram();
}


Shader::~Shader()
{
	std::cout << "Shader Object Deleted" << std::endl;
	glDeleteShader(m_vertexShaderID);
	glDeleteShader(m_fragmentShaderID);

	std::cout << "Program Object Deleted" << std::endl;
	glUseProgram(0);
	glDeleteProgram(m_programID);
}


void Shader::addUniform3f(const GLchar * uniformName, glm::vec3 value)
{

	m_uniformLocation = glGetUniformLocation(m_programID, uniformName);

	if (m_uniformLocation == 0xFFFFFFFF)
	{
	//	std::cout << "Error : Could not find uniform: " << uniformName << std::endl;
	//	return;
	}

	glUniform3f(m_uniformLocation, value.x, value.y, value.z);
	
	m_uniformLocation = 0;

}

void  Shader::addUniformMatrix4fv(const GLchar * uniformName, glm::mat4 matrix)
{
	m_uniformLocation = glGetUniformLocation(m_programID, uniformName);

	if (m_uniformLocation == 0xFFFFFFFF)
	{
	//	std::cout << "Error : Could not find uniform: " << uniformName << std::endl;
	//	return;
	}

	glUniformMatrix4fv(m_uniformLocation, 1, GL_FALSE, &matrix[0][0]);

	m_uniformLocation = 0;
}

void Shader::installShaders(const char * VertexShader, const char * FragmentShader)
{
	m_vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	m_fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);


	const char * adapter[1];

	std::string temp = readShaderCode(VertexShader);
	adapter[0] = temp.c_str();

	
	glShaderSource(m_vertexShaderID, 1, adapter, NULL);
	glCompileShader(m_vertexShaderID);



	temp = readShaderCode(FragmentShader);
	adapter[0] = temp.c_str();

	glShaderSource(m_fragmentShaderID, 1, adapter, NULL);
	glCompileShader(m_fragmentShaderID);



	checkShaderStatus(m_vertexShaderID);
	checkShaderStatus(m_fragmentShaderID);

	m_programID = glCreateProgram();

	glAttachShader(m_programID, m_vertexShaderID);
	glAttachShader(m_programID, m_fragmentShaderID);

	glLinkProgram(m_programID);

	
	

	checkProgramStatus(m_programID);

	glUseProgram(m_programID);

}

std::string  Shader::readShaderCode(const char* fileName)
{
	std::ifstream meInput(fileName);

	if (!meInput.good())
	{
		std::cout << "File failed to load..." << fileName << std::endl;
		exit(1);
	}

	return std::string(std::istreambuf_iterator<char>(meInput), std::istreambuf_iterator<char>());
}

bool Shader::checkShaderStatus(GLuint shaderID)
{
	
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &m_compileStatus);
	if (m_compileStatus == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar * buffer = new GLchar[infoLogLength];
		GLsizei bufferSize;
		glGetShaderInfoLog(shaderID, infoLogLength, &bufferSize, buffer);
		std::cout << "COMPILE FAIL :" << buffer << std::endl;
		return false;
		delete[] buffer;
	}
	else
	{
		std::cout << "COMPILE WIN" << std::endl;
		return true;
	}

}

bool Shader::checkProgramStatus(GLuint programID)
{
	
	glGetProgramiv(programID, GL_LINK_STATUS, &m_linkStatus);
	if (m_linkStatus == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar * buffer = new GLchar[infoLogLength];
		GLsizei bufferSize;
		glGetProgramInfoLog(programID, infoLogLength, &bufferSize, buffer);
		std::cout << "LINK FAIL :" << buffer << std::endl;
		return false;
		delete[] buffer;
	}
	else
	{
		std::cout << "LINK WIN" << std::endl;
		return true;
	}

}

GLuint Shader::getProgramID()
{
	return m_programID;
}