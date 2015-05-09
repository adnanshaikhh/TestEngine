#pragma once

#include"btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"



#ifdef DYNAMIC_CHARACTER_CONTROLLER
#include "DynamicCharacterController.h"
#else
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#endif


#include"Camera.h"
#include"BspLoader.h"


class Physics
{
public:
	Physics();
	~Physics();

	void StepBulletPhysics();
	void InitBulletPhysics();
	void ExitBulletPhysics();
	void addCube(Camera * camera);
	btRigidBody* localCreateRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape);
	void addConvexVerticesCollider(btAlignedObjectArray<btVector3>& vertices, bool isEntity, const btVector3& entityTargetLocation);
	void convertBsp(BspLoader& bspLoader, float scaling);


#ifdef DYNAMIC_CHARACTER_CONTROLLER

	btCharacterControllerInterface* m_character;

#else

	btKinematicCharacterController* m_character;
	btPairCachingGhostObject* m_ghostObject;

#endif


	btAlignedObjectArray<btCollisionShape*>	m_collisionShapes; //keep the collision shapes, for deletion/cleanup
	btBroadphaseInterface*					m_broadphase;
	btCollisionDispatcher*					m_dispatcher;
	btConstraintSolver*						m_solver;
	btDefaultCollisionConfiguration*		m_collisionConfiguration;
	btDynamicsWorld*						m_dynamicsWorld; //this is the most important class

	btScalar		m_defaultContactProcessingThreshold;

	BspLoader bspLoader;
};
