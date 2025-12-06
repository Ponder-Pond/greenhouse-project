#include "area.hpp"
#include "stage/gnt_bt00.hpp"

#include "battle/common/newactor/gnat_attack_actors.hpp"

// #include "battle/common/newactor/small_fly.inc.cpp"
#include "battle/common/newactor/fly.inc.cpp"
#include "battle/common/newactor/super_fly.inc.cpp"
#include "battle/common/newactor/parent_fly.inc.cpp"
#include "battle/common/newactor/bomb_fly.inc.cpp"
#include "battle/common/newactor/king_watinga.inc.cpp"

using namespace battle::actor;

namespace battle::area::kmr_part_1 {

Vec3i KingWatingaPos = { 70, 30, 0 };

Formation Formation_GnatAttack = {
    ACTOR_BY_POS(KingWatinga, KingWatingaPos, 10),
};

Formation Formation_Fly = {
    ACTOR_BY_IDX(Fly, BTL_POS_AIR_B, 10),
};

Formation Formation_SuperFly = {
    ACTOR_BY_IDX(SuperFly, BTL_POS_AIR_B, 10),
};

Formation Formation_ParentFly = {
    ACTOR_BY_IDX(ParentFly, BTL_POS_AIR_B, 10),
};

Formation Formation_BombFly = {
    ACTOR_BY_IDX(BombFly, BTL_POS_AIR_B, 10),
};

BattleList Battles = {
    BATTLE(Formation_GnatAttack, GnatAttack, "Gnat Attack"), // Battle 0
    BATTLE(Formation_Fly, GnatAttack, "Fly"), // Battle 1
    BATTLE(Formation_SuperFly, GnatAttack, "Super Fly"), // Battle 2
    BATTLE(Formation_ParentFly, GnatAttack, "Parent Fly"), // Battle 3
    BATTLE(Formation_BombFly, GnatAttack, "Bomb Fly"), // Battle 4
    {},
};

StageList Stages = {
    STAGE("Gnat Attack", GnatAttack), // Stage 0
    {},
};

}; // namespace battle::area::kmr_part_1
