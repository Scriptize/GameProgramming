#include "EncounterScene.h"
#include "raylib.h"
#include "cs3113.h"

EncounterScene::EncounterScene(Vector2 origin, const char *bgHex)
: Scene(origin, bgHex) {}

EncounterScene::~EncounterScene() {
    shutdown();
}

void EncounterScene::initialise() {
    static unsigned int dummyLevel[16] = {
        0,0,0,0,
        0,0,0,0,
        0,0,0,0,
        0,0,0,0
    };

    map = new Map(
        4, 4,
        dummyLevel,
        "assets/game/tileset.png",
        64,
        4, 1,
        mOrigin
    );

    
    // -------- PLAYER / ENEMY ENTITIES --------
    player = new Entity(
        { mOrigin.x - 300, mOrigin.y }, 
        { 250, 250 },
        "assets/roguelike/Sprites/Player/Sword/Defence0/Player_Attack_Sword_Defence0_0.png",
        PLAYER
    );

    enemy = new Entity(
        { mOrigin.x + 300, mOrigin.y },
        { 250, 250 },
        "assets/roguelike/Sprites/Bosses/DungeonMaster/DungeonMaster_Death_00.png",
        NPC
    );

    playerHP = enemyHP = maxHP;

    // -------- CARDS --------
    cards.clear();
    cards.push_back({"Strike", 6,  {200, 650, 200, 250}});
    cards.push_back({"Bash",   10, {450, 650, 200, 250}});
    cards.push_back({"Mega",   20, {700, 650, 200, 250}});

    gameOver = false;
}

void EncounterScene::update(float dt) {
    if (gameOver) {
        if (IsKeyPressed(KEY_R))
            initialise();
        return;
    }

    Vector2 mouse = GetMousePosition();

    // -------- CARD CLICK LOGIC --------
    for (auto &c : cards) {
        c.hovered = CheckCollisionPointRec(mouse, c.box);

        if (c.hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            enemyHP -= c.damage;

            if (enemyHP <= 0) {
                enemyHP = 0;
                gameOver = true;
            }
        }
    }
}

void EncounterScene::drawHealthBar(const Entity *e, int hp, int maxHP) {
    float ratio = (float)hp / maxHP;

    Vector2 pos = e->getPosition();
    pos.y -= e->getScale().y * 0.75f;

    DrawRectangle(pos.x - 100, pos.y, 200, 25, RED);
    DrawRectangle(pos.x - 100, pos.y, 200 * ratio, 25, GREEN);
    DrawRectangleLines(pos.x - 100, pos.y, 200, 25, BLACK);

    DrawText(TextFormat("%d/%d", hp, maxHP), pos.x - 25, pos.y - 35, 25, WHITE);
}

void EncounterScene::drawCard(const Card &c) {
    Color fill = c.hovered ? Color{240,240,240,255} : Color{200,200,200,255};

    DrawRectangleRec(c.box, fill);
    DrawRectangleLinesEx(c.box, 4, BLACK);

    DrawText(c.name.c_str(), c.box.x + 20, c.box.y + 20, 30, BLACK);
    DrawText(TextFormat("%d dmg", c.damage), c.box.x + 20, c.box.y + 70, 25, BLACK);
}

void EncounterScene::render() {
    ClearBackground(ColorFromHex(mBGColourHexCode));
    
    player->render();
    enemy->render();

    // Health bars
    drawHealthBar(player, playerHP, maxHP);
    drawHealthBar(enemy,  enemyHP, maxHP);

    // Cards
    for (auto &c : cards) {
        drawCard(c);
    }

    if (gameOver) {
        DrawText("ENEMY DEFEATED!", mOrigin.x - 250, mOrigin.y - 350, 75, YELLOW);
        DrawText("Press R to restart", mOrigin.x - 200, mOrigin.y - 250, 40, WHITE);
    }
}

void EncounterScene::shutdown() {
    if (player) { delete player; player = nullptr; }
    if (enemy)  { delete enemy;  enemy = nullptr; }
    if (map)    { delete map;    map = nullptr; }
}
