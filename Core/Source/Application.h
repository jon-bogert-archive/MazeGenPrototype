#pragma once
#include "Pch.h"
#include "Maze.h"

class Maze;

class Application
{
private:
	//Constants
	const sf::Vector2<int> REF_RESOLUTION = sf::Vector2<int>(1080, 1080);

	//References
	Maze* maze;

	//Members
	sf::RenderWindow* window;
	sf::Vector2<int> resolution = sf::Vector2<int>(1080, 1080);
	std::string title = "Application Title";
	sf::Color backgroundColor = sf::Color::Black;

	sf::Clock* clock;
	float deltaTime{};

public:
	Application();
	~Application();
	Application(Application()) = delete;
	Application operator=(Application()) = delete;

	void SetMazeRef(Maze* _maze);

	sf::RenderWindow* GetWindow() const;

	void Start();
	void GameLoop();
	void Update();

	sf::Vector2<int> GetResolution() const;
	sf::Vector2<float> GetResolutionScale() const;
	float DeltaTime() const;

	void LoadSprite(sf::Texture* texture, sf::Sprite* sprite, std::string path, int mapHeight);


};

