#include "RewardScene.h"
#include "EncounterScene.h"
#include "CardDatabase.h"
#include "raylib.h"

// From main.cpp
extern CardDatabase gCardDB;
extern EncounterScene* gEncounterScene;
extern RewardScene* gRewardScene;
extern void switchToScene(Scene*);

// ---------------------------------------------------
RewardScene::RewardScene(Vector2 origin, const char* bgHex)
: Scene(origin, bgHex) {}

RewardScene::~RewardScene() {
    shutdown();
}

// ---------------------------------------------------
void RewardScene::initialise() {
    bgm = LoadMusicStream("assets/roguelike/music/reward_win.mp3");
    SetMusicVolume(bgm, 0.7f);
    PlayMusicStream(bgm);
    bg = LoadTexture("assets/roguelike/backgrounds/reward.png");

    rewards.clear();
    deckRef = gEncounterScene->getDeck();

    std::vector<Card*> allCards = gCardDB.getAllCards();

    for (int i = 0; i < 3; i++) {
        Card* c = allCards[GetRandomValue(0, (int)allCards.size() - 1)];

        RewardCard rc;
        rc.card = c;
        rc.x = mOrigin.x + (i - 1) * 300.0f;
        rc.y = mOrigin.y + 100.0f;
        rc.bounds = c->getBounds(rc.x, rc.y, 1.0f);

        rewards.push_back(rc);
    }
}

// ---------------------------------------------------
void RewardScene::update(float dt) {
    UpdateMusicStream(bgm);
    Vector2 mouse = GetMousePosition();

    for (auto& r : rewards)
        r.hovering = CheckCollisionPointRec(mouse, r.bounds);

    for (auto& r : rewards) {
        if (r.hovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

            deckRef->drawPile.push_back(r.card);
            switchToScene((Scene*)gEncounterScene);
            return;
        }
    }
}

// ---------------------------------------------------
void RewardScene::render() {
    DrawTexturePro(
        bg,
        {0,0,(float)bg.width,(float)bg.height},
        {0,0,(float)GetScreenWidth(),(float)GetScreenHeight()},
        {0,0}, 0, WHITE
    );

    DrawText("Choose One", mOrigin.x - 120, mOrigin.y - 200, 42, YELLOW);

    for (auto& r : rewards) {
        float scale = r.hovering ? 1.15f : 1.0f;
        r.card->render(r.x, r.y, scale);
    }
}

// ---------------------------------------------------
void RewardScene::shutdown() {
    if (bg.id != 0) UnloadTexture(bg);
    UnloadMusicStream(bgm);
    rewards.clear();
}
