#include "common.h"
#include "effects.h"
#include "battle/battle.h"
#include "script_api/battle.h"
#include "sprite/npc/KingWatinga.h"
#include "sprite/npc/GnatAttackFlies.h"
// #include "sprite/player.h"
#include "gnat_attack_actors.hpp"
#include "dx/debug_menu.h"

namespace battle::actor {

namespace king_watinga {

extern EvtScript EVS_Init;
extern EvtScript EVS_Idle;
extern EvtScript EVS_TakeTurn_PhaseOne;
extern EvtScript EVS_TakeTurn_PhaseTwo;
extern EvtScript EVS_HandleEvent;
extern EvtScript EVS_HandlePhase;
extern EvtScript EVS_Fly_Summons;
extern EvtScript EVS_Summon_Fly;
extern EvtScript EVS_Summon_SuperFly;
extern EvtScript EVS_Summon_ParentFly;
extern EvtScript EVS_Summon_BombFly;
extern EvtScript EVS_Assist_Attacks;
extern EvtScript EVS_Attack_SmallFlyShot;
extern EvtScript EVS_Attack_EnergyShot;
extern EvtScript EVS_Move_FlyAway;
extern EvtScript EVS_Move_Charge;
extern EvtScript EVS_TransitionToPhaseTwo;
extern EvtScript EVS_KingWatinga_Death;

enum ActorPartIDs {
    PRT_MAIN        = 1,
    // PRT_TARGET      = 2,
};

enum ActorVars {
    AVAR_Phase        = 0,
    AVAL_PhaseOne     = 1,
    AVAL_PhaseTwo     = 2,
    AVAR_TurnCount    = 1,
    AVAL_TurnOne      = 1,
    AVAL_TurnTwo      = 2,
    AVAL_TurnThree    = 3,
    AVAL_TurnFour     = 4,
    AVAL_TurnFive     = 5,
};

// Actor Stats
constexpr s32 hp = 40;
constexpr s32 dmgSmallFlyShot = 2;
constexpr s32 dmgEnergyShot = 4;

s32 DefaultAnims[] = {
    STATUS_KEY_NORMAL,    ANIM_KingWatinga_Idle,
    STATUS_KEY_STONE,     ANIM_KingWatinga_Still,
    // STATUS_KEY_SLEEP,     ANIM_GoombaKing_Sleep,
    STATUS_KEY_POISON,    ANIM_KingWatinga_Still,
    STATUS_KEY_STOP,      ANIM_KingWatinga_Still,
    STATUS_KEY_STATIC,    ANIM_KingWatinga_Still,
    STATUS_KEY_PARALYZE,  ANIM_KingWatinga_Still,
    // STATUS_KEY_DIZZY,     ANIM_GoombaKing_Dizzy,
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
        .targetOffset = { 0, 65 },
        .opacity = 255,
        .idleAnimations = DefaultAnims,
        .defenseTable = DefenseTable,
        .eventFlags = 0,
        .elementImmunityFlags = 0,
        .projectileTargetOffset = { 0, 0 },
    },
    // {
    //     .flags = ACTOR_PART_FLAG_IGNORE_BELOW_CHECK | ACTOR_PART_FLAG_PRIMARY_TARGET,
    //     .index = PRT_TARGET,
    //     .posOffset = { 0, 0, 0 },
    //     .targetOffset = { 0, 80 },
    //     .opacity = 255,
    //     .idleAnimations = NULL,
    //     .defenseTable = DefenseTable,
    //     .eventFlags = 0,
    //     .elementImmunityFlags = 0,
    //     .projectileTargetOffset = { 0, 0 },
    // },
};

EvtScript EVS_Init = {
    Call(BindTakeTurn, ACTOR_SELF, Ref(EVS_TakeTurn_PhaseOne))
    Call(BindIdle, ACTOR_SELF, Ref(EVS_Idle))
    Call(BindHandleEvent, ACTOR_SELF, Ref(EVS_HandleEvent))
    Call(BindHandlePhase, ACTOR_SELF, Ref(EVS_HandlePhase))
    Call(SetActorVar, ACTOR_SELF, AVAR_Phase, AVAL_PhaseOne)
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
            SetConst(LVar1, ANIM_KingWatinga_Hurt)
            ExecWait(EVS_Enemy_Hit)
        CaseEq(EVENT_HIT)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_KingWatinga_Hurt)
            ExecWait(EVS_Enemy_Hit)
        CaseEq(EVENT_BURN_HIT)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_KingWatinga_Hurt) // BurnHurt
            SetConst(LVar2, ANIM_KingWatinga_Hurt) // BurnStill
            ExecWait(EVS_Enemy_BurnHit)
        CaseEq(EVENT_SHOCK_HIT)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_KingWatinga_Hurt)
            ExecWait(EVS_Enemy_ShockHit)
        CaseEq(EVENT_SHOCK_DEATH)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_KingWatinga_Hurt)
            ExecWait(EVS_Enemy_ShockHit)
            KillThread(LVar0)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_KingWatinga_Hurt)
            ExecWait(EVS_KingWatinga_Death)
            Return
        CaseOrEq(EVENT_ZERO_DAMAGE)
        CaseOrEq(EVENT_IMMUNE)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_KingWatinga_Idle)
            ExecWait(EVS_Enemy_NoDamageHit)
        EndCaseGroup
        CaseEq(EVENT_DEATH)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_KingWatinga_Hurt)
            ExecWait(EVS_Enemy_Hit)
            Wait(10)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_KingWatinga_Hurt)
            ExecWait(EVS_KingWatinga_Death)
            Return
        CaseEq(EVENT_BURN_DEATH)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_KingWatinga_Hurt)
            SetConst(LVar2, ANIM_KingWatinga_Hurt)
            ExecWait(EVS_Enemy_BurnHit)
            Wait(10)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_KingWatinga_Hurt)
            ExecWait(EVS_KingWatinga_Death)
            Return
        CaseEq(EVENT_END_FIRST_STRIKE)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_KingWatinga_Idle)
            ExecWait(EVS_Enemy_ReturnHome)
            Call(HPBarToHome, ACTOR_SELF)
        CaseEq(EVENT_RECOVER_STATUS)
            SetConst(LVar0, PRT_MAIN)
            SetConst(LVar1, ANIM_KingWatinga_Idle)
            ExecWait(EVS_Enemy_Recover)
        CaseDefault
    EndSwitch
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};

