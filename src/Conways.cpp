#include <algorithm>
#include <functional>
#include <numeric>
#include <unordered_map>
#include "Conways.hpp"
#include <iostream>

Conways::Conways()
{
	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<float> random;

	cells.reserve(200 * 200);

	for (int i = 0; i < 200; i++)
	{
		for (size_t j = 0; j < 200; j++)
		{
			random = std::uniform_real_distribution<float>(0, 100);

			//PLEASE FIX THIS UGLY WORK AROUND
			std::pair<int, int> position = { (int)i * GameSize, (int)j * GameSize };
			Cell currentCell = Cell(
				position.first,
				position.second,
				sf::Vector2f(GameSize, GameSize),
				sf::Color::White
			);

			if (random(gen) <= 40) {
				currentCell.isActive = true;
				currentCell.previouseActive = false;
			}

			cells.insert({ position, currentCell });
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
			std::pair<int, int> currentDir = { cell.first.first + diraction.first, cell.first.second + diraction.second };

			if (cells.find(currentDir) == cells.end()) continue;

			cell.second.neighbours.emplace_back(currentDir);
		}
	}
}

void Conways::UpdateCells(sf::RenderWindow& window)
{
	for (auto& cell : cells) {

		auto count = 0;
		std::for_each(cell.second.neighbours.begin(), cell.second.neighbours.end(), [=, &count](std::pair<int,int>& p) {
			if (cells.at(p).previouseActive) count++;
		});

		if (!cell.second.previouseActive && count == 3) {
			cell.second.isActive = true;
			continue;
		}

		if (count < 2 || count > 3) {
			cell.second.isActive = false;
			continue;
		}
	}
}

void Conways::DisplayCells(sf::RenderWindow& window)
{
	for (auto& cell : cells) {
		cell.second.previouseActive = cell.second.isActive;
		
		if (!cell.second) continue;
		window.draw(cell.second.shape);
	}
}