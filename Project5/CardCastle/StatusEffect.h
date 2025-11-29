#pragma once
#include <map>


enum StatusEffect {
    STATUS_NONE = 0,

    STATUS_STRENGTH,            // +X damage to attacks
    STATUS_WEAKEN,              // lowers enemy outgoing damage
    STATUS_BLOCK,               // absorbs incoming damage
    STATUS_DOUBLE_DAMAGE_NEXT,  // next attack deals double
    STATUS_CRIT_UP,             // +25% crit chance per stack
    STATUS_VAMPIRISM,           // heal 1/3 damage dealt
    STATUS_THORNS               // enemy takes flat damage when player is hit
};


struct Status {
    int amount   = 0;   
    int duration = 0;   
};
