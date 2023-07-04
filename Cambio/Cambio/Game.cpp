#include "Game.h"
#include <algorithm>
#include <thread>
#include <chrono>

Game::Game(ViewInput& f){
  _isActive = true;
  _playerTurn = 0;
  _gameTurn = 0;
  _Players = 2;
  _front = &f;
}

void Game::createGame() {
  createPlayers();
  makeDeck();
  _deck.shuffleDeck();
  dealCards();

  _front->setPositions(_vPlayers);


}


bool Game::isActive() {
  return _isActive;
}

void Game::setActive(bool active) {
  _isActive = active;
}

int Game::gameLoop() {
  while (isActive()) {
    _front->update(_vPlayers);

 
        

    _playerTurn = _gameTurn % _vPlayers.size();
    if (_vPlayers[_playerTurn]->isHuman)
        playerAction(*_vPlayers[_playerTurn]);
    else
        aiAction(*_vPlayers[_playerTurn]);
    
    _gameTurn++;
  }

  return this->deduceWinner();
}

void Game::aiAction(Player& player) {
    _deck.deal(player.getHand());
    _front->setPositions(_vPlayers);

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    _front->update(_vPlayers);

    auto c = player.getHand().popCard();
    _front->positions[_playerTurn].pop_back();
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    _front->update(_vPlayers);

    switch (c->getRank()) {
    case 7:
    case 8: {
        int yourCardIndex = rand() % player.getHand().size();
        player.getHand()[yourCardIndex]->setHidden(false);

        _front->update(_vPlayers);
        player.getHand()[yourCardIndex]->setHidden(true);
    }
          break;
    case 9:
    case 10: {
        //auto [playerIndex, cardIndex] = _front->InputChooseOtherPlayerCard(_playerTurn, true);
        
        int playerIndex = (_playerTurn - 1) % this->getNumberOfPlayers();
        int cardIndex = rand() % _vPlayers[playerIndex]->getHand().size();

        _vPlayers[playerIndex]->getHand()[cardIndex]->setHidden(false);
        _front->update(_vPlayers);
        player.getHand()[cardIndex]->setHidden(true);
    }
           break;
    }
}

void Game::playerAction(Player& player) {
    bool isCambio = _front->InputDecideCallCambio();
    if (isCambio) {
        int w = this->deduceWinner();
        std::cout << "Cambio called, winner is " << w << std::endl;
        this->setActive(false);
        return;
    }


    _deck.deal(player.getHand());
    _front->setPositions(_vPlayers);
    _front->update(_vPlayers);
    int isPlayCardInput = _front->InputDecideCardPlay(_playerTurn, _vPlayers[_playerTurn]->getHand().size()-1);

    if (isPlayCardInput == 1) {
        auto c = player.getHand().popCard();
        _front->positions[_playerTurn].pop_back();
        _front->update(_vPlayers);
        switch (c->getRank()) {
            case 7:
            case 8: {
                auto [playerIndex, cardIndex] = _front->InputChooseOtherPlayerCard(_playerTurn, false);
                std::cout << cardIndex << std::endl;
                player.getHand()[cardIndex]->setHidden(false);
                
                _front->update(_vPlayers);
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
                player.getHand()[cardIndex]->setHidden(true);
            }
                break;
            case 9:
            case 10: {
                auto [playerIndex, cardIndex] = _front->InputChooseOtherPlayerCard(_playerTurn, true);
                _vPlayers[playerIndex]->getHand()[cardIndex]->setHidden(false);

                _front->update(_vPlayers);

                std::this_thread::sleep_for(std::chrono::milliseconds(150));
                _vPlayers[playerIndex]->getHand()[cardIndex]->setHidden(true);
            }
                break;
            case 11:
            case 12: {
                auto [_, yourCardIndex] = _front->InputChooseOtherPlayerCard(_playerTurn, false);
                auto [playerIndex, cardIndex] = _front->InputChooseOtherPlayerCard(_playerTurn, true);
                _vPlayers[playerIndex]->getHand().switchCards(player.getHand(), yourCardIndex, cardIndex);
            }
                break;
            case 13: {
                if (c->getColor() == BLACK) {
                    auto [playerIndex, cardIndex] = _front->InputChooseOtherPlayerCard(_playerTurn, true);
                    _vPlayers[playerIndex]->getHand()[cardIndex]->setHidden(false);

                    _front->update(_vPlayers);
                    _vPlayers[playerIndex]->getHand()[cardIndex]->setHidden(true);
                    auto [_, yourCardIndex] = _front->InputChooseOtherPlayerCard(_playerTurn, false);               

                    auto [i, j] = _front->InputChooseOtherPlayerCard(_playerTurn, true);
                    playerIndex = i;
                    cardIndex = j;
                    _vPlayers[playerIndex]->getHand().switchCards(player.getHand(), yourCardIndex, cardIndex);
                    if (yourCardIndex >= 0 && cardIndex >= 0) {
                        sf::Sprite mSprite = _front->positions[playerIndex][cardIndex];
                        sf::Sprite sprite = _front->positions[_playerTurn][yourCardIndex];
                        auto mPos = mSprite.getPosition();
                        mSprite.setPosition(sprite.getPosition());
                        sprite.setPosition(mPos);
                        
                        _front->positions[playerIndex][cardIndex] = sprite;
                        _front->positions[_playerTurn][yourCardIndex] = mSprite;
                    }
                }
            }
                break;
        }
    }
    else if (isPlayCardInput != -1) {
        auto c = player.getHand().popCard();
        sf::Sprite sp = _front->positions[_playerTurn].back();
        _front->positions[_playerTurn].pop_back();
        _front->update(_vPlayers);
        int yourCardIndex = isPlayCardInput;
        std::cout << yourCardIndex << std::endl;
        player.getHand().setCard(yourCardIndex, c);

        sp.setPosition(_front->positions[_playerTurn][yourCardIndex].getPosition());
        _front->positions[_playerTurn][yourCardIndex] = sp;
        
    }
    
}

int Game::deduceWinner() {
    std::vector<Player*>::iterator result = std::min_element(_vPlayers.begin(), _vPlayers.end(),
        [](Player*  lhs, Player* rhs) {
            return lhs->getHand().getScore() < rhs->getHand().getScore();
        });
     return std::distance(_vPlayers.begin(), result);
}


void Game::setNumberOfPlayers(int num){
  _Players = num;
}

int Game::getNumberOfPlayers(){
  return _Players;
}

void Game::makeDeck() {
    _deck = Deck();
    _deck.makeDeck();
}

void Game::dealCards() {
    for (int i = 0; i < this->getNumberOfPlayers(); ++i) {
        _vPlayers[i] = new Player(true);
        for (int j = 0; j < 4; ++j) {
            _deck.deal(_vPlayers[i]->getHand());
        }
    }
    _vPlayers[this->getNumberOfPlayers() - 1]->isHuman = false;
}

void Game::createPlayers() {
    for (int i = 0; i < this->getNumberOfPlayers(); ++i) {
        _vPlayers.push_back(nullptr);
    }
}


Game::~Game() {
}
