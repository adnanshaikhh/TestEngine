#include<iostream>

#include<string>
#include<math.h>
#include<ctime>
#include<vector>

#include<GL\glew.h>
#include<GL\freeglut.h>
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtx\quaternion.hpp>

#include"Vertex.h"
#include"Mesh.h"
#include"ResourceLoader.h"
#include"Camera.h"
#include"Shader.h"

#include"Physics.h"



float previousTime = 0;
float frameCount = 0;
float fps;


//time related
std::clock_t g_PreviousTicks;
std::clock_t g_CurrentTicks;

float fDeltaTime;

//Gloval General Variables
GLsizei numIndices;
int g_W = 0, g_A = 0, g_S = 0, g_D = 0, g_Q = 0, g_E = 0;
bool g_bShift = false;

//Objects
Camera camera;
Camera prevCamera;
Shader phongShader;


//Camera Related
int g_iWindowWidth = 800;
int g_iWindowHeight = 600;

std::vector<glm::vec3> vertices;
std::vector<glm::vec3> normals;
std::vector<GLushort> elements;



POLYGON polygon;


Physics phy;



//Function Declarations

void sendDataToOpenGL();
void installShaders();
std::string  readShaderCode(const char*);
bool checkShaderStatus(GLuint);
bool checkProgramStatus(GLuint);
void render();
void IdleGL();
void ReshapeGL(int w, int h);
void KeyboardGL(unsigned char, int, int);
void KeyboardUpGL(unsigned char, int, int);
void SpecialGL(int key, int x, int y);
void SpecialUpGL(int key, int x, int y);
void MouseGL(int button, int state, int x, int y);
void MotionGL(int, int);
void calculateFPS();
void getGLVersion();
void ClearScreen();
void DrawGrid(int);


int main(int argc, char * argv[])
{

	g_PreviousTicks = std::clock();


	camera.SetClipping(0.1f, 100.0f);
	camera.SetPosition(glm::vec3(0.0f, 10.0f, 10.0f));
	camera.SetLookAt(glm::vec3(0.0f, 0.0f, -1.0f));


	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1024, 768);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	//glutInitContextVersion(4, 4);
	//glutInitContextProfile(GLUT_CORE_PROFILE);
	//glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	glutCreateWindow("Grahics Pad");

	//initialize BulletPhysics
	phy.InitBulletPhysics();


	glewInit();

	glEnable(GL_DEPTH_TEST);


	glEnable(GL_DEPTH_CLAMP);
	//glEnable(GL_FRAMEBUFFER_SRGB);



	getGLVersion();


	sendDataToOpenGL();


	phongShader.installShaders("PhongVertex.glsl", "PhongFragment.glsl");



	glClearColor(0.0, 0.0, 0.3, 1.0);


	glutDisplayFunc(render);
	glutIdleFunc(IdleGL);
	glutReshapeFunc(ReshapeGL);
	glutKeyboardFunc(KeyboardGL);
	glutKeyboardUpFunc(KeyboardUpGL);
	glutSpecialFunc(SpecialGL);
	glutSpecialUpFunc(SpecialUpGL);
	glutMouseFunc(MouseGL);
	glutMotionFunc(MotionGL);



	glutMainLoop();



	return 0;
}

void sendDataToOpenGL()
{
	Mesh test;



	ResourceLoader::load_obj("suzanne.obj", vertices, normals, elements);



	polygon.vertices = vertices;
	polygon.normals = normals;
	polygon.elements = elements;




	test.addVertices(vertices, elements, normals);
	numIndices = test.getIndices();


}



//Callback Functions


void render()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	camera.Update();

	//DrawGrid(50);


	phy.StepBulletPhysics();

	btScalar	m[16];
	btMatrix3x3	rot; rot.setIdentity();
	const int	numObjects = phy.m_dynamicsWorld->getNumCollisionObjects();



	for (int i = 0; i<numObjects; i++)
	{
		btCollisionObject*	colObj = phy.m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody*		body = btRigidBody::upcast(colObj);

		if (body && body->getMotionState())
		{
			btDefaultMotionState* myMotionState = (btDefaultMotionState*)body->getMotionState();
			myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(m);
			rot = myMotionState->m_graphicsWorldTrans.getBasis();


			glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, NULL);







			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 projection;

			camera.GetMatricies(projection, view, model);

			model = glm::make_mat4(m);

			glm::mat4 fullTransformMatrix = projection * view * model;


			//Shader Uniform Data

			phongShader.addUniformMatrix4fv("fullTransformMatrix", fullTransformMatrix);

			phongShader.addUniform3f("eyePos", camera.camera_look_at);
			phongShader.addUniform3f("baseColor", glm::vec3(0.3, 0.5, 0.4));
			phongShader.addUniform3f("ambientLight", glm::vec3(0.0, 0.0, 1.0));



		}
		else
		{
			colObj->getWorldTransform().getOpenGLMatrix(m);
			rot = colObj->getWorldTransform().getBasis();
		}

	}


	//std::cout << camera.camera_look_at.x << " " << camera.camera_look_at.y << " " << camera.camera_look_at.z << std::endl;

	glutSwapBuffers();


}

