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
#include <ranges>

/// <summary>
/// Instantiate set amount of cells
///	all all pos are done via ints so that is the grid system
///		(rand pos is done is a set x, y range)
/// 
/// 
/// </summary>

template <>
struct std::hash<std::pair<int, int>> // jogojapan : https://stackoverflow.com/questions/17016175/c-unordered-map-using-a-custom-class-type-as-the-key
{
	std::size_t operator()(const std::pair<int, int>& k) const
	{
		using std::size_t;
		using std::hash;

		// Compute individual hash values for first,
		// second and third and combine them using XOR
		// and bit shifting:

		return (hash<int>()(k.first)
			^ (hash<int>()(k.second)));
	}
};

class Conways {
public:
	int GameSize = 2;

	Conways();
	~Conways();

	void GetNeighbours();

	void UpdateCells(sf::RenderWindow& window);

	void DisplayCells(sf::RenderWindow& window);

	struct Cell
	{
		sf::RectangleShape shape;
		std::vector<std::pair<int,int>> neighbours = {};

		bool isActive;
		bool previouseActive;

		Cell(float x, float y, sf::Vector2f size, sf::Color color) {
			shape.setSize(size);
			shape.setFillColor(color);
			shape.setPosition(sf::Vector2f(x, y));
			
			isActive = false;
			previouseActive = !isActive;
		}

		operator bool() const
		{
			return isActive;
		}
	};

	std::unordered_map<std::pair<int, int>, Cell> cells = {};
private:

	int gs = GameSize;
	std::vector<std::pair<int, int>> diractions = {
		{0, gs}, {gs, 0}, {0, -gs},
		{-gs, 0}, {gs, gs}, {-gs, -gs},
		{gs, -gs}, {-gs, gs}
	};
};