#include "Physics.h"

#include "LinearMath/btVector3.h"
#include "LinearMath/btAlignedObjectArray.h"
#include "LinearMath/btGeometryUtil.h"
#include "LinearMath\btTransform.h"
#include "BulletCollision/CollisionShapes/btConvexHullShape.h"

#include <stdio.h>
#include <string.h>
#include"Camera.h"

#include"BspLoader.h"


Physics::Physics()
{
}


Physics::~Physics()
{
}

void Physics::StepBulletPhysics()
{
	if (m_dynamicsWorld)//step the simulation
		m_dynamicsWorld->stepSimulation(1.0f / 60.0f);
}

void Physics::InitBulletPhysics()
{



	m_broadphase = new btDbvtBroadphase();


	//start here
	btCollisionShape* groundShape = new btBoxShape(btVector3(50, 3, 50));
	m_collisionShapes.push_back(groundShape);

	m_collisionConfiguration = new btDefaultCollisionConfiguration(); //collision configuration contains default setup for memory, collision setup

	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration); //use the default collision dispatcher 

	btVector3 worldMin(-1000, -1000, -1000);
	btVector3 worldMax(1000, 1000, 1000);


	btAxisSweep3* sweepBP = new btAxisSweep3(worldMin, worldMax);
	m_broadphase = sweepBP;

	m_solver = new btSequentialImpulseConstraintSolver; //the default constraint solver

	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);

	m_dynamicsWorld->getDispatchInfo().m_allowedCcdPenetration = 0.0001f;



#ifdef DYNAMIC_CHARACTER_CONTROLLER
	m_character = new DynamicCharacterController();
#else
	btTransform startTransform;
	startTransform.setIdentity();
	//startTransform.setOrigin (btVector3(0.0, 4.0, 0.0));
	startTransform.setOrigin(btVector3(10.210098, -1.6433364, 16.453260));


	m_ghostObject = new btPairCachingGhostObject();
	m_ghostObject->setWorldTransform(startTransform);
	sweepBP->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	btScalar characterHeight = 1.75;
	btScalar characterWidth = 1.75;
	btConvexShape* capsule = new btCapsuleShape(characterWidth, characterHeight);
	m_ghostObject->setCollisionShape(capsule);
	m_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	btScalar stepHeight = btScalar(0.35);
	m_character = new btKinematicCharacterController(m_ghostObject, capsule, stepHeight);
#endif
	////////////////

	/// Create some basic environment from a Quake level


	m_dynamicsWorld->setGravity(btVector3(0, -10, 0));
	btTransform tr;
	tr.setIdentity();

	const char* filename = "BSPdemo.bsp";

	const char* prefix[] = { "./", "../", "../../", "../../../", "../../../../", "BspDemo/", "Demos/BspDemo/",
		"../Demos/BspDemo/", "../../Demos/BspDemo/" };

	int numPrefixes = sizeof(prefix) / sizeof(const char*);
	char relativeFileName[1024];
	FILE* file = 0;


	for (int i = 0; i<numPrefixes; i++)
	{
		sprintf(relativeFileName, "%s%s", prefix[i], filename);
		file = fopen(relativeFileName, "r");
		if (file)
			break;
	}
	void* memoryBuffer = 0;

	if (file)
	{

		int size = 0;
		if (fseek(file, 0, SEEK_END) || (size = ftell(file)) == EOF || fseek(file, 0, SEEK_SET)) {        /* File operations denied? ok, just close and return failure */
			printf("Error: cannot get filesize from %s\n", filename);
		}
		else
		{
			//how to detect file size?
			memoryBuffer = malloc(size + 1);
			fread(memoryBuffer, 1, size, file);
			bspLoader.loadBSPFile(memoryBuffer);

			//BspToBulletConverter bsp2bullet(this);

			//	BspConverter bsp2bullet;

			float bspScaling = 0.1f;

			convertBsp(bspLoader, bspScaling);

		}
		fclose(file);
	}

	///only collide with static for now (no interaction with dynamic objects)
	m_dynamicsWorld->addCollisionObject(m_ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);

	m_dynamicsWorld->addAction(m_character);


	///////////////


}

