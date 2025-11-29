#include "GameOverScene.h"
#include "../CS3113/cs3113.h"

GameOverScene::GameOverScene(Vector2 origin, const char *bgHex)
: Scene(origin, bgHex) {}

GameOverScene::~GameOverScene() {
    shutdown();
}

void GameOverScene::initialise() {
    // Load background/title image
    titleImage = LoadTexture("assets/roguelike/backgrounds/game_over.png");

    // Load background music
    bgm = LoadMusicStream("assets/roguelike/music/game_over.mp3");
    SetMusicVolume(bgm, 0.7f);
    PlayMusicStream(bgm);
}

void GameOverScene::update(float dt) {
    UpdateMusicStream(bgm);

    // Blink "Press any key"
    blinkTimer += dt;
    if (blinkTimer >= 0.6f) {
        showPressKey = !showPressKey;
        blinkTimer = 0.0f;
    }
}

void GameOverScene::render() {
    ClearBackground(ColorFromHex(mBGColourHexCode));
    
    // Draw background image
    DrawTexturePro(
        titleImage,
        {0, 0, (float)titleImage.width, (float)titleImage.height},
        {mOrigin.x, mOrigin.y, 1280, 800},   // full window
        {640, 400},                          // center origin
        0.0f,
        WHITE
    );

    // Draw blinking text
    if (showPressKey) {
        DrawText(
            "PRESS ENTER TO RESTART",
            mOrigin.x - 400,
            mOrigin.y + 300,
            60,
            RED
        );
    }
}

void GameOverScene::shutdown() {
    UnloadTexture(titleImage);
    UnloadMusicStream(bgm);
}
