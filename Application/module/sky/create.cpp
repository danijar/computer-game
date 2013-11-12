#include "module.h"

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glew/glew.h>
#include <sfml/Graphics.hpp>
using namespace std;
using namespace glm;
using namespace sf;

#include "model.h"
#include "form.h"


Model ModuleSky::CreateSky()
{
	Model model;

	Mesh mesh = CreateMesh();
	model.Positions = mesh.Positions;
	model.Normals   = mesh.Normals;
	model.Texcoords = mesh.Texcoords;
	model.Elements  = mesh.Elements;

	GLuint texture = TextureLoad("sky.jpg");
	model.Diffuse = texture;

	return model;
}

ModuleSky::Mesh ModuleSky::CreateMesh(int Steps, float Radius)
{
	vector<GLfloat> vertices, normals, texcoords;
	vector<GLuint> elements;

	vec3 vertex, normal; vec2 texcoord;
	int current = 0;

	// for each latitude
	const float Step = 2.0f / Steps;
	const float Tolerance = Step / 2;
	const int Row = Steps + 1;
	for(float latitude = -1; latitude < 1 + Step + Tolerance; latitude += Step)
	{
		// create circle of longitudes
		float radius = sqrt(1 - pow(latitude, 2)) * Radius;
		for(float longitude = -1; longitude < 1 + Step + Tolerance; longitude += Step) // use range from 0 to 1 instead
		{
			float longitude_rad = (longitude / 2 + 0.5f) * 2 * glm::pi<float>();

			vertex.x = sin(longitude_rad) * radius;
			vertex.y = latitude * Radius;
			vertex.z = cos(longitude_rad) * radius;

			normal = normalize(-vertex);

			texcoord.x = longitude / 2 + 0.5f;
			texcoord.y = latitude  / 2 + 0.5f;

			vertices .push_back(vertex  .x);
			vertices .push_back(vertex  .y);
			vertices .push_back(vertex  .z);
			normals  .push_back(normal  .x);
			normals  .push_back(normal  .y);
			normals  .push_back(normal  .z);
			texcoords.push_back(texcoord.x);
			texcoords.push_back(texcoord.y);

			if(longitude > 1 - Tolerance) {}          // last longitude
			else if(latitude < -1 + Tolerance) {}     // southpole
			else if(latitude < -1 + Step + Tolerance) // most south latitude
			{
				elements.push_back(current);
				elements.push_back(current + 1);
				elements.push_back(current - Row);
			}
			else if(latitude > 1 - Tolerance)         // northpole
			{
				elements.push_back(current - Row + 1);
				elements.push_back(current - Row);
				elements.push_back(current + 1);
			}
			else                                      // all other latitudes
			{
				elements.push_back(current);
				elements.push_back(current + 1);
				elements.push_back(current - Row);
				elements.push_back(current - Row + 1);
				elements.push_back(current - Row);
				elements.push_back(current + 1);
			}
			current++;
		}
	}

	// create buffers
	Mesh mesh;
	glGenBuffers(1, &mesh.Positions);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.Positions);
	glBufferData(GL_ARRAY_BUFFER,         sizeof(GLfloat) * vertices.size(),  &vertices[0],  GL_STATIC_DRAW);
	glGenBuffers(1, &mesh.Normals);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.Normals);
	glBufferData(GL_ARRAY_BUFFER,         sizeof(GLfloat) * normals.size(),   &normals[0],   GL_STATIC_DRAW);
	glGenBuffers(1, &mesh.Texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.Texcoords);
	glBufferData(GL_ARRAY_BUFFER,         sizeof(GLfloat) * texcoords.size(), &texcoords[0], GL_STATIC_DRAW);
	glGenBuffers(1, &mesh.Elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.Elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)  * elements.size(),  &elements[0],  GL_STATIC_DRAW);

	return mesh;
}

GLuint ModuleSky::TextureLoad(string Path)
{
	Image image;
	bool result = image.loadFromFile("module/" + Name() + "/texture/" + Path);
	if(!result)
	{
		Debug->Fail("texture " + Path + " cannot be loaded");
		return 0;
	}
	image.flipVertically();

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // use GL_CLAMP_TO_BORDER to detect issue in texture coordinates
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}
