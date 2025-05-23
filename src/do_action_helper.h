#ifndef DO_ACTION_HELPER

#define DO_ACTION_HELPER

#include "global.h"

#define CONDITION_PARAMETERS DoAction* doAction, PlayState* play, Player* this

typedef enum {
    ATTACK_GORON,
    ATTACK_ZORA,
    ATTACK_DEKU,
    ATTACK_HUMAN,
    ACTION
} DoActionLevel;

typedef bool (*DoActionCondition) (CONDITION_PARAMETERS);

#endif // DO_ACTION_HELPER