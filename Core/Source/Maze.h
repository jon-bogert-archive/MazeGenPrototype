#pragma once
#include "Pch.h"
#include "Application.h"
#include "TileMap.h"

enum class TileID
{
	wall,
	path,
	bound,
	flag
};

class Application;

class Maze
{
private:
	Application* app;

	exo::TileMap<TileID>* map;
	exo::Tile<TileID>* startTile;
	exo::Tile<TileID>* currentTile;
	sf::Vector2<int> dimensions = { 50, 50 };

	sf::Texture* wallTexture;
	sf::Texture* pathTexture;
	sf::Sprite* wallSprite;
	sf::Sprite* pathSprite;
	float scale;

public:
	Maze(Application* app);
	~Maze();

	void SetTile(sf::Vector2<int> coord, TileID id);

	void Generate();
	void Draw();

private:
	void SetBounds();
};

