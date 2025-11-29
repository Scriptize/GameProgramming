#pragma once
#include <map>
#include <string>
#include <vector>
#include "../CS3113/Entity.h"

class FXManager {
public:
    static void Load();
    static void Unload();
    static void Update(float dt);
    static void Render();

    static void SpawnFX(const std::string& fxName, Vector2 position);
    static void PlaySFX(const std::string& sfxName);

private:
    static std::map<std::string, Entity*> fxTemplates;
    static std::map<std::string, Sound> sfxLibrary;
    static std::vector<Entity*> activeFX;
};
