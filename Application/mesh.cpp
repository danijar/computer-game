#pragma once

#include "system.h"
#include "debug.h"

#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;
#include "GLEW/glew.h"
#include <LIB3DS/lib3ds.h>

#include "mesh.h"


class ModuleMesh : public Module
{
	struct Vertices
	{
		vector<GLfloat> Positions, Normals, Texcoords;
		vector<GLuint> Elements;
	};

	void Init()
	{
		Listeners();
	}

	void Update()
	{
		auto mes = Entity->Get<StorageMesh>();
		for(auto i = mes.begin(); i != mes.end(); ++i)
		{
			if(i->second->Changed)
			{
				if     (i->second->Path == "PrimitiveQube" ) PrimitiveQube(i->first);
				else if(i->second->Path == "PrimitivePlane") PrimitivePlane(i->first);
				else Load(i->first);
				i->second->Changed = false;
			}
		}
	}

	void Listeners()
	{
		Event->Listen("WindowFocusGained", [=]{
			auto mes = Entity->Get<StorageMesh>();
			int Count = 0;
			for(auto i = mes.begin(); i != mes.end(); ++i)
			{
				bool Changed = true; // check if the file actually changed
				if(Changed)
				{
					i->second->Changed = true;
					Count++;
				}
			}
			if(Count > 0)
			{
				Debug::Info("Meshes reloaded " + to_string(Count));
			}
		});
	}

	string Extension(string Path)
	{
		return Path.substr(Path.find_last_of(".") + 1);
	}

