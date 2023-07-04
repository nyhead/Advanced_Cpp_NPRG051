#include <string>
#include <ctime>
#include <map>
#include "Hand.h"
#include "Deck.h"
#include <cstdlib>

Deck::Deck() : Hand(){ }

int Deck::deal(Hand &hand){
    Card* card = popCard();
    hand.addCard(card);
    return card->getRank();
}


Deck::~Deck() {
  for (int i = _vOriginal.size() - 1; i >= 0; --i) {
    delete _vOriginal[i];
  }
}

void Deck::makeDeck() {
  for(int suite=0; suite<= 3; suite++){
      for (int color = 0; color <= 2; color++) {
          for (int rank = 1; rank <= 13; rank++) {
              addCard(new Card(rank, (Suite)suite, (Color)color));
              //addCard(new Card(13, (Suite)suite, BLACK));
          }
      }
  }
  _vOriginal = _vCards;
}

void Deck::shuffleDeck() {
    std::srand(unsigned(std::time(NULL)));
    int size = _vCards.size();
    std::vector<Card*> old = _vCards;
    _vCards.clear();
    while (_vCards.size() < size) {
        int place = (std::rand() % size);
        Card* card = old[place];
        old[place] = card;
        _vCards.push_back(card);
    }
}