void IdleGL()
{
	g_CurrentTicks = std::clock();
	float deltaTicks = (float)(g_CurrentTicks - g_PreviousTicks);
	g_PreviousTicks = g_CurrentTicks;

	fDeltaTime = deltaTicks / (float)CLOCKS_PER_SEC;

	float cameraSpeed = 1.0f;
	if (g_bShift)
	{
		cameraSpeed = 5.0f;
	}

	camera.Translate(glm::vec3((g_D - g_A) * 5, (g_Q - g_E) * 5, (g_W - g_S) * 5), cameraSpeed * fDeltaTime);

	//calculateFPS();
	//	ClearScreen();



	glutPostRedisplay();
}

void ReshapeGL(int w, int h)
{
	if (h == 0)
	{
		h = 1;
	}

	g_iWindowWidth = w;
	g_iWindowHeight = h;

	camera.SetViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	std::cout << "ReshapeGL" << std::endl;

}

void KeyboardGL(unsigned char c, int x, int y)
{
	switch (c)
	{
	case 'w':
	case 'W':
		g_W = 1;
		//std::cout << "W" <<std::endl;
		break;
	case 'a':
	case 'A':
		g_A = 1;
		//std::cout << "A" << std::endl;
		break;
	case 's':
	case 'S':
		g_S = 1;
		//	std::cout << "S" << std::endl;
		break;
	case 'd':
	case 'D':
		g_D = 1;
		//	std::cout << "D" << std::endl;
		break;
	case 'q':
	case 'Q':
		g_Q = 1;
		//	std::cout << "Q" << std::endl;
		break;
	case 'e':
	case 'E':
		g_E = 1;
		//	std::cout << "E" << std::endl;
		break;
	case 'r':
	case 'R':
		//std::cout << "R" << std::endl;
		phy.ExitBulletPhysics();
		phy.InitBulletPhysics();
		camera.Reset();
		break;
	case 'F':
	case 'f':
		std::cout << "F";
		phy.addCube(&camera);
		break;
	case 27:
		std::cout << "ESC" << std::endl;
		glutLeaveMainLoop();
		break;
	}
}

void KeyboardUpGL(unsigned char c, int x, int y)
{
	switch (c)
	{
	case 'w':
	case 'W':
		g_W = 0;
		break;
	case 'a':
	case 'A':
		g_A = 0;
		break;
	case 's':
	case 'S':
		g_S = 0;
		break;
	case 'd':
	case 'D':
		g_D = 0;
		break;
	case 'q':
	case 'Q':
		g_Q = 0;
		break;
	case 'e':
	case 'E':
		g_E = 0;
		break;

	default:
		break;
	}
}

void SpecialGL(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_SHIFT_L:
	case GLUT_KEY_SHIFT_R:
	{
							 g_bShift = true;
	}
		break;
	}
}

void SpecialUpGL(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_SHIFT_L:
	case GLUT_KEY_SHIFT_R:
	{
							 g_bShift = false;
	}
		break;
	}
}

void MouseGL(int button, int state, int x, int y)
{
	camera.SetPos(button, state, x, y);
}

void MotionGL(int x, int y)
{
	camera.Move2D(x, y);

	//glutWarpPointer((glutGet(GLUT_WINDOW_WIDTH)/2),(glutGet(GLUT_WINDOW_HEIGHT)/2));


}

void calculateFPS()
{

	//  Increase frame count
	frameCount++;

	//  Get the number of milliseconds since glutInit called
	//  (or first call to glutGet(GLUT ELAPSED TIME)).
	float currentTime = glutGet(GLUT_ELAPSED_TIME);

	//  Calculate time passed
	int timeInterval = currentTime - previousTime;

	if (timeInterval > 1000)
	{
		//  calculate the number of frames per second
		fps = frameCount / (timeInterval / 1000.0f);

		std::cout << fps << std::endl;

		//  Set time
		previousTime = currentTime;

		//  Reset frame count
		frameCount = 0;
	}
}

void getGLVersion()
{
	std::cout << glGetString(GL_VERSION) << std::endl;
}

void ClearScreen()
{
	std::cout << std::string(100, '\n');
}

void DrawGrid(int GRID_SIZE)
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glColor3f(0.75f, 0.75f, 0.75f);
	for (int i = -GRID_SIZE; i <= GRID_SIZE; i++)
	{
		glVertex3f((float)i, 0, (float)-GRID_SIZE);
		glVertex3f((float)i, 0, (float)GRID_SIZE);

		glVertex3f((float)-GRID_SIZE, 0, (float)i);
		glVertex3f((float)GRID_SIZE, 0, (float)i);
	}
	glEnd();
	glEnable(GL_LIGHTING);
}



