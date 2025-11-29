#include "TitleScene.h"
#include "../CS3113/cs3113.h"

TitleScene::TitleScene(Vector2 origin, const char *bgHex)
: Scene(origin, bgHex) {}

TitleScene::~TitleScene() {
    shutdown();
}

void TitleScene::initialise() {
    // Load background/title image
    titleImage = LoadTexture("assets/roguelike/backgrounds/titlescreen.png");

    // Load background music
    bgm = LoadMusicStream("assets/roguelike/music/mt_coronet.mp3");
    SetMusicVolume(bgm, 0.7f);
    PlayMusicStream(bgm);
}

void TitleScene::update(float dt) {
    UpdateMusicStream(bgm);

    // Blink "Press any key"
    blinkTimer += dt;
    if (blinkTimer >= 0.6f) {
        showPressKey = !showPressKey;
        blinkTimer = 0.0f;
    }
}

void TitleScene::render() {
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
            "PRESS ENTER",
            mOrigin.x - 220,
            mOrigin.y + 300,
            60,
            WHITE
        );
    }
}

void TitleScene::shutdown() {
    UnloadTexture(titleImage);
    UnloadMusicStream(bgm);
}
