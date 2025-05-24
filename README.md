# Majora's Mask: Recompiled Do Action Helper

A library mod that aims to provide mod developers a centralised interface for adding, changing, or removing Do Action conditions.

Usage:
1. Download the (header)[https://github.com/t0mtee/MMRecompDoActionHelper/tree/main/include_in_dependents/DAH_header.h] and place it in your mods include folder.
2. Add `mm_recomp_do_action_helper` as a dependency in `mod.toml`.
3. Add `#include "DAH_header.h"` to your mod's source file.
4. Create your condition. Have `CONDITION_PARAMETERS` be the parameters and set the Do Action with `*doAction = DO_ACTION_XXXX;`. Have the function return `true` if don't want other conditions to be checked, return `false` if you do. Here's an example:
```C
bool Attack_Drop(CONDITION_PARAMETERS) {
    Actor* heldActor = this->heldActor;

    if ((this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->getItemId == GI_NONE) &&
        (heldActor != NULL)) {
            if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (heldActor->id == ACTOR_EN_NIW)) {
                *doAction = DO_ACTION_DROP;
                return true;
            }
        }
    return false;
}
```
5. Use `DAH_ON_INIT` to register your Do Action conditions using `DoActionHelper_RegisterAction`. Make sure your `name` argument is 32 characters or less (including NULL terminator). Here's an example:
```C
DAH_ON_INIT void DAH_on_init() {
    DoActionHelper_RegisterAction(ACTION, Action_Drop_Replace, 146, "Modern Throwing");
    DoActionHelper_RegisterAction(ATTACK_GORON, Attack_Drop, 32, "Modern Throwing");
    DoActionHelper_RegisterAction(ATTACK_ZORA, Attack_Drop, 32, "Modern Throwing");
    DoActionHelper_RegisterAction(ATTACK_DEKU, Attack_Drop, 32, "Modern Throwing");
    DoActionHelper_RegisterAction(ATTACK_HUMAN, Attack_Drop, 32, "Modern Throwing");
}
```

Notes:
- The priorities of vanilla conditions can be seen in the source code.
- You should check which priorities other mods use when they register conditions to prevent conflicts and make sure your condition is checked before or after other mods.