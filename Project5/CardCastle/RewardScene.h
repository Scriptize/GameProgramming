#pragma once
#include "../CS3113/Scene.h"
#include "Card.h"
#include "Deck.h"
#include "CardDatabase.h"
#include "raylib.h"
#include <vector>
#include <string>

class EncounterScene; // forward declaration

class RewardScene : public Scene {
public:
    RewardScene(Vector2 origin, const char* bgHex);
    ~RewardScene();

    void initialise() override;
    void update(float dt) override;
    void render() override;
    void shutdown() override;
    void setDeck(Deck* d) { deckRef = d; }


private:
    struct RewardCard {
        Card* card;
        float x, y;
        Rectangle bounds;
        bool hovering = false;
    };

    std::vector<RewardCard> rewards;
    Texture2D bg;
    Music bgm;

    Deck* deckRef = nullptr; // initialize safely
};
