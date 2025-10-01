#include <iostream>
#include <SFML/Graphics.hpp>
#include "RenderTexture.hpp"


RenderTexture::RenderTexture(unsigned int x, unsigned int y) {

	if (!baseTexture.loadFromFile("../Assets/background.png")) {

		std::cout << "Error loading texture" << std::endl;
	}
	baseTexture.resize({ x,y });
	currentState = baseTexture.copyToImage();
}

RenderTexture::~RenderTexture() {}

void RenderTexture::SetPixel(unsigned int x, unsigned int y, sf::Color color) {
	sf::Vector2u pos = {
		(unsigned int)x,
		(unsigned int)y };

	currentState.setPixel(pos, color);
}

void RenderTexture::DisplayVisual(sf::RenderWindow& window) {
	renderTexture.loadFromImage(currentState);

	sf::Sprite visual(renderTexture);
	window.draw(visual);
}

