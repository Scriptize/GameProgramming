#pragma once
#include <string>
#include <functional>
#include "raylib.h"

class EncounterScene;

struct CardEffectData {
    std::string fxSprite;     // file path
    std::string sfxSound;     // file path
    float scale = 1.0f;
};

class Card {
public:
    using EffectFn = std::function<void(EncounterScene*)>;
    std::string fxName;
    std::string sfxName;

private:
    std::string name;
    std::string desc;
    int cost = 1;

    std::string artPath;
    std::string framePath;

    Texture2D art;
    Texture2D frame;

    
    bool loaded = false;

public:
    bool hovering = false;
    EffectFn effectFn = nullptr;

    Card(const std::string& n,
         const std::string& d,
         int c,
         const std::string& artP,
         const std::string& frameP,
         EffectFn fn = nullptr)
        : name(n), desc(d), cost(c),
          artPath(artP), framePath(frameP),
          effectFn(fn)
    {}

    // ------------------------------------------------------
    void loadTextures() {
        if (!loaded) {
            art   = LoadTexture(artPath.c_str());
            frame = LoadTexture(framePath.c_str());
            loaded = true;
        }
    }

    const std::string& getName() const { return name; }
    const std::string& getDesc() const { return desc; }
    int getCost() const { return cost; }

    void apply(EncounterScene* s) {
        if (effectFn) effectFn(s);
    }

    Rectangle getBounds(float x, float y, float scale) const {
        float w = 150 * scale;
        float h = 210 * scale;
        return { x - w * 0.5f, y - h * 0.5f, w, h };
    }

    // ------------------------------------------------------
    void render(float x, float y, float scale = 1.0f) {
        loadTextures();

        float w  = 150 * scale;
        float h  = 210 * scale;
        float fx = x - w * 0.5f;
        float fy = y - h * 0.5f;

        // FRAME
        DrawTexturePro(
            frame,
            {0,0,(float)frame.width,(float)frame.height},
            {fx, fy, w, h},
            {0,0},
            0,
            WHITE
        );

        // ART
        float paddingX = 18 * scale;
        float paddingY = 32 * scale;
        float artH = h * 0.45f;

        DrawTexturePro(
            art,
            {0,0,(float)art.width,(float)art.height},
            {
                fx + paddingX,
                fy + paddingY,
                w - 2*paddingX,
                artH
            },
            {0,0},
            0,
            WHITE
        );

        // NAME — centered
        int nameFont = (int)(22 * scale);
        int nameWidth = MeasureText(name.c_str(), nameFont);
        DrawText(
            name.c_str(),
            fx + (w - nameWidth) * 0.5f,
            fy + paddingY + artH + 5,
            nameFont,
            BLACK
        );

        
        int descFont = (int)(15 * scale);
        float descY = fy + h - 55 * scale;
        int descWidth = MeasureText(desc.c_str(), descFont);

        DrawText(
            desc.c_str(),
            fx + (w - descWidth) * 0.5f,
            descY,
            descFont,
            BLACK
        );

        // COST
        DrawText(
            TextFormat("%d", cost),
            fx + 10 * scale,
            fy + 10 * scale,
            (int)(28 * scale),
            YELLOW
        );
    }
};
