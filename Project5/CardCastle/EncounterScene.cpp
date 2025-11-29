#include "EncounterScene.h"
#include "RewardScene.h"
#include "GameOverScene.h"
#include "VictoryScene.h"
#include "raylib.h"
#include "../CS3113/cs3113.h"
#include "CardDatabase.h"
#include <algorithm>
#include "FXManager.h"


extern CardDatabase gCardDB;
extern RewardScene* gRewardScene;
extern GameOverScene* gGameOverScene;
extern VictoryScene* gVictoryScene;
extern void switchToScene(Scene*);


EncounterScene::EncounterScene(Vector2 origin, const char *bgHex)
    : Scene(origin, bgHex) {}

EncounterScene::~EncounterScene() {
    shutdown();
}

// ============================================================================
// INITIALISE
// ============================================================================

void EncounterScene::initialise() {
    // scalable music selection: pick a track based on level, clamping to last track
    switch (level) {
        case 0:
            bgm = LoadMusicStream("assets/roguelike/music/bw_battle.mp3");
            break;
        case 1:
            bgm = LoadMusicStream("assets/roguelike/music/goblin_theme.mp3");
            break;
        default:
            bgm = LoadMusicStream("assets/roguelike/music/final_boss.mp3");
            break;
    }
    
    SetMusicVolume(bgm, 0.4f);
    PlayMusicStream(bgm);
    FXManager::Load(); 

    bgTexture       = LoadTexture("assets/roguelike/backgrounds/encounter.png");
    intentAttackTex = LoadTexture("assets/roguelike/Props/atk_intent.png");
    intentDefendTex = LoadTexture("assets/roguelike/Props/block_intent.png");
    attackTex = LoadTexture("assets/rougelike/spritesheets/attack.png");


    cardDB = &gCardDB;

    playerHP = maxHP;
    enemyHP = maxHP + level * 20;


    mana               = maxMana;
    skipPlayerTurn     = false;
    cardsPlayedThisTurn= 0;
    battleOver         = false;

    playerStatus.clear();
    enemyStatus.clear();
    visibleCards.clear();

    std::map<Direction, std::vector<int>> animAtlas = {
        { RIGHT, {0,1,2,3} }
    };

    if (player) { delete player; player = nullptr; }
    if (enemy)  { delete enemy;  enemy  = nullptr; }

    player = new Entity(
        { mOrigin.x - 300, mOrigin.y + 50 },
        { 220, 220 },
        "assets/roguelike/spritesheets/player.png",
        ATLAS,
        { 2, 2 },
        animAtlas,
        PLAYER
    );
    

    struct EnemyDef { const char* path; Vector2 size; };
    std::vector<EnemyDef> enemyDefs = {
        { "assets/roguelike/spritesheets/slime.png", {220,220} },   // level 1
        { "assets/roguelike/spritesheets/goblin_king.png", {300,300} },  // level 2
        { "assets/roguelike/spritesheets/skeleton_king.png", {400,400}}
    };

    // clamp index to available definitions
    const EnemyDef& ed = enemyDefs[std::min(level, (int)enemyDefs.size() - 1)];


    enemy = new Entity(
        { mOrigin.x + 300, mOrigin.y + 50 },
        ed.size,
        ed.path,
        ATLAS,
        { 2, 2 },
        animAtlas,
        NPC
    );

    

    if (deck.drawPile.empty() && deck.discardPile.empty() && deck.hand.empty()) {
        deck.initialize({
            cardDB->get("Slash"),
            cardDB->get("Slash"),
            cardDB->get("Slash"),
            cardDB->get("Defend"),
            cardDB->get("Defend"),
            cardDB->get("Bloodblade"),
            cardDB->get("Defend"),
            cardDB->get("Gash"),
            cardDB->get("Vampirism")
        });
    }

    deck.reshuffleForEncounter();
    turnState = PLAYER_TURN_START;
}

// ============================================================================
// STATUS HELPERS
// ============================================================================
void EncounterScene::applyEffectToPlayer(StatusEffect eff, int amt, int duration) {
    auto &s = playerStatus[eff];
    s.amount  += amt;
    s.duration = std::max(s.duration, duration);
}

void EncounterScene::applyEffectToEnemy(StatusEffect eff, int amt, int duration) {
    auto &s = enemyStatus[eff];
    s.amount  += amt;
    s.duration = std::max(s.duration, duration);
}

