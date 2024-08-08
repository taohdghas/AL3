#include "Enemy.h"

//メンバ関数ポインタテーブル
void (Enemy::*Enemy::spPhase[])() = {&Enemy::Approach, &Enemy::Leave};

void Enemy::Initialize(Model* model, uint32_t textureHandle,const Vector3& velocity) {
	// NULLポインタチェック
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 1.0f, 10.0f};
	velocity_ = velocity;
	//初期状態をセット
	ChangeState(std::make_unique<EnemyStateApproach>(this));
}

void Enemy::Update() {
    
	//現在フェーズの関数を実行
	(this->*spPhase[static_cast<size_t>(phase_)])();
	state_->Update();

	//移動
	worldTransform_.translation_ = Add(worldTransform_.translation_,velocity_ );

	worldTransform_.UpdateMatrix(); 
}

void Enemy::Draw(const ViewProjection& viewProjection) { 
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

//接近フェーズ


void Enemy::ChangeState(std::unique_ptr<BaseEnemyState> state) {
	//引数で受け取った状態を次の状態としてセットする
	state_ = std::move(state);
}