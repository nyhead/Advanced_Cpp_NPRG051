#include <iostream>
#include "Game.h"
#include "SFML/Graphics.hpp"

#include <unordered_map>

int main() {
	const int width = 1280, height = 720;
	sf::RenderWindow window(sf::VideoMode(width, height), "SFML works!");

	sf::Texture BackgroundTexture;
	sf::Sprite background;
	sf::Vector2u TextureSize;  //Added to store texture size.
	sf::Vector2u WindowSize;   //Added to store window size.

	if (!BackgroundTexture.loadFromFile("background.jpg"))
	{
		return -1;
	}
	else
	{
		TextureSize = BackgroundTexture.getSize(); //Get size of texture.
		WindowSize = window.getSize();             //Get size of window.

		float ScaleX = (float)WindowSize.x / TextureSize.x;
		float ScaleY = (float)WindowSize.y / TextureSize.y;     //Calculate scale.

		background.setTexture(BackgroundTexture);
		background.setScale(ScaleX, ScaleY);      //Set scale.  
	}


	sf::Texture texture;
	if (!texture.loadFromFile("cards1.jpeg"))
	{
	}
	std::unordered_map<std::string, sf::Sprite> mp;
	const int spriteHeight = 110, spriteWidth = 65;
	int rectTop = 0, rectLeft = 0;
	int sIndex = 0, cIndex = 1;
	Suite sOrder[4] = { HEARTS, CLUBS, DIAMONDS, SPADES };
	for (int y = 0; y < texture.getSize().y; y += 100) {
		for (int x = 0; x < texture.getSize().x; x += 79) {
			std::string cardName = VIEWINPUT_HPP_::getRankString(cIndex);
			std::string suitName = VIEWINPUT_HPP_::getSuiteString(sOrder[sIndex]);
			sf::Sprite sp(texture, sf::IntRect(x, y, spriteWidth, spriteHeight));
			sp.setPosition(sf::Vector2f(x-350, y-100));
			mp[suitName + cardName] = sp;

			//x += xmargins;
			cIndex = (cIndex + 1) % 14;
			//break;
		}

		sIndex = (sIndex + 1) % 4;
		cIndex = 1;
		//y += ymargins;
	}
	sf::Texture hiddenTx;
	hiddenTx.loadFromFile("hidden.png");
	sf::Sprite rectangle(hiddenTx, sf::IntRect(0,0,spriteWidth, spriteHeight));
	//rectangle.setFillColor(sf::Color(128, 128, 128));
	//rectangle.setTexture(hiddenTx)
	sf::Font font;
	font.loadFromFile("Poker.ttf");
	sf::Text text;
	text.setFont(font);
	text.setString("Play");
	text.setCharacterSize(200);
	sf::FloatRect textRect = text.getLocalBounds();
	text.setOrigin(textRect.left + textRect.width / 2.0f,
		textRect.top + textRect.height / 2.0f);
	text.setPosition(sf::Vector2f(width / 2.0f, height / 2.0f));

	sf::Text callCambioText;
	callCambioText.setFont(font);
	callCambioText.setString("Call Cambio?");
	callCambioText.setCharacterSize(32);
	textRect = callCambioText.getLocalBounds();
	callCambioText.setOrigin(textRect.left + textRect.width / 2.0f,
		textRect.top + textRect.height / 2.0f);
	callCambioText.setPosition(sf::Vector2f(width / 2.0f, 20));


	ViewInput v(window, width, height, mp, 2, background, rectangle, callCambioText, font);
	Game g(v);
	g.createGame();

	bool menu = true;
	bool endMenu = false;
	int winner = -1;
	while (v._w->isOpen()) {


		if (menu) {
			if (v.startMenu(text)) {
				menu = false;
			}
		}
		else if (!menu && g.isActive()) {
			winner = g.gameLoop();
			endMenu = true;
		}
		else if (endMenu) {
			
			v.endMenu(winner);
			endMenu = false;
		}

		sf::Event event;
		while (v._w->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				v._w->close();
		}
		
	}
	return 0;
}