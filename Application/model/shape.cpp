#include "module.h"

#include <BULLET/btBulletDynamicsCommon.h>
#include <ASSIMP/cimport.h>
#include <ASSIMP/scene.h>
#include <ASSIMP/postprocess.h>
using namespace std;


btRigidBody *ModuleModel::CreateBody(string Path, float Mass)
{
	btCollisionShape *shape = GetShape(Path, (Mass == 0));
	btRigidBody *body = new btRigidBody(Mass, new btDefaultMotionState(), shape);

	if(Mass == 0)
	{
		body->setMassProps(0, btVector3(0, 0, 0));
	}
	else
	{
		btVector3 inertia;
		shape->calculateLocalInertia(Mass, inertia);
		body->setMassProps(Mass, inertia);
	}

	return body;
}

btCollisionShape *ModuleModel::GetShape(string Path, bool Static)
{
	if(Static)
	{
		auto i = staticshapes.find(Path);
		if(i != staticshapes.end()) return i->second;
	}
	else
	{
		auto i = dynamicshapes.find(Path);
		if(i != dynamicshapes.end()) return i->second;
	}

	btCollisionShape *shape = NULL;
	LoadShape(shape, Path, Static);

	if(Static) staticshapes.insert(make_pair(Path, shape));
	else      dynamicshapes.insert(make_pair(Path, shape));

	return shape;
}

void ModuleModel::LoadShape(btCollisionShape *&Shape, string Path, bool Static)
{
	delete Shape;
	Shape = NULL;

	if(Path == "qube.prim")  return LoadShapeCube (Shape);
	if(Path == "plane.prim") return LoadShapePlane(Shape);

	if(Static)
	{
		btTriangleMesh* triangles = new btTriangleMesh();

		const aiScene *scene = aiImportFile((Name() + "/mesh/" + Path).c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
		if(!scene)
		{
			Debug->Fail("mesh " + Path + " cannot be loaded for collision");
			return;
		}

		for(unsigned int i = 0; i < scene->mNumMeshes; ++i)
		{
			const aiMesh* mesh = scene->mMeshes[i];

			vector<vector<int>> faces;
			for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
			{
				vector<int> face;
				for(int j = 0; j < 3; ++j)
				{
					face.push_back(mesh->mFaces[i].mIndices[j]);
				}
				faces.push_back(face);
			}

			if(mesh->HasPositions())
			{
				for(auto face : faces)
				{
					vector<btVector3> triangle;
					for(auto index : face)
					{
						aiVector3D vertex = mesh->mVertices[index];
						triangle.push_back(btVector3(vertex.x, vertex.y, vertex.z));
					}
					triangles->addTriangle(triangle[0], triangle[1], triangle[2]);
				}
			}
		}

		Debug->Pass("collision shape with " + to_string(triangles->getNumTriangles()) + " triangles");

		Shape = new btBvhTriangleMeshShape(triangles, true, true);
	}
	else
	{

	}
}

void ModuleModel::LoadShapeCube(btCollisionShape *&Shape)
{
	Shape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
}

void ModuleModel::LoadShapePlane(btCollisionShape *&Shape)
{
	Shape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
}
