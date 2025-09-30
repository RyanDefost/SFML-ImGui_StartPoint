#include "Balls.hpp"
#include <iostream>

BallGame::BallGame() {
	gen = std::mt19937(rd());
	posDist = std::uniform_real_distribution<float>(5.0f, 795.0f);
	velDist = std::uniform_real_distribution<float>(-200.0f, 200.0f);
	colorDist = std::uniform_int_distribution<int>(0, 255);
	radiusDist = std::uniform_real_distribution<float>(2.5f, 2.5f);

	// Generate random balls
	for (int i = 0; i < 2500; ++i) {
		sf::Color randomColor(colorDist(gen), colorDist(gen), colorDist(gen));

		Ball currentBall = {
			posDist(gen), posDist(gen), // position
			radiusDist(gen), // radius
			randomColor, // color
			velDist(gen), velDist(gen), // velocity
			i
		};

		std::pair<int, int> currentGridPosition = {
			std::floor(currentBall.position.first / gridSize),
			std::floor(currentBall.position.second / gridSize) 
		};
		
		auto newCurrent = balls_m.emplace_back(currentBall);
		auto findit = grid_m.find(currentGridPosition);
		if (findit != grid_m.end()) {
			grid_m.at(currentGridPosition).emplace_back(currentBall);
		}
		else {
			grid_m.insert({ currentGridPosition, {newCurrent} });
		}
	}
}

BallGame::~BallGame() {
	
}

void BallGame::UpdateBalls(const sf::Vector2u& windowSize, float deltaTime) {
	for (auto& ball : balls_m) {
		ball.shape.move(ball.velocity * deltaTime);
	}

	// Handle ball-to-ball collisions
	for (auto& ball : balls_m) {
		auto currentCell = grid_m.at(ball.gridPosition);

		for (auto& otherBall : currentCell) {
			Ball& ball1 = ball;
			Ball& ball2 = otherBall;

			if (ball1 == ball2) continue;

			sf::Vector2f pos1 = ball1.shape.getPosition();
			sf::Vector2f pos2 = ball2.shape.getPosition();

			float radius1 = ball1.shape.getRadius();
			float radius2 = ball2.shape.getRadius();

			// Calculate distance between centers
			sf::Vector2f delta =    pos2 - pos1;
			float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
			float minDistance = radius1 + radius2;

			if (distance < minDistance && distance > 0) {
				// Normalize collision vector
				sf::Vector2f normal = delta / distance;

				// Separate balls to prevent overlap
				float overlap = minDistance - distance;
				sf::Vector2f separation = normal * (overlap * 0.5f);

				ball1.shape.setPosition(pos1 - separation);
				ball2.shape.setPosition(pos2 + separation);

				// Calculate relative velocity
				sf::Vector2f relativeVel = ball2.velocity - ball1.velocity;
				float velAlongNormal = relativeVel.x * normal.x + relativeVel.y * normal.y;

				// Don't resolve if velocities are separating
				if (velAlongNormal > 0) continue;

				// Apply collision response (elastic collision)
				float restitution = 0.0f; // Bounce factor (0 = no bounce, 1 = perfect bounce)
				float impulse = -(1 + restitution) * velAlongNormal;

				// Assume equal mass for simplicity
				sf::Vector2f impulseVector = impulse * normal;

				ball1.velocity -= impulseVector;
				ball2.velocity += impulseVector;
			}
		}

		grid_m.at(ball.gridPosition).erase(remove(
			grid_m.at(ball.gridPosition).begin(),
			grid_m.at(ball.gridPosition).end(), ball),
			grid_m.at(ball.gridPosition).end()
		);

		//Update it's current grid position
		ball.position = { ball.shape.getPosition().x, ball.shape.getPosition().y };
		ball.gridPosition = {
			std::floor(ball.position.first / gridSize),
			std::floor(ball.position.second / gridSize)
		};

		auto findit = grid_m.find(ball.gridPosition);
		if (findit != grid_m.end()) {
			grid_m.at(ball.gridPosition).emplace_back(ball);
		}
		else {
			grid_m.insert({ ball.gridPosition, {ball} });
		}
	}

	// Handle wall collisions
	for (auto& ball : balls_m) {
		sf::Vector2f pos = ball.shape.getPosition();
		float radius = ball.shape.getRadius();

		// Bounce off walls
		if (pos.x - radius <= 0 || pos.x + radius >= windowSize.x) {
			ball.velocity.x = -ball.velocity.x;

			// Clamp position to prevent sticking
			if (pos.x - radius <= 0) {
				ball.shape.setPosition(sf::Vector2f(radius, pos.y));
			}
			else {
				ball.shape.setPosition(sf::Vector2f(windowSize.x - radius, pos.y));
			}
		}

		if (pos.y - radius <= 0 || pos.y + radius >= windowSize.y) {
			ball.velocity.y = -ball.velocity.y;

			// Clamp position to prevent sticking
			if (pos.y - radius <= 0) {
				ball.shape.setPosition(sf::Vector2f(pos.x, radius));
			}
			else {
				ball.shape.setPosition(sf::Vector2f(pos.x, windowSize.y -
					radius));
			}
		}
	}
}

void BallGame::DrawBalls(sf::RenderWindow& window) const
{
	for (const auto& ball : balls_m) {
		window.draw(ball.shape);
	}
}


