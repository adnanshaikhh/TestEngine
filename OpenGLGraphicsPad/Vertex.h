#pragma once
#include<glm.hpp>
#include<vector>

//const int SIZE = 3;

struct Vertex
{
	glm::vec3 postion;
};



struct POLYGON
{
	std::vector<glm::vec3> vertices;
	std::vector<GLushort> elements;
	std::vector<glm::vec3> normals;
};


