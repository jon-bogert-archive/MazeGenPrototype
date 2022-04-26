#include "Pch.h"
#include "Application.h"
#include "Maze.h"

int main()
{
	Application* application = new Application();
	Maze* maze = new Maze(application);

	application->SetMazeRef(maze);

	//maze->SetTile({10, 10}, TileID::path);

	application->Start();
	application->GameLoop();

	delete maze;
	delete application;

	return 0;
}