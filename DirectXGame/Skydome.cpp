#include "Skydome.h"

void Skydome::Initialize(Model* model,uint32_t textureHandle) { 
	// NULLポインタチェック
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize(); 
}

void Skydome::Update() { worldTransform_.UpdateMatrix(); }

void Skydome::Draw(const ViewProjection&viewProjection) {
	//3Dモデル描画
	model_->Draw(worldTransform_, viewProjection);
}