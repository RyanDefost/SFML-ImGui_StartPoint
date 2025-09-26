#pragma once
#include <vector>
#include <random>
#include <cmath>
#include <SFML/Graphics.hpp>

struct Ball {
	sf::CircleShape shape;
	sf::Vector2f velocity;

	Ball(float x, float y, float radius, sf::Color color, float vx, float vy) {

		shape.setRadius(radius);
		shape.setPosition(sf::Vector2f(x, y));
		shape.setFillColor(color);
		shape.setOrigin(sf::Vector2f(radius, radius)); // Center origin
		
		velocity = sf::Vector2f(vx, vy);
	}
};

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

class BallGame {
private:
	// Create balls
	std::vector<Ball> balls;
	std::unordered_map<std::pair<int,int>, Ball> balls_m = {};

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
	
	void DrawBalls(sf::RenderWindow& window) const;
};