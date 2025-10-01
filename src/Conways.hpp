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

// Only for pairs of std::hash-able types for simplicity.
// You can of course template this struct to allow other hash functions
struct pair_hash { //Baum mit Augen: https://stackoverflow.com/questions/32685540/why-cant-i-compile-an-unordered-map-with-a-pair-as-key
	template <class T1, class T2>
	std::size_t operator () (const std::pair<T1, T2>& p) const {
		auto h1 = std::hash<T1>{}(p.first);
		auto h2 = std::hash<T2>{}(p.second);

		// Mainly for demonstration purposes, i.e. works but is overly simple
		// In the real world, use sth. like boost.hash_combine
		return h1 ^ h2;
	}
};

struct Cell
{
	std::pair<int, int> position;
	std::vector<Cell*> neighbours = {};

	sf::VertexArray visual; // ADDING THESE ADDS +-10FPS !?
	sf::VertexArray visual2; // WHY???

	bool isActive;
	bool previouseActive;

	Cell(std::pair<int, int> pos) {
		position = pos;

		isActive = false;
		previouseActive = true;
	}

	operator bool() const { return isActive; }
};

class Conways {
public:
	Conways();
	~Conways();

	void GetNeighbours();

	void UpdateCells(sf::RenderWindow& window);

	void DisplayCells(sf::RenderWindow& window);

	
	std::unordered_map<std::pair<int, int>, Cell*, pair_hash> grid;
	std::vector<Cell> cells;

private:
	std::vector<std::pair<int, int>> diractions = {
		{0 , 1}, {1 , 0}, {0 , -1},
		{-1, 0}, {1 , 1}, {-1, -1},
		{1 ,-1}, {-1, 1}
	};
};