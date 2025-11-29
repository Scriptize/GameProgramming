#ifndef LEVEL1_H
#define LEVEL1_H
#include "Scene.h"
#include "AIEntity.h"

class Level1 : public Scene {
    AIEntity *enemy;
    unsigned int mLevel[14*8];
public:
    static constexpr float TILE = 75.0f, GRAV = 981.0f;
    Level1(Vector2 origin,const char*bg);
    void initialise() override;
    void update(float dt) override;
    void render() override;
    void shutdown() override;
};
#endif
