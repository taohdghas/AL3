#include "Player.h"
#include <cassert>

void Player::Initialize(Model* model, ViewProjection* viewProjection) {
	assert(model);
	model_ = model;
	viewProjection_ = viewProjection;
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 2.0f, 0.0f};
}
void Player::Update() { 
	worldTransform_.UpdateMatrix(); 
}
void Player::Draw() {
	model_->Draw(worldTransform_, *viewProjection_);
}