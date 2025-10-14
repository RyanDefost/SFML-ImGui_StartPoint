#pragma once
#include <random>
#include <SFML/Graphics.hpp>
// Base Class
class GameObject {
public:
	virtual ~GameObject() = default;
	virtual void update(float deltaTime) = 0;
	virtual void render(sf::RenderWindow& window) = 0;
	virtual bool isAlive() const = 0;
protected:
	bool active;
	double creationTime;
	int id;
	float x, y;
};

// Particle Class, inherits from GameObject
class Particle : public GameObject {
private:
	bool isDying;
	sf::Vector2f position;
	double lifetime;
	sf::Vector2f velocity;
	bool hasGravity;
	sf::Color color;
	char effectType;
	sf::Vector2f acceleration;
	double maxLifetime;
	sf::CircleShape* shape;
	bool isVisible;
	float mass;
	sf::Vector2f lastPosition;
	std::string debugName;
	double lastUpdateTime;
	bool collisionEnabled;
	float radius;

public:
	Particle(sf::Vector2f pos, sf::Vector2f vel, sf::Color col, float life)
		: isDying(false), position(pos), lifetime(life), velocity(vel),
		hasGravity(true), color(col), effectType('n'), maxLifetime(life),
		isVisible(true), mass(1.0f), collisionEnabled(true), radius(2.0f) {
		// Shape
		shape = new sf::CircleShape(radius);
		shape->setFillColor(color);
		shape->setPosition(position);
		// Debug info
		static int counter = 0;
		debugName = "Particle_" + std::to_string(counter++);
		// Other data
		active = true;
		creationTime = std::chrono::duration<double>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();
		id = counter;
		lastPosition = position;
		lastUpdateTime = creationTime;
		acceleration = sf::Vector2f(0, 98.1f); // Gravity
	}

	~Particle() override {
	}

	void update(float deltaTime) override {
		if (!active) return;
		// Timing Update
		auto now = std::chrono::high_resolution_clock::now();
		double currentTime =
			std::chrono::duration<double>(now.time_since_epoch()).count();
		lastUpdateTime = currentTime;
		// Store last position
		lastPosition = position;
		// Physics calculations
		if (hasGravity) {
			velocity += acceleration * deltaTime;
		}
		position += velocity * deltaTime;
		// Color calculations
		float alpha = static_cast<float>(lifetime / maxLifetime);
		color.a = static_cast<uint8_t>(alpha * 255);
		// Update shape properties every frame
		shape->setPosition(position);
		shape->setFillColor(color);
		// Lifetime management
		lifetime -= deltaTime;
		if (lifetime <= 0) {
			isDying = true;
			active = false;
		}
		// Bounds checking
		if (position.x < 0 || position.x > 800 || position.y < 0 || position.y >
			800) {
			if (collisionEnabled) {
				velocity *= -0.8f;
				position.x = std::max(0.0f, std::min(800.0f, position.x));
				position.y = std::max(0.0f, std::min(800.0f, position.y));
			}
		}
	}

	void render(sf::RenderWindow& window) override {
		if (isVisible && active) {
			window.draw(*shape);
		}
	}

	bool isAlive() const override {
		return active && !isDying;
	}
	// Expensive getter functions called frequently
	sf::Vector2f getPosition() const { return position; }
	sf::Vector2f getVelocity() const { return velocity; }
	std::string getDebugName() const { return debugName; }
	double getCreationTime() const { return creationTime; }
};

// Inefficient particle system manager
class ParticleSystem {
private:
	// Particle Container
	std::vector<std::unique_ptr<GameObject>> particles;
	// Separate vectors per property
	std::vector<sf::Vector2f> positions;
	std::vector<sf::Vector2f> velocities;
	std::vector<sf::Color> colors;
	std::vector<bool> aliveFlags;
	sf::RenderWindow* window;
	std::mt19937 rng;

public:
	ParticleSystem(sf::RenderWindow* win) : window(win), rng(std::random_device{}
		()) {
		// Pre-allocation
		particles.reserve(100);
	}

	void spawnParticles(int count, sf::Vector2f origin) {
		std::uniform_real_distribution<float> angleDist(0, 2 * 3.14159f);
		std::uniform_real_distribution<float> speedDist(50, 200);
		std::uniform_int_distribution<int> colorDist(0, 255);
		std::uniform_real_distribution<float> lifeDist(1.0f, 5.0f);
		for (int i = 0; i < count; ++i) {
			float angle = angleDist(rng);
			float speed = speedDist(rng);
			sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) *
				speed);
			sf::Color color(colorDist(rng), colorDist(rng), colorDist(rng), 255);
			float lifetime = lifeDist(rng);
			// Create particle
			auto particle = std::make_unique<Particle>(origin, velocity, color,
				lifetime);
			// Store in containers
			particles.push_back(std::move(particle));
		}
	}

	void update(float deltaTime) {
		// Iterator
		for (auto it = particles.begin(); it != particles.end();) {
			(*it)->update(deltaTime);
			if (!(*it)->isAlive()) {
				it = particles.erase(it);
			}
			else {
				++it;
			}
		}
	}

	void render() {
		// Render all particles
		for (auto& particle : particles) {
			particle->render(*window);
		}
	}

	size_t getParticleCount() const {
		return particles.size();
	}
};