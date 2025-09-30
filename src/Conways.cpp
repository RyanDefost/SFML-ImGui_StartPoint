#include <algorithm>
#include <functional>
#include <numeric>
#include <unordered_map>
#include "Conways.hpp"
#include <iostream>

Conways::Conways()
{
	sf::VertexArray point(sf::PrimitiveType::Points, 1);
	point[0].position = { 0,0 };
	point[0].color = sf::Color::White;
	visual = point;

	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<float> random;

	cells.reserve(500 * 500);
	grid.reserve(500 * 500);

	for (int i = 0; i < 500; i++)
	{
		for (size_t j = 0; j < 500; j++)
		{
			std::pair<int, int> position = {
				(int)i * GameSize, (int)j * GameSize 
			};
			
			Cell currentCell = Cell(
				position.first,
				position.second,
				sf::Vector2f(GameSize, GameSize),
				sf::Color::White
			);

			random = std::uniform_real_distribution<float>(0, 2);
			if (random(gen) < 1) {
				currentCell.isActive = true;
				currentCell.previouseActive = false;
			}

			cells.emplace_back(currentCell);
			
			Cell& cell = cells.back();
			grid.insert({ position, &cell});
		}
		
	}

	GetNeighbours();
}

Conways::~Conways()
{

}

void Conways::GetNeighbours()
{
	for (auto& cell : cells) {

		for (auto& diraction : diractions) {
			
			std::pair<int, int> currentDir = {
				cell.position.x + diraction.first, 
				cell.position.y + diraction.second
			};

			if (grid.find(currentDir) == grid.end()) continue;

			cell.neighbours.emplace_back(grid.at(currentDir));
		}
	}
}

void Conways::UpdateCells(sf::RenderWindow& window)
{
	for (auto& cell : cells) {

		auto count = 0;
		for (auto& neighbour : cell.neighbours) {
			if (neighbour->previouseActive) count++;

			if (count > 3) {
				cell.isActive = false;
				continue;
			}
		}

		if (!cell.previouseActive && count == 3) {
			cell.isActive = true;
			continue;
		}
		else if (count < 2) {
			cell.isActive = false;
			continue;
		}
	}
}

void Conways::DisplayCells(sf::RenderWindow& window)
{
	for (auto& cell : cells) {
		cell.previouseActive = cell.isActive;
		
		if (!cell) continue;

		visual[0].position = cell.position;
		window.draw(visual);
	}
}