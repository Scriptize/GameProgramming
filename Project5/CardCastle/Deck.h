#pragma once
#include <vector>
#include <algorithm>
#include <random>
#include "Card.h"

class Deck {
public:
    std::vector<Card*> drawPile;
    std::vector<Card*> discardPile;
    std::vector<Card*> hand;

    Deck() = default;

    void initialize(const std::vector<Card*>& starter);
    void reshuffleForEncounter();
    void shufflePile(std::vector<Card*>& pile);

    void draw(int amount);
    void discard(Card* c);
    void endTurnDiscard();
};