EvtScript EVS_TakeTurn_PhaseOne = {
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)

    //? Is Boss at half health  ?//
    Call(GetActorHP, ACTOR_SELF, LVarA)
    Mul(LVarA, 100)
    Div(LVarA, LVarB)
    IfLt(LVarA, 50)
        Call(SetActorVar, ACTOR_SELF, AVAR_Phase, AVAL_PhaseTwo)
        ExecWait(EVS_TransitionToPhaseTwo)
    EndIf

    //~ Phase 1 ~//
    Call(GetActorVar, ACTOR_SELF, AVAR_TurnCount, LVar1)
    Switch(LVar1)
    CaseEq(AVAL_TurnOne)
        ExecWait(EVS_Fly_Summons)
        Call(SetActorVar, ACTOR_SELF, AVAR_TurnCount, AVAL_TurnTwo)
    CaseEq(AVAL_TurnTwo)
        ExecWait(EVS_Fly_Summons)
        Call(SetActorVar, ACTOR_SELF, AVAR_TurnCount, AVAL_TurnThree)
    CaseEq(AVAL_TurnThree)
        ExecWait(EVS_Fly_Summons)
        Call(SetActorVar, ACTOR_SELF, AVAR_TurnCount, AVAL_TurnFour)
    CaseEq(AVAL_TurnFour)
        ExecWait(EVS_Assist_Attacks)
        Call(SetActorVar, ACTOR_SELF, AVAR_TurnCount, AVAL_TurnFive)
    CaseEq(AVAL_TurnFive)
        ExecWait(EVS_Move_Charge)
        Call(SetActorVar, ACTOR_SELF, AVAR_TurnCount, AVAL_TurnOne)
    EndSwitch

    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};

EvtScript EVS_TransitionToPhaseTwo = {
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)

    Call(BindTakeTurn, ACTOR_SELF, Ref(EVS_TakeTurn_PhaseTwo))
    Wait(30)

    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};

// EvtScript EVS_TakeTurn_PhaseTwo = {
//     Call(UseIdleAnimation, ACTOR_SELF, false)
//     Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)

