#include "Level1.h"

Level1::Level1(Vector2 origin,const char*bg):Scene(origin,bg){}

void Level1::initialise() {
    mGameState.bgm = LoadMusicStream("assets/level_music.wav");
    PlayMusicStream(mGameState.bgm);
    SetMusicVolume(mGameState.bgm,0.5f);
    mGameState.jumpSound=LoadSound("assets/jump.wav");

    unsigned int data[14*8]={
        4,0,0,0,0,0,0,0,0,0,0,0,0,4,
        4,0,0,0,0,0,0,0,0,0,0,0,0,4,
        4,0,0,0,0,2,2,2,0,0,0,0,0,4,
        4,0,0,0,0,0,0,0,0,0,0,0,0,4,
        4,0,0,0,0,0,0,0,0,0,2,2,2,4,
        4,2,2,2,0,0,0,2,2,2,3,3,3,4,
        4,3,3,3,0,0,0,3,3,3,3,3,3,4,
        4,3,3,3,0,0,0,3,3,3,3,3,3,4};
    memcpy(mLevel,data,sizeof(data));

    mGameState.map=new Map(14,8,mLevel,"assets/tileset.png",TILE,4,1,mOrigin);
    mGameState.xochitl=new Entity({mOrigin.x-300,mOrigin.y-200},{160,200},
                                  "assets/player.png",SINGLE,{1,1},{},PLAYER);
    mGameState.xochitl->setAcceleration({0,GRAV});
    enemy = new AIEntity({mOrigin.x+200,mOrigin.y-200},{100,100},"assets/enemy.png",ENEMY_WANDERER,120);
}
void Level1::update(float dt){
    UpdateMusicStream(mGameState.bgm);
    mGameState.xochitl->update(dt,nullptr,mGameState.map,nullptr,0);
    enemy->updateAI(dt,mGameState.xochitl);

    extern int gLives;
    Rectangle playerBox = {
        mGameState.xochitl->getPosition().x - mGameState.xochitl->getScale().x / 2.0f,
        mGameState.xochitl->getPosition().y - mGameState.xochitl->getScale().y / 2.0f,
        mGameState.xochitl->getScale().x,
        mGameState.xochitl->getScale().y
    };

    if (CheckCollisionRecs(enemy->getAABB(), playerBox)) {
        PlaySound(LoadSound("assets/hit.wav"));
        gLives--;
        if (gLives <= 0) mGameState.nextSceneID = 0;
        else initialise();
    }

    if(mGameState.xochitl->getPosition().x>800)mGameState.nextSceneID=2;
}
void Level1::render(){
    ClearBackground(ColorFromHex(mBGColourHexCode));
    mGameState.map->render();
    mGameState.xochitl->render();
    enemy->render();
}
void Level1::shutdown(){
    delete mGameState.xochitl; delete mGameState.map; delete enemy;
    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
}
