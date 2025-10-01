#include <iostream>
#include <SFML/Graphics.hpp>
#include "RenderTexture.hpp"


RenderTexture::RenderTexture(unsigned int x, unsigned int y) {

	if (!baseTexture.loadFromFile("../Assets/background.png")) {
		std::cout << "Error loading texture" << std::endl;
	}

	if (!baseTexture.resize({ x, y })) {
		std::cout << "Error texture could not be re-sized" << std::endl;
	}

	ClearRenderState();
}

RenderTexture::~RenderTexture() {}

void RenderTexture::ClearRenderState()
{
	currentState = baseTexture.copyToImage();
}

void RenderTexture::SetPixel(unsigned int x, unsigned int y, sf::Color color) {
	
	currentState.setPixel({ x, y }, color);
}

void RenderTexture::DisplayVisual(sf::RenderWindow& window) {

	if (!renderTexture.loadFromImage(currentState)) {
		std::cout << "Error texture could not be loaded from image" << std::endl;
	}

	sf::Sprite visual(renderTexture);
	visual.setScale({textureScale, textureScale});
	window.draw(visual);
}

