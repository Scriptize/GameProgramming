#pragma once
#include "Scene.h"
#include <vector>
#include <string>

struct RewardCard {
    std::string name;
    Rectangle box;
    bool hovered = false;
};

class RewardScene : public Scene {
public:
    RewardScene(Vector2 origin, const char *bgHex);
    ~RewardScene();

    void initialise() override;
    void update(float dt) override;
    void render() override;
    void shutdown() override;

private:
    std::vector<RewardCard> rewardCards;
};
