#include <SFML/Graphics.hpp>

class RenderTexture {

public:
	RenderTexture(unsigned int x, unsigned int y);
	~RenderTexture();

	void ClearRenderState();

	void SetPixel(unsigned int x, unsigned int y, sf::Color color);

	void DisplayVisual(sf::RenderWindow& window);
private:
	float textureScale = 0.5;

	sf::Texture baseTexture;
	sf::Texture renderTexture;

	sf::Image currentState;
};
