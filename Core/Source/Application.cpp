#include "Pch.h"
#include "Application.h"

Application::Application()
	:
	clock(new sf::Clock())
{
	window = new sf::RenderWindow(sf::VideoMode(resolution.x, resolution.y), title);
}

Application::~Application()
{
	//Members
	delete clock;
	delete window;
}

void Application::SetMazeRef(Maze* _maze)
{
	maze = _maze;
}

sf::RenderWindow* Application::GetWindow() const
{
	return window;
}

void Application::Start()
{
	maze->Generate();
}

void Application::GameLoop()
{
	while (window->isOpen())
	{
		sf::Event* windowEvent = new sf::Event;
		while (window->pollEvent(*windowEvent))
		{
			if (windowEvent->type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				window->close();
			}
		}
		delete windowEvent;

		deltaTime = clock->getElapsedTime().asSeconds();
		clock->restart();

		window->clear(backgroundColor);

		Update();

		window->display();
	}
}

void Application::Update()
{
	maze->Draw();
}

sf::Vector2<int> Application::GetResolution() const
{
	return resolution;
}

sf::Vector2<float> Application::GetResolutionScale() const
{
	return sf::Vector2<float>
		(
		(float)resolution.x / (float) REF_RESOLUTION.x,
		(float)resolution.y / (float)REF_RESOLUTION.y
		);
}

float Application::DeltaTime() const
{
	return deltaTime;
}

void Application::LoadSprite(sf::Texture* texture, sf::Sprite* sprite, std::string path, int mapHeight)
{
	if (!texture->loadFromFile(path))
	{
		std::cout << "Could not loat texture from " << path << std::endl;
		return;
	}
	sprite->setTexture(*texture);
	sprite->setTextureRect(sf::Rect(0, 0, 1, 1));
	float scale = ((REF_RESOLUTION.y / (float)mapHeight) / texture->getSize().y) * GetResolutionScale().y;
	sprite->setScale(scale, scale);
}
