#include "ViewInput.h"


void ViewInput::printCard(Card& c) {

	std::cout << c.toString();
}

void ViewInput::setPositions(std::vector<Player*>& players) {
	int x = 10, y = 10;
	for (int i = 0; i < players.size(); i++) {
		int size = players[i]->getHand().size();
		int j = 0;
		if (positions[i].size() > 0) {
			auto v = positions[i].back().getPosition();
			j = positions[i].size();
			x = positions[i].size() % 2 == 0 ? v.x - 150 : v.x + 150, y = positions[i].size() % 2 == 0 ? v.y + 120 : v.y;
		}
		while (positions[i].size() != size) {
			std::cout << "setting pos of i's player and j's card: " << i << "," << j << std::endl;


			auto str = players[i]->getHand()[j]->toString();
			sf::Sprite sp = mp[str];
			sp.setPosition(sf::Vector2f(x, y));
			positions[i].push_back(sp);
			if ((j + 1) % 2 == 0) {
				y += 120;
				x -= 150;
			}
			else {
				x += 150;
			}
			j++;
		}
		y = 10;
		x = 980;

	}
}

void ViewInput::update(std::vector<Player*>& players) {
	//system("cls");

	for (int i = 0; i < players.size(); ++i) {
		Hand* h = &players[i]->getHand();
		for (int j = 0; j < h->size(); ++j) {
			printCard(*h->getCard(j));
			if ((j + 1) % 2 == 0) std::cout << std::endl;
			else std::cout << " ";
		}
		std::cout << std::endl << std::endl;
	}



	_w->clear();
	_w->draw(background);
	_w->draw(callCambioText);

	for (int i = 0; i < players.size(); ++i) {
		Hand* h = &players[i]->getHand();
		for (int j = 0; j < h->size(); ++j) {
			auto c = *h->getCard(j);
			auto str = c.toString();
			sf::Sprite sp = positions[i][j];
			//sp.setPosition(positions[i][j]);
			if (c.getHidden()) {
				auto rec = hidden;
				rec.setPosition(sp.getPosition());
				_w->draw(rec);
			}
			else {

				_w->draw(sp);
			}
		}
	}
	_w->display();

}
int ViewInput::InputDecideCardPlay(int turn, int cardIndex) {
	auto res = InputChooseOtherPlayerCard(turn, false);
	static std::pair<int, int> invalid = std::pair<int, int>(-1, -1);
	std::pair<int, int> p = std::make_pair(turn, cardIndex);
	if (p == res) {
		return 1;
	}
	else if (res != invalid) {
		return res.second;
	}
	return -1;
}

bool ViewInput::InputDecideCallCambio() {
	sf::Event event;
	bool lock_click = false;
	while (!lock_click) {
		_w->pollEvent(event);
		if (event.mouseButton.button == sf::Mouse::Left) {
			sf::Vector2f mouse = _w->mapPixelToCoords(sf::Mouse::getPosition(*_w));
			sf::FloatRect bounds = callCambioText.getGlobalBounds();
			lock_click = true;
			if (bounds.contains(mouse)) {
				return true;
			}
		}
	}
	return false;
}

int ViewInput::InputChooseOtherPlayer(Player& p) {
	std::cout << "Choose other player index: ";
	int index = 1;
	std::cin >> index;
	return index;
}
std::pair<int, int> ViewInput::InputChooseOtherPlayerCard(int currentPlayer, bool ignoreIdentity) {

	//ignore_identity is for deciding whether we want to choose 
	//one of the opponent's cards or only our cards when ignore_identity set to false


	std::cout << "Inpute choose other player's card" << std::endl;

	bool lock_click = false;
	sf::Event event;
	while (!lock_click) {
		_w->pollEvent(event);
		if (event.type == sf::Event::Closed)
			_w->close();
		if (event.type == sf::Event::MouseButtonPressed) //Mouse button Pressed
		{
			if (event.mouseButton.button == sf::Mouse::Left) //specifies
			{
				sf::Vector2f mouse = _w->mapPixelToCoords(sf::Mouse::getPosition(*_w));


				lock_click = true;

				// hit test
				for (int i = 0; i < positions.size(); i++) {
					if (!ignoreIdentity and i != currentPlayer) continue;
					if (ignoreIdentity and i == currentPlayer) continue;
					for (int j = 0; j < positions[i].size(); j++) {

						sf::FloatRect bounds = positions[i][j].getGlobalBounds();


						if (bounds.contains(mouse))
						{
							// mouse is on sprite!
							
							std::cout << "pressed " << i << std::endl;
							return std::pair<int,int>(i,j);
						}

					}
				}
			}
		}
	}
	return std::pair<int, int>(-1, -1);
}

void ViewInput::endMenu(int winner) {
	std::string num;
	std::stringstream ss;
	ss << winner;
	num = ss.str();
	std::string str = num + "won!";
	sf::Text text;
	text.setFont(font);
	text.setString(str);
	text.setCharacterSize(200);
	sf::FloatRect textRect = text.getLocalBounds();
	text.setOrigin(textRect.left + textRect.width / 2.0f,
		textRect.top + textRect.height / 2.0f);
	text.setPosition(sf::Vector2f(_width / 2.0f, _height / 2.0f));

	_w->clear();
	_w->draw(background);
	_w->draw(text);
	_w->display();
}

bool ViewInput::startMenu(sf::Text text) {
	bool m = false;
	_w->clear();
	_w->draw(background);
	_w->draw(text);
	_w->display();
	sf::Event event;
	bool lock_click = false;
	while (!lock_click)
	{
		_w->pollEvent(event);
		if (event.type == sf::Event::Closed)
			_w->close();
		if (event.mouseButton.button == sf::Mouse::Left) {
			sf::Vector2f mouse = _w->mapPixelToCoords(sf::Mouse::getPosition(*_w));
			sf::FloatRect bounds = text.getGlobalBounds();
			if (bounds.contains(mouse)) {
				lock_click = true;
				m = true;
			}
		}
	}

	return m;
}