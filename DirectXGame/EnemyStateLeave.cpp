#include "EnemyStateLeave.h"

EnemyStateLeave::EnemyStateLeave(Enemy* enemy) : BaseEnemyState("State Leave", enemy) {}

void EnemyStateLeave::Update() {
	enemy_->SetVelocity({-0.1f, 0.1f, 0.0f});
    //移動
	enemy_->SetWorld(enemy_->GetWorld() + enemy_->GetVelocity());

}
