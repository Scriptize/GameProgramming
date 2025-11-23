#pragma once
#include "Scene.h"
#include <vector>
#include <string>

struct DeckCard {
    std::string name;
    Rectangle box;
};

class DeckScene : public Scene {
public:
    DeckScene(Vector2 origin, const char *bgHex);
    ~DeckScene();

    void initialise() override;
    void update(float dt) override;
    void render() override;
    void shutdown() override;

private:
    std::vector<DeckCard> deck;
};
