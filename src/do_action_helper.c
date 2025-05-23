#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"

#include "libc/string.h"
#include "libc64/sprintf.h"
#include "overlays/actors/ovl_En_Horse/z_en_horse.h"

#include "do_action_helper.h"

RECOMP_IMPORT("ProxyMM_Notifications", void Notifications_Emit(const char* prefix, const char* msg, const char* suffix));

extern FloorType sPlayerFloorType;
extern void Player_Action_28(Player* this, PlayState* play);
extern void Player_Action_80(Player* this, PlayState* play);
extern void Player_Action_53(Player* this, PlayState* play);
extern bool func_8082DA90(PlayState* play);
extern s32 Player_CanThrowCarriedActor(Player* this, Actor* heldActor);
extern bool Player_IsZTargeting(Player* this);

// Goron Conditions

bool Goron_None(CONDITION_PARAMETERS) {
    if (this->stateFlags3 & PLAYER_STATE3_80000) {
        *doAction = DO_ACTION_NONE;
        return true;
    }
    return false;
}

bool Goron_Pound(CONDITION_PARAMETERS) {
    if (this->stateFlags3 & PLAYER_STATE3_1000) {
        *doAction = DO_ACTION_POUND;
        return true;
    }
    return false;
}

bool Goron_Punch(CONDITION_PARAMETERS) {
    *doAction = DO_ACTION_PUNCH;
    return true;
}

// Zora Conditions

bool Zora_Punch(CONDITION_PARAMETERS) {
    if ((!(this->stateFlags1 & PLAYER_STATE1_8000000)) || (!(func_801242B4(this) || (Player_Action_28 == this->actionFunc)) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND))) {
        *doAction = DO_ACTION_PUNCH;
        return true;
    }
    return false;
}

bool Zora_Dive(CONDITION_PARAMETERS) {
    *doAction = DO_ACTION_DIVE;
    return true;
}

// Deku Conditions

bool Deku_Shoot(CONDITION_PARAMETERS) {
    *doAction = DO_ACTION_SHOOT;
    return true;
}

// Human Conditions

bool Human_Blast(CONDITION_PARAMETERS) {
    if (this->currentMask == PLAYER_MASK_BLAST) {
        *doAction = DO_ACTION_EXPLODE;
        return true;
    }
    return false;
}

bool Human_Bremen(CONDITION_PARAMETERS) {
    if (this->currentMask == PLAYER_MASK_BREMEN) {
        *doAction = DO_ACTION_MARCH;
        return true;
    }
    return false;
}

bool Human_Dance(CONDITION_PARAMETERS) {
    if (this->currentMask == PLAYER_MASK_KAMARO) {
        *doAction = DO_ACTION_DANCE;
        return true;
    }
    return false;
}

// Action Conditions

bool Action_Curl(CONDITION_PARAMETERS) {
    if ((this->transformation == PLAYER_FORM_GORON) && !(this->stateFlags1 & PLAYER_STATE1_400000)) {
        *doAction = DO_ACTION_CURL;
    }
    return false;
}

bool Action_Snap(CONDITION_PARAMETERS) {
    if (play->actorCtx.flags & ACTORCTX_FLAG_PICTO_BOX_ON) {
        *doAction = DO_ACTION_SNAP;
        return true;
    }
    return false;
}

bool Action_None_1(CONDITION_PARAMETERS) {
    if (Player_InBlockingCsMode(play, this) || (this->actor.flags & ACTOR_FLAG_OCARINA_INTERACTION) ||
        (this->stateFlags1 & PLAYER_STATE1_CHARGING_SPIN_ATTACK) ||
        (this->stateFlags3 & PLAYER_STATE3_80000) || (Player_Action_80 == this->actionFunc)) {
            *doAction = DO_ACTION_NONE;
            return true;
        }
    return false;
}

bool Action_Return_1(CONDITION_PARAMETERS) {
    if (this->stateFlags1 & PLAYER_STATE1_100000) {
        *doAction = DO_ACTION_RETURN;
        return true;
    }
    return false;
}

bool Action_Reel(CONDITION_PARAMETERS) {
    if ((this->heldItemAction == PLAYER_IA_FISHING_ROD) && (this->unk_B28 != 0)) {
        if (this->unk_B28 == 2) {
            *doAction = DO_ACTION_REEL;
            return true;
        }
    }
    return false;
}

