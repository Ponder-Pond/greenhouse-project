#include "common.h"
#include "effects.h"
#include "battle/battle.h"
#include "script_api/battle.h"
#include "sprite/npc/GnatAttackFlies.h"
#include "gnat_attack_actors.hpp"
#include "dx/debug_menu.h"

namespace battle::actor {

namespace bomb_fly {

extern EvtScript EVS_Init;
extern EvtScript EVS_Idle;
extern EvtScript EVS_TakeTurn;
extern EvtScript EVS_HandleEvent;
extern EvtScript EVS_HandleEvent_Ignited;
extern EvtScript EVS_HandlePhase;
extern EvtScript EVS_Cleanup;
extern EvtScript EVS_Explode;
extern EvtScript EVS_Defuse;
extern EvtScript EVS_Attack_ChargeBlast;

enum ActorPartIDs {
    PRT_MAIN        = 1,
};

enum ActorVars {
    AVAR_TurnCount    = 0,
    AVAL_TurnOne      = 1,
    AVAL_TurnTwo      = 2,
    AVAR_Ignited      = 1,
};

// Actor Stats
constexpr s32 hp = 6;
constexpr s32 dmgChargeBlast = 6;

s32 DefaultAnims[] = {
    STATUS_KEY_NORMAL,    ANIM_GnatAttackFlies_IdleBombFly,
    STATUS_KEY_STONE,     ANIM_GnatAttackFlies_IdleBombFly,
    STATUS_KEY_SLEEP,     ANIM_GnatAttackFlies_IdleBombFly,
    STATUS_KEY_POISON,    ANIM_GnatAttackFlies_IdleBombFly,
    STATUS_KEY_STOP,      ANIM_GnatAttackFlies_IdleBombFly,
    STATUS_KEY_STATIC,    ANIM_GnatAttackFlies_IdleBombFly,
    STATUS_KEY_PARALYZE,  ANIM_GnatAttackFlies_IdleBombFly,
    STATUS_KEY_DIZZY,     ANIM_GnatAttackFlies_IdleBombFly,
    STATUS_END,
};

s32 IgnitedAnims[] = {
    STATUS_KEY_NORMAL,    ANIM_GnatAttackFlies_AttackBombFly,
};

s32 DefenseTable[] = {
    ELEMENT_NORMAL,   0,
    ELEMENT_END,
};

s32 StatusTable[] = {
    STATUS_KEY_NORMAL,              0,
    STATUS_KEY_DEFAULT,             0,
    STATUS_KEY_SLEEP,             100,
    STATUS_KEY_POISON,            100,
    STATUS_KEY_FROZEN,            100,
    STATUS_KEY_DIZZY,             100,
    STATUS_KEY_UNUSED,            100,
    STATUS_KEY_STATIC,            100,
    STATUS_KEY_PARALYZE,          100,
    STATUS_KEY_SHRINK,            100,
    STATUS_KEY_STOP,              100,
    STATUS_TURN_MOD_DEFAULT,        0,
    STATUS_TURN_MOD_SLEEP,          0,
    STATUS_TURN_MOD_POISON,         0,
    STATUS_TURN_MOD_FROZEN,         0,
    STATUS_TURN_MOD_DIZZY,          0,
    STATUS_TURN_MOD_UNUSED,         0,
    STATUS_TURN_MOD_STATIC,         0,
    STATUS_TURN_MOD_PARALYZE,       0,
    STATUS_TURN_MOD_SHRINK,         0,
    STATUS_TURN_MOD_STOP,           0,
    STATUS_END,
};

ActorPartBlueprint ActorParts[] = {
    {
        .flags = ACTOR_PART_FLAG_PRIMARY_TARGET,
        .index = PRT_MAIN,
        .posOffset = { 0, 0, 0 },
        .targetOffset = { 0, 0 },
        .opacity = 255,
        .idleAnimations = DefaultAnims,
        .defenseTable = DefenseTable,
        .eventFlags = 0,
        .elementImmunityFlags = 0,
        .projectileTargetOffset = { 0, 0 },
    },
};

EvtScript EVS_Init = {
    Call(BindTakeTurn, ACTOR_SELF, Ref(EVS_TakeTurn))
    Call(BindIdle, ACTOR_SELF, Ref(EVS_Idle))
    Call(BindHandleEvent, ACTOR_SELF, Ref(EVS_HandleEvent))
    Call(BindHandlePhase, ACTOR_SELF, Ref(EVS_HandlePhase))
    Call(SetActorVar, ACTOR_SELF, AVAR_TurnCount, AVAL_TurnOne)
    Return
    End
};

EvtScript EVS_Idle = {
    Return
    End
};

EvtScript EVS_HandleEvent = {
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    Call(SetActorDispOffset, ACTOR_SELF, 0, 0, 0)
    Call(GetLastEvent, ACTOR_SELF, LVar0)
    Switch(LVar0)
        CaseEq(EVENT_HIT_COMBO)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
            ExecWait(EVS_Enemy_Hit)
        CaseEq(EVENT_HIT)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
            ExecWait(EVS_Enemy_Hit)
        CaseEq(EVENT_BURN_HIT)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly) // BurnHurt
            SetConst(LVar2, ANIM_GnatAttackFlies_IdleBombFly) // BurnStill
            ExecWait(EVS_Enemy_BurnHit)
        CaseEq(EVENT_SHOCK_HIT)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
            ExecWait(EVS_Enemy_ShockHit)
        CaseEq(EVENT_SHOCK_DEATH)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
            ExecWait(EVS_Enemy_ShockHit)
            KillThread(LVar0)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
            ExecWait(EVS_Enemy_Death)
            Return
        CaseOrEq(EVENT_ZERO_DAMAGE)
        CaseOrEq(EVENT_IMMUNE)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
            ExecWait(EVS_Enemy_NoDamageHit)
        EndCaseGroup
        CaseEq(EVENT_DEATH)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
            ExecWait(EVS_Enemy_Hit)
            Wait(10)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
            ExecWait(EVS_Enemy_Death)
            Return
        CaseEq(EVENT_BURN_DEATH)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
            SetConst(LVar2, ANIM_GnatAttackFlies_IdleBombFly)
            ExecWait(EVS_Enemy_BurnHit)
            Wait(10)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
            ExecWait(EVS_Enemy_Death)
            Return
        CaseEq(EVENT_END_FIRST_STRIKE)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
            ExecWait(EVS_Enemy_ReturnHome)
            Call(HPBarToHome, ACTOR_SELF)
        CaseEq(EVENT_RECOVER_STATUS)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
            ExecWait(EVS_Enemy_Recover)
        CaseDefault
    EndSwitch
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};

EvtScript EVS_HandleEvent_Ignited = {
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    Call(GetLastEvent, ACTOR_SELF, LVar0)
    Switch(LVar0)
        CaseEq(EVENT_HIT_COMBO)
            Call(GetLastElement, LVarE)
            IfFlag(LVarE, DAMAGE_TYPE_WATER)
                ExecWait(EVS_Defuse)
                SetConst(LVar0, PRT_MAIN)
                SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
                ExecWait(EVS_Enemy_Hit)
            Else
                SetConst(LVar0, PRT_MAIN)
                SetConst(LVar1, ANIM_GnatAttackFlies_AttackBombFly)
                ExecWait(EVS_Enemy_Hit)
            EndIf
        CaseEq(EVENT_HIT)
            Call(GetLastElement, LVarE)
            Switch(LVarE)
                CaseFlag(DAMAGE_TYPE_WATER)
                    ExecWait(EVS_Defuse)
                    SetConst(LVar0, PRT_MAIN)
                    SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
                    ExecWait(EVS_Enemy_Hit)
                CaseFlag(DAMAGE_TYPE_SHOCK)
                    SetConst(LVar0, PRT_MAIN)
                    SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
                    ExecWait(EVS_Enemy_Hit)
                    ExecWait(EVS_Explode)
                    SetConst(LVar0, PRT_MAIN)
                    SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
                    Set(LVar2, EXEC_DEATH_NO_SPINNING)
                    ExecWait(EVS_Enemy_Death)
                    Return
                CaseDefault
                    SetConst(LVar0, PRT_MAIN)
                    SetConst(LVar1, ANIM_GnatAttackFlies_AttackBombFly)
                    ExecWait(EVS_Enemy_Hit)
                    Call(GetLastDamage, ACTOR_SELF, LVar0)
                    IfGt(LVar0, 0)
                        ExecWait(EVS_Explode)
                        SetConst(LVar0, PRT_MAIN)
                        SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
                        Set(LVar2, EXEC_DEATH_NO_SPINNING)
                        ExecWait(EVS_Enemy_Death)
                        Return
                    EndIf
            EndSwitch
        CaseOrEq(EVENT_BURN_HIT)
        CaseOrEq(EVENT_BURN_DEATH)
        CaseOrEq(EVENT_SPIN_SMASH_HIT)
        CaseOrEq(EVENT_SPIN_SMASH_DEATH)
        CaseOrEq(EVENT_EXPLODE_TRIGGER)
            ExecWait(EVS_Explode)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
            Set(LVar2, EXEC_DEATH_NO_SPINNING)
            ExecWait(EVS_Enemy_Death)
            Return
        EndCaseGroup
        CaseOrEq(EVENT_ZERO_DAMAGE)
        CaseOrEq(EVENT_IMMUNE)
        CaseOrEq(EVENT_AIR_LIFT_FAILED)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_AttackBombFly)
            ExecWait(EVS_Enemy_NoDamageHit)
        EndCaseGroup
        CaseEq(EVENT_DEATH)
            Call(GetLastElement, LVarE)
            IfFlag(LVarE, DAMAGE_TYPE_WATER)
                ExecWait(EVS_Defuse)
                SetConst(LVar0, PRT_MAIN)
                SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
            Else
                ExecWait(EVS_Explode)
                SetConst(LVar0, PRT_MAIN)
                SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
                Set(LVar2, EXEC_DEATH_NO_SPINNING)
            EndIf
            ExecWait(EVS_Enemy_Death)
            Return
        CaseOrEq(EVENT_SHOCK_HIT)
        CaseOrEq(EVENT_SHOCK_DEATH)
            ExecWait(EVS_Explode)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
            Set(LVar2, EXEC_DEATH_NO_SPINNING)
            ExecWait(EVS_Enemy_Death)
            Return
        EndCaseGroup
        CaseEq(EVENT_RECOVER_STATUS)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_AttackBombFly)
            ExecWait(EVS_Enemy_Recover)
        CaseEq(EVENT_BEGIN_AIR_LIFT)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
            ExecWait(EVS_Enemy_AirLift)
        CaseEq(EVENT_BLOW_AWAY)
            Call(GetDamageSource, LVar0)
            IfEq(LVar0, DMG_SRC_HURRICANE)
                ExecWait(EVS_Defuse)
                SetConst(LVar0, PRT_MAIN)
                SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
            Else
                ChildThread
                    Wait(100)
                    ExecWait(EVS_Explode)
                EndChildThread
                SetConst(LVar0, PRT_MAIN)
                SetConst(LVar1, ANIM_GnatAttackFlies_IdleBombFly)
            EndIf
            ExecWait(EVS_Enemy_BlowAway)
            Return
        CaseEq(EVENT_UP_AND_AWAY)
            ExecWait(EVS_Defuse)
        CaseDefault
    EndSwitch
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};

