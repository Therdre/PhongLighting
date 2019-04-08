#include "GameLoop.h"
#include "../Graphics/Graphics.h"

GameLoop::GameLoop(): previousFrameTime(0), timePerFrame(1000.0f/60.0f)
{

}

GameLoop& GameLoop::GetInstace()
{
	static GameLoop gameLoop;
	return gameLoop;
}

void GameLoop::RunLoop()
{
	DWORD currentFrame = GetTickCount();
	DWORD elapsed = currentFrame - previousFrameTime;
	//process the update

	Graphics::GetInstance().Update((float)elapsed/1000.0f);

	float updateTime = (float)(GetTickCount() - currentFrame);
	if (updateTime < timePerFrame)
	{
		Sleep(timePerFrame - updateTime);
	}
	
	previousFrameTime = currentFrame;



}