bool Action_None_2(CONDITION_PARAMETERS) {
    if ((this->heldItemAction == PLAYER_IA_FISHING_ROD) && (this->unk_B28 != 0)) {
        if (this->unk_B28 == 2) {
        } else {
            *doAction = DO_ACTION_NONE;
            return true;
        }
    }
    return false;
}

bool Action_Down_1(CONDITION_PARAMETERS) {
    if (this->stateFlags3 & PLAYER_STATE3_2000) {
        *doAction = DO_ACTION_DOWN;
        return true;
    }
    return false;
}

bool Action_Open_1(CONDITION_PARAMETERS) {
    if ((this->doorType != PLAYER_DOORTYPE_NONE) && (this->doorType != PLAYER_DOORTYPE_STAIRCASE) &&
        !(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
            *doAction = DO_ACTION_OPEN;
            return true;
        }
    return false;
}

bool Action_Hops(CONDITION_PARAMETERS) {
    if (this->stateFlags3 & PLAYER_STATE3_200000) {
        static u8 D_8085D34C[] = {
            DO_ACTION_1, DO_ACTION_2, DO_ACTION_3, DO_ACTION_4, DO_ACTION_5, DO_ACTION_6, DO_ACTION_7, DO_ACTION_8,
        };

        *doAction = D_8085D34C[this->remainingHopsCounter];
        return true;
    }
    return false;
}

bool Action_Open_2(CONDITION_PARAMETERS) {
    if ((!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) || (this->heldActor == NULL)) &&
        (this->interactRangeActor != NULL) && (this->getItemId < GI_NONE)) {
            *doAction = DO_ACTION_OPEN;
            return true;
        }
    return false;
}

bool Action_Grab_1(CONDITION_PARAMETERS) {
    if (!(func_801242B4(this) || (Player_Action_28 == this->actionFunc)) && (this->stateFlags2 & PLAYER_STATE2_1)) {
        *doAction = DO_ACTION_GRAB;
        return true;
    }
    return false;
}

bool Action_Climb(CONDITION_PARAMETERS) {
    if ((this->stateFlags2 & PLAYER_STATE2_4) ||
        (!(this->stateFlags1 & PLAYER_STATE1_800000) && (this->rideActor != NULL))) {
            *doAction = DO_ACTION_CLIMB;
            return true;
        }
    return false;
}

bool Action_Horse_Speak(CONDITION_PARAMETERS) {
    if ((this->stateFlags1 & PLAYER_STATE1_800000) &&
        (!EN_HORSE_CHECK_4((EnHorse*)this->rideActor) && (Player_Action_53 != this->actionFunc))) {
            if ((this->stateFlags2 & PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER) && (this->talkActor != NULL)) {
                if ((this->talkActor->category == ACTORCAT_NPC) || (this->talkActor->id == ACTOR_DM_CHAR08)) {
                    *doAction = DO_ACTION_SPEAK;
                    return true;
                }
            }
        }
    return false;
}

bool Action_Horse_Check(CONDITION_PARAMETERS) {
    if ((this->stateFlags1 & PLAYER_STATE1_800000) &&
        (!EN_HORSE_CHECK_4((EnHorse*)this->rideActor) && (Player_Action_53 != this->actionFunc))) {
            if ((this->stateFlags2 & PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER) && (this->talkActor != NULL)) {
                if (!((this->talkActor->category == ACTORCAT_NPC) || (this->talkActor->id == ACTOR_DM_CHAR08))) {
                    *doAction = DO_ACTION_CHECK;
                    return true;
                }
            }
        }
    return false;
}

bool Action_Horse_Faster(CONDITION_PARAMETERS) {
    if ((this->stateFlags1 & PLAYER_STATE1_800000) &&
        (!EN_HORSE_CHECK_4((EnHorse*)this->rideActor) && (Player_Action_53 != this->actionFunc))) {
            if ((this->stateFlags2 & PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER) && (this->talkActor != NULL)) {   
            } else if (!func_8082DA90(play) && !func_800B7128(this) && !(this->stateFlags1 & PLAYER_STATE1_100000)) {
                *doAction = DO_ACTION_FASTER;
                return true;
            }
        }
    return false;
}

