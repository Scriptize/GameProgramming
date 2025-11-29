#pragma once
#include <map>
#include <vector>
#include "raylib.h"
#include "../CS3113/Scene.h"
#include "StatusEffect.h"
#include "Deck.h"


class Entity;
class Card;
class CardDatabase;

// ============================================================
// EncounterScene
// ============================================================
class EncounterScene : public Scene {
public:

    enum TurnState {
        PLAYER_TURN_START,
        PLAYER_TURN,
        ENEMY_TURN,
        RESOLVE_TURN
    };

    struct EnemyIntent {
    int attack = 0;   // 0 means no attack intent
    int block  = 0;   // 0 means no defend intent
    };

    EnemyIntent enemyIntent;


    struct VisibleCard {
        Card* card;
        float x, y;
        Rectangle bounds;
    };
    Texture2D bgTexture;

private:
    
    // ----------------------------
    // COMBAT STATE
    // ----------------------------
    int playerHP = 50;
    int enemyHP  = 50;
    int maxHP    = 50;
    int level    = 0;
    int mana     = 3;
    int maxMana  = 3;

    int cardsPerTurn = 3;

    bool skipPlayerTurn = false;
    bool battleOver     = false;

    int cardsPlayedThisTurn = 0;

    // ----------------------------
    // STATUS EFFECT STORAGE
    // ----------------------------
    std::map<StatusEffect, Status> playerStatus;
    std::map<StatusEffect, Status> enemyStatus;

    // ----------------------------
    // CARD & DECK
    // ----------------------------
    Deck deck;
    CardDatabase* cardDB = nullptr;
    std::vector<VisibleCard> visibleCards;
   


    // ----------------------------
    // ENTITIES
    // ----------------------------
    Entity* player = nullptr;
    Entity* enemy  = nullptr;
    Texture2D intentAttackTex;
    Texture2D intentDefendTex;
    Texture2D attackTex;

    TurnState turnState = PLAYER_TURN_START;

public:

    // ============================================================
    // Constructor / Destructor
    // ============================================================
    EncounterScene(Vector2 origin, const char* bgHex);
    ~EncounterScene();

    // ============================================================
    // INITIALIZE / SHUTDOWN
    // ============================================================
    void initialise() override;
    void shutdown() override;
    // --- Public accessors for card effects ---
    int getCardsPlayedThisTurn() const { return cardsPlayedThisTurn; }
    void incrementCardsPlayed() { cardsPlayedThisTurn++; }
    void increaseLevel() {level++;}
    Deck* getDeck() { return &deck; }
    void setSkipPlayerTurn(bool v) { skipPlayerTurn = v; }
    bool getSkipPlayerTurn() const { return skipPlayerTurn; }

    // ============================================================
    // PUBLIC HP ACCESSORS (Fix for CardDatabase)
    // ============================================================
    int getPlayerHP() const { return playerHP; }
    int getEnemyHP() const { return enemyHP; }
    int getMaxHP()   const { return maxHP; }
    void rollEnemyIntent();

    void addToPlayerHP(int v) {
        playerHP = std::min(maxHP, playerHP + v);
    }

    void addToEnemyHP(int v) {
        enemyHP = std::max(0, enemyHP + v);
    }

    // ============================================================
    // STATUS EFFECT APPLICATION
    // ============================================================
    void applyEffectToPlayer(StatusEffect eff, int amt, int duration = 1);
    void applyEffectToEnemy(StatusEffect eff, int amt, int duration = 1);

    // ============================================================
    // DAMAGE PIPELINE
    // ============================================================
    void dealDamageToEnemy(int dmg);
    void dealDamageToPlayer(int dmg);

    // ============================================================
    // TURN PHASE HELPERS
    // ============================================================
    void startTurnSetup();
    void endTurnCleanup();

    // ============================================================
    // UPDATE / RENDER
    // ============================================================
    void update(float dt) override;
    void render() override;

private:
    Music bgm;
    void drawHealthBar(const Entity* e, int hp, int maxHP);
    void drawHealthBarWithBlock(const Entity* e, int hp, int maxHP, int block);
};
