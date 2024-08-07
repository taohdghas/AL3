#include "Model.h"
#include "WorldTransform.h"
#include "assert.h"
#include "TextureManager.h"
#include "mymath.h"
#pragma once
/// <summary>
/// 敵
/// </summary>
class Enemy {
public:
	void Initialize(Model* model, uint32_t textureHandle, const Vector3& velocity);

	void Update();

	void Draw(const ViewProjection& viewProjection);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	//移動
	Vector3 velocity_ = {-0.1f, -0.1f, -0.1f};
};
