#include "Model.h"
#include "WorldTransform.h"
#include "assert.h"
#include "TextureManager.h"
#pragma once
/// <summary>
/// 自キャラの弾
/// </summary>
class PlayerBullet {
public:
	void Initialize(Model* model, const Vector3& position);

	void Update();

	void Draw(const ViewProjection&viewProjection);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
};
