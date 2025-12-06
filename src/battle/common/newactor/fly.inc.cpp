#include "common.h"
#include "effects.h"
#include "battle/battle.h"
#include "script_api/battle.h"
#include "sprite/npc/GnatAttackFlies.h"
#include "gnat_attack_actors.hpp"
#include "dx/debug_menu.h"

namespace battle::actor {

namespace fly {

extern EvtScript EVS_Init;
extern EvtScript EVS_Idle;
extern EvtScript EVS_TakeTurn;
extern EvtScript EVS_HandleEvent;
extern EvtScript EVS_HandlePhase;
extern EvtScript EVS_Attack_Charge;

enum ActorPartIDs {
    PRT_MAIN        = 1,
};

enum ActorVars {
    AVAR_TurnCount    = 0,
    AVAL_TurnOne      = 1,
};

// Actor Stats
constexpr s32 hp = 2;
constexpr s32 dmgCharge = 1;

s32 DefaultAnims[] = {
    STATUS_KEY_NORMAL,    ANIM_GnatAttackFlies_IdleFly,
    STATUS_KEY_STONE,     ANIM_GnatAttackFlies_IdleFly,
    STATUS_KEY_SLEEP,     ANIM_GnatAttackFlies_IdleFly,
    STATUS_KEY_POISON,    ANIM_GnatAttackFlies_IdleFly,
    STATUS_KEY_STOP,      ANIM_GnatAttackFlies_IdleFly,
    STATUS_KEY_STATIC,    ANIM_GnatAttackFlies_IdleFly,
    STATUS_KEY_PARALYZE,  ANIM_GnatAttackFlies_IdleFly,
    STATUS_KEY_DIZZY,     ANIM_GnatAttackFlies_IdleFly,
    STATUS_END,
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
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleFly)
            ExecWait(EVS_Enemy_Hit)
        CaseEq(EVENT_HIT)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleFly)
            ExecWait(EVS_Enemy_Hit)
        CaseEq(EVENT_BURN_HIT)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleFly) // BurnHurt
            SetConst(LVar2, ANIM_GnatAttackFlies_IdleFly) // BurnStill
            ExecWait(EVS_Enemy_BurnHit)
        CaseEq(EVENT_SHOCK_HIT)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleFly)
            ExecWait(EVS_Enemy_ShockHit)
        CaseEq(EVENT_SHOCK_DEATH)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleFly)
            ExecWait(EVS_Enemy_ShockHit)
            KillThread(LVar0)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleFly)
            ExecWait(EVS_Enemy_Death)
            Return
        CaseOrEq(EVENT_ZERO_DAMAGE)
        CaseOrEq(EVENT_IMMUNE)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleFly)
            ExecWait(EVS_Enemy_NoDamageHit)
        EndCaseGroup
        CaseEq(EVENT_DEATH)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleFly)
            ExecWait(EVS_Enemy_Hit)
            Wait(10)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleFly)
            ExecWait(EVS_Enemy_Death)
            Return
        CaseEq(EVENT_BURN_DEATH)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleFly)
            SetConst(LVar2, ANIM_GnatAttackFlies_IdleFly)
            ExecWait(EVS_Enemy_BurnHit)
            Wait(10)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleFly)
            ExecWait(EVS_Enemy_Death)
            Return
        CaseEq(EVENT_END_FIRST_STRIKE)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleFly)
            ExecWait(EVS_Enemy_ReturnHome)
            Call(HPBarToHome, ACTOR_SELF)
        CaseEq(EVENT_RECOVER_STATUS)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_GnatAttackFlies_IdleFly)
            ExecWait(EVS_Enemy_Recover)
        CaseDefault
    EndSwitch
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};

EvtScript EVS_TakeTurn = {
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    Call(GetActorVar, ACTOR_SELF, AVAR_TurnCount, LVar0)
    Switch(LVar0)
    CaseEq(AVAL_TurnOne)
        ExecWait(EVS_Attack_Charge)
    EndSwitch
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};


EvtScript EVS_Attack_Charge = {
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    Call(SetTargetActor, ACTOR_SELF, ACTOR_PLAYER)
    Call(SetGoalToTarget, ACTOR_SELF)
    Call(UseBattleCamPreset, BTL_CAM_ENEMY_APPROACH)
    Call(BattleCamTargetActor, ACTOR_SELF)
    Call(SetBattleCamTargetingModes, BTL_CAM_YADJ_TARGET, BTL_CAM_XADJ_AVG, FALSE)
    Call(SetAnimation, ACTOR_SELF, PRT_MAIN, ANIM_GnatAttackFlies_IdleFly)
    Call(UseBattleCamPresetImmediately, BTL_CAM_REPOSITION)
    Call(EnemyTestTarget, ACTOR_SELF, LVar0, 0, 0, 1, BS_FLAGS1_INCLUDE_POWER_UPS)
    Switch(LVar0)
        CaseOrEq(HIT_RESULT_MISS)
        CaseOrEq(HIT_RESULT_LUCKY)
            Set(LVarA, LVar0)
            Call(GetActorPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Set(LVar3, LVar1)
            Call(GetActorPos, ACTOR_PLAYER, LVar0, LVar1, LVar2)
            Set(LVar1, LVar3)
            Sub(LVar0, 90)
            Call(SetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
            Call(FlyToGoal, ACTOR_SELF, 30, 0, EASING_LINEAR)
            IfEq(LVarA, HIT_RESULT_LUCKY)
                Call(EnemyTestTarget, ACTOR_SELF, LVar0, DAMAGE_TYPE_TRIGGER_LUCKY, 0, 0, 0)
            EndIf
            Call(UseBattleCamPreset, BTL_CAM_DEFAULT)
            Call(YieldTurn)
            Call(HideHealthBar, ACTOR_SELF)
            Call(UseIdleAnimation, ACTOR_SELF, FALSE)
            Call(RemoveActor, ACTOR_SELF)
            Return
        EndCaseGroup
    EndSwitch
    // Call(GetActorPos, ACTOR_SELF, LVar0, LVar1, LVar2)
    // Set(LVar3, LVar1)
    Call(GetActorPos, ACTOR_PLAYER, LVar0, LVar1, LVar2)
    // Set(LVar1, LVar3)
    Add(LVar0, 10)
    Add(LVar1, 10)
    Call(SetGoalPos, ACTOR_SELF, LVar0, LVar1, LVar2)
    Call(FlyToGoal, ACTOR_SELF, 15, 0, EASING_LINEAR)
    Call(SetGoalToTarget, ACTOR_SELF)
    Wait(2)
    Call(EnemyDamageTarget, ACTOR_SELF, LVar0, 0, 0, 0, dmgCharge, BS_FLAGS1_TRIGGER_EVENTS)
    Call(UseBattleCamPreset, BTL_CAM_DEFAULT)
    Call(YieldTurn)
    SetConst(LVar0, PRT_MAIN)
    SetConst(LVar1, ANIM_GnatAttackFlies_IdleFly)
    Set(LVar2, EXEC_DEATH_NO_SPINNING)
    ExecWait(EVS_Enemy_Death)
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

}; // namespace fly

ActorBlueprint Fly = {
    .flags = ACTOR_FLAG_FLYING,
    .maxHP = fly::hp,
    .type = ACTOR_TYPE_FLY,
    .level = ACTOR_LEVEL_FLY,
    .partCount = ARRAY_COUNT(fly::ActorParts),
    .partsData = fly::ActorParts,
    .initScript = &fly::EVS_Init,
    .statusTable = fly::StatusTable,
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
