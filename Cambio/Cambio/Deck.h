#pragma once
#include "Hand.h"

class Deck : public Hand {
protected:
	std::vector<Card*> _vOriginal;
public:
	~Deck();
	Deck();
	int deal(Hand& hand);
	void makeDeck();
	void shuffleDeck();
};