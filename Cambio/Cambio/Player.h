#pragma once
#include <iostream>
#include "Hand.h"
class Player {
protected:
	Hand* _hand = NULL;
public:
	Player(bool);
	Hand& getHand();
	 ~Player();
	 bool isHuman = true;
};