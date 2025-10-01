#include <algorithm>
#include <functional>
#include <numeric>
#include <unordered_map>
#include "Conways.hpp"
#include "RenderTexture.hpp"
#include <iostream>

Conways::Conways()
{
	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<float> random;

	int Xsize = 1000;
	int Ysize = 1000;

	cells.reserve(Xsize * Ysize);
	grid.reserve(Xsize * Ysize);

	for (int i = 0; i < Xsize; i++)
	{
		for (size_t j = 0; j < Ysize; j++)
		{
			std::pair<int, int> position = {
				(int)i, (int)j
			};
			
			Cell currentCell = Cell(position);

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
				cell.position.first + diraction.first, 
				cell.position.second + diraction.second
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
	RenderTexture renderTexture{ 1000,1000 };

	for (auto& cell : cells) {
		cell.previouseActive = cell.isActive;
		
		if (!cell) continue;
		renderTexture.SetPixel(cell.position.first, cell.position.second, sf::Color::White);
	}

	renderTexture.DisplayVisual(window);
}