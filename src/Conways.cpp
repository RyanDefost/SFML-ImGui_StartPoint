#include <algorithm>
#include <functional>
#include <unordered_map>
#include "Conways.hpp"


Conways::Conways()
{

}

Conways::~Conways()
{

}

struct Cell
{
	sf::RectangleShape shape;
	sf::Vector2f position;

	Cell(float x, float y) {
		position = { x,y };
	}

	bool operator==(const Cell &other) {
		return (position == other.position);
	}
};

template <>
struct std::hash<Cell>
{
	std::size_t operator()(const Cell& k) const
	{
		using std::size_t;
		using std::hash;
		using sf::Vector2f;

		// Compute individual hash values for first,
		// second and third and combine them using XOR
		// and bit shifting:

		return ((hash<float>()(k.position.x)
			^ (hash<float>()(k.position.y))));
	}
};

void Conways::DisplayCells(sf::RenderWindow& window)
{
	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<float> posDist;

	for (int i = 0; i < window.getSize().x; i++)
	{
		std::pair<int, int> position = { posDist(gen), posDist(gen) };
		Cell currentCell = Cell(position.first, position.second);

		cells.insert({ position, currentCell });
	}
}