// ============================================================================
// DAMAGE PIPELINE
// ============================================================================
void EncounterScene::dealDamageToEnemy(int base) {
    int dmg = base;

    // Enemy BLOCK first
    if (enemyStatus[STATUS_BLOCK].amount > 0) {
        int absorbed = std::min(enemyStatus[STATUS_BLOCK].amount, dmg);
        dmg -= absorbed;
        enemyStatus[STATUS_BLOCK].amount -= absorbed;

        if (dmg <= 0) return; // fully blocked
    }

    // Strength
    if (playerStatus[STATUS_STRENGTH].amount > 0)
        dmg += playerStatus[STATUS_STRENGTH].amount;

    // Double damage next
    if (playerStatus[STATUS_DOUBLE_DAMAGE_NEXT].amount > 0) {
        dmg *= 2;
        playerStatus[STATUS_DOUBLE_DAMAGE_NEXT].amount = 0;
    }

    // Crit (25% per stack)
    int critChance = playerStatus[STATUS_CRIT_UP].amount * 25;
    if (critChance > 0 && GetRandomValue(1, 100) <= critChance)
        dmg = (int)(dmg * 1.5f);

    enemyHP -= dmg;

    // Vampirism
    if (playerStatus[STATUS_VAMPIRISM].amount > 0)
        addToPlayerHP(dmg / 3);

    if (enemyHP < 0) enemyHP = 0;
}

void EncounterScene::dealDamageToPlayer(int base) {
    int incoming = base;

    // Enemy weakened
    if (enemyStatus[STATUS_WEAKEN].amount > 0)
        incoming = std::max(0, incoming - enemyStatus[STATUS_WEAKEN].amount);

    // Player BLOCK
    int block    = playerStatus[STATUS_BLOCK].amount;
    int absorbed = std::min(block, incoming);

    incoming -= absorbed;
    playerStatus[STATUS_BLOCK].amount -= absorbed;

    // Thorns
    if (playerStatus[STATUS_THORNS].amount > 0 && incoming > 0)
        enemyHP -= playerStatus[STATUS_THORNS].amount;

    playerHP -= incoming;

    if (playerHP < 0) playerHP = 0;
    if (enemyHP < 0) enemyHP = 0;
}

// ============================================================================
// TURN MANAGEMENT
// ============================================================================

void EncounterScene::startTurnSetup() {
    
    playerStatus[STATUS_BLOCK].amount = 0;

    mana               = maxMana;
    cardsPlayedThisTurn= 0;

    // Tick down player statuses except BLOCK 
    for (auto &p : playerStatus) {
        if (p.first == STATUS_BLOCK) continue; // skip block
        if (p.second.duration > 0) {
            p.second.duration--;
            if (p.second.duration == 0)
                p.second.amount = 0;
        }
    }

   
    for (auto &p : enemyStatus) {
        if (p.first == STATUS_BLOCK) continue; // skip block
        if (p.second.duration > 0) {
            p.second.duration--;
            if (p.second.duration == 0)
                p.second.amount = 0;
        }
    }
}

void EncounterScene::endTurnCleanup() {
    deck.endTurnDiscard();
}