void Physics::ExitBulletPhysics()
{
	//cleanup in the reverse order of creation/initialization

	for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) //remove the rigidbodies from the dynamics world and delete them
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
			delete body->getMotionState();

		m_dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}


	for (int j = 0; j<m_collisionShapes.size(); j++) //delete collision shapes
	{
		btCollisionShape* shape = m_collisionShapes[j];
		delete shape;
	}

	m_collisionShapes.clear();
	delete m_dynamicsWorld;
	delete m_solver;
	delete m_broadphase;
	delete m_dispatcher;
	delete m_collisionConfiguration;
}

void Physics::addCube(Camera * camera)
{

	btCollisionShape* boxShape = new btBoxShape(btVector3(1.0, 1.0, 1.0));
	m_collisionShapes.push_back(boxShape);

	btScalar mass = 1.0f;
	btVector3 localInertia(0, 0, 0);

	boxShape->calculateLocalInertia(mass, localInertia);

	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(camera->camera_position.x, camera->camera_position.y, camera->camera_position.z - 2));



	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, boxShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);



	body->setLinearVelocity(btVector3(camera->camera_direction.x * 50, camera->camera_direction.y * 50, camera->camera_direction.z * 50));

	m_dynamicsWorld->addRigidBody(body);

}

void Physics::convertBsp(BspLoader& bspLoader, float scaling)
{
	{

		float	playstartf[3] = { 0, 0, 100 };

		if (bspLoader.findVectorByName(&playstartf[0], "info_player_start"))
		{
			printf("found playerstart\n");
		}
		else
		{
			if (bspLoader.findVectorByName(&playstartf[0], "info_player_deathmatch"))
			{
				printf("found deatchmatch start\n");
			}
		}

		btVector3 playerStart(playstartf[0], playstartf[1], playstartf[2]);


		playerStart[2] += 20.f; //start a bit higher

		playerStart *= scaling;



		//progressBegin("Loading bsp");

		for (int i = 0; i<bspLoader.m_numleafs; i++)
		{
			printf("Reading bspLeaf %i from total %i (%f procent)\n", i, bspLoader.m_numleafs, (100.f*(float)i / float(bspLoader.m_numleafs)));

			bool isValidBrush = false;

			BSPLeaf&	leaf = bspLoader.m_dleafs[i];

			for (int b = 0; b<leaf.numLeafBrushes; b++)
			{
				btAlignedObjectArray<btVector3> planeEquations;

				int brushid = bspLoader.m_dleafbrushes[leaf.firstLeafBrush + b];

				BSPBrush& brush = bspLoader.m_dbrushes[brushid];
				if (brush.shaderNum != -1)
				{
					if (bspLoader.m_dshaders[brush.shaderNum].contentFlags & BSPCONTENTS_SOLID)
					{
						brush.shaderNum = -1;

						for (int p = 0; p<brush.numSides; p++)
						{
							int sideid = brush.firstSide + p;
							BSPBrushSide& brushside = bspLoader.m_dbrushsides[sideid];
							int planeid = brushside.planeNum;
							BSPPlane& plane = bspLoader.m_dplanes[planeid];
							btVector3 planeEq;
							planeEq.setValue(
								plane.normal[0],
								plane.normal[1],
								plane.normal[2]);
							planeEq[3] = scaling*-plane.dist;

							planeEquations.push_back(planeEq);
							isValidBrush = true;
						}
						if (isValidBrush)
						{

							btAlignedObjectArray<btVector3>	vertices;
							btGeometryUtil::getVerticesFromPlaneEquations(planeEquations, vertices);

							bool isEntity = false;
							btVector3 entityTarget(0.f, 0.f, 0.f);

							addConvexVerticesCollider(vertices, isEntity, entityTarget);


						}
					}
				}
			}
		}

#define USE_ENTITIES
#ifdef USE_ENTITIES


		{
			int i;
			for (i = 0; i<bspLoader.m_num_entities; i++)
			{
				const BSPEntity& entity = bspLoader.m_entities[i];
				const char* cl = bspLoader.getValueForKey(&entity, "classname");
				if (!strcmp(cl, "trigger_push")) {
					btVector3 targetLocation(0.f, 0.f, 0.f);

					cl = bspLoader.getValueForKey(&entity, "target");
					if (strcmp(cl, "")) {
						//its not empty so ...

						/*
						//lookup the target position for the jumppad:
						const BSPEntity* targetentity = bspLoader.getEntityByValue( "targetname" , cl );
						if (targetentity)
						{
						if (bspLoader.getVectorForKey( targetentity , "origin",&targetLocation[0]))
						{

						}
						}
						*/


						cl = bspLoader.getValueForKey(&entity, "model");
						if (strcmp(cl, "")) {
							// add the model as a brush
							if (cl[0] == '*')
							{
								int modelnr = atoi(&cl[1]);
								if ((modelnr >= 0) && (modelnr < bspLoader.m_nummodels))
								{
									const BSPModel& model = bspLoader.m_dmodels[modelnr];
									for (int n = 0; n<model.numBrushes; n++)
									{
										btAlignedObjectArray<btVector3> planeEquations;
										bool	isValidBrush = false;

										//convert brush
										const BSPBrush& brush = bspLoader.m_dbrushes[model.firstBrush + n];
										{
											for (int p = 0; p<brush.numSides; p++)
											{
												int sideid = brush.firstSide + p;
												BSPBrushSide& brushside = bspLoader.m_dbrushsides[sideid];
												int planeid = brushside.planeNum;
												BSPPlane& plane = bspLoader.m_dplanes[planeid];
												btVector3 planeEq;
												planeEq.setValue(
													plane.normal[0],
													plane.normal[1],
													plane.normal[2]);
												planeEq[3] = scaling*-plane.dist;
												planeEquations.push_back(planeEq);
												isValidBrush = true;
											}
											if (isValidBrush)
											{

												btAlignedObjectArray<btVector3>	vertices;
												btGeometryUtil::getVerticesFromPlaneEquations(planeEquations, vertices);

												bool isEntity = true;
												addConvexVerticesCollider(vertices, isEntity, targetLocation);

											}
										}

									}
								}
							}
							else
							{
								printf("unsupported trigger_push model, md3 ?\n");
							}
						}

					}
				}
			}
		}

#endif //USE_ENTITIES



		//progressEnd();
	}

}

