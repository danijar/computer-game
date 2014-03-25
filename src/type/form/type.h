#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <bullet/btBulletDynamicsCommon.h>

struct Form
{
	Form()
	{
		btCollisionShape *shape = new btEmptyShape();
		shape->setLocalScaling(btVector3(1, 1, 1));
		Body = new btRigidBody(0, new btDefaultMotionState(), shape, btVector3(0, 0, 0));
	}
	~Form()
	{
		if(Form::World)
			if(Body->isInWorld())
				Form::World->removeRigidBody(Body);
		delete Body;
	}

	glm::vec3 Position()
	{
		btVector3 origin(Body->getWorldTransform().getOrigin());
		return glm::vec3(origin.getX(), origin.getY(), origin.getZ());
	}
	void Position(glm::vec3 Coordinates)
	{
		Body->activate();
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
		Body->activate();
		btTransform transform(Body->getWorldTransform());
		transform.setRotation(btQuaternion(Angles.y, Angles.x, Angles.z));
		Body->setWorldTransform(transform);
	}
	glm::quat Quaternion()
	{
		btQuaternion rotation(Body->getWorldTransform().getRotation());
		return glm::quat(rotation.getW(), rotation.getX(), rotation.getY(), rotation.getZ());
	}
	void Quaternion(glm::quat Rotation)
	{
		Body->activate();
		btTransform transform(Body->getWorldTransform());
		transform.setRotation(btQuaternion(Rotation.x, Rotation.y, Rotation.z, Rotation.w));
		Body->setWorldTransform(transform);
	}
	glm::vec3 Scale()
	{
		btVector3 factors(Body->getCollisionShape()->getLocalScaling());
		return glm::vec3(factors.getX(), factors.getY(), factors.getZ());
	}
	void Scale(glm::vec3 Factors)
	{
		if(Form::World) Form::World->removeRigidBody(Body);
		Body->getCollisionShape()->setLocalScaling(btVector3(Factors.x, Factors.y, Factors.z));
		if(Form::World) Form::World->addRigidBody(Body);
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
	static btDynamicsWorld *World;
};