/////////
// UPDATE
/////////
void EncounterScene::update(float dt) {
    UpdateMusicStream(bgm);
    FXManager::Update(dt);
    if (player) player->updateSimple(dt);
    if (enemy)  enemy->updateSimple(dt);

    if (battleOver) {
        switchToScene(gRewardScene);
        return;
    }

    Vector2 mouse = GetMousePosition();

    Rectangle endTurnBtn = {
        (float)(GetScreenWidth() - 260),
        (float)(GetScreenHeight() - 140),
        200, 80
    };

    bool hasValidAction = false;
    for (Card* c : deck.hand)
        if (c && c->getCost() <= mana)
            hasValidAction = true;

    switch (turnState) {

        case PLAYER_TURN_START:
            startTurnSetup();
            deck.draw(cardsPerTurn);
            rollEnemyIntent();
            turnState = skipPlayerTurn ? ENEMY_TURN : PLAYER_TURN;
            skipPlayerTurn = false;
            break;

        case PLAYER_TURN:
        {
            visibleCards.clear();

            int   maxRender = std::min(cardsPerTurn, (int)deck.hand.size());
            float spacing   = 300.0f;
            float startX    = mOrigin.x - spacing * (maxRender - 1) / 2.0f;
            float cy        = mOrigin.y + 310.0f;

            for (int i = 0; i < maxRender; i++) {
                Card* c = deck.hand[i];
                float cx = startX + i * spacing;

                // Temporary visible card struct
                VisibleCard vc;
                vc.card = c;
                vc.x    = cx;
                vc.y    = cy;

                // First compute hover using unscaled bounds
                Rectangle baseBounds = c->getBounds(cx, cy, 1.0f);
                bool isHover = CheckCollisionPointRec(mouse, baseBounds);

                c->hovering = isHover;

                // Now compute ACTUAL bounds according to scale
                float scale = isHover ? 1.12f : 1.0f;
                vc.bounds   = c->getBounds(cx, cy, scale);

                visibleCards.push_back(vc);
            }


            // card click handling
            for (auto &vc : visibleCards) {
                Card* c = vc.card;

                if (CheckCollisionPointRec(mouse, vc.bounds) &&
                    IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    if (mana >= c->getCost()) {

                        mana -= c->getCost();
                        cardsPlayedThisTurn++;
                        if (c->getName() == "Slash"){

                        }

                        c->apply(this);

                        if (!c->fxName.empty()) {
                            FXManager::SpawnFX(c->fxName, enemy->getPosition());
                        }

                        if (playerHP <= 0){
                            playerHP = 0;
                            switchToScene(gGameOverScene);

                        }
                        if (c->sfxName == "SLASH") {
                                player->swapSpritesheet(
                                "assets/roguelike/spritesheets/attack.png",
                                {2,2},                 // columns x rows
                                {0,1,2,3},             // frames in order
                                0.30f                 // duration to stay on attack sheet
                            );

                        }

                        // Spawn visuals
                        
                        // Play sound
                        if (!c->sfxName.empty()) {
                            FXManager::PlaySFX(c->sfxName);
}

                        auto it = std::find(deck.hand.begin(), deck.hand.end(), c);
                        if (it != deck.hand.end()) {
                            deck.discard(*it);
                            deck.hand.erase(it);
                        }

                        if (enemyHP <= 0) {
                            enemyHP = 0;

                            
                            gRewardScene->setDeck(&deck);

                            
                            shutdown();

                            // switch scenes
                            if (level < 2) {

                                switchToScene((Scene*)gRewardScene);
                            }
                            else{
                                switchToScene((Scene*)gVictoryScene);
                            }
                            return;   
                        }



                        break;
                    }
                }
            }


            if (!battleOver &&
                CheckCollisionPointRec(mouse, endTurnBtn) &&
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                endTurnCleanup();
                turnState = ENEMY_TURN;
            }

            break;
        }

        case ENEMY_TURN:
            // Clear old enemy block at START of enemy turn,
            // so block only lasts through the player's turn.
            enemyStatus[STATUS_BLOCK].amount = 0;

            if (enemyIntent.attack > 0)
                dealDamageToPlayer(enemyIntent.attack);
                FXManager::PlaySFX("WHACK");

            if (enemyIntent.block > 0)
                applyEffectToEnemy(STATUS_BLOCK, enemyIntent.block, 0); // duration ignored
                FXManager::PlaySFX("BLOCK");
            
            if (playerHP <= 0){
                playerHP = 0;
                shutdown();
                switchToScene(gGameOverScene);

            }
            endTurnCleanup();
            turnState = PLAYER_TURN_START;
            break;

        case RESOLVE_TURN:
            break;
    }
}


void EncounterScene::drawHealthBarWithBlock(const Entity* e, int hp, int maxHP, int block) {
    Vector2 pos = e->getPosition();
    pos.y -= e->getScale().y * 0.9f;

    float barW = 250;
    float barH = 25;
    float ratioHP = (float)hp / maxHP;

    int blockWidth = (block > 0)
        ? (int)((float)block / maxHP * barW)
        : 0;

    // Base bar
    DrawRectangle(pos.x - 125, pos.y, barW, barH, DARKGRAY);

    // HP bar (green)
    DrawRectangle(pos.x - 125, pos.y, barW * ratioHP, barH, GREEN);

    // Block overlay (grey, to the right of current HP)
    if (block > 0) {
        DrawRectangle(
            pos.x - 125 + barW * ratioHP,
            pos.y,
            blockWidth,
            barH,
            Color{150,150,150,255}
        );
    }

    // Border
    DrawRectangleLines(pos.x - 125, pos.y, barW, barH, BLACK);

    DrawText(TextFormat("%d/%d", hp, maxHP),
             pos.x - 25, pos.y - 35, 28, WHITE);
}

void EncounterScene::drawHealthBar(const Entity* e, int hp, int maxHP) {
    float ratio = (float)hp / maxHP;

    Vector2 pos = e->getPosition();
    pos.y -= e->getScale().y * 0.9f;

    DrawRectangle(pos.x - 125, pos.y, 250, 25, DARKGRAY);
    DrawRectangle(pos.x - 125, pos.y, 250 * ratio, 25, GREEN);
    DrawRectangleLines(pos.x - 125, pos.y, 250, 25, BLACK);

    DrawText(TextFormat("%d/%d", hp, maxHP),
             pos.x - 25, pos.y - 35, 28, WHITE);
}

