#include "Player.h"

Player::Player(bool isH = true) : _hand(nullptr), isHuman(isH) {
  _hand = new Hand();
}

Hand& Player::getHand() {
  return *_hand;
}


Player::~Player(){
  if (_hand){
    delete _hand;
  }
}
