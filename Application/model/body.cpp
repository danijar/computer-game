#include "module.h"

#include <GLM/glm.hpp>
#include <BULLET/btBulletDynamicsCommon.h>
#include <ASSIMP/cimport.h>
#include <ASSIMP/scene.h>
#include <ASSIMP/postprocess.h>
using namespace std;
using namespace glm;


btRigidBody *ModuleModel::CreateBody(string Path, vec3 Scale, float Mass)
{
	btCollisionShape *shape = GetShape(Path, Scale, (Mass == 0));
	btRigidBody *body = new btRigidBody(Mass, new btDefaultMotionState(), shape);

	if(Mass)
	{
		btVector3 inertia;
		shape->calculateLocalInertia(Mass, inertia);
		body->setMassProps(Mass, inertia);
	}
	else
	{
		body->setMassProps(0, btVector3(0, 0, 0));
	}

	return body;
}

btCollisionShape *ModuleModel::GetShape(string Path, vec3 Scale, bool Static) // later on deep copy shapes to allow different scaling
{
	shape_key key = make_tuple(Path, Scale, Static);
	auto i = shapes.find(key);
	if(i != shapes.end()) return i->second.first;

	btCollisionShape *shape = NULL;
	LoadShape(shape, Path, Scale, Static);

	shapes.insert(make_pair(key, make_pair(shape, Hash(Name() + "/mesh/" + Path))));

	return shape;
}

void ModuleModel::ReloadShapes()
{

}

void ModuleModel::LoadShape(btCollisionShape *&Shape, string Path, vec3 Scale, bool Static)
{
	delete Shape;
	Shape = NULL;

	if(Path == "qube.prim")  return LoadShapeCube (Shape, Scale);
	if(Path == "plane.prim") return LoadShapePlane(Shape);

	if(Static)
	{
		btTriangleMesh *triangles = new btTriangleMesh();

		const aiScene *scene = aiImportFile((Name() + "/mesh/" + Path).c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
		if(!scene)
		{
			Debug->Fail("mesh " + Path + " cannot be loaded for collision");
			return;
		}

		for(unsigned int i = 0; i < scene->mNumMeshes; ++i)
		{
			const aiMesh *mesh = scene->mMeshes[i];

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

	Shape->setLocalScaling(btVector3(Scale.x, Scale.y, Scale.z));
}

void ModuleModel::LoadShapeCube(btCollisionShape *&Shape, vec3 Scale)
{
	Shape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
	Shape->setLocalScaling(btVector3(Scale.x, Scale.y, Scale.z));
}

void ModuleModel::LoadShapePlane(btCollisionShape *&Shape)
{
	Shape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
}