EvtScript EVS_Cleanup = {
    Call(GetActorVar, ACTOR_SELF, AVAR_Ignited, LVar0)
    IfEq(LVar0, true)
        // Call(StopLoopingSoundAtActor, ACTOR_SELF, 0)
        // Call(EnableActorPaletteEffects, ACTOR_SELF, PRT_MAIN, false)
    EndIf
    Return
    End
};

EvtScript EVS_Explode = {
    // ExecWait(EVS_Cleanup)
    Call(StartRumble, BTL_RUMBLE_PLAYER_MAX)
    Thread
        Call(ShakeCam, CAM_BATTLE, 0, 2, Float(0.75))
        Call(ShakeCam, CAM_BATTLE, 0, 5, Float(3.0))
        Call(ShakeCam, CAM_BATTLE, 0, 10, Float(4.5))
        Call(ShakeCam, CAM_BATTLE, 0, 5, Float(3.0))
    EndThread
    Call(GetActorPos, ACTOR_SELF, LVar0, LVar1, LVar2)
    Add(LVar2, 2)
    PlayEffect(EFFECT_SMOKE_RING, 0, LVar0, LVar1, LVar2, 0)
    Add(LVar1, 20)
    Add(LVar2, 2)
    PlayEffect(EFFECT_EXPLOSION, 0, LVar0, LVar1, LVar2, 0)
    Call(PlaySoundAtActor, ACTOR_SELF, SOUND_BOMB_BLAST)
    Return
    End
};

