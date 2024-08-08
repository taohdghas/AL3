#include "EnemyStateApproach.h"

EnemyStateApproach::EnemyStateApproach(Enemy* enemy) : BaseEnemyState("State Approach", enemy) {}

void EnemyStateApproach::Update() {
	//移動
	enemy_->SetWorld(enemy_->GetWorld() + enemy_->GetVelocity());
	// 既定の位置に到達したら離脱
	if (enemy_->GetWorld().z < 0.0f) {
		enemy_->ChangeState(std::make_unique<EnemyStateLeave>(enemy_));
	}
}