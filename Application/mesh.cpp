#pragma once

#include "system.h"
#include "debug.h"

#include <vector>
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

	void Load(unsigned int Id)
	{
		auto msh = Entity->Get<StorageMesh>(Id);

		Lib3dsFile* model = lib3ds_file_open((Name() + "/" + msh->Path).c_str());
		if(model == false)
		{
			Debug::Fail("Mesh (" + msh->Path + ") cannot be loaded.");
			return;
		}

		vector<GLfloat> positions, normals, texcoords;
		vector<GLuint> elements;

		GLuint element = 0;
		for (int i = 0; i < model->nmeshes; ++i)
		{
			Lib3dsMesh *mesh = model->meshes[i];
			for(unsigned int j = 0; j < mesh->nfaces; ++j)
			{
				Lib3dsFace *face = &mesh->faces[j];
				for(int k = 0; k < 3; ++k)
				{
					positions.push_back(mesh->vertices[face->index[k]][0]);
					positions.push_back(mesh->vertices[face->index[k]][1]);
					positions.push_back(mesh->vertices[face->index[k]][2]);
					texcoords.push_back(mesh->texcos[face->index[k]][0]);
					texcoords.push_back(mesh->texcos[face->index[k]][1]);
					elements.push_back(element++);
				}
			}
			int last = normals.size();
			normals.resize(positions.size());
			lib3ds_mesh_calculate_vertex_normals(mesh, (float(*)[3])(&normals[last]));
		}

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
