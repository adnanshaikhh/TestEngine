#include "ResourceLoader.h"
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

ResourceLoader::ResourceLoader()
{
}


ResourceLoader::~ResourceLoader()
{
}

void ResourceLoader::load_obj(const char* filename, std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &normals, std::vector<GLushort> &elements)
{
	ifstream in(filename, ios::in);
	if (!in)
	{
		std::cout << "Cannot open " << filename << endl; 
	}

	string line;
	while (getline(in, line))
	{
		if (line.substr(0, 2) == "v ")
		{
			istringstream s(line.substr(2));
			glm::vec3 v;
			s >> v.x;
			s >> v.y;
			s >> v.z;
			//v.w = 1.0f;
			vertices.push_back(v);
		}
		else if (line.substr(0, 2) == "f ")
		{
			//for (int l = 0; l < line.size(); l++)
			//{
			//	if (line[l] == '/') line[l] = ' ';
			//}
			
			istringstream s(line.substr(2));
			
			GLushort a, b, c;
			//GLushort tempa1, tempa2, tempb1, tempb2;
			
			s >> a;
			//s >> tempa1;
			//s >> tempa2;

			s >> b;
			//s >> tempb1;
			//s >> tempb2;

			s >> c;
			
			a--;
			b--;
			c--;
		//	cout << a << " " << b << " " << c << endl;
			elements.push_back(a); elements.push_back(b); elements.push_back(c);
		}
		else if (line[0] == '#')
		{
			/* ignoring this line */
		}
		else
		{
			/* ignoring this line */
		}
	}

	normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
	
	for (int i = 0; i < elements.size(); i += 3)
	{
		GLushort ia = elements[i];
		GLushort ib = elements[i + 1];
		GLushort ic = elements[i + 2];
		glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
		normals[ia] = normals[ib] = normals[ic] = normal;

	}


}