bool Action_Horse_None(CONDITION_PARAMETERS) {
    if ((this->stateFlags1 & PLAYER_STATE1_800000) &&
        (!EN_HORSE_CHECK_4((EnHorse*)this->rideActor) && (Player_Action_53 != this->actionFunc))) {
            if ((this->stateFlags2 & PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER) && (this->talkActor != NULL)) {   
            } else if (!func_8082DA90(play) && !func_800B7128(this) && !(this->stateFlags1 & PLAYER_STATE1_100000)) {
            } else {
                *doAction = DO_ACTION_NONE;
                return true;
            }
        }
    return false;
}

bool Action_Speak(CONDITION_PARAMETERS) {
    if ((this->stateFlags2 & PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER) && (this->talkActor != NULL)) {
        if ((this->talkActor->category == ACTORCAT_NPC) || (this->talkActor->category == ACTORCAT_ENEMY) ||
            (this->talkActor->id == ACTOR_DM_CHAR08)) {
                *doAction = DO_ACTION_SPEAK;
                return true;
            }
    }
    return false;
}

bool Action_Check(CONDITION_PARAMETERS) {
    if ((this->stateFlags2 & PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER) && (this->talkActor != NULL)) {
        if ((this->talkActor->category == ACTORCAT_NPC) || (this->talkActor->category == ACTORCAT_ENEMY) ||
            (this->talkActor->id == ACTOR_DM_CHAR08)) {
            } else {
                *doAction = DO_ACTION_CHECK;
                return true;
            }
    }
    return false;
}

bool Action_Down_2(CONDITION_PARAMETERS) {
    if ((this->stateFlags1 & (PLAYER_STATE1_2000 | PLAYER_STATE1_200000)) ||
        ((this->stateFlags1 & PLAYER_STATE1_800000) && (this->stateFlags2 & PLAYER_STATE2_400000))) {
            *doAction = DO_ACTION_DOWN;
            return true;
        }
    return false;
}

bool Action_Drop(CONDITION_PARAMETERS) {
    Actor* heldActor = this->heldActor;

    if ((this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->getItemId == GI_NONE) &&
        (heldActor != NULL)) {
            if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (heldActor->id == ACTOR_EN_NIW)) {
                if (!Player_CanThrowCarriedActor(this, heldActor)) {
                    *doAction = DO_ACTION_DROP;
                    return true;
                }
            }
        }
    return false;
}

bool Action_Throw(CONDITION_PARAMETERS) {
    Actor* heldActor = this->heldActor;

    if ((this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->getItemId == GI_NONE) &&
        (heldActor != NULL)) {
            if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (heldActor->id == ACTOR_EN_NIW)) {
                if (!Player_CanThrowCarriedActor(this, heldActor)) {
                } else {
                    *doAction = DO_ACTION_THROW;
                    return true;
                }
            }
        }
    return false;
}

bool Action_None_4(CONDITION_PARAMETERS) {
    Actor* heldActor = this->heldActor;

    if ((this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->getItemId == GI_NONE) &&
        (heldActor != NULL)) {
            if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (heldActor->id == ACTOR_EN_NIW)) {
            } else {
                *doAction = DO_ACTION_NONE;
                return true;
            }
        }
    return false;
}

bool Action_Grab_2(CONDITION_PARAMETERS) {
    if (this->stateFlags2 & PLAYER_STATE2_10000) {
        *doAction = DO_ACTION_GRAB;
        return true;
    }
    return false;
}

bool Action_Dive_1(CONDITION_PARAMETERS) {
    if (this->stateFlags2 & PLAYER_STATE2_800) {
        static u8 D_8085D354[] = { DO_ACTION_1, DO_ACTION_2 };
        s32 var_v0;

        var_v0 = ((120.0f - this->actor.depthInWater) / 40.0f);
        var_v0 = CLAMP(var_v0, 0, ARRAY_COUNT(D_8085D354) - 1);

        *doAction = D_8085D354[var_v0];
        return true;
    }
    return false;
}

bool Action_Jump_1(CONDITION_PARAMETERS) {
    if (this->stateFlags3 & PLAYER_STATE3_100) {
        *doAction = DO_ACTION_JUMP;
        return true;
    }
    return false;
}

