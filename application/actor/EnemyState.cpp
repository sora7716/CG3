#include "EnemyState.h"
#include "Enemy.h"

//初期化
void IEnemyState::Initialize(Enemy* enemy) {
	enemy_ = enemy;
}

//追従
void EnemyStateChase::Exce() {
	enemy_->Chase();
}

//攻撃
void EnemeyStateAttack::Exce() {
	enemy_->Attack();
}

