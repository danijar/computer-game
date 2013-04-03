#pragma once

#include "system.h"

#include <vector>
#include <algorithm>
using namespace std;
#include <GLEW/glew.h>
#include <ASSIMP/cimport.h>
#include <ASSIMP/scene.h>
#include <ASSIMP/postprocess.h>

#include "mesh.h"


class ModuleMesh : public Module
{
	void Init()
	{
		Listeners();
	}

	void Update()
	{
		auto mes = Entity->Get<StorageMesh>();
		int Count = 0;
		for(auto i = mes.begin(); i != mes.end(); ++i)
		{
			if(i->second->Changed)
			{
				if     (i->second->Path == "PrimitiveQube" ) PrimitiveQube(i->first);
				else if(i->second->Path == "PrimitivePlane") PrimitivePlane(i->first);
				else Load(i->first);
				i->second->Changed = false;
				Count++;
			}
		}
		if(Count > 0)
		{
			Debug->Print("Meshes reloaded " + to_string(Count));
		}
	}

	void Listeners()
	{
		Event->Listen("WindowFocusGained", [=]{
			auto mes = Entity->Get<StorageMesh>();
			for(auto i = mes.begin(); i != mes.end(); ++i)
			{
				i->second->Changed = true; // check if the file actually changed
			}
		});
	}

	void Load(unsigned int Id)
	{
		auto msh = Entity->Get<StorageMesh>(Id);

		string path = "model/" + Name() + "/" + msh->Path;
		const aiScene *scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
		if(!scene)
		{
			Debug->Fail("Mesh (" + msh->Path + ") cannot be loaded.");
			return;
		}

		for(unsigned int i = 0; i < scene->mNumMeshes; ++i)
		{
			const aiMesh* mesh = scene->mMeshes[i];

			vector<GLuint> faces;
			for (unsigned int t = 0; t < mesh->mNumFaces; ++t)
			{
				faces.push_back(mesh->mFaces[t].mIndices[0]);
				faces.push_back(mesh->mFaces[t].mIndices[1]);
				faces.push_back(mesh->mFaces[t].mIndices[2]);
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, msh->Elements);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof GLuint, &faces[0], GL_STATIC_DRAW);

			if(mesh->HasPositions())
			{
				glBindBuffer(GL_ARRAY_BUFFER, msh->Positions);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);
			}

			if(mesh->HasNormals())
			{
				glBindBuffer(GL_ARRAY_BUFFER, msh->Normals);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mNormals, GL_STATIC_DRAW);
			}

			if(mesh->HasTextureCoords(0))
			{
				vector<GLfloat> texcoords;
				for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
				{
					texcoords.push_back(mesh->mTextureCoords[0][i].x);
					texcoords.push_back(mesh->mTextureCoords[0][i].y);
				}
				glBindBuffer(GL_ARRAY_BUFFER, msh->Texcoords);
				glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof GLfloat, &texcoords[0], GL_STATIC_DRAW);
			}
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
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
