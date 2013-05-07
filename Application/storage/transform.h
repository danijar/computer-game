#pragma once

#include <GLM/glm.hpp>
#include <GLM/gtc/quaternion.hpp>
#include <GLM/gtx/quaternion.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <BULLET/btBulletDynamicsCommon.h>


struct StorageTransform
{
	StorageTransform() : Added(false)
	{
		Body = new btRigidBody(0, new btDefaultMotionState(), new btEmptyShape(), btVector3(0, 0, 0));
	}
	glm::vec3 Position()
	{
		btVector3 origin(Body->getWorldTransform().getOrigin());
		return glm::vec3(origin.getX(), origin.getY(), origin.getZ());
	}
	void Position(glm::vec3 Coordinates)
	{
		btTransform transform(Body->getWorldTransform());
		transform.setOrigin(btVector3(Coordinates.x, Coordinates.y, Coordinates.z));
		Body->setWorldTransform(transform);
	}
	glm::vec3 Rotation()
	{
		btQuaternion rotation(Body->getWorldTransform().getRotation());
		return glm::radians(glm::eulerAngles(glm::quat(rotation.getW(), rotation.getX(), rotation.getY(), rotation.getZ())));
	}
	void Rotation(glm::vec3 Angles)
	{
		btTransform transform(Body->getWorldTransform());
		transform.setRotation(btQuaternion(Angles.y, Angles.x, Angles.z));
		Body->setWorldTransform(transform);
	}
	glm::quat Quaternion()
	{
		btQuaternion rotation(Body->getWorldTransform().getRotation());
		return glm::quat(rotation.getW(), rotation.getX(), rotation.getY(), rotation.getZ());
	}
	glm::vec3 Scale()
	{
		btVector3 factors(Body->getCollisionShape()->getLocalScaling());
		return glm::vec3(factors.getX(), factors.getY(), factors.getZ());
	}
	void Scale(glm::vec3 Factors)
	{
		Body->getCollisionShape()->setLocalScaling(btVector3(Factors.x, Factors.y, Factors.z));
	}
	glm::vec3 Direction()
	{
		btVector3 direction = quatRotate(Body->getWorldTransform().getRotation(), btVector3(0, 0, 1));
		return glm::vec3(direction.getX(), direction.getY(), direction.getZ());
	}
	glm::mat4 Matrix()
	{
		btQuaternion rotation(Body->getWorldTransform().getRotation());
		glm::quat quaternion(rotation.getW(), rotation.getX(), rotation.getY(), rotation.getZ());
		return glm::translate(glm::mat4(1), Position()) * glm::toMat4(quaternion) * glm::scale(glm::mat4(1), Scale());
	}
	btRigidBody *Body;
	bool Added;
};
