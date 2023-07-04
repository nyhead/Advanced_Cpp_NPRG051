#pragma once

#ifndef VIEWINPUT_HPP_
#define VIEWINPUT_HPP_

#include <iostream>
#include <unordered_map>
#include "Player.h"
#include "SFML/Graphics.hpp"
#include <sstream>
#include <utility>

class ViewInput {
public:

    ViewInput(sf::RenderWindow& w, int width, int height, std::unordered_map<std::string, sf::Sprite>& map, int number,sf::Sprite bg, sf::Sprite hddn, sf::Text cmb,sf::Font fnt) : _w(&w), _width(width), _height(height) {
        positions = std::vector<std::vector<sf::Sprite>>(number);
        hidden = hddn;
        background = bg;
        font = fnt;
        callCambioText = cmb;
        mp = map;
    }
    ~ViewInput() {}

    void endMenu(int);
    bool startMenu(sf::Text);
	void printCard(Card& c);
	void update(std::vector<Player*>&);
    void setPositions(std::vector<Player*>&);
	bool InputDecideCallCambio();
	int InputDecideCardPlay(int, int);
	int InputChooseOtherPlayer(Player&);
    //this function returns a pair whose first and second elements are player's index and its card index respectively
	std::pair<int,int> InputChooseOtherPlayerCard(int, bool);

	sf::RenderWindow* _w;
	std::unordered_map<std::string, sf::Sprite> mp;
    sf::Sprite hidden;
    sf::Sprite background;
    sf::Text callCambioText;
    sf::Font font;
    std::vector<std::vector<sf::Sprite>> positions;
    int _width, _height;
};

inline std::string getSuiteString(Suite suite) {
    switch (suite) {
    case HEARTS: return "H";
    case DIAMONDS: return "D";
    case SPADES: return "S";
    case CLUBS: return "C";
    default: return "Joker";
    }
}

inline std::string getRankString(int rank) {
    std::string num = "JOKER";
    int number = rank;
    if (number == 1) {
        num = "A";
    }
    else if (number < 11) {
        std::stringstream ss;
        ss << number;
        num = ss.str();
    }
    else if (number == 11) {
        num = "J";
    }
    else if (number == 12) {
        num = "Q";
    }
    else if (number == 13) {
        num = "K";
    }
    return num;
}
#endif