EvtScript EVS_Defuse = {
    Call(BindHandleEvent, ACTOR_SELF, Ref(EVS_HandleEvent))
    Call(SetActorVar, ACTOR_SELF, AVAR_Ignited, false)
    Call(SetIdleAnimations, ACTOR_SELF, PRT_MAIN, Ref(DefaultAnims))
    Call(SetPartEventBits, ACTOR_SELF, PRT_MAIN, ACTOR_EVENT_FLAG_EXPLODE_ON_CONTACT, false)
    Call(SetStatusTable, ACTOR_SELF, Ref(StatusTable))
    // Call(GetActorPos, ACTOR_SELF, LVar0, LVar1, LVar2)
    // Add(LVar2, 2)
    // PlayEffect(EFFECT_LANDING_DUST, 3, LVar0, LVar1, LVar2, 0, 0)
    // Call(StopLoopingSoundAtActor, ACTOR_SELF, 0)
    // Call(EnableActorPaletteEffects, ACTOR_SELF, PRT_MAIN, false)
    Return
    End
};

EvtScript EVS_TakeTurn = {
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    Call(GetActorVar, ACTOR_SELF, AVAR_TurnCount, LVar0)
    Switch(LVar0)
    CaseEq(AVAL_TurnOne)
        Call(SetIdleAnimations, ACTOR_SELF, PRT_MAIN, Ref(IgnitedAnims))
        Call(BindHandleEvent, ACTOR_SELF, Ref(EVS_HandleEvent_Ignited))
        Call(SetPartEventBits, ACTOR_SELF, PRT_MAIN, ACTOR_EVENT_FLAG_EXPLODE_ON_CONTACT, true)
        Call(SetActorVar, ACTOR_SELF, AVAR_Ignited, true )
    CaseEq(AVAL_TurnTwo)
        ExecWait(EVS_Attack_ChargeBlast)
    EndSwitch
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};