void EncounterScene::render() {

    DrawTexturePro(
        bgTexture,
        {0, 0, (float)bgTexture.width, (float)bgTexture.height},
        {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
        {0, 0},
        0,
        WHITE
    );

    if (player) player->render();
    if (enemy)  enemy->render();
    FXManager::Render();

    drawHealthBarWithBlock(
        player,
        playerHP,
        maxHP,
        playerStatus[STATUS_BLOCK].amount
    );

    drawHealthBarWithBlock(
        enemy,
        enemyHP,
        maxHP + level * 20,
        enemyStatus[STATUS_BLOCK].amount
    );

    // ---- Enemy intent icons ----
    if (enemy) {
        Vector2 pos = enemy->getPosition();

        // Bring icons closer above the enemy
        pos.y -= enemy->getScale().y * 0.6f;

        float x         = pos.x;
        float iconSize  = 48.0f;
        float half      = iconSize / 2.0f;
        float spacing   = 60.0f;

        if (enemyIntent.attack > 0) {
            DrawTexturePro(
                intentAttackTex,
                {0,0,(float)intentAttackTex.width,(float)intentAttackTex.height},
                {x - half, pos.y - half, iconSize, iconSize},
                {0,0}, 0, WHITE
            );

            DrawText(
                TextFormat("%d", enemyIntent.attack),
                x + half + 6,
                pos.y - 10,
                26,
                WHITE
            );

            x += spacing;
        }

        if (enemyIntent.block > 0) {
            DrawTexturePro(
                intentDefendTex,
                {0,0,(float)intentDefendTex.width,(float)intentDefendTex.height},
                {x - half, pos.y - half, iconSize, iconSize},
                {0,0}, 0, WHITE
            );

            DrawText(
                TextFormat("%d", enemyIntent.block),
                x + half + 6,
                pos.y - 10,
                26,
                WHITE
            );
        }
    }

    // Mana
    DrawText(TextFormat("Mana: %d/%d", mana, maxMana),
             50, mOrigin.y + 250, 32, WHITE);

    // End turn button
    bool hasValidAction = false;
    for (Card* c : deck.hand)
        if (c && c->getCost() <= mana)
            hasValidAction = true;

    Color btnColor = hasValidAction
        ? Color{240,220,40,255}
        : Color{200,40,40,255};

    Rectangle endTurnBtn = {
        (float)(GetScreenWidth() - 260),
        (float)(GetScreenHeight() - 140),
        200, 80
    };

    DrawRectangleRec(endTurnBtn, btnColor);
    DrawRectangleLinesEx(endTurnBtn, 3, WHITE);
    DrawText("END TURN",
             endTurnBtn.x + 25, endTurnBtn.y + 28,
             28, WHITE);

    // Cards
    for (auto &vc : visibleCards) {
        float scale = vc.card->hovering ? 1.12f : 1.0f;
        vc.card->render(vc.x, vc.y, scale);
    }

    if (battleOver) {
        const char* msg = (enemyHP <= 0) ? "ENEMY DEFEATED!" : "YOU DIED!";
        Color col       = (enemyHP <= 0) ? YELLOW : RED;

        DrawText(msg, mOrigin.x - 260, mOrigin.y - 350, 60, col);
        DrawText("Press ENTER to restart",
                 mOrigin.x - 220, mOrigin.y - 280, 36, WHITE);
    }
}

// ============================================================================
// ENEMY INTENT
// ============================================================================
void EncounterScene::rollEnemyIntent() {
    float dmgScale = 1.0f + level * 0.35f;   
    enemyIntent.attack = 0;
    enemyIntent.block  = 0;

    int roll = GetRandomValue(1, 3); // 1 = attack, 2 = defend, 3 = both

    switch (roll) {
        case 1: // pure attack
            enemyIntent.attack = (int)(GetRandomValue(4, 10) * dmgScale);

            break;
        case 2: // pure defend
            enemyIntent.block = GetRandomValue(4, 12);
            break;
        case 3: // both
            enemyIntent.attack = GetRandomValue(3, 8);
            enemyIntent.block  = GetRandomValue(3, 8);
            break;
    }
}

// ============================================================================
// SHUTDOWN
// ============================================================================
void EncounterScene::shutdown() {
    if (player) { delete player; player = nullptr; }
    if (enemy)  { delete enemy;  enemy  = nullptr; }

    UnloadMusicStream(bgm);
    UnloadTexture(bgTexture);
    UnloadTexture(intentAttackTex);
    UnloadTexture(intentDefendTex);
    FXManager::Unload(); 

    visibleCards.clear();
}
