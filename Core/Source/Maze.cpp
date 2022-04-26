#include "Pch.h"
#include "Maze.h"

#define BOUND_SIZE 3

Maze::Maze(Application* app)
	:
	app(app),
	wallTexture(new sf::Texture),
	pathTexture(new sf::Texture),
	wallSprite(new sf::Sprite),
	pathSprite(new sf::Sprite)
{
	map = new exo::TileMap<TileID>(dimensions.x, dimensions.y);
	SetBounds();
	app->LoadSprite(wallTexture, wallSprite, "Assets/wall.png", dimensions.y);
	app->LoadSprite(pathTexture, pathSprite, "Assets/path.png", dimensions.y);
	scale = pathSprite->getScale().y;

	//Set Start
	startTile = map->GetStartTile();
	for (int y = 0; y < map->GetHeight() / 2; y++)
	{
		startTile = startTile->down;
	}
	for (int x = 0; x < map->GetWidth() / 2; x++)
	{
		startTile = startTile->right;
	}
	currentTile = startTile;
	startTile->id = TileID::path;
}

Maze::~Maze()
{
	//Members
	delete wallSprite;
	delete pathSprite;
	delete wallTexture;
	delete pathTexture;
	delete map;

	//references
	app = nullptr;
}

void Maze::SetTile(sf::Vector2<int> coord, TileID id)
{
	map->SetID(coord.x, coord.y, id);
}


void Maze::Generate()
{
	srand(time(NULL));
	struct Corridor //2x5
	{
		int direction = 0; // 0 = up, + clockwise %4;
		int sLength = 2;
		int lLength = 5;
		exo::Tile<TileID>* anchor; //bottom left (when up)
		exo::Tile<TileID>* detector; //top right (when up)

		void Revert(exo::Tile<TileID>* _anchor, exo::Tile<TileID>* _detector, int _direction)
		{
			anchor = _anchor;
			detector = _detector;
			direction = _direction;
		}
		void UpdateDetector()
		{
			exo::Tile<TileID>* thru = anchor;
			switch (direction)
			{
			case 0: // Up
				for (int i = 0; i < lLength - 1; i++)
					thru = thru->up;
				thru = thru->right;
				break;
			case 1: // Right
				for (int i = 0; i < lLength - 1; i++)
					thru = thru->right;
				thru = thru->down;
				break;
			case 2: // Down
				for (int i = 0; i < lLength - 1; i++)
					thru = thru->down;
				thru = thru->left;
				break;
			case 3: // Left
				for (int i = 0; i < lLength - 1; i++)
					thru = thru->left;
				thru = thru->up;
				break;
			}
			detector = thru;
		}
		void Rotate(int cw)
		{
			for (int i = 0; i < cw; i++)
			{
				switch (direction)
				{
				case 0: // Up
					anchor = anchor->up;
					break;
				case 1: // Right
					anchor = anchor->right;
					break;
				case 2: // Down
					anchor = anchor->down;
					break;
				case 3: // Left
					anchor = anchor->left;
					break;
				}
				direction = (direction + 1) % 4;
				UpdateDetector();
			}
		}
		void Commit()
		{
			exo::Tile<TileID>* thru = anchor;
			thru->id = TileID::path; // stamp start
			switch (direction)
			{
			case 0: // Up
				for (int i = 0; i < lLength - 1; i++)
				{
					thru = thru->up;
					thru->id = TileID::path; // Stamp all going up
				}
				thru = thru->right;
				thru->id = TileID::path; // stamp other next row
				for (int i = 0; i < lLength - 1; i++)
				{
					thru = thru->down;
					thru->id = TileID::path; // stamp all down other row
				}
				anchor = anchor->up->up->up;
				UpdateDetector();
				break;

			case 1: // Right
				for (int i = 0; i < lLength - 1; i++)
				{
					thru = thru->right;
					thru->id = TileID::path;
				}
				thru = thru->down;
				thru->id = TileID::path;
				for (int i = 0; i < lLength - 1; i++)
				{
					thru = thru->left;
					thru->id = TileID::path;
				};
				anchor = anchor->right->right->right;
				UpdateDetector();
				break;

			case 2: // Down
				for (int i = 0; i < lLength - 1; i++)
				{
					thru = thru->down;
					thru->id = TileID::path;
				}
				thru = thru->left;
				thru->id = TileID::path;
				for (int i = 0; i < lLength - 1; i++)
				{
					thru = thru->up;
					thru->id = TileID::path;
				}
				anchor = anchor->down->down->down;
				UpdateDetector();
				break;

			case 3: // Left
				for (int i = 0; i < lLength - 1; i++)
				{
					thru = thru->left;
					thru->id = TileID::path;
				}
				thru = thru->up;
				thru->id = TileID::path;
				for (int i = 0; i < lLength - 1; i++)
				{
					thru = thru->right;
					thru->id = TileID::path;
				}
				anchor = anchor->left->left->left;
				UpdateDetector();
				break;
			}
		}
		bool CheckValid()
		{
			switch (direction)
			{
			case 0: // Up
				return (detector->id == TileID::wall && detector->left->id == TileID::wall);
			case 1: // Right
				return (detector->id == TileID::wall && detector->up->id == TileID::wall);
			case 2: // Down
				return (detector->id == TileID::wall && detector->right->id == TileID::wall);
			case 3: // Left
				return (detector->id == TileID::wall && detector->down->id == TileID::wall);
			}
		}
		void GenLoop()
		{
			//save current position
			exo::Tile<TileID>* _anchor = anchor;
			exo::Tile<TileID>* _detector = detector;
			int _direction = direction;

			int initR = rand() % 2;

			//Check Straight
			if (initR == 0)
			{
				if (CheckValid()) // no path exists
				{
					Commit(); // stamp
					GenLoop(); // recursive call
					Revert(_anchor, _detector, _direction); // go back to previous point
				}
			}

			//Check Rotations
			int r = (rand() % 2) * 2 + 1;
			Rotate(r);
			if (CheckValid())
			{
				Commit();
				GenLoop();
				Revert(_anchor, _detector, _direction);
			}
			Rotate((r + 2) % 4);
			if (CheckValid())
			{
				Commit();
				GenLoop();
				Revert(_anchor, _detector, _direction);
			}

			// Check Straight if not checked
			if (initR == 1) 
			{
				if (CheckValid())
				{
					Commit();
					GenLoop();
					Revert(_anchor, _detector, _direction);
				}
			}

			return;
		}
	};

	//Setup
	Corridor* corridor = new Corridor;
	corridor->anchor = startTile;
	corridor->Rotate(rand() % 4); // Get Random Start Direction
	corridor->Commit();
	
	//Draw Maze
	corridor->GenLoop();

	delete corridor;
	//map->SaveToFile("Assets/maze1.tilemap");
}

