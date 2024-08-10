#include "Skydome.h"

void Skydome::Initialize(Model* model) { 
	// NULLポインタチェック
	assert(model);
	model_ = model;
	worldTransform_.Initialize(); 
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
}

void Skydome::Update() {

}

void Skydome::Draw(const ViewProjection&viewProjection) {
	//3Dモデル描画
	model_->Draw(worldTransform_, viewProjection);
}