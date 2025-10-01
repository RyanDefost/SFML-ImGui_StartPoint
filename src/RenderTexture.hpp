#include <SFML/Graphics.hpp>

class RenderTexture {

public:
	RenderTexture(int x, int y);
	~RenderTexture();

	void SetPixel(unsigned int x, unsigned int y, sf::Color color);

	sf::Texture GetTexture();
private:

	//sf::Sprite background;
	sf::Texture baseTexture;
	sf::Texture renderTexture;
};
