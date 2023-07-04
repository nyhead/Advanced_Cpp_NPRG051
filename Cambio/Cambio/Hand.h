#pragma once
#include <vector>
#include <string>
#include "Card.h"

class Hand {
protected:
	std::vector<Card*> _vCards;
public:
	Hand();
	~Hand();
	void switchCards(Hand& to, int index, int myIndex);
	int size();
	int getScore();

	Card* getCard(int index);
	void setCard(int, Card*);
	Card* operator[](int index);
	void addCard(Card* card);
	void addCard(int index, Card* card);
	Card* popCard();
};
