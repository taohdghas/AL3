#include "Skydome.h"

void Skydome::Initialize(Model* model, ViewProjection* viewProjection) {
	// NULLポインタチェック
	assert(model);
	model_ = model;
	viewProjection_ = viewProjection;
	worldTransform_.Initialize();
}

void Skydome::Update() { worldTransform_.UpdateMatrix(); }

void Skydome::Draw(const ViewProjection& viewProjection) {
	// 3Dモデル描画
	model_->Draw(worldTransform_, viewProjection);
}