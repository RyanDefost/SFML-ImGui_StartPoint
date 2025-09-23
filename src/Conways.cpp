#include <algorithm>
#include <functional>
#include <unordered_map>
#include "Conways.hpp"


Conways::Conways()
{
	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<float> posDist;

	/*for (int i = 0; i < 500; i++)
	{
		
	}*/
	std::pair<int, int> position = { posDist(gen), posDist(gen) };
	Cell currentCell = Cell(position.first, position.second);

	//cells.insert({ position,1 });
	cells.insert({ position, currentCell });
}

Conways::~Conways()
{

}

void Conways::DisplayCells(sf::RenderWindow& window)
{
	for (auto& cell : cells) {
		window.draw(cell.second.shape);
	}
}


