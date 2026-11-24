#include "ObjModel.h"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

using namespace std;
using namespace glm;

ObjModel::ObjModel(const string &path, GLuint texture) : texture(texture), model(1.0f)
{
	vector<vec3> positions;
	vector<vec2> uvs;
	vector<float> data; // for every vertex of every triangle: x y z  u v

	ifstream file(path);
	if (!file.is_open())
		cout << "Failed to open model: " << path << endl;

	string line;
	while (getline(file, line))
	{
		istringstream words(line);
		string type;
		words >> type;

		if (type == "v")
		{
			vec3 p;
			words >> p.x >> p.y >> p.z;
			positions.push_back(p);
		}
		else if (type == "vt")
		{
			vec2 t;
			words >> t.x >> t.y;
			uvs.push_back(t);
		}
		else if (type == "f") // a face: every corner looks like  position/uv/normal  (1-based numbers; the normal is not used)
		{
			vector<int> corner[3];
			string word;
			while (words >> word)
			{
				int a = 0, b = 0, c = 0;
				sscanf(word.c_str(), "%d/%d/%d", &a, &b, &c);
				corner[0].push_back(a);
				corner[1].push_back(b);
				corner[2].push_back(c);
			}
			// our model only has triangles, so every face has 3 corners
			for (int k = 0; k < 3 && k < corner[0].size(); k++)
			{
				vec3 p = positions[corner[0][k] - 1];
				vec2 t = uvs[corner[1][k] - 1];
				float v[5] = {p.x, p.y, p.z, t.x, t.y};
				for (int i = 0; i < 5; i++)
					data.push_back(v[i]);
			}
		}
	}
	vertexCount = data.size() / 5;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void ObjModel::draw(Shader &shader)
{
	shader.setMat4("model", model);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}
