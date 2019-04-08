#pragma once

#include <Windows.h>

class GameLoop
{
	public:
		static GameLoop& GetInstace();

		void RunLoop();

	private:
		GameLoop();

		DWORD previousFrameTime;
		float timePerFrame;

};