bool Action_Return_2(CONDITION_PARAMETERS) {
    if (this->stateFlags3 & PLAYER_STATE3_1000) {
        *doAction = DO_ACTION_RETURN;
        return true;
    }
    return false;
}

bool Action_Surface(CONDITION_PARAMETERS) {
    if (!Player_IsZTargeting(this) && (this->stateFlags1 & PLAYER_STATE1_8000000) && !(func_801242B4(this) || (Player_Action_28 == this->actionFunc))) {
        *doAction = DO_ACTION_SURFACE;
        return true;
    }
    return false;
}

bool Action_Swim(CONDITION_PARAMETERS) {
    if (((this->transformation != PLAYER_FORM_DEKU) &&
        ((func_801242B4(this) || (Player_Action_28 == this->actionFunc)) || ((this->stateFlags1 & PLAYER_STATE1_8000000) &&
                    !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)))) ||
        ((this->transformation == PLAYER_FORM_DEKU) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
        func_800C9DDC(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId))) {
            if (this->transformation == PLAYER_FORM_ZORA) {
                *doAction = DO_ACTION_SWIM;
                return true;
            }
        }
    return false;
}

bool Action_Grab_3(CONDITION_PARAMETERS) {
    if (((this->transformation != PLAYER_FORM_DEKU) &&
        ((func_801242B4(this) || (Player_Action_28 == this->actionFunc)) || ((this->stateFlags1 & PLAYER_STATE1_8000000) &&
                    !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)))) ||
        ((this->transformation == PLAYER_FORM_DEKU) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
        func_800C9DDC(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId))) {
            Actor* interactRangeActor = this->interactRangeActor;

            if (this->transformation == PLAYER_FORM_ZORA) {
            } else {
                if ((this->stateFlags1 & PLAYER_STATE1_8000000) && (interactRangeActor != NULL) &&
                    (interactRangeActor->id == ACTOR_EN_ZOG)) {
                        *doAction = DO_ACTION_GRAB;
                        return true;
                    }
            }
        }
    return false;
}

bool Action_Dive_2(CONDITION_PARAMETERS) {
    if (((this->transformation != PLAYER_FORM_DEKU) &&
        ((func_801242B4(this) || (Player_Action_28 == this->actionFunc)) || ((this->stateFlags1 & PLAYER_STATE1_8000000) &&
                    !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)))) ||
        ((this->transformation == PLAYER_FORM_DEKU) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
        func_800C9DDC(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId))) {
            Actor* interactRangeActor = this->interactRangeActor;

            if (this->transformation == PLAYER_FORM_ZORA) {
            } else {
                if ((this->stateFlags1 & PLAYER_STATE1_8000000) && (interactRangeActor != NULL) &&
                    (interactRangeActor->id == ACTOR_EN_ZOG)) {
                    } else {
                        *doAction = DO_ACTION_DIVE;
                        return true;
                    }
            }
        }
    return false;
}

bool Action_Attack_1(CONDITION_PARAMETERS) {
    if ((Player_IsZTargeting(this) && (this->transformation != PLAYER_FORM_DEKU)) || !(this->stateFlags1 & PLAYER_STATE1_400000) ||
        !Player_IsGoronOrDeku(this)) {
            s32 controlStickDirection = this->controlStickDirections[this->controlStickDataIndex];

            if ((this->transformation != PLAYER_FORM_GORON) &&
                !(this->stateFlags1 & (PLAYER_STATE1_4 | PLAYER_STATE1_4000)) &&
                (controlStickDirection <= PLAYER_STICK_DIR_FORWARD) &&
                (Player_CheckHostileLockOn(this) ||
                    ((sPlayerFloorType != FLOOR_TYPE_7) && (Player_FriendlyLockOnOrParallel(this) ||
                                                            ((play->roomCtx.curRoom.type != ROOM_TYPE_INDOORS) &&
                                                            !(this->stateFlags1 & PLAYER_STATE1_400000) &&
                                                            (controlStickDirection == PLAYER_STICK_DIR_FORWARD)))))) {
                    *doAction = DO_ACTION_ATTACK;
                    return true;
                }
        }
    return false;
}

