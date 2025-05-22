#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"

#include "overlays/actors/ovl_En_Horse/z_en_horse.h"

extern FloorType sPlayerFloorType;
extern void Player_Action_28(Player* this, PlayState* play);
extern void Player_Action_80(Player* this, PlayState* play);
extern void Player_Action_53(Player* this, PlayState* play);
extern bool func_8082DA90(PlayState* play);
extern s32 Player_CanThrowCarriedActor(Player* this, Actor* heldActor);
extern bool Player_IsZTargeting(Player* this);

/**
 * Updates the two main interface elements that player is responsible for:
 *     - Do Action label on the A/B buttons
 *     - Tatl C-up icon for hints
 */
RECOMP_PATCH void Player_UpdateInterface(PlayState* play, Player* this) {
    s32 doActionB;
    s32 sp38;

    if (this != GET_PLAYER(play)) {
        return;
    }

    doActionB = -1;
    sp38 = func_801242B4(this) || (Player_Action_28 == this->actionFunc);

    // Set B do action
    if (this->transformation == PLAYER_FORM_GORON) {
        if (this->stateFlags3 & PLAYER_STATE3_80000) {
            doActionB = DO_ACTION_NONE;
        } else if (this->stateFlags3 & PLAYER_STATE3_1000) {
            doActionB = DO_ACTION_POUND;
        } else {
            doActionB = DO_ACTION_PUNCH;
        }
    } else if (this->transformation == PLAYER_FORM_ZORA) {
        if ((!(this->stateFlags1 & PLAYER_STATE1_8000000)) ||
            (!sp38 && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND))) {
            doActionB = DO_ACTION_PUNCH;
        } else {
            doActionB = DO_ACTION_DIVE;
        }
    } else if (this->transformation == PLAYER_FORM_DEKU) {
        doActionB = DO_ACTION_SHOOT;
    } else { // PLAYER_FORM_HUMAN
        if (this->currentMask == PLAYER_MASK_BLAST) {
            doActionB = DO_ACTION_EXPLODE;
        } else if (this->currentMask == PLAYER_MASK_BREMEN) {
            doActionB = DO_ACTION_MARCH;
        } else if (this->currentMask == PLAYER_MASK_KAMARO) {
            doActionB = DO_ACTION_DANCE;
        }
    }

    if (doActionB > -1) {
        Interface_SetBButtonPlayerDoAction(play, doActionB);
    } else if (play->interfaceCtx.bButtonPlayerDoActionActive) {
        play->interfaceCtx.bButtonPlayerDoActionActive = false;
        play->interfaceCtx.bButtonPlayerDoAction = 0;
    }

    // Set A do action
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) ||
        ((play->msgCtx.currentTextId >= 0x100) && (play->msgCtx.currentTextId <= 0x200)) ||
        ((play->msgCtx.currentTextId >= 0x1BB2) && (play->msgCtx.currentTextId < 0x1BB7))) {
        Actor* heldActor = this->heldActor;
        Actor* interactRangeActor = this->interactRangeActor;
        s32 pad;
        s32 controlStickDirection = this->controlStickDirections[this->controlStickDataIndex];
        s32 sp24;
        DoAction doActionA =
            ((this->transformation == PLAYER_FORM_GORON) && !(this->stateFlags1 & PLAYER_STATE1_400000))
                ? DO_ACTION_CURL
                : DO_ACTION_NONE;

        if (play->actorCtx.flags & ACTORCTX_FLAG_PICTO_BOX_ON) {
            doActionA = DO_ACTION_SNAP;
        } else if (Player_InBlockingCsMode(play, this) || (this->actor.flags & ACTOR_FLAG_OCARINA_INTERACTION) ||
                   (this->stateFlags1 & PLAYER_STATE1_CHARGING_SPIN_ATTACK) ||
                   (this->stateFlags3 & PLAYER_STATE3_80000) || (Player_Action_80 == this->actionFunc)) {
            doActionA = DO_ACTION_NONE;
        } else if (this->stateFlags1 & PLAYER_STATE1_100000) {
            doActionA = DO_ACTION_RETURN;
        } else if ((this->heldItemAction == PLAYER_IA_FISHING_ROD) && (this->unk_B28 != 0)) {
            doActionA = (this->unk_B28 == 2) ? DO_ACTION_REEL : DO_ACTION_NONE;
        } else if (this->stateFlags3 & PLAYER_STATE3_2000) {
            doActionA = DO_ACTION_DOWN;
        } else if ((this->doorType != PLAYER_DOORTYPE_NONE) && (this->doorType != PLAYER_DOORTYPE_STAIRCASE) &&
                   !(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
            doActionA = DO_ACTION_OPEN;
        } else if (this->stateFlags3 & PLAYER_STATE3_200000) {
            static u8 D_8085D34C[] = {
                DO_ACTION_1, DO_ACTION_2, DO_ACTION_3, DO_ACTION_4, DO_ACTION_5, DO_ACTION_6, DO_ACTION_7, DO_ACTION_8,
            };

            doActionA = D_8085D34C[this->remainingHopsCounter];
        } else if ((!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) || (heldActor == NULL)) &&
                   (interactRangeActor != NULL) && (this->getItemId < GI_NONE)) {
            doActionA = DO_ACTION_OPEN;
        } else if (!sp38 && (this->stateFlags2 & PLAYER_STATE2_1)) {
            doActionA = DO_ACTION_GRAB;
        } else if ((this->stateFlags2 & PLAYER_STATE2_4) ||
                   (!(this->stateFlags1 & PLAYER_STATE1_800000) && (this->rideActor != NULL))) {
            doActionA = DO_ACTION_CLIMB;
        } else if ((this->stateFlags1 & PLAYER_STATE1_800000) &&
                   (!EN_HORSE_CHECK_4((EnHorse*)this->rideActor) && (Player_Action_53 != this->actionFunc))) {
            if ((this->stateFlags2 & PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER) && (this->talkActor != NULL)) {
                if ((this->talkActor->category == ACTORCAT_NPC) || (this->talkActor->id == ACTOR_DM_CHAR08)) {
                    doActionA = DO_ACTION_SPEAK;
                } else {
                    doActionA = DO_ACTION_CHECK;
                }
            } else if (!func_8082DA90(play) && !func_800B7128(this) && !(this->stateFlags1 & PLAYER_STATE1_100000)) {
                doActionA = DO_ACTION_FASTER;
            } else {
                doActionA = DO_ACTION_NONE;
            }
        } else if ((this->stateFlags2 & PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER) && (this->talkActor != NULL)) {
            if ((this->talkActor->category == ACTORCAT_NPC) || (this->talkActor->category == ACTORCAT_ENEMY) ||
                (this->talkActor->id == ACTOR_DM_CHAR08)) {
                doActionA = DO_ACTION_SPEAK;
            } else {
                doActionA = DO_ACTION_CHECK;
            }
        } else if ((this->stateFlags1 & (PLAYER_STATE1_2000 | PLAYER_STATE1_200000)) ||
                   ((this->stateFlags1 & PLAYER_STATE1_800000) && (this->stateFlags2 & PLAYER_STATE2_400000))) {
            doActionA = DO_ACTION_DOWN;
        } else if ((this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->getItemId == GI_NONE) &&
                   (heldActor != NULL)) {
            if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (heldActor->id == ACTOR_EN_NIW)) {
                if (!Player_CanThrowCarriedActor(this, heldActor)) {
                    doActionA = DO_ACTION_DROP;
                } else {
                    doActionA = DO_ACTION_THROW;
                }
            } else {
                doActionA = DO_ACTION_NONE;
            }
        } else if (this->stateFlags2 & PLAYER_STATE2_10000) {
            doActionA = DO_ACTION_GRAB;
        } else if (this->stateFlags2 & PLAYER_STATE2_800) {
            static u8 D_8085D354[] = { DO_ACTION_1, DO_ACTION_2 };
            s32 var_v0;

            var_v0 = ((120.0f - this->actor.depthInWater) / 40.0f);
            var_v0 = CLAMP(var_v0, 0, ARRAY_COUNT(D_8085D354) - 1);

            doActionA = D_8085D354[var_v0];
        } else if (this->stateFlags3 & PLAYER_STATE3_100) {
            doActionA = DO_ACTION_JUMP;
        } else if (this->stateFlags3 & PLAYER_STATE3_1000) {
            doActionA = DO_ACTION_RETURN;
        } else if (!Player_IsZTargeting(this) && (this->stateFlags1 & PLAYER_STATE1_8000000) && !sp38) {
            doActionA = DO_ACTION_SURFACE;
        } else if (((this->transformation != PLAYER_FORM_DEKU) &&
                    (sp38 || ((this->stateFlags1 & PLAYER_STATE1_8000000) &&
                              !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)))) ||
                   ((this->transformation == PLAYER_FORM_DEKU) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
                    func_800C9DDC(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId))) {
            doActionA = (this->transformation == PLAYER_FORM_ZORA) ? DO_ACTION_SWIM
                        : ((this->stateFlags1 & PLAYER_STATE1_8000000) && (interactRangeActor != NULL) &&
                           (interactRangeActor->id == ACTOR_EN_ZOG))
                            ? DO_ACTION_GRAB
                            : DO_ACTION_DIVE;
        } else {
            sp24 = Player_IsZTargeting(this);
            if ((sp24 && (this->transformation != PLAYER_FORM_DEKU)) || !(this->stateFlags1 & PLAYER_STATE1_400000) ||
                !Player_IsGoronOrDeku(this)) {
                if ((this->transformation != PLAYER_FORM_GORON) &&
                    !(this->stateFlags1 & (PLAYER_STATE1_4 | PLAYER_STATE1_4000)) &&
                    (controlStickDirection <= PLAYER_STICK_DIR_FORWARD) &&
                    (Player_CheckHostileLockOn(this) ||
                     ((sPlayerFloorType != FLOOR_TYPE_7) && (Player_FriendlyLockOnOrParallel(this) ||
                                                             ((play->roomCtx.curRoom.type != ROOM_TYPE_INDOORS) &&
                                                              !(this->stateFlags1 & PLAYER_STATE1_400000) &&
                                                              (controlStickDirection == PLAYER_STICK_DIR_FORWARD)))))) {
                    doActionA = DO_ACTION_ATTACK;
                } else if ((play->roomCtx.curRoom.type != ROOM_TYPE_INDOORS) && sp24 &&
                           (controlStickDirection >= PLAYER_STICK_DIR_LEFT)) {
                    doActionA = DO_ACTION_JUMP;
                } else if ((this->transformation == PLAYER_FORM_DEKU) && !(this->stateFlags1 & PLAYER_STATE1_8000000) &&
                           (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                    doActionA = DO_ACTION_ATTACK;
                } else if (((this->transformation == PLAYER_FORM_HUMAN) ||
                            (this->transformation == PLAYER_FORM_ZORA)) &&
                           ((this->heldItemAction >= PLAYER_IA_SWORD_KOKIRI) ||
                            ((this->stateFlags2 & PLAYER_STATE2_100000) &&
                             (play->actorCtx.attention.tatlHoverActor == NULL)))) {
                    doActionA = DO_ACTION_PUTAWAY;

                    if (play->msgCtx.currentTextId == 0) {} //! FAKE
                }
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