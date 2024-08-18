#include "Ground.h"

void Ground::Initialize(Model* model, ViewProjection* viewProjection) {
	// NULLポインタチェック
	assert(model);
	model_ = model;
	viewProjection_ = viewProjection;
	worldTransform_.Initialize();
}

void Ground::Update() { worldTransform_.UpdateMatrix(); }

void Ground::Draw(const ViewProjection& viewProjection) { model_->Draw(worldTransform_, viewProjection); }