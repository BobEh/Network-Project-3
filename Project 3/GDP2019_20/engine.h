#pragma once
#include <string>
#include <vector>
#include <map>

#include "System.h"

namespace Engine
{
	int Initialize(void);
	int Destroy(void);

	int Run(void);

	int AddSystem(System* system);

}