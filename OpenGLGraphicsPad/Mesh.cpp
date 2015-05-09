#include "Mesh.h"
#include<iostream>
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


Mesh::Mesh() :m_numIndices(0), m_numVertices(0), m_numNormals(0)
{
	glGenBuffers(1, &m_vertexBufferID);
	glGenBuffers(1, &m_indexBufferID);
	glGenBuffers(1, &m_normalBufferID);

}


Mesh::~Mesh()
{
}


void Mesh::addVertices(std::vector<glm::vec3> vertices, std::vector<GLushort> indices, std::vector<glm::vec3> normals)
{



	
	m_numVertices = vertices.size();

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, m_numVertices * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	m_numNormals = normals.size();

	glBindBuffer(GL_ARRAY_BUFFER, m_normalBufferID);
	glBufferData(GL_ARRAY_BUFFER, m_numNormals*sizeof(Vertex), &normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);





	m_numIndices = indices.size();
	//m_numIndices = 3;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_numIndices*sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
}


void Mesh::addVertices(std::vector<glm::vec3> vertices, std::vector<GLushort> indices)
{

	m_numVertices = vertices.size();

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, m_numVertices * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	

	m_numIndices = indices.size();
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_numIndices*sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
	

}

GLsizei Mesh::getIndices()
{
	return m_numIndices * sizeof(GLushort);
}
