#include "Enemy.h"

void Enemy::Initialize(Model* model, uint32_t textureHandle,const Vector3& velocity) {
	// NULLポインタチェック
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 1.0f, 10.0f};
	velocity_ = velocity;
}

void Enemy::Update() {
	switch (phase_) { 
	case Phase::Approach:
	default:
		Approach();
		break;
	case Phase::Leave:
		Leave();
		break;
	}
	//移動
	worldTransform_.translation_ = Add(worldTransform_.translation_,velocity_ );

	worldTransform_.UpdateMatrix(); 
}

void Enemy::Draw(const ViewProjection& viewProjection) { 
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

//接近フェーズ
void Enemy::Approach() {
	// 移動(ベクトルを加算)
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);
	// 既定の位置に到達したら離脱
	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}
}

//離脱フェーズ
void Enemy::Leave() {
	velocity_ = {-0.1f, 0.1f, 0.0f};
	// 移動(ベクトルを加算)
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);
}