#pragma once
#include<iostream>
#include<vector>
#include<gl\glew.h>
#include<glm.hpp>

class ResourceLoader
{
public:
	ResourceLoader();
	~ResourceLoader();
	static void load_obj(const char* filename, std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &normals, std::vector<GLushort> &elements);
	
};

