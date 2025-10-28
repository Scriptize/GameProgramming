#include "raylib.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <unistd.h>
#include <limits.h>
#include "CS3113/cs3113.h"
#include "CS3113/Entity.h"
#include "CS3113/Map.h"

constexpr int SCREEN_WIDTH  = 1400;
constexpr int SCREEN_HEIGHT = 500;
constexpr int FPS = 60;

constexpr float GRAVITY  = 10.0f;
constexpr float H_ACCEL  = 200.0f;
constexpr float FUEL_USE = 30.0f;
constexpr float PLAYER_SIZE = 40.0f;

enum GameState { PLAYING, WIN, LOSE };

GameState gState = PLAYING;
float gFuel = 100.0f;
float gSafeTimer = 1.0f;
bool gShowColliders = false;

Entity gPlayer;
std::vector<Entity> gUpperPlatforms;
std::vector<Entity> gLowerPlatforms;

Texture2D texShip, texGoal, texGround;
std::string gAssetPath = "./assets/";

bool CheckAABB(const Entity &a, const Entity &b) {
    Vector2 pa = a.getPosition(), sa = a.getColliderDimensions();
    Vector2 pb = b.getPosition(), sb = b.getColliderDimensions();
    float xDist = fabsf(pa.x - pb.x) - ((sa.x + sb.x) / 2.0f);
    float yDist = fabsf(pa.y - pb.y) - ((sa.y + sb.y) / 2.0f);
    return (xDist < 0 && yDist < 0);
}

void VerifyAssetsPath() {
    if (!FileExists((gAssetPath + "ship.png").c_str())) {
        if (FileExists("../assets/ship.png")) gAssetPath = "../assets/";
    }
    std::cout << "Using asset path: " << gAssetPath << std::endl;
}

void ResetGame() {
    gPlayer.setPosition({ SCREEN_WIDTH / 2.0f, PLAYER_SIZE / 2.0f + 10.0f });
    gPlayer.setAcceleration({ 0, GRAVITY });
    gFuel = 100.0f;
    gState = PLAYING;
    gSafeTimer = 1.0f;
}

void initialise() {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lunar Land");
    SetTargetFPS(FPS);
    VerifyAssetsPath();

    // Load textures once
    texShip   = LoadTexture((gAssetPath + "ship.png").c_str());
    texGoal   = LoadTexture((gAssetPath + "goal.png").c_str());
    texGround = LoadTexture((gAssetPath + "ground.png").c_str());

    gPlayer = Entity({ SCREEN_WIDTH / 2.0f, PLAYER_SIZE / 2.0f + 10.0f },
                     { PLAYER_SIZE, PLAYER_SIZE }, "", PLAYER);
    gPlayer.setTexture(texShip);
    gPlayer.setAcceleration({ 0, GRAVITY });

    auto addPlatform = [&](std::vector<Entity>& layer, float x, float y, float w, float h, Texture2D tex) {
        Entity e({ x, y }, { w, h }, "", PLATFORM);
        e.setTexture(tex);
        layer.push_back(e);
    };

    addPlatform(gUpperPlatforms, 300, 300, 100, 20, texGoal);
    addPlatform(gUpperPlatforms, 700, 250, 120, 20, texGoal);
    addPlatform(gUpperPlatforms, 1100, 220, 100, 20, texGoal);

    addPlatform(gLowerPlatforms, 200, 420, 120, 20, texGround);
    addPlatform(gLowerPlatforms, 600, 440, 180, 20, texGround);
    addPlatform(gLowerPlatforms, 1000, 410, 100, 20, texGround);
}

void processInput(float dt) {
    if (gState != PLAYING) return;
    if (IsKeyPressed(KEY_C)) gShowColliders = !gShowColliders;

    gPlayer.setAcceleration({ 0, GRAVITY });
    bool usingFuel = false;

    if (IsKeyDown(KEY_LEFT) && gFuel > 0) {
        gPlayer.setAcceleration({ -H_ACCEL, GRAVITY });
        usingFuel = true;
    }
    if (IsKeyDown(KEY_RIGHT) && gFuel > 0) {
        gPlayer.setAcceleration({ H_ACCEL, GRAVITY });
        usingFuel = true;
    }

    if (usingFuel) {
        gFuel -= FUEL_USE * dt;
        if (gFuel < 0) gFuel = 0;
    }
}

