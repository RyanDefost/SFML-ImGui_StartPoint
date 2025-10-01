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

class Conways {
public:
	int GameSize = 1;

	Conways();
	~Conways();

	void GetNeighbours();

	void UpdateCells(sf::RenderWindow& window);

	void DisplayCells(sf::RenderWindow& window);

	struct Cell
	{
		sf::Texture displayTexture;

		sf::VertexArray visual;
		sf::Vector2f position;
		std::vector<Cell*> neighbours = {};

		bool isActive;
		bool previouseActive;

		Cell(float x, float y, sf::Vector2f size, sf::Color color) {
			position = {x,y};

			//sf::VertexArray point(sf::PrimitiveType::Points, 1);
			//point[0].position = position;
			//point[0].color = sf::Color::White;
			//visual = point;

			isActive = false;
			previouseActive = true;
		}

		operator bool() const
		{
			return isActive;
		}
	};
	std::unordered_map<std::pair<int, int>, Cell*, pair_hash> grid;
	std::vector<Cell> cells;

private:
	//RenderTexture renderTexture = RenderTexture{100,100};

	int gs = GameSize;
	std::vector<std::pair<int, int>> diractions = {
		{0, gs}, {gs, 0}, {0, -gs},
		{-gs, 0}, {gs, gs}, {-gs, -gs},
		{gs, -gs}, {-gs, gs}
	};
};