void Maze::Draw()
{
	//app->GetWindow()->clear();
	auto it = map->Begin();
	for (int y = 0; y < map->GetHeight(); y++)
	{
		for (int x = 0; x < map->GetWidth(); x++)
		{
			switch (*it)
			{
			case TileID::wall:
				wallSprite->setPosition(
					{
						x * scale,
						y * scale
					});
				app->GetWindow()->draw(*wallSprite);
				break;
			case TileID::path:
				pathSprite->setPosition(
					{
						x * scale,
						y * scale
					});
				app->GetWindow()->draw(*pathSprite);
				break;
			}
			it++;
		}
	}
	//app->GetWindow()->display();
}

void Maze::SetBounds()
{
	exo::Tile<TileID>* thru = map->GetStartTile();
	exo::Tile<TileID>* nextRow = thru->down;

	for (int y = 0; y < BOUND_SIZE; y++)
	{
		for (int x = 0; x < map->GetWidth(); x++)
		{
			thru->id = TileID::bound;
			thru = thru->right;
		}
		thru = nextRow;
		nextRow = nextRow->down;
	}
	for (int y = 0; y < map->GetHeight() - (BOUND_SIZE * 2); y++)
	{
		for (int x = 0; x < map->GetWidth(); x++)
		{
			if (x < BOUND_SIZE || x >= map->GetWidth() - BOUND_SIZE)
				thru->id = TileID::bound;
			thru = thru->right;
		}
		thru = nextRow;
		nextRow = nextRow->down;
	}
	for (int y = 0; y < BOUND_SIZE; y++)
	{
		for (int x = 0; x < map->GetWidth(); x++)
		{
			thru->id = TileID::bound;
			thru = thru->right;
		}
		thru = nextRow;
		nextRow = nextRow->down;
	}

}