//     //~ Phase 2 ~//
//     Call(GetActorVar, ACTOR_SELF, AVAR_TurnCount, LVar1)
//     Switch(LVar1)
//     CaseEq(AVAL_TurnOne)
//         ExecWait(EVS_Fly_Summons)
//         Call(SetActorVar, ACTOR_SELF, AVAR_TurnCount, AVAL_TurnTwo)
//     CaseEq(AVAL_TurnTwo)
//         ExecWait(EVS_Fly_Summons)
//         Call(SetActorVar, ACTOR_SELF, AVAR_TurnCount, AVAL_TurnThree)
//     CaseEq(AVAL_TurnThree)
//         ExecWait(EVS_Fly_Summons)
//         Call(SetActorVar, ACTOR_SELF, AVAR_TurnCount, AVAL_TurnFour)
//     CaseEq(AVAL_TurnFour)
//         ExecWait(EVS_Assist_Attacks)
//         Call(SetActorVar, ACTOR_SELF, AVAR_TurnCount, AVAL_TurnFive)
//     CaseEq(AVAL_TurnFive)
//         ExecWait(EVS_Move_Charge)
//         Call(SetActorVar, ACTOR_SELF, AVAR_TurnCount, AVAL_TurnOne)
//     EndSwitch

//     Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
//     Call(UseIdleAnimation, ACTOR_SELF, true)
//     Return
//     End
// };

EvtScript EVS_TakeTurn_PhaseTwo = {
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)

    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};

// Define macros
#define SummonFly 0
#define SummonSuperFly 1
#define SummonParentFly 2
#define SummonBombFly 3
#define SummonEventScript LVar3
#define SummonPriority LVar4
EvtScript EVS_Fly_Summons = {
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    Call(GetActorVar, ACTOR_SELF, AVAR_Phase, LVar0)
    Switch(LVar0)
    CaseEq(AVAL_PhaseOne)
        Call(RandInt, 100, SummonPriority) // SummonPriority will be a random number from 0 to 99
        Switch(SummonPriority)
        CaseLt(40) // Fly 40% (0-39)
            Set(SummonEventScript, SummonFly)
        CaseLt(70) // Super Fly 30% (40-69)
            Set(SummonEventScript, SummonSuperFly)
        CaseLt(90) // Parent Fly 20% (70-89)
            Set(SummonEventScript, SummonParentFly)
        CaseDefault // Bomb Fly 10% (90-99)
            Set(SummonEventScript, SummonBombFly)
        EndSwitch
        Switch(SummonEventScript)
        CaseEq(SummonFly)
            ExecWait(EVS_Summon_Fly)
        CaseEq(SummonSuperFly)
            ExecWait(EVS_Summon_SuperFly)
        CaseEq(SummonParentFly)
            ExecWait(EVS_Summon_ParentFly)
        CaseEq(SummonBombFly)
            ExecWait(EVS_Summon_BombFly)
        EndSwitch
    CaseEq(AVAL_PhaseTwo)
        Call(RandInt, 100, SummonPriority) // SummonPriority will be a random number from 0 to 99
        Switch(SummonPriority)
        CaseLt(10) // Fly 10% (0-9)
            Set(SummonEventScript, SummonFly)
        CaseLt(50) // Big Fly 40% (10-49)
            Set(SummonEventScript, SummonSuperFly)
        CaseLt(80) // Parent Fly 30% (50-79)
            Set(SummonEventScript, SummonParentFly)
        CaseDefault // Bomb Fly 20% (80-99)
            Set(SummonEventScript, SummonBombFly)
        EndSwitch
        Switch(SummonEventScript)
        CaseEq(SummonFly)
            ExecWait(EVS_Summon_Fly)
        CaseEq(SummonSuperFly)
            ExecWait(EVS_Summon_SuperFly)
        CaseEq(SummonParentFly)
            ExecWait(EVS_Summon_ParentFly)
        CaseEq(SummonBombFly)
            ExecWait(EVS_Summon_BombFly)
        EndSwitch
    EndSwitch
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};
// Undefine macros
#undef SummonFly
#undef SummonSuperFly
#undef SummonParentFly
#undef SummonBombFly
#undef SummonEventScript
#undef SummonPriority

