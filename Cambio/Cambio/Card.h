#pragma once
#include <string>

enum Suite { DIAMONDS, CLUBS, HEARTS, SPADES };
enum Color { BLACK, RED };

class Card {

public:
	Card(int rank, Suite suite, Color color, bool hidden = true);
	int getRank();
	int getValue();
	Suite getSuite();
	Color getColor();
	std::string toString();
	bool getHidden();
	void setHidden(bool hidden);
protected:
	int _rank;
	int _value = INT32_MIN;
	Suite _suite;
	Color _color;
	bool _hidden;
	std::string getRankString();
	std::string getSuiteString();

};