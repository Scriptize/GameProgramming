#ifndef MENUSCENE_H
#define MENUSCENE_H
#include "Scene.h"

class MenuScene : public Scene {
public:
    MenuScene(Vector2 origin, const char *bgHex);
    void initialise() override;
    void update(float dt) override;
    void render() override;
    void shutdown() override;
};
#endif
