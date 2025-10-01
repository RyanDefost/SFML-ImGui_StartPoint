#include <iostream>
#include <SFML/Graphics.hpp>
#include "RenderTexture.hpp"


RenderTexture::RenderTexture(int x, int y) {

	if (!baseTexture.loadFromFile("../Assets/background.png")) {

		std::cout << "Error loading texture" << std::endl;
	}
	baseTexture.resize({ x,y });
}

RenderTexture::~RenderTexture() {}

void RenderTexture::SetPixel(unsigned int x, unsigned int y, sf::Color color) {
	sf::Image image = baseTexture.copyToImage();

	sf::Vector2u test = { x,y };
	image.setPixel(test, color);
	renderTexture.loadFromImage(image);
}

sf::Texture RenderTexture::GetTexture() {
	return renderTexture;
}

