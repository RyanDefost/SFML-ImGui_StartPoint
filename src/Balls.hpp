#pragma once
#include <vector>
#include <random>
#include <cmath>
#include <unordered_set>
#include <SFML/Graphics.hpp>

struct Ball {
	int index;
	
	sf::CircleShape shape;
	sf::Vector2f velocity;

	std::pair<int, int> position;
	std::pair<int, int> gridPosition;

	Ball(float x, float y, float radius, sf::Color color, float vx, float vy, int index) {
		this->index = index;

		shape.setRadius(radius);
		shape.setPosition(sf::Vector2f(x, y));
		shape.setFillColor(color);
		shape.setOrigin(sf::Vector2f(radius, radius)); // Center origin
		
		velocity = sf::Vector2f(vx, vy);

		gridPosition = {
			std::floor(x / 8),
			std::floor(y / 8)
		};

		position = { x,y };
	}

	bool operator==(const Ball& b) {
		if (b.index == this->index) return true;
		return false;
	}
};

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

class BallGame {
private:
	int gridSize = 8;

	// Create balls
	std::vector<Ball> balls;
	std::unordered_map<std::pair<int,int>, std::vector<Ball>, pair_hash> grid_m = {};

	std::vector<Ball> balls_m = {};

	// Random
	std::random_device rd;
	std::mt19937 gen;
	std::uniform_real_distribution<float> posDist;
	std::uniform_real_distribution<float> velDist;
	std::uniform_int_distribution<int> colorDist;
	std::uniform_real_distribution<float> radiusDist;

public:

	BallGame();
	
	~BallGame();

	void UpdateBalls(const sf::Vector2u& windowSize, float deltaTime);
	
	void HandleBallCollision(Ball& ball);
	void HandleWallCollision(Ball& ball, const sf::Vector2u& windowSize);

	void ReAssignBall(Ball& ball);

	void DrawBalls(sf::RenderWindow& window) const;
};