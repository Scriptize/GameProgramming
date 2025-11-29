#include "FXManager.h"
#include "raylib.h"
#include <iostream>

std::map<std::string, Entity*> FXManager::fxTemplates;
std::map<std::string, Sound> FXManager::sfxLibrary;
std::vector<Entity*> FXManager::activeFX;

void FXManager::Load() {
    fxTemplates.clear();
    sfxLibrary.clear();
    activeFX.clear();

   
    Entity* slash = new Entity(
        {0,0},
        {128,128},
        "assets/roguelike/fx/slash_vfx.png",
        ATLAS,
        {2, 2}, 
        {
            {RIGHT, {0,1,2,3}}
        },
        FX
    );
    slash->setFrameSpeed(15);
    slash->deactivate();

    fxTemplates["SLASH"] = slash;


    // -------------------------------
    // LOAD SOUNDS
    // -------------------------------
    sfxLibrary["SLASH"]    = LoadSound("assets/roguelike/fx/slash_sfx.mp3");
    sfxLibrary["BLOOD"]    = LoadSound("assets/roguelike/fx/blood_sfx.mp3");
    sfxLibrary["BLOCK"]    = LoadSound("assets/roguelike/fx/shield_sfx.mp3");
    sfxLibrary["VAMP"]     = LoadSound("assets/roguelike/fx/dracula_sfx.mp3");
    sfxLibrary["WHACK"]    = LoadSound("assets/roguelike/fx/enemy_hit.mp3");

    // sfxLibrary["FIREBALL"] = LoadSound("assets/sfx/fireball.wav");
}

void FXManager::Unload() {
    for (auto& it : fxTemplates)
        delete it.second;

    fxTemplates.clear();
    sfxLibrary.clear();

    for (auto fx : activeFX)
        delete fx;

    activeFX.clear();
}

void FXManager::SpawnFX(const std::string& fxName, Vector2 position) {
    if (fxTemplates.find(fxName) == fxTemplates.end()) return;

    Entity* fx = new Entity(*fxTemplates[fxName]);     
    fx->setPosition(position);
    fx->activate();
    fx->resetAnimation();

    activeFX.push_back(fx);
}

void FXManager::PlaySFX(const std::string& sfxName) {
    if (sfxLibrary.find(sfxName) != sfxLibrary.end())
        PlaySound(sfxLibrary[sfxName]);
}

void FXManager::Update(float dt) {
    for (int i = activeFX.size() - 1; i >= 0; i--) {
        Entity* fx = activeFX[i];

        // animate non-loop FX
        fx->animateOnce(dt);

        if (fx->animationIsDoneOnce()) {
            delete fx;
            activeFX.erase(activeFX.begin() + i);
        }

    }
}

void FXManager::Render() {
    for (auto fx : activeFX)
        fx->render();
}
