#pragma once
#include "Player.h"
#include "Deck.h"
#include "ViewInput.h"
class Game {
private:
	int _Players;
	int _playerTurn;
	int _gameTurn;
	ViewInput* _front = nullptr;
protected:
	Deck _deck;
	bool _isActive;
	/*std::vector<Player*> _vPlayers;*/
public:
	std::vector<Player*> _vPlayers;
	Game(ViewInput&);
	~Game();

	void createGame(); // template method
	void setNumberOfPlayers(int);
	int getNumberOfPlayers();

	void makeDeck();
	bool isActive();
	void setActive(bool);

	int gameLoop();

	void playerAction(Player&);
	void aiAction(Player&);
	void dealCards();
	void createPlayers();
	int deduceWinner();
};