// Define macros
#define SmallFlyShot 0
#define EnergyShot 1
#define FlyAway 2
#define AssistAttackScript LVar3
#define AssistPriority LVar4
EvtScript EVS_Assist_Attacks = {
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)
    Call(GetActorVar, ACTOR_SELF, AVAR_Phase, LVar0)
    Switch(LVar0)
    CaseEq(AVAL_PhaseOne)
        Call(RandInt, 100, AssistPriority) // AssistPriority will be a random number from 0 to 99
        Switch(AssistPriority)
        CaseLt(75) // Small Fly Shot 75%
            Set(AssistAttackScript, SmallFlyShot)
        CaseDefault // Energy Shot 25%
            Set(AssistAttackScript, EnergyShot)
        EndSwitch
        Switch(AssistAttackScript)
        CaseEq(SmallFlyShot)
            ExecWait(EVS_Attack_SmallFlyShot)
        CaseEq(EnergyShot)
            ExecWait(EVS_Attack_EnergyShot)
        EndSwitch
    CaseEq(AVAL_PhaseTwo)
        Call(RandInt, 100, AssistPriority) // AssistPriority will be a random number from 0 to 99
        Switch(AssistPriority)
        CaseLt(50) // Small Fly Shot 50% (0-49)
            Set(AssistAttackScript, SmallFlyShot)
        CaseLt(80) // Energy Shot 30% (50-79)
            Set(AssistAttackScript, EnergyShot)
        CaseDefault // Fly Away 20% (80-99)
            Set(AssistAttackScript, FlyAway)
        EndSwitch
        Switch(AssistAttackScript)
        CaseEq(SmallFlyShot)
            ExecWait(EVS_Attack_SmallFlyShot)
        CaseEq(EnergyShot)
            ExecWait(EVS_Attack_EnergyShot)
        CaseEq(FlyAway)
            ExecWait(EVS_Move_FlyAway)
        EndSwitch
    EndSwitch
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};
// Undefine macros
#undef SmallFlyShot
#undef EnergyShot
#undef FlyAway
#undef AssistAttackScript
#undef AssistPriority

EvtScript EVS_Summon_Fly = {
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)

    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};

EvtScript EVS_Summon_SuperFly = {
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)

    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};

EvtScript EVS_Summon_ParentFly = {
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)

    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};

EvtScript EVS_Summon_BombFly = {
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)

    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};

EvtScript EVS_Attack_SmallFlyShot = {
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)

    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};

EvtScript EVS_Attack_EnergyShot = {
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)

    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};

EvtScript EVS_Move_FlyAway = {
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_DISABLE)

    Call(EnableIdleScript, ACTOR_SELF, IDLE_SCRIPT_ENABLE)
    Call(UseIdleAnimation, ACTOR_SELF, true)
    Return
    End
};

EvtScript EVS_Move_Charge = {
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

EvtScript EVS_KingWatinga_Death = {
    Call(HideHealthBar, ACTOR_SELF)
    Call(UseIdleAnimation, ACTOR_SELF, false)
    Set(LVar2, 0)
    Call(SetAnimation, ACTOR_SELF, LVar0, LVar1)
    Wait(10)
    Loop(24)
        Call(SetActorYaw, ACTOR_SELF, LVar2)
        Add(LVar2, 30)
        Wait(1)
    EndLoop
    Call(SetActorYaw, ACTOR_SELF, 0)
    Call(GetActorPos, ACTOR_SELF, LVar2, LVar3, LVar4)
    PlayEffect(EFFECT_BIG_SMOKE_PUFF, LVar2, LVar3, LVar4, 0, 0, 0, 0, 0)
    Call(PlaySoundAtActor, ACTOR_SELF, SOUND_ACTOR_DEATH)
    Call(DropStarPoints, ACTOR_SELF)
    Set(LVar3, 0)
    Loop(12)
        Call(SetActorRotation, ACTOR_SELF, LVar3, 0, 0)
        Add(LVar3, 8)
        Wait(1)
    EndLoop
    Call(SetPartFlagBits, ACTOR_SELF, PRT_MAIN, ACTOR_PART_FLAG_INVISIBLE, true)
    Call(SetActorFlagBits, ACTOR_SELF, ACTOR_FLAG_NO_SHADOW, true)
    Wait(30)
    Call(RemoveActor, ACTOR_SELF)
    Return
    End
};

}; // namespace king_watinga

ActorBlueprint KingWatinga = {
    .flags = ACTOR_FLAG_FLYING,
    .maxHP = king_watinga::hp,
    .type = ACTOR_TYPE_KING_WATINGA,
    .level = ACTOR_LEVEL_KING_WATINGA,
    .partCount = ARRAY_COUNT(king_watinga::ActorParts),
    .partsData = king_watinga::ActorParts,
    .initScript = &king_watinga::EVS_Init,
    .statusTable = king_watinga::StatusTable,
    .escapeChance = 0,
    .airLiftChance = 0,
    .hurricaneChance = 0,
    .spookChance = 0,
    .upAndAwayChance = 0,
    .spinSmashReq = 4,
    .powerBounceChance = 80,
    .coinReward = 0,
    .size = { 80, 80 },
    .healthBarOffset = { 0, 0 },
    .statusIconOffset = { -6, 38 },
    .statusTextOffset = { 12, 75 },
};

}; // namespace battle::actor
