#pragma once

#include <BULLET/btBulletDynamicsCommon.h>


struct StoragePhysic
{
	StoragePhysic() : InWorld(false) {}
	btRigidBody *Body;
	bool InWorld;
};
