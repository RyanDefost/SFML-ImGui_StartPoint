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
/// 
class Conways {
public:

	Conways();
	~Conways();

	void DisplayCells(sf::RenderWindow& window);

	struct Cell
	{
		sf::RectangleShape shape;
		sf::Vector2f position;

		Cell(float x, float y) {
			position = { x,y };
		}

		bool operator==(const Cell& other) {
			return (position == other.position);
		}
	};
	
	struct pair_hash {
		template <class T1, class T2>
		std::size_t operator () (const std::pair<T1, T2>& p) const {
			auto h1 = std::hash<T1>{}(p.first);
			auto h2 = std::hash<T2>{}(p.second);

			// Mainly for demonstration purposes, i.e. works but is overly simple
			// In the real world, use sth. like boost.hash_combine
			return h1 ^ h2;
		}
	};

	std::unordered_map<std::pair<int, int>, Cell> cells = {};

private:

};