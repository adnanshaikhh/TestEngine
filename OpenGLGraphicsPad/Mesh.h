#pragma once
#include<gl\glew.h>
#include"Vertex.h"
#include<vector>

class Mesh
{
public:
	Mesh();
	~Mesh();
	void addVertices(std::vector<glm::vec3> vertices, std::vector<GLushort> indices);
	void addVertices(std::vector<glm::vec3> vertices, std::vector<GLushort> indices, std::vector<glm::vec3> normals);
	GLsizei getIndices();


private:
	GLuint m_vertexBufferID;
	GLuint m_indexBufferID;
	GLuint m_normalBufferID;
	GLuint m_numVertices;
	GLuint m_numIndices;
	GLuint m_numNormals;
};