	void Load(unsigned int Id)
	{
		auto msh = Entity->Get<StorageMesh>(Id);

		vector<GLfloat> positions, normals, texcoords;
		vector<GLuint> elements;

		string path = Name() + "/" + msh->Path;
		string extension = Extension(msh->Path);

		if(extension == "") return;
		else if(extension == "3ds") Parse3ds(path, positions, normals, texcoords, elements);
		else if(extension == "obj") ParseObj(path, positions, normals, texcoords, elements);
		else Debug::Fail("Mesh " + msh->Path + " format is not supported");

		glBindBuffer(GL_ARRAY_BUFFER, msh->Positions);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(GLfloat), &positions[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, msh->Normals);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, msh->Texcoords);
		glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(GLfloat), &texcoords[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, msh->Elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLuint), &elements[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void Parse3ds(string Path, vector<GLfloat> &Positions, vector<GLfloat> &Normals, vector<GLfloat> &Texcoords, vector<GLuint> &Elements)
	{
		Lib3dsFile* model = lib3ds_file_open(Path.c_str());
		if(model == false)
		{
			Debug::Fail("Mesh (" + Path + ") cannot be loaded.");
			return;
		}

		GLuint element = 0;
		for (int i = 0; i < model->nmeshes; ++i)
		{
			Lib3dsMesh *mesh = model->meshes[i];
			for(unsigned int j = 0; j < mesh->nfaces; ++j)
			{
				Lib3dsFace *face = &mesh->faces[j];
				for(int k = 0; k < 3; ++k)
				{
					Positions.push_back(mesh->vertices[face->index[k]][0]);
					Positions.push_back(mesh->vertices[face->index[k]][1]);
					Positions.push_back(mesh->vertices[face->index[k]][2]);
					Texcoords.push_back(mesh->texcos[face->index[k]][0]);
					Texcoords.push_back(mesh->texcos[face->index[k]][1]);
					Elements.push_back(element++);
				}
			}
			int last = Normals.size();
			Normals.resize(Positions.size());
			lib3ds_mesh_calculate_vertex_normals(mesh, (float(*)[3])(&Normals[last]));
		}
	}

	void ParseObj(string Path, vector<GLfloat> &Positions, vector<GLfloat> &Normals, vector<GLfloat> &Texcoords, vector<GLuint> &Elements)
	{
		ifstream stream(Path);
		if(!stream.is_open())
		{
			Debug::Fail("Mesh (" + Path + ") cannot be loaded.");
			return;
		}

		vector<GLfloat> positions, normals, texcoords;
		string line;
		while(getline(stream, line))
		{
			istringstream input(line);
			string key;
			input >> key;

			if(key == "") continue;
			else if(key == "v" )
			{
				float x, y, z;
				input >> x >> y >> z;
				positions.push_back(z);
				positions.push_back(y);
				positions.push_back(z);
			}
			else if(key == "vn")
			{
				float x, y, z;
				input >> x >> y >> z;
				normals.push_back(z);
				normals.push_back(y);
				normals.push_back(z);
			}
			else if(key == "vt")
			{
				float u, v;
				input >> u >> v;
				texcoords.push_back(u);
				texcoords.push_back(v);
			}
		}

		stream.clear();
		stream.seekg(ios::beg);

		while(getline(stream, line))
		{
			replace(line.begin(), line.end(), '/', ' ');
			istringstream input(line);
			string key;
			input >> key;
			if(key == "f")
			{
				int p, t, n;
				while(input >> p >> t >> n)
				{
					Positions.push_back(positions[3 * (p - 1) + 0]);
					Positions.push_back(positions[3 * (p - 1) + 1]);
					Positions.push_back(positions[3 * (p - 1) + 2]);
					Normals  .push_back(normals  [3 * (n - 1) + 0]);
					Normals  .push_back(normals  [3 * (n - 1) + 1]);
					Normals  .push_back(normals  [3 * (n - 1) + 2]);
					Texcoords.push_back(texcoords[2 * (t - 1) + 0]);
					Texcoords.push_back(texcoords[2 * (t - 1) + 1]);
					Elements.push_back(Elements.size());
				}
			}
		}


	}

	void PrimitiveQube(unsigned int Id)
	{
		auto msh = Entity->Get<StorageMesh>(Id);
		const GLfloat positions[] = {-1.,-1.,1.,1.,-1.,1.,1.,1.,1.,-1.,1.,1.,-1.,1.,1.,1.,1.,1.,1.,1.,-1.,-1.,1.,-1.,1.,-1.,-1.,-1.,-1.,-1.,-1.,1.,-1.,1.,1.,-1.,-1.,-1.,-1.,1.,-1.,-1.,1.,-1.,1.,-1.,-1.,1.,-1.,-1.,-1.,-1.,-1.,1.,-1.,1.,1.,-1.,1.,-1.,1.,-1.,1.,1.,-1.,-1.,1.,1.,-1.,1.,1.,1.};
		const GLfloat normals[]   = {0,0,1,0,0,1,0,0,1,0,0,1,0,1,0,0,1,0,0,1,0,0,1,0,0,0,-1,0,0,-1,0,0,-1,0,0,-1,0,-1,0,0,-1,0,0,-1,0,0,-1,0,-1,0,0,-1,0,0,-1,0,0,-1,0,0,1,0,0,1,0,0,1,0,0,1,0,0};
		const GLfloat texcoords[] = {0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.};
		const GLuint  elements[]  = {0,1,2,2,3,0,4,5,6,6,7,4,8,9,10,10,11,8,12,13,14,14,15,12,16,17,18,18,19,16,20,21,22,22,23,20};
		glBindBuffer(GL_ARRAY_BUFFER, msh->Positions);
		glBufferData(GL_ARRAY_BUFFER, 72 * sizeof(GLfloat), &positions[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, msh->Normals);
		glBufferData(GL_ARRAY_BUFFER, 72 * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, msh->Texcoords);
		glBufferData(GL_ARRAY_BUFFER, 48 * sizeof(GLfloat), &texcoords[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, msh->Elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), &elements[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void PrimitivePlane(unsigned int Id)
	{
		auto msh = Entity->Get<StorageMesh>(Id);
		const float l = 100;
		const GLfloat positions[] = { -l,0,-l, -l,0,l, l,0,l, l,0,-l };
		const GLfloat normals[]   = { 0,1,0, 0,1,0., 0,1,0, 0,1,0 };
		const GLfloat texcoords[] = { 0,0, l/2,0, l/2,l/2, 0,l/2 };
		const GLuint  elements[]  = { 0,1,2, 2,3,0 };
		glBindBuffer(GL_ARRAY_BUFFER, msh->Positions);
		glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), &positions[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, msh->Normals);
		glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, msh->Texcoords);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), &texcoords[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, msh->Elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), &elements[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
};