bool Action_Jump_2(CONDITION_PARAMETERS) {
    s32 sp24 = Player_IsZTargeting(this);

    if ((sp24 && (this->transformation != PLAYER_FORM_DEKU)) || !(this->stateFlags1 & PLAYER_STATE1_400000) ||
        !Player_IsGoronOrDeku(this)) {
            s32 controlStickDirection = this->controlStickDirections[this->controlStickDataIndex];

            if ((this->transformation != PLAYER_FORM_GORON) &&
                !(this->stateFlags1 & (PLAYER_STATE1_4 | PLAYER_STATE1_4000)) &&
                (controlStickDirection <= PLAYER_STICK_DIR_FORWARD) &&
                (Player_CheckHostileLockOn(this) ||
                    ((sPlayerFloorType != FLOOR_TYPE_7) && (Player_FriendlyLockOnOrParallel(this) ||
                                                            ((play->roomCtx.curRoom.type != ROOM_TYPE_INDOORS) &&
                                                            !(this->stateFlags1 & PLAYER_STATE1_400000) &&
                                                            (controlStickDirection == PLAYER_STICK_DIR_FORWARD)))))) {
                } else if ((play->roomCtx.curRoom.type != ROOM_TYPE_INDOORS) && sp24 &&
                    (controlStickDirection >= PLAYER_STICK_DIR_LEFT)) {
                        *doAction = DO_ACTION_JUMP;
                        return true;
                }
        }
    return false;
}

