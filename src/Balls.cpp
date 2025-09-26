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
			velDist(gen), velDist(gen) // velocity
		};

		balls.emplace_back(
			posDist(gen), posDist(gen), // position
			radiusDist(gen), // radius
			randomColor, // color
			velDist(gen), velDist(gen) // velocity
		);
		std::pair<int, int> currentPosition = { 1,1 };


		balls_m.insert({ currentPosition, currentBall });
	}

	std::for_each(balls_m.begin(), balls_m.end(), [](std::pair<std::pair<int,int>, Ball> m) {
		std::cout << m.first.first << "," << "\n";
	});
}

BallGame::~BallGame() {
	
}

void BallGame::UpdateBalls(const sf::Vector2u& windowSize, float deltaTime) {
	// Update positions
	for (auto& ball : balls) {
		ball.shape.move(ball.velocity * deltaTime);
	}

	// Handle ball-to-ball collisions
	for (size_t i = 0; i < balls.size(); ++i) {
		for (size_t j = i + 1; j < balls.size(); ++j) {
			Ball& ball1 = balls[i];
			Ball& ball2 = balls[j];

			sf::Vector2f pos1 = ball1.shape.getPosition();
			sf::Vector2f pos2 = ball2.shape.getPosition();
			
			float radius1 = ball1.shape.getRadius();
			float radius2 = ball2.shape.getRadius();
			
			// Calculate distance between centers
			sf::Vector2f delta = pos2 - pos1;
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
	}
	// Handle wall collisions
	for (auto& ball : balls) {
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
	for (const auto& ball : balls) {
		window.draw(ball.shape);
	}
}


