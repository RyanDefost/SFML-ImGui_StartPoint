#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <imgui.h>
#include <imgui-SFML.h>
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <random>

/// <summary>
/// Instantiate set amount of cells
///	all all pos are done via ints so that is the grid system
///		(rand pos is done is a set x, y range)
/// 
/// 
/// </summary>
class Conways {
public:

	Conways();
	~Conways();

	void DisplayCells(sf::RenderWindow& window);

private:
	std::unordered_map<std::pair<int, int>, Cell> cells;
};
