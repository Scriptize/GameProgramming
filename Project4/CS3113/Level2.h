#ifndef LEVEL2_H
#define LEVEL2_H
#include "Scene.h"
#include "AIEntity.h"

class Level2 : public Scene {
    AIEntity *enemy;
    unsigned int mLevel[14*8];
public:
    static constexpr float TILE = 75.0f, GRAV = 981.0f;
    Level2(Vector2 origin,const char*bg);
    void initialise() override;
    void update(float dt) override;
    void render() override;
    void shutdown() override;
};
#endif
