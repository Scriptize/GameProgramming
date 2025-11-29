#include "Deck.h"

static std::mt19937& rng() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

void Deck::initialize(const std::vector<Card*>& starter) {
    drawPile = starter;
    discardPile.clear();
    hand.clear();
    shufflePile(drawPile);
}

void Deck::reshuffleForEncounter() {
    if (drawPile.empty() && discardPile.empty() && hand.empty())
        return;

    for (Card* c : hand)
        discardPile.push_back(c);
    hand.clear();

    drawPile.insert(drawPile.end(), discardPile.begin(), discardPile.end());
    discardPile.clear();

    shufflePile(drawPile);
}

void Deck::shufflePile(std::vector<Card*>& pile) {
    if (pile.size() > 1)
        std::shuffle(pile.begin(), pile.end(), rng());
}

void Deck::draw(int amount) {
    for (int i = 0; i < amount; i++) {

        if (drawPile.empty()) {
            if (!discardPile.empty()) {
                drawPile.insert(drawPile.end(), 
                                discardPile.begin(), 
                                discardPile.end());
                discardPile.clear();
                shufflePile(drawPile);
            } else {
                return;
            }
        }

        if (!drawPile.empty()) {
            Card* c = drawPile.back();
            drawPile.pop_back();
            hand.push_back(c);
        }
    }
}

void Deck::discard(Card* c) {
    discardPile.push_back(c);
}

void Deck::endTurnDiscard() {
    for (Card* c : hand)
        discardPile.push_back(c);

    hand.clear();
}
