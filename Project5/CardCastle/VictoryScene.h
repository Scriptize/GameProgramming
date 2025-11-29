#pragma once
#include "../CS3113/Scene.h"
#include "raylib.h"

class VictoryScene : public Scene {
public:
    VictoryScene(Vector2 origin, const char *bgHexCode);
    ~VictoryScene();

    void initialise() override;
    void update(float dt) override;
    void render() override;
    void shutdown() override;

private:
    Texture2D titleImage;
    Music bgm;

    bool showPressKey = true;
    float blinkTimer = 0.0f;
};
