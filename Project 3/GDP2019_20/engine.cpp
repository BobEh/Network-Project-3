#include "Engine.h"

#include <assert.h>
#include <conio.h>
#include <iostream>
#include <list>

#include "l_opengl.h"
#include <GL/freeglut.h>

#include "input_manager.h"
#include "entity_manager.h"
#include "shader_manager.h"
#include "mesh_manager.h"
#include "scene_manager.h"

#include "mesh_renderer.h"
#include "transform.h"

#include "render_system.h"

#include "network_test_scene.h"

// #define LOG_SYSTEMS


InputManager gInputManager;
ShaderManager gShaderManager;
MeshManager gMeshManager;
SceneManager gSceneManager;

RenderSystem* gRenderSystem;

void Resize(int width, int height);
void Update(void);
void Draw(void);

void PressKey(unsigned char key, int x, int y);
void ReleaseKey(unsigned char key, int x, int y);
void PressSpecialKey(int key, int x, int y);
void ReleaseSpecialKey(int key, int x, int y);

void MouseButton(int button, int state, int x, int y);
void MousePassiveMotion(int x, int y);
void MouseDragMotion(int x, int y);

std::vector<System*> gSystems;		// Container of Systems

/**
 * Initialize
 * Acts as constructor for the Engine
 * @return - engine status code
 */
int Engine::Initialize(void)
{
	int argc = 0;
	char** argv = 0;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 100);
	glutInitWindowSize(1200, 800);
	glutCreateWindow("Title");

	GLenum result = glewInit();
	if (result != GLEW_OK)
	{
		return 1;
	}

	// Register callbacks
	glutDisplayFunc(Draw);
	glutReshapeFunc(Resize);
	glutIdleFunc(Update);

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(PressKey);
	glutKeyboardUpFunc(ReleaseKey);
	glutSpecialFunc(PressSpecialKey);
	glutSpecialUpFunc(ReleaseSpecialKey);

	glutMouseFunc(MouseButton);
	glutPassiveMotionFunc(MousePassiveMotion);
	glutMotionFunc(MouseDragMotion);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	gShaderManager.StartUp();
	gMeshManager.StartUp();
	gInputManager.StartUp();
	gSceneManager.StartUp();

	gSceneManager.ChangeScene(new NetworkTestScene());

	return 0;
}

/**
* Destroy
* Acts as destructor for the Engine
* @return - engine status code
*/
int Engine::Destroy(void)
{
	unsigned int numSystems = gSystems.size();
	for (unsigned int i = 0; i < numSystems; i++)
	{
		delete gSystems[i];
	}
	gSystems.clear();

	gSceneManager.Shutdown();
	gInputManager.Shutdown();
	gMeshManager.Shutdown();
	gShaderManager.Shutdown();

	return 0;
}

/**
 * AddSystem
 * Adds a system to the engine
 */
int Engine::AddSystem(System* system)
{
	assert(system);
	gSystems.push_back(system);

	return 0;
}

/**
* Run
* The game loop
* @return - engine status code
*/
int Engine::Run(void)
{
	gRenderSystem = new RenderSystem();

	glutMainLoop();

	return 0;
}

void Resize(int width, int height)
{
	glViewport(0, 0, width, height);
}

/**
* Update
* Code for the update phase
*/
unsigned int loopId = 0;
const double maxFPS = 60.0;
const double maxPeriod = 1000.0 / maxFPS;
double lastTime = 0.0;
void Update(void)
{
	bool running = true;

	double time = glutGet(GLUT_ELAPSED_TIME);
	double deltaTime = time - lastTime;

	if (deltaTime < maxPeriod) return;
#ifdef LOG_SYSTEMS
	printf("%2.4f time...\n", deltaTime);
#endif
	lastTime = time;

	//if (_kbhit())			// Do one iteration on a keypress
	{
		loopId++;
#ifdef LOG_SYSTEMS
		printf("Engine loop #%d\n", loopId);
#endif
		//char ch = _getch();
		//if (ch == 27)		// If the keypress is ESC, quit
		//{
		//	running = false;
		//}

		float dt = 0.10f;
		gInputManager.Update();
		gSceneManager.Update();

		unsigned int numSystems = gSystems.size();
		for (unsigned int i = 0; i < numSystems; i++)
		{

#ifdef LOG_SYSTEMS
			printf("  %s is processing...\n", gSystems[i]->GetName().c_str());
#endif

			gSystems[i]->Process(EntityManager::GetEntityList(), dt);

#ifdef LOG_SYSTEMS
			printf("\n");
#endif
		}

		// Clear the buffer
		//_getch();
	}

	Draw();
}

void Draw(void)
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	gRenderSystem->Process(EntityManager::GetEntityList(), 0.0f);

	glutSwapBuffers();
}

void PressKey(unsigned char key, int x, int y)
{
	gInputManager.PressKey(key, x, y);
}

void ReleaseKey(unsigned char key, int x, int y)
{
	gInputManager.ReleaseKey(key, x, y);
}

void PressSpecialKey(int key, int x, int y)
{
	gInputManager.PressSpecialKey(key, x, y);
}

void ReleaseSpecialKey(int key, int x, int y)
{
	gInputManager.ReleaseSpecialKey(key, x, y);
}

void MouseButton(int button, int state, int x, int y)
{
	gInputManager.MouseButton(button, state, x, y);
}

void MousePassiveMotion(int x, int y)
{
	gInputManager.MouseMotion(x, y);
}

void MouseDragMotion(int x, int y)
{
	gInputManager.MouseMotion(x, y);
}
