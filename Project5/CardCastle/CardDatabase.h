#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "Card.h"

class CardDatabase {
public:
    // Loads all cards from files (call ONCE at game start)
    void load();

    // Get card definition by name
    Card* get(const std::string& name);

    // Convenience: return list of all cards
    std::vector<Card*> getAllCards() {
        std::vector<Card*> out;
        for (auto& p : cards)
            out.push_back(p.second);
        return out;
}

    

private:
    std::unordered_map<std::string, Card*> cards;
};
