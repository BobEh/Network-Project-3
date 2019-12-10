#pragma once

#include "client.h"

class NetworkTestScene
{
public:
	NetworkTestScene(void);
	~NetworkTestScene(void);

protected:
	virtual void OnLoad();
	virtual void OnUnload();
	virtual void OnUpdate();

private:
	Client mClient;
};