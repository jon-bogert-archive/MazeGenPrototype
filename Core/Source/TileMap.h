/*
* ============================================
* eXomorph Tools: TileMap (C++ Edition) v0.0.0
* ============================================
* 
* CREATED BY: Jon Bogert in 2022
* 
* FEATURES:
* - Quad-linked map of values
* - Each tile is linked up, down, left, or right;
* - Meant for Staticly-sized 2D containers such as terrain or inventory in games
* 
* NOTES:
* - Container is static and cannot be resized.
* - Best used with enum's (or int's)
* 	- Do not use SaveToFile() or Constructor with "std::string path" if using an object
* 	  that cannot be converted to an int
* - Version available in C (limited functionality)
*
* INSTRUCTIONS:
* - just include "TileMap.h" in your project
* - include "using namespace exo" if wanted or use "exo::TileMap<Type>"
*
* Copyright (c) 2022 Jon Bogert (@xepherin)
*/



#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace exo
{
	template <class Type>
	struct Tile
	{
		Tile<Type>* up = nullptr;
		Tile<Type>* down = nullptr;
		Tile<Type>* left = nullptr;
		Tile<Type>* right = nullptr;

		Type id = Type();
	};

	template <class Type>
	class TileMapItr
	{
	private:
		Tile<Type>* current;
		Tile<Type>* nextRow;

	public:
		TileMapItr<Type>() = default;
		TileMapItr<Type>(Tile<Type>* start);
		
		Type operator*();
		TileMapItr<Type> operator++();
		TileMapItr<Type> operator++(int);
		bool isNull() const;
	};

	template <class Type>
	class TileMap
	{
		
	private:
		Tile<Type>* start;
		int width;
		int height;

	public:
		TileMap<Type>(int width, int height); // Creates map of specified width and height with default values
		TileMap<Type>(std::string path);// Loads int values with width and height from file at specified path
		~TileMap<Type>();
		TileMap(TileMap<Type>& other); // Creates map with same values and dimensions as provided map
		TileMap<Type>& operator=(TileMap<Type>& other); // Destroys current map and replaces with new map with same values and dimensions as provided

		typedef TileMapItr<Type> iterator;

		void Print() const; // Outputs int values to console
		void SaveToFile(const std::string path); // Saves int values with width and height to file at specified path

		//Getters
		Type GetID(const int x, const int y) const;
		Tile<Type>* GetStartTile() const; // For Manual Navigation
		int GetWidth() const;
		int GetHeight() const;

		//Setters
		void SetID(const int x, const int y, const Type value);

		iterator Begin() const;

	private:
		void Init(const int _width, const int _height);
		void Copy(TileMap<Type>& other);
		void Destroy();
		Tile<Type>* FindTile(const int x, const int y) const;
		void Next(Tile<Type>*& thru, Tile<Type>*& below); // used to iterate through the map, thru and below ptr's must be provided outside the fn
	};



	// ===== IMPLEMENTATION =====


	template<class Type>
	inline TileMap<Type>::TileMap(int width, int height)
	{
		Init(width, height);
	}

	template<class Type>
	inline TileMap<Type>::TileMap(std::string path)
	{
		std::ifstream file(path);
		if (!file.is_open())
		{
			Init(0, 0);
			std::cout << "[ERROR] Unable to open file at " << path << std::endl;
			return;
		}
		std::string data;

		//Get Width
		getline(file, data);
		data.erase(0, 6);
		width = atoi(data.c_str());

		//Get Height
		getline(file, data);
		data.erase(0, 7);
		height = atoi(data.c_str());

		//Create default map
		Init(width, height);

		//Collect and fill id's
		Tile<Type>* thru = start;
		Tile<Type>* below = start->down;
		for (int i = 0; i < width * height; i++)
		{
			file >> data;
			thru->id = (Type)(atoi(data.c_str()));
			Next(thru, below);
		}

		file.close();
	}

	template<class Type>
	inline TileMap<Type>::~TileMap()
	{
		Destroy();
	}

	template<class Type>
	inline TileMap<Type>::TileMap(TileMap<Type>& other)
	{
		Init(other.GetWidth(), other.GetHeight());
		Copy(other);
	}

	template<class Type>
	inline TileMap<Type>& TileMap<Type>::operator=(TileMap<Type>& other)
	{
		Destroy();
		Init(other.GetWidth(), other.GetHeight());
		Copy(other);
		return *this;
	}

	template<class Type>
	inline void TileMap<Type>::Print() const
	{
		Tile<Type>* thru = start; // create pointer to navigate tile map

		for (int y = 0; y < height; y++)
		{
			//Move Through Row
			for (int x = 0; x < width; x++) // moving left in row (connecting left-right, but not up down)
			{
				std::cout << (int)thru->id;
				thru = thru->right;
			}

			//Next Row
			thru = start; //reset thru pointer
			for (int i = 0; i < y + 1; i++)
				thru = thru->down;
			std::cout << std::endl;
		}
	}

	template<class Type>
	inline void TileMap<Type>::SaveToFile(const std::string path)
	{
		std::ofstream file(path);

		if (file.is_open())
		{
			file << "WIDTH=" << width << std::endl;
			file << "HEIGHT=" << height << std::endl;

			Tile<Type>* thru = start; // create pointer to navigate tile map

			for (int y = 0; y < height; y++)
			{
				//Move Through Row
				for (int x = 0; x < width; x++) // moving left in row (connecting left-right, but not up down)
				{
					file << (int)thru->id << " ";
					thru = thru->right;
				}

				//Next Row
				thru = start; //reset thru pointer
				for (int i = 0; i < y + 1; i++)
					thru = thru->down;
				file << std::endl;
			}
		}
		file.close();
	}

	template<class Type>
	inline Type TileMap<Type>::GetID(const int x, const int y) const
	{
		return FindTile(x, y)->id;
	}

	template<class Type>
	inline Tile<Type>* TileMap<Type>::GetStartTile() const
	{
		return start;
	}

	template<class Type>
	inline int TileMap<Type>::GetWidth() const
	{
		return width;
	}

	template<class Type>
	inline int TileMap<Type>::GetHeight() const
	{
		return height;
	}

	template<class Type>
	inline void TileMap<Type>::SetID(const int x, const int y, const Type value)
	{
		FindTile(x, y)->id = value;
	}

	template<class Type>
	inline TileMapItr<Type> TileMap<Type>::Begin() const
	{
		return TileMapItr<Type>(start);
	}

	template<class Type>
	inline void TileMap<Type>::Init(const int _width, const int _height)
	{
		width = _width;
		height = _height;

		start = new Tile<Type>; // Create start (top left)
		Tile<Type>* thru = start; // create pointer to navigate tile map

		//Create rows
		for (int y = 0; y < height; y++)
		{
			//Construct Row
			for (int x = 1; x < width; x++) // moving left in row (connecting left-right, but not up down)
			{
				thru->right = new Tile<Type>;
				thru->right->left = thru;
				thru = thru->right;
			}

			//Next Row
			thru = start; //reset thru pointer
			for (int i = 0; i < y; i++) //move down existsing rows;
				thru = thru->down;

			thru->down = new Tile<Type>;
			thru->down->up = thru;
			thru = thru->down;
		}

		//Connect Rows
		thru = start;//reset thru pointer
		Tile<Type>* below = thru->down; // create reference pointer below thru

		for (int y = 0; y < height - 1; y++) // Moving Down
		{
			for (int x = 0; x < width; x++) // Moving Right
			{
				//Set Refrences
				thru->down = below;
				below->up = thru;
				//Move Next
				thru = thru->right;
				below = below->right;
			}
			//Next Row
			thru = start; //reset pointers
			below = start->down;
			for (int i = 0; i < y + 1; i++) //move down existsing rows;
			{
				thru = thru->down;
				below = below->down;
			}
		}
	}

	template<class Type>
	inline void TileMap<Type>::Copy(TileMap<Type>& other)
	{
		Tile<Type>* thruThis = start;
		Tile<Type>* belowThis = start->down;
		Tile<Type>* thruOther = other.GetStartTile();
		Tile<Type>* belowOther = other.GetStartTile()->down;

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				thruThis->id = thruOther->id;
				thruThis = thruThis->right;
				thruOther = thruOther->right;
			}

			thruThis = belowThis;
			thruOther = belowOther;
			belowThis = belowThis->down;
			belowOther = belowOther->down;
		}
	}

	template<class Type>
	inline void TileMap<Type>::Destroy()
	{
		Tile<Type>* thru = start;
		Tile<Type>* nextRow;
		do
		{
			nextRow = thru->down;
			while (thru != nullptr)
			{
				Tile<Type>* temp = thru;
				thru = thru->right;
				delete temp;
			}
			thru = nextRow;
		} while (nextRow != nullptr);
	}

	template<class Type>
	inline Tile<Type>* TileMap<Type>::FindTile(const int x, const int y) const
	{
		Tile<Type>* thru = start;

		for (int i = 0; i < x; i++) // Right
			thru = thru->right;
		for (int i = 0; i < y; i++) // Down
			thru = thru->down;

		return thru;
	}

	template<class Type>
	inline void TileMap<Type>::Next(Tile<Type>*& thru, Tile<Type>*& below) //Will return nullptr when done
	{
		if (thru->right == nullptr)
		{
			if (below == nullptr)
			{
				thru == nullptr;
				return;
			}
			else
			{
				thru = below;
				below = below->down;
			}
		}
		else
		{
			thru = thru->right;
		}
	}

	// ====== Iterator =====


	template<class Type>
	inline Type TileMapItr<Type>::operator*()
	{
		return current->id;
	}

	template<class Type>
	inline TileMapItr<Type> TileMapItr<Type>::operator++()
	{
		if (current->right == nullptr)
		{
			if (nextRow == nullptr)
			{
				current = nullptr;
				return *this;
			}
			else
			{
				current = nextRow;
				nextRow = nextRow->down;
			}
		}
		else
		{
			current = current->right;
		}
		return *this;
	}

	template<class Type>
	inline TileMapItr<Type> TileMapItr<Type>::operator++(int)
	{
		if (current->right == nullptr)
		{
			if (nextRow == nullptr)
			{
				current = nullptr;
				return *this;
			}
			else
			{
				current = nextRow;
				nextRow = nextRow->down;
			}
		}
		else
		{
			current = current->right;
		}
		return *this;
	}

	template<class Type>
	inline bool TileMapItr<Type>::isNull() const
	{
		return current == nullptr;
	}

	template<class Type>
	inline TileMapItr<Type>::TileMapItr(Tile<Type>* start)
	{
		if (start->up != nullptr && start->left != nullptr)
		{
			std::cout << "[ERROR] Ill-defined iterator for TileMap" << std::endl;
			return;
		}
		current = start;
		nextRow = start->down;
	}
}
