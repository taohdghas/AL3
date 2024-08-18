#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "assert.h"
/// <summary>
/// 天球
/// </summary>
class Skydome {
public:
	void Initialize(Model* model,ViewProjection*viewProjection);

	void Update();

	void Draw(const ViewProjection& viewProjection);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	//ビュープロジェクション
	ViewProjection* viewProjection_ = nullptr;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
};