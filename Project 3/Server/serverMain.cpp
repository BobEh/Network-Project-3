#include <conio.h>

#include "server.h"

int main()
{
	Server server;
	int ch;

	while (1)
	{
		if (_kbhit())
		{
			ch = _getch();

			if (ch == 27) break;
		}
		server.Update();
	}

	return 0;
}