EvtScript EVS_Attack_ChargeBlast = {
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)

    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};

EvtScript EVS_HandlePhase = {
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    Call(GetBattlePhase, LVar0)
    Switch(LVar0)
        CaseEq(PHASE_PLAYER_BEGIN)
    EndSwitch
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};

}; // namespace bomb_fly

ActorBlueprint BombFly = {
    .flags = ACTOR_FLAG_FLYING,
    .maxHP = bomb_fly::hp,
    .type = ACTOR_TYPE_BOMB_FLY,
    .level = ACTOR_LEVEL_BOMB_FLY,
    .partCount = ARRAY_COUNT(bomb_fly::ActorParts),
    .partsData = bomb_fly::ActorParts,
    .initScript = &bomb_fly::EVS_Init,
    .statusTable = bomb_fly::StatusTable,
    .escapeChance = 0,
    .airLiftChance = 0,
    .hurricaneChance = 0,
    .spookChance = 0,
    .upAndAwayChance = 0,
    .spinSmashReq = 0,
    .powerBounceChance = 80,
    .coinReward = 0,
    .size = { 24, 24 },
    .healthBarOffset = { 0, 0 },
    .statusIconOffset = { -10, 20 },
    .statusTextOffset = { 10, 20 },
};

}; // namespace battle::actor
