#include "raylib.h"
#include "CS3113/EncounterScene.h"

int main() {
    InitWindow(1280, 960, "Slay the Spire Clone");
    SetTargetFPS(60);

    Vector2 origin = { 640, 480 };  
    
    EncounterScene *scene = new EncounterScene(origin, "#1a1a29");
    scene->initialise();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        scene->update(dt);

        BeginDrawing();
        scene->render();
        EndDrawing();
    }

    scene->shutdown();
    delete scene;

    CloseWindow();
    return 0;
}


