#include "RewardScene.h"
#include "raylib.h"

RewardScene::RewardScene(Vector2 origin, const char *bgHex)
: Scene(origin, bgHex) {}

RewardScene::~RewardScene() { shutdown(); }

void RewardScene::initialise() {
    rewardCards.push_back({"Power Strike", {200, 400, 200, 300}});
    rewardCards.push_back({"Block+",       {450, 400, 200, 300}});
    rewardCards.push_back({"Flame Orb",    {700, 400, 200, 300}});
}

void RewardScene::update(float dt) {
    Vector2 mouse = GetMousePosition();
    for (auto &c : rewardCards) {
        c.hovered = CheckCollisionPointRec(mouse, c.box);

        if (c.hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // TODO: Add to deck
            // TODO: return to map / next encounter
        }
    }
}

void RewardScene::render() {
    ClearBackground(ColorFromHex(mBGColourHexCode));

    DrawText("Choose a Reward", 400, 150, 50, WHITE);

    for (auto &c : rewardCards) {
        DrawRectangleRec(c.box, c.hovered ? Color{240,240,240,255} : Color{200,200,200,255});
        DrawRectangleLinesEx(c.box, 4, WHITE);
        DrawText(c.name.c_str(), c.box.x + 15, c.box.y + 15, 28, BLACK);
    }
}

void RewardScene::shutdown() {}
