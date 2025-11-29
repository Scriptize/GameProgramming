#include "CS3113/MenuScene.h"
#include "CS3113/Level1.h"
#include "CS3113/Level2.h"
#include "CS3113/Level3.h"

constexpr int SCREEN_W=1000,SCREEN_H=600,FPS=120;
constexpr Vector2 ORIGIN={SCREEN_W/2,SCREEN_H/2};
AppStatus gAppStatus=RUNNING;
Scene* gScene=nullptr; std::vector<Scene*> gScenes;
int gLives=3; bool gWin=false;

void switchScene(Scene*s){gScene=s;s->initialise();}
void initialise(){
    InitWindow(SCREEN_W,SCREEN_H,"Platformer");
    InitAudioDevice();
    gScenes={
        new MenuScene(ORIGIN,"#111133"),
        new Level1(ORIGIN,"#CCE3DE"),
        new Level2(ORIGIN,"#A4C3B2"),
        new Level3(ORIGIN,"#6B9080")
    };
    switchScene(gScenes[0]);
    SetTargetFPS(FPS);
}
void processInput(){
    if(!gScene->getState().xochitl)return;
    auto* p=gScene->getState().xochitl;
    p->resetMovement();
    if(IsKeyDown(KEY_A))p->moveLeft();
    if(IsKeyDown(KEY_D))p->moveRight();
    if(IsKeyPressed(KEY_W)&&p->isCollidingBottom()){
        p->jump(); PlaySound(gScene->getState().jumpSound);
    }
}
void update(){
    static float prev=0; float now=GetTime(),dt=now-prev; prev=now;
    gScene->update(dt);
    int next=gScene->getState().nextSceneID;
    if(next==4)gWin=true;
    else if(next>0&&next<gScenes.size())switchScene(gScenes[next]);
}
void render(){
    BeginDrawing();
    if(!gWin){
        BeginMode2D(gScene->getState().camera);
        gScene->render();
        EndMode2D();
        DrawText(TextFormat("Lives: %d",gLives),20,20,25,WHITE);
    }else{
        ClearBackground(BLACK);
        DrawText("YOU WIN!",380,250,60,WHITE);
        DrawText("Press ENTER to return to Menu",260,330,25,GRAY);
        if(IsKeyPressed(KEY_ENTER)){gWin=false;gLives=3;switchScene(gScenes[0]);}
    }
    EndDrawing();
}
void shutdown(){
    for(auto*s:gScenes){s->shutdown();delete s;}
    CloseAudioDevice();CloseWindow();
}
int main(){
    initialise();
    while(gAppStatus==RUNNING){
        processInput();update();render();
        if(WindowShouldClose())gAppStatus=TERMINATED;
    }
    shutdown();
    return 0;
}



