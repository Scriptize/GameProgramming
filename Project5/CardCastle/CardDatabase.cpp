#include "CardDatabase.h"
#include "Card.h"
#include "StatusEffect.h"
#include "EncounterScene.h"
#include <iostream>

void CardDatabase::load() {
    std::string atkFrame   = "assets/roguelike/card_frames/attack_frame.png";
    std::string skillFrame = "assets/roguelike/card_frames/skill_frame.png";

    // SLASH
    cards["Slash"] = new Card(
        "Slash",
        "6 dmg.",
        1,
        "assets/roguelike/attack_icons/slash.png",
        atkFrame,
        [](EncounterScene* e) {
            e->dealDamageToEnemy(6);
            
        }
    );

    cards["Slash"]->fxName = "SLASH";
    cards["Slash"]->sfxName = "SLASH";


    // REAP
    cards["Reap"] = new Card(
        "Reap",
        "Steal life (8)",
        2,
        "assets/roguelike/attack_icons/reap.png",
        atkFrame,
        [](EncounterScene* e) {
            int before = e->getEnemyHP();
            e->dealDamageToEnemy(8);
            if (before > 0 && e->getEnemyHP() <= 0)
                e->addToPlayerHP(5);
        }
    );
    cards["Reap"]->sfxName = "SLASH";

    // GASH
    cards["Gash"] = new Card(
        "Gash",
        "7 dmg. +2 next",
        2,
        "assets/roguelike/attack_icons/gash.png",
        atkFrame,
        [](EncounterScene* e) {
            e->dealDamageToEnemy(7);
            e->applyEffectToPlayer(STATUS_STRENGTH, 2, 1);
        }
    );
    cards["Gash"]->sfxName = "SLASH";

    // FINISHER
    cards["Finisher"] = new Card(
        "Finisher",
        "Combo!",
        1,
        "assets/roguelike/attack_icons/gash.png",
        atkFrame,
        [](EncounterScene* e) {
            int dmg = 6 * e->getCardsPlayedThisTurn();
            e->dealDamageToEnemy(dmg);
        }
    );
    cards["Finisher"]->sfxName = "SLASH";

    // DUAL STRIKE
    cards["Dual Hit"] = new Card(
        "Dual Hit",
        "5 x 2 dmg.",
        1,
        "assets/roguelike/attack_icons/dual_strike.png",
        atkFrame,
        [](EncounterScene* e) {
            e->dealDamageToEnemy(5);
            e->dealDamageToEnemy(5);
        }
    );
    cards["Dual Hit"]->sfxName = "SLASH";

    // BLOODBLADE
    cards["Bloodblade"] = new Card(
        "Bloodblade",
        "15 dmg. +10.",
        3,
        "assets/roguelike/attack_icons/bloodblade.png",
        atkFrame,
        [](EncounterScene* e) {
            e->dealDamageToEnemy(15);
            e->addToPlayerHP(10);
        }
    );
    cards["Bloodblade"]->sfxName = "BLOOD";

    // BLITZ
    cards["Blitz"] = new Card(
        "Blitz",
        "5/10 dmg. +crit.",
        3,
        "assets/roguelike/attack_icons/blitz.png",
        atkFrame,
        [](EncounterScene* e) {
            int dmg = (GetRandomValue(0, 1) ? 10 : 5);
            e->dealDamageToEnemy(dmg);
            e->applyEffectToPlayer(STATUS_CRIT_UP, 1, 99999);
        }
    );

    // DEFEND
    cards["Defend"] = new Card(
        "Defend",
        "5 block.",
        1,
        "assets/roguelike/skill_icons/defend.png",
        skillFrame,
        [](EncounterScene* e) {
            e->applyEffectToPlayer(STATUS_BLOCK, 5, 1);
        }
    );
    cards["Defend"]->sfxName = "BLOCK";

    // DEAD MAN'S HAND
    cards["Dead Man's"] = new Card(
        "Dead Man's",
        "10% die/win.",
        3,
        "assets/roguelike/skill_icons/dmh.png",
        skillFrame,
        [](EncounterScene* e) {
            int roll = GetRandomValue(1, 10);
            if (roll == 1) e->addToPlayerHP(-9999);
            if (roll == 10) e->addToEnemyHP(-9999);
        }
    );

    // GAMBLER'S VICE
    cards["All In"] = new Card(
        "All In",
        "+DMH odds.",
        2,
        "assets/roguelike/skill_icons/gamblers_vice.png",
        skillFrame,
        [](EncounterScene* e) {
            e->applyEffectToPlayer(STATUS_CRIT_UP, 1, 99999);
        }
    );

    // GLARE
    cards["Glare"] = new Card(
        "Glare",
        "-3 enemy.",
        1,
        "assets/roguelike/skill_icons/glare.png",
        skillFrame,
        [](EncounterScene* e) {
            e->applyEffectToEnemy(STATUS_WEAKEN, 3, 1);
        }
    );

    // PREPARATION
    cards["Prepare"] = new Card(
        "Preparation",
        "2x dmg",
        0,
        "assets/roguelike/skill_icons/preparation.png",
        skillFrame,
        [](EncounterScene* e) {
            e->applyEffectToPlayer(STATUS_DOUBLE_DAMAGE_NEXT, 1, 1);
        }
    );

    // REST
    cards["Rest"] = new Card(
        "Rest",
        "+20. Skip turn.",
        3,
        "assets/roguelike/skill_icons/rest.png",
        skillFrame,
        [](EncounterScene* e) {
            e->addToPlayerHP(20);
            e->setSkipPlayerTurn(true);
        }
    );

    // VAMPIRISM
    cards["Vampirism"] = new Card(
        "Vampirism",
        "+33% dmg",
        2,
        "assets/roguelike/skill_icons/vampirism.png",
        skillFrame,
        [](EncounterScene* e) {
            e->applyEffectToPlayer(STATUS_VAMPIRISM, 1, 9999);
        }
    );
    cards["Vampirism"]->sfxName = "VAMP";
    // VOODOO
    cards["Voodoo"] = new Card(
        "Voodoo",
        "Match 5 dmg",
        2,
        "assets/roguelike/skill_icons/voodoo.png",
        skillFrame,
        [](EncounterScene* e) {
            e->applyEffectToPlayer(STATUS_THORNS, 5, 3);
        }
    );

    std::cout << "Loaded " << cards.size() << " cards.\n";
}

Card* CardDatabase::get(const std::string& name) {
    auto it = cards.find(name);
    return (it == cards.end()) ? nullptr : it->second;
}
