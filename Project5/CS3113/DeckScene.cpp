#include "DeckScene.h"
#include "raylib.h"

DeckScene::DeckScene(Vector2 origin, const char *bgHex)
: Scene(origin, bgHex) {}

DeckScene::~DeckScene() { shutdown(); }

void DeckScene::initialise() {
    // Dummy deck
    for (int i = 0; i < 10; i++) {
        float x = 100.0f + static_cast<float>((i % 5) * 200);
        float y = 200.0f + static_cast<float>((i / 5) * 300);

        deck.push_back({
            "Strike",
            { x, y, 150.0f, 200.0f }
        });
    }
}

void DeckScene::update(float dt) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        // return to previous scene
    }
}

void DeckScene::render() {
    ClearBackground(ColorFromHex(mBGColourHexCode));
    DrawText("Your Deck", 100, 50, 50, WHITE);

    for (auto &c : deck) {
        DrawRectangleRec(c.box, GRAY);
        DrawText(c.name.c_str(), c.box.x + 10, c.box.y + 10, 25, BLACK);
    }
}

void DeckScene::shutdown() {}