bool Action_Attack_2(CONDITION_PARAMETERS) {
    s32 sp24 = Player_IsZTargeting(this);

    if ((sp24 && (this->transformation != PLAYER_FORM_DEKU)) || !(this->stateFlags1 & PLAYER_STATE1_400000) ||
        !Player_IsGoronOrDeku(this)) {
            s32 controlStickDirection = this->controlStickDirections[this->controlStickDataIndex];

            if ((this->transformation != PLAYER_FORM_GORON) &&
                !(this->stateFlags1 & (PLAYER_STATE1_4 | PLAYER_STATE1_4000)) &&
                (controlStickDirection <= PLAYER_STICK_DIR_FORWARD) &&
                (Player_CheckHostileLockOn(this) ||
                    ((sPlayerFloorType != FLOOR_TYPE_7) && (Player_FriendlyLockOnOrParallel(this) ||
                                                            ((play->roomCtx.curRoom.type != ROOM_TYPE_INDOORS) &&
                                                            !(this->stateFlags1 & PLAYER_STATE1_400000) &&
                                                            (controlStickDirection == PLAYER_STICK_DIR_FORWARD)))))) {
                } else if ((play->roomCtx.curRoom.type != ROOM_TYPE_INDOORS) && sp24 &&
                    (controlStickDirection >= PLAYER_STICK_DIR_LEFT)) {
                } else if ((this->transformation == PLAYER_FORM_DEKU) && !(this->stateFlags1 & PLAYER_STATE1_8000000) &&
                    (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                        *doAction = DO_ACTION_ATTACK;
                        return true;
                }
        }
    return false;
}

bool Action_Putaway(CONDITION_PARAMETERS) {
    s32 sp24 = Player_IsZTargeting(this);

    if ((sp24 && (this->transformation != PLAYER_FORM_DEKU)) || !(this->stateFlags1 & PLAYER_STATE1_400000) ||
        !Player_IsGoronOrDeku(this)) {
            s32 controlStickDirection = this->controlStickDirections[this->controlStickDataIndex];

            if ((this->transformation != PLAYER_FORM_GORON) &&
                !(this->stateFlags1 & (PLAYER_STATE1_4 | PLAYER_STATE1_4000)) &&
                (controlStickDirection <= PLAYER_STICK_DIR_FORWARD) &&
                (Player_CheckHostileLockOn(this) ||
                    ((sPlayerFloorType != FLOOR_TYPE_7) && (Player_FriendlyLockOnOrParallel(this) ||
                                                            ((play->roomCtx.curRoom.type != ROOM_TYPE_INDOORS) &&
                                                            !(this->stateFlags1 & PLAYER_STATE1_400000) &&
                                                            (controlStickDirection == PLAYER_STICK_DIR_FORWARD)))))) {
                } else if ((play->roomCtx.curRoom.type != ROOM_TYPE_INDOORS) && sp24 &&
                    (controlStickDirection >= PLAYER_STICK_DIR_LEFT)) {
                } else if ((this->transformation == PLAYER_FORM_DEKU) && !(this->stateFlags1 & PLAYER_STATE1_8000000) &&
                    (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                } else if (((this->transformation == PLAYER_FORM_HUMAN) ||
                    (this->transformation == PLAYER_FORM_ZORA)) &&
                    ((this->heldItemAction >= PLAYER_IA_SWORD_KOKIRI) ||
                    ((this->stateFlags2 & PLAYER_STATE2_100000) &&
                        (play->actorCtx.attention.tatlHoverActor == NULL)))) {
                        *doAction = DO_ACTION_PUTAWAY;
                        return true;
                }
        }
    return false;
}

DoActionCondition doActionConditions[5][256];
char doActionSources[5][256][128];

#define QUART_1 63
#define QUART_2 127
#define QUART_3 191

RECOMP_DECLARE_EVENT(DAH_on_init());

RECOMP_CALLBACK("*", recomp_on_init) void on_init() {
    doActionConditions[ATTACK_GORON][QUART_1] = Goron_None;
    doActionConditions[ATTACK_GORON][QUART_2] = Goron_Pound;
    doActionConditions[ATTACK_GORON][QUART_3] = Goron_Punch;

    doActionConditions[ATTACK_ZORA][QUART_1] = Zora_Punch;
    doActionConditions[ATTACK_ZORA][QUART_3] = Zora_Dive;

    doActionConditions[ATTACK_DEKU][QUART_2] = Deku_Shoot;

    doActionConditions[ATTACK_HUMAN][QUART_1] = Human_Blast;
    doActionConditions[ATTACK_HUMAN][QUART_2] = Human_Bremen;
    doActionConditions[ATTACK_HUMAN][QUART_3] = Human_Dance;

    doActionConditions[ACTION][7] = Action_Curl;
    doActionConditions[ACTION][14] = Action_Snap;
    doActionConditions[ACTION][22] = Action_None_1;
    doActionConditions[ACTION][29] = Action_Return_1;
    doActionConditions[ACTION][36] = Action_Reel;
    doActionConditions[ACTION][44] = Action_None_2;
    doActionConditions[ACTION][51] = Action_Down_1;
    doActionConditions[ACTION][58] = Action_Open_1;
    doActionConditions[ACTION][66] = Action_Hops;
    doActionConditions[ACTION][73] = Action_Open_2;
    doActionConditions[ACTION][80] = Action_Grab_1;
    doActionConditions[ACTION][88] = Action_Climb;
    doActionConditions[ACTION][95] = Action_Horse_Speak;
    doActionConditions[ACTION][102] = Action_Horse_Check;
    doActionConditions[ACTION][110] = Action_Horse_Faster;
    doActionConditions[ACTION][117] = Action_Horse_None;
    doActionConditions[ACTION][124] = Action_Speak;
    doActionConditions[ACTION][132] = Action_Check;
    doActionConditions[ACTION][139] = Action_Down_2;
    doActionConditions[ACTION][146] = Action_Drop;
    doActionConditions[ACTION][154] = Action_Throw;
    doActionConditions[ACTION][161] = Action_None_4;
    doActionConditions[ACTION][168] = Action_Grab_2;
    doActionConditions[ACTION][176] = Action_Dive_1;
    doActionConditions[ACTION][183] = Action_Jump_1;
    doActionConditions[ACTION][190] = Action_Return_2;
    doActionConditions[ACTION][198] = Action_Surface;
    doActionConditions[ACTION][205] = Action_Swim;
    doActionConditions[ACTION][212] = Action_Grab_3;
    doActionConditions[ACTION][220] = Action_Dive_2;
    doActionConditions[ACTION][227] = Action_Attack_1;
    doActionConditions[ACTION][234] = Action_Jump_2;
    doActionConditions[ACTION][242] = Action_Attack_2;
    doActionConditions[ACTION][249] = Action_Putaway;

    for (DoActionLevel level = ATTACK_GORON; level <= ACTION; level++) {
        for (int priority = 0; priority < 256; priority++) {
            strcpy(doActionSources[level][priority], "Vanilla");
        }
    }

    DAH_on_init();
}

// Ensure that name is less than 33 characters in length, including null terminator
RECOMP_EXPORT void DoActionHelper_RegisterAction(DoActionLevel level, DoActionCondition condition, unsigned int priority, char name[32]) {
    if (strlen(name) > 31) {
        Notifications_Emit("Do Action Helper", "Do Action registration aborted - mod name too long.", "");
        return;
    }

    static char actionLevels[5][7] = {
        "GORON",
        "ZORA",
        "DEKU",
        "HUMAN",
        "ACTION"
    };

    if (doActionConditions[level][priority] != NULL){
        char message[128];
        sprintf(message, "%s has overwriten %s's Do Action with priority %u on level %s.", name,
            doActionSources[level][priority], priority, actionLevels[level]);

        char bugs[13];
        if (strcmp(doActionSources[level][priority], "Vanilla") == 0) {
            bugs[0] = '\0';
        } else {
            strcpy(bugs, "Expect bugs.");
        }

        Notifications_Emit("Do Action Helper", message, bugs);
    }

    doActionConditions[level][priority] = condition;
    strcpy(doActionSources[level][priority], name);
}

#define CONDITION_CHECK_ATTACK(level)   for (int temp = 0; temp < 256; temp++) { \
                                            if (doActionConditions[level][temp] != NULL && doActionConditions[level][temp](&doActionB, play, this)) { \
                                                break; \
                                            } \
                                        }

/**
 * Updates the two main interface elements that player is responsible for:
 *     - Do Action label on the A/B buttons
 *     - Tatl C-up icon for hints
 */
RECOMP_PATCH void Player_UpdateInterface(PlayState* play, Player* this) {
    DoAction doActionB;

    if (this != GET_PLAYER(play)) {
        return;
    }

    doActionB = DO_ACTION_MAX;

    // Set B do action
    if (this->transformation == PLAYER_FORM_GORON) {
        CONDITION_CHECK_ATTACK(ATTACK_GORON)
    } else if (this->transformation == PLAYER_FORM_ZORA) {
        CONDITION_CHECK_ATTACK(ATTACK_ZORA)
    } else if (this->transformation == PLAYER_FORM_DEKU) {
        CONDITION_CHECK_ATTACK(ATTACK_DEKU)
    } else { // PLAYER_FORM_HUMAN
        CONDITION_CHECK_ATTACK(ATTACK_HUMAN)
    }

    if (doActionB != DO_ACTION_MAX) {
        Interface_SetBButtonPlayerDoAction(play, doActionB);
    } else if (play->interfaceCtx.bButtonPlayerDoActionActive) {
        play->interfaceCtx.bButtonPlayerDoActionActive = false;
        play->interfaceCtx.bButtonPlayerDoAction = 0;
    }

    // Set A do action
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) ||
        ((play->msgCtx.currentTextId >= 0x100) && (play->msgCtx.currentTextId <= 0x200)) ||
        ((play->msgCtx.currentTextId >= 0x1BB2) && (play->msgCtx.currentTextId < 0x1BB7))) {
        DoAction doActionA = DO_ACTION_NONE;

        for (int temp = 0; temp < 256; temp++) {
            if (doActionConditions[ACTION][temp] != NULL && doActionConditions[ACTION][temp](&doActionA, play, this)) {
                break;
            }
        }

        if (doActionA != DO_ACTION_PUTAWAY) {
            this->putAwayCooldownTimer = 20;
        } else if (this->putAwayCooldownTimer != 0) {
            // Replace the "Put Away" Do Action label with a blank label while
            // the cooldown timer is counting down
            doActionA = DO_ACTION_NONE;
            this->putAwayCooldownTimer--;
        }

        Interface_SetAButtonDoAction(play, doActionA);

        // Set Tatl state
        if (!Play_InCsMode(play) && (this->stateFlags2 & PLAYER_STATE2_200000) &&
            !(this->stateFlags3 & PLAYER_STATE3_100)) {
            if (this->focusActor != NULL) {
                Interface_SetTatlCall(play, TATL_STATE_2B);
            } else {
                Interface_SetTatlCall(play, TATL_STATE_2A);
            }
            CutsceneManager_Queue(CS_ID_GLOBAL_TALK);
        } else {
            Interface_SetTatlCall(play, TATL_STATE_2C);
        }
    }
}