void Physics::addConvexVerticesCollider(btAlignedObjectArray<btVector3>& vertices, bool isEntity, const btVector3& entityTargetLocation)
{
	///perhaps we can do something special with entities (isEntity)
	///like adding a collision Triggering (as example)

	if (vertices.size() > 0)
	{
		float mass = 0.f;
		btTransform startTransform;
		//can use a shift
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(0, -10.0f, 0.0f));
		//this create an internal copy of the vertices
		for (int i = 0; i < vertices.size(); i++)
		{
			vertices[i] *= btScalar(0.5);
			float t = vertices[i].getZ() * btScalar(0.75);
			vertices[i].setZ(-vertices[i].getY());
			vertices[i].setY(t);
		}

		btCollisionShape* shape = new btConvexHullShape(&(vertices[0].getX()), vertices.size());
		m_collisionShapes.push_back(shape);

		//btRigidBody* body = m_demoApp->localCreateRigidBody(mass, startTransform,shape);
		localCreateRigidBody(mass, startTransform, shape);
	}
}

btRigidBody* Physics::localCreateRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape)
{
	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

#define USE_MOTIONSTATE 1
#ifdef USE_MOTIONSTATE
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

	btRigidBody* body = new btRigidBody(cInfo);
	body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);

#else
	btRigidBody* body = new btRigidBody(mass, 0, shape, localInertia);
	body->setWorldTransform(startTransform);
#endif//

	m_dynamicsWorld->addRigidBody(body);

	return body;
}



