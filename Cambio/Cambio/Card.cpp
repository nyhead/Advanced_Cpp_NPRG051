#include "Card.h"
#include <sstream>

Card::Card(int rank, Suite suite, Color color, bool hidden){
  _rank = rank;
  _suite = suite;
  _hidden = hidden;
  _color = color;
  _value = getValue();
}

int Card::getRank() {
  return _rank;
}

int Card::getValue() {
    if (_value == INT32_MIN) {
        switch (getRank())
        {
        case 1: //Ace
            return 1;
        case 13: //Red kinh
            return getColor() == RED ? -1 : 10;
        case -1: //Joker
            return 0;
        default:
            return 10;
            break;
        }
    }
    return _value;
}

Suite Card::getSuite() {
  return _suite;
}

Color Card::getColor() {
    return _color;
}

std::string Card::getSuiteString() {
    switch (_suite) {
    case HEARTS: return "H";
    case DIAMONDS: return "D";
    case SPADES: return "S";
    case CLUBS: return "C";
    default: return "Joker";
    }
}

std::string Card::getRankString() {
    std::string num = "JOKER";
    int number = _rank;
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

std::string Card::toString() {
    return getSuiteString() + getRankString();
}

bool Card::getHidden() {
  return _hidden;
}

void Card::setHidden(bool hidden) {
  _hidden = hidden;
}
