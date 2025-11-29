#include "MenuScene.h"

MenuScene::MenuScene(Vector2 origin, const char *bgHex)
: Scene(origin, bgHex) {}

void MenuScene::initialise() {
    mGameState.bgm = LoadMusicStream("assets/menu_music.wav");
    PlayMusicStream(mGameState.bgm);
    SetMusicVolume(mGameState.bgm, 0.5f);
}
void MenuScene::update(float dt) {
    UpdateMusicStream(mGameState.bgm);
    if (IsKeyPressed(KEY_ENTER)) mGameState.nextSceneID = 1;
}
void MenuScene::render() {
    ClearBackground(ColorFromHex(mBGColourHexCode));
    DrawText("MY PLATFORMER", 310, 200, 50, RAYWHITE);
    DrawText("Press ENTER to Start", 330, 300, 25, GRAY);
}
void MenuScene::shutdown() {
    StopMusicStream(mGameState.bgm);
    UnloadMusicStream(mGameState.bgm);
}
