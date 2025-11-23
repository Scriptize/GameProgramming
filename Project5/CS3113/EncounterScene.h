#pragma once
#include "Scene.h"
#include "Entity.h"
#include "Map.h"
#include <vector>
#include <string>

struct Card {
    std::string name;
    int damage;
    Rectangle box;
    bool hovered = false;
};

class EncounterScene : public Scene {
public:
    EncounterScene(Vector2 origin, const char *bgHexCode);
    ~EncounterScene();

    void initialise() override;
    void update(float dt) override;
    void render() override;
    void shutdown() override;

private:
    Entity *player;
    Entity *enemy;

    Map *map; // minimal blank map (required by your engine)

    std::vector<Card> cards;

    int playerHP, enemyHP;
    int maxHP = 50;

    bool gameOver = false;

    void drawHealthBar(const Entity *e, int hp, int maxHP);
    void drawCard(const Card &c);
};