void update(float dt) {
    if (gState != PLAYING) return;
    gPlayer.update(dt, nullptr, nullptr, nullptr, 0);

    static std::vector<float> upperSpeeds;
    if (upperSpeeds.empty()) {
        upperSpeeds.resize(gUpperPlatforms.size());
        for (auto &spd : upperSpeeds)
            spd = (GetRandomValue(0, 1) ? 1.0f : -1.0f) * (80.0f + GetRandomValue(0, 40));
    }

    static std::vector<float> lowerSpeeds;
    if (lowerSpeeds.empty()) {
        lowerSpeeds.resize(gLowerPlatforms.size());
        for (auto &spd : lowerSpeeds)
            spd = (GetRandomValue(0, 1) ? 1.0f : -1.0f) * (100.0f + GetRandomValue(0, 60));
    }

    for (size_t i = 0; i < gUpperPlatforms.size(); ++i) {
        Vector2 pos = gUpperPlatforms[i].getPosition();
        pos.x += upperSpeeds[i] * dt;
        if (pos.x < 100 || pos.x > SCREEN_WIDTH - 100) upperSpeeds[i] *= -1;
        gUpperPlatforms[i].setPosition(pos);
    }
    for (size_t i = 0; i < gLowerPlatforms.size(); ++i) {
        Vector2 pos = gLowerPlatforms[i].getPosition();
        pos.x += lowerSpeeds[i] * dt;
        if (pos.x < 100 || pos.x > SCREEN_WIDTH - 100) lowerSpeeds[i] *= -1;
        gLowerPlatforms[i].setPosition(pos);
    }

    if (gSafeTimer > 0) { gSafeTimer -= dt; return; }

    for (auto& plat : gUpperPlatforms)
        if (CheckAABB(gPlayer, plat)) gState = WIN;

    for (auto& plat : gLowerPlatforms)
        if (CheckAABB(gPlayer, plat)) gState = LOSE;

    Vector2 pos = gPlayer.getPosition();
    if (pos.y > SCREEN_HEIGHT + PLAYER_SIZE || pos.x < -PLAYER_SIZE || pos.x > SCREEN_WIDTH + PLAYER_SIZE)
        gState = LOSE;
}

void render() {
    BeginDrawing();
    ClearBackground(DARKGRAY);

    auto drawEntity = [&](Entity& e, Color tint) {
        Texture2D tex = e.getTexture();
        Vector2 pos = e.getPosition();
        Vector2 dim = e.getColliderDimensions();
        if (tex.id > 0) {
            Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
            Rectangle dst = { pos.x - dim.x / 2.0f, pos.y - dim.y / 2.0f, dim.x, dim.y };
            DrawTexturePro(tex, src, dst, { 0, 0 }, 0.0f, tint);
        } else {
            DrawRectangleV({ pos.x - dim.x / 2.0f, pos.y - dim.y / 2.0f }, dim, tint);
        }
        if (gShowColliders) e.displayCollider();
    };

    for (auto& p : gUpperPlatforms) drawEntity(p, GREEN);
    for (auto& p : gLowerPlatforms) drawEntity(p, RED);
    drawEntity(gPlayer, WHITE);

    DrawText(TextFormat("Fuel: %.0f", gFuel), 20, 20, 20, WHITE);
    if (gState == WIN)
        DrawText("LANDED SAFELY!", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 20, 30, GREEN);
    else if (gState == LOSE)
        DrawText("MISSION FAILED", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 20, 30, RED);

    EndDrawing();
}

int main() {
    initialise();
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (IsKeyPressed(KEY_R)) ResetGame();
        processInput(dt);
        update(dt);
        render();
    }

    UnloadTexture(texShip);
    UnloadTexture(texGoal);
    UnloadTexture(texGround);

    CloseWindow();
    return 0;
}
