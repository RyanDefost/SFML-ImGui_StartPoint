#pragma once
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>

struct Ball {
	sf::CircleShape shape;
	sf::Vector2f velocity;
	int number;

	Ball(int numb, float x, float y, float radius, sf::Color color, float vx, float vy) {
		shape.setRadius(radius);
		shape.setPosition(sf::Vector2f(x, y));
		shape.setFillColor(color);
		shape.setOrigin(sf::Vector2f(radius, radius)); // Center origin
		velocity = sf::Vector2f(vx, vy);

		number = numb;
	}

	/*bool operator==(const Ball &other) const {
		return (number == other.number);
	}*/
};

template <>
struct std::hash<std::pair<int,int>> // jogojapan : https://stackoverflow.com/questions/17016175/c-unordered-map-using-a-custom-class-type-as-the-key
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

class BallGame {
private:
	// Create balls
	//std::unordered_map<Ball, std::pair<int, int>> unOrderdBalls;
	std::unordered_map<std::pair<int, int>, std::vector<Ball>> unOrderdBalls;

	//std::vector<Ball> balls;
	std::random_device rd;
	std::mt19937 gen;
	std::uniform_real_distribution<float> posDist;
	std::uniform_real_distribution<float> velDist;
	std::uniform_int_distribution<int> colorDist;
	std::uniform_real_distribution<float> radiusDist;

public:
	BallGame::BallGame() {
		gen = std::mt19937(rd());
		posDist = std::uniform_real_distribution<float>(5.0f, 795.0f);
		velDist = std::uniform_real_distribution<float>(-200.0f, 200.0f);
		colorDist = std::uniform_int_distribution<int>(0, 255);
		radiusDist = std::uniform_real_distribution<float>(2.5f, 2.5f);
		// Generate random balls
		for (int i = 0; i < 2500; ++i) {
			sf::Color randomColor(colorDist(gen), colorDist(gen), colorDist(gen));
			/*balls.emplace_back(
				posDist(gen), posDist(gen), // position
				radiusDist(gen), // radius
				randomColor, // color
				velDist(gen), velDist(gen) // velocity
			);*/

			Ball currentBall = {
				i, // Id number
				posDist(gen), posDist(gen), //Position
				radiusDist(gen), // radius
				randomColor, // color
				velDist(gen), velDist(gen) // velocity
			};

			std::pair<int, int> currentGridPosition = {
				(currentBall.shape.getPosition().x/5),
				(currentBall.shape.getPosition().y/5)
			};

			unOrderdBalls.insert({ currentGridPosition, {currentBall} });

		}
	}

	void NewUpdateBalls(const sf::Vector2u& windowSize, float deltaTime) {

		// Update positions
		for (auto& ballRegion : unOrderdBalls) {
			for (auto& ball : ballRegion.second) {

				ball.shape.move(ball.velocity * deltaTime);
			}
		}

		for (auto& ballRegion : unOrderdBalls) {

			for (auto& ball : ballRegion.second) {
				for (auto& collidingBall : ballRegion.second) {
					if (ball.number == collidingBall.number) continue;

					Ball& ball1 = ball;
					Ball& ball2 = collidingBall;

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
							//
						ball2.shape.setPosition(pos2 + separation);

						// Calculate relative velocity
						sf::Vector2f relativeVel = ball2.velocity - ball1.velocity;
						float velAlongNormal = relativeVel.x * normal.x + relativeVel.y
							* normal.y;

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
		}

		// Handle wall collisions
		for (auto& ballRegion : unOrderdBalls) {
			for (auto& ball : ballRegion.second) {
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
						ball.shape.setPosition(sf::Vector2f(windowSize.x - radius,
							pos.y));
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
	}
	/*void NewUpdateBalls(const sf::Vector2u& windowSize, float deltaTime) {
		// Update positions
		for (auto& ballValue : unOrderdBalls) {
			Ball currentBall = ballValue.first;

			currentBall.shape.move(currentBall.velocity * deltaTime);
		}

		// Handle ball-to-ball collisions
		for (auto& ballValue : unOrderdBalls) {
			auto currentBall = ballValue.first;
			auto currentGridPosition = ballValue.second;

			for (auto& collidingBallValue : unOrderdBalls) {
				if (ballValue.second != collidingBallValue.second) continue;
				
				auto collidingBall = collidingBallValue.first;
				auto collidingGridPosition = collidingBallValue.second;

				sf::Vector2f pos1 = currentBall.shape.getPosition();
				sf::Vector2f pos2 = collidingBall.shape.getPosition();

				float radius1 = currentBall.shape.getRadius();
				float radius2 = collidingBall.shape.getRadius();

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

					currentBall.shape.setPosition(pos1 - separation);
					collidingBall.shape.setPosition(pos2 + separation);

					// Calculate relative velocity
					sf::Vector2f relativeVel = collidingBall.velocity - currentBall.velocity;
					float velAlongNormal = relativeVel.x * normal.x + relativeVel.y
						* normal.y;

					// Don't resolve if velocities are separating
					if (velAlongNormal > 0) continue;

					// Apply collision response (elastic collision)
					float restitution = 0.0f; // Bounce factor (0 = no bounce, 1 = perfect bounce)
					float impulse = -(1 + restitution) * velAlongNormal;

					// Assume equal mass for simplicity
					sf::Vector2f impulseVector = impulse * normal;

					currentBall.velocity -= impulseVector;
					collidingBall.velocity += impulseVector;
				}
			}
		}

		// Handle wall collisions
		for (auto& ballValue : unOrderdBalls) {
			auto currentBall = ballValue.first;
			auto currentGridPosition = ballValue.second;

			sf::Vector2f pos = currentBall.shape.getPosition();
			float radius = currentBall.shape.getRadius();

			// Bounce off walls
			if (pos.x - radius <= 0 || pos.x + radius >= windowSize.x) {

				currentBall.velocity.x = -currentBall.velocity.x;

				// Clamp position to prevent sticking
				if (pos.x - radius <= 0) {
					currentBall.shape.setPosition(sf::Vector2f(radius, pos.y));
				}
				else {
					currentBall.shape.setPosition(sf::Vector2f(windowSize.x - radius, pos.y));
				}
			}

			if (pos.y - radius <= 0 || pos.y + radius >= windowSize.y) {
				currentBall.velocity.y = -currentBall.velocity.y;

				// Clamp position to prevent sticking
				if (pos.y - radius <= 0) {
					currentBall.shape.setPosition(sf::Vector2f(pos.x, radius));
				}
				else {
					currentBall.shape.setPosition(sf::Vector2f(pos.x, windowSize.y - radius));
				}
			}
		}
	}*/

	/*void updateBalls(const sf::Vector2u& windowSize, float deltaTime) {

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
					float velAlongNormal = relativeVel.x * normal.x + relativeVel.y
						* normal.y;

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
					ball.shape.setPosition(sf::Vector2f(windowSize.x - radius,
						pos.y));
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
	}*/

	void drawBalls(sf::RenderWindow& window) const {
		for (auto& ballRegion : unOrderdBalls) {
			for (auto& ball : ballRegion.second) {
				window.draw(ball.shape);
			}
		}
	}
};