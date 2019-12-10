#include <conio.h>
#include <iostream>
#include <assert.h>
#include <random>
#include <time.h>

#include "Engine.h"
#include "Component.h"
#include "render_system.h"

#include "client.h"

void RunGUI(void)
{
	srand(time(NULL));
	int result;

	printf("Initializing engine...\n");
	result = Engine::Initialize();
	if (result != 0) {
		printf("Failed to initialize engine with error code %d\n", result);
		return;
	}

	printf("Running...\n");
	result = Engine::Run();
	if (result != 0) {
		printf("Error running the engine with error code %d\n", result);
		return;
	}

	printf("Shutting down the engine...\n");
	result = Engine::Destroy();
	if (result != 0) {
		printf("Failed to shut down the engine with error code %d\n", result);
		return;
	}
}

int main(int argc, char** argv)
{
	RunGUI();

	system("Pause");
}