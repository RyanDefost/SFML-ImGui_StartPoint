#include <SFML/Graphics.hpp>

class RenderTexture {

public:
	RenderTexture(unsigned int x, unsigned int y);
	~RenderTexture();

	void SetPixel(unsigned int x, unsigned int y, sf::Color color);

	void DisplayVisual(sf::RenderWindow& window);
private:

	//sf::Sprite background;
	sf::Image currentState;

	sf::Texture baseTexture;
	sf::Texture renderTexture;
};
