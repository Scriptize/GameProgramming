#include "raylib.h"
#include "CardCastle/EncounterScene.h"
#include "CardCastle/TitleScene.h"
#include "CardCastle/CardDatabase.h"
#include "CardCastle/RewardScene.h"
#include "CardCastle/GameOverScene.h"
#include "CardCastle/VictoryScene.h"

#include <vector>

// -----------------------------------------------------
// Global Constants
// -----------------------------------------------------
constexpr int SCREEN_WIDTH  = 1280;
constexpr int SCREEN_HEIGHT = 800;
constexpr int FPS           = 60;

const Vector2 ORIGIN = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// -----------------------------------------------------
// Global State
// -----------------------------------------------------
AppStatus gAppStatus = RUNNING;

float gPreviousTicks   = 0.0f;
float gTimeAccumulator = 0.0f;

Scene* gCurrentScene = nullptr;
std::vector<Scene*> gScenes;
int gCurrentSceneIndex = 0;


// Concrete scenes
TitleScene* gTitleScene = nullptr;
EncounterScene* gEncounterScene = nullptr;
RewardScene* gRewardScene = nullptr;
GameOverScene* gGameOverScene = nullptr;
VictoryScene* gVictoryScene = nullptr;



CardDatabase gCardDB;

void switchToScene(Scene* scene)
{
    
    if (gCurrentScene && dynamic_cast<EncounterScene*>(gCurrentScene)) {
        static_cast<EncounterScene*>(gCurrentScene)->increaseLevel();
    }

    gCurrentScene = scene;
    if (gCurrentScene) {
        gCurrentScene->initialise();
    }
}

void initialise()
{   
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Card Castle");
    InitAudioDevice();
    SetTargetFPS(FPS);

    gCardDB.load();


    gTitleScene     = new TitleScene(ORIGIN, "#1a1a29");
    gEncounterScene = new EncounterScene(ORIGIN, "#1a1a29");
    gRewardScene = new RewardScene(ORIGIN, "#1a1a29");
    gGameOverScene = new GameOverScene(ORIGIN, "#1a1a29");
    gVictoryScene = new VictoryScene(ORIGIN, "#1a1a29");

    gScenes.push_back(gTitleScene);
    gScenes.push_back(gEncounterScene);
    gScenes.push_back(gRewardScene);
    gScenes.push_back(gGameOverScene);
    gScenes.push_back(gVictoryScene);

    gCurrentSceneIndex = 0;
    switchToScene(gScenes[gCurrentSceneIndex]);

    gPreviousTicks = (float)GetTime();
    gTimeAccumulator = 0.0f;
}

void processInput()
{
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) {
        gAppStatus = TERMINATED;
        return;
    }

    if (IsKeyPressed(KEY_F11)) {
        ToggleFullscreen();
    }


    if (IsKeyPressed(KEY_ENTER) && gCurrentSceneIndex == 0) {
        gCurrentSceneIndex = (gCurrentSceneIndex + 1) % gScenes.size();
        switchToScene(gScenes[gCurrentSceneIndex]);
    }
    if (IsKeyPressed(KEY_ENTER)) {
        if (gCurrentScene == gGameOverScene || gCurrentScene == gVictoryScene) {
            gCurrentSceneIndex = 0;
            switchToScene(gScenes[gCurrentSceneIndex]);
        }
    }
}

void update()
{
    float ticks = (float)GetTime();
    float delta = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    delta += gTimeAccumulator;

    if (delta < FIXED_TIMESTEP) {
        gTimeAccumulator = delta;
        return;
    }

    while (delta >= FIXED_TIMESTEP) {
        if (gCurrentScene) gCurrentScene->update(FIXED_TIMESTEP);
        delta -= FIXED_TIMESTEP;
    }

    gTimeAccumulator = delta;
}

void render()
{
    BeginDrawing();
    if (gCurrentScene) gCurrentScene->render();
    EndDrawing();
}

void shutdown()
{
    if (gTitleScene)        { gTitleScene->shutdown(); delete gTitleScene; }
    if (gEncounterScene)    { gEncounterScene->shutdown(); delete gEncounterScene; }

    gScenes.clear();
    CloseWindow();
}

int main()
{
    
    SetMasterVolume(1.0f);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

    initialise();

    while (gAppStatus == RUNNING) {
        processInput();
        update();
        render();
    }

    shutdown();
    return 0;
}
