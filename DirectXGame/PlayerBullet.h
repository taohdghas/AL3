#include "Model.h"
#include "WorldTransform.h"
#include "assert.h"
#include "TextureManager.h"
#include "mymath.h"
#pragma once
/// <summary>
/// 自キャラの弾
/// </summary>
class PlayerBullet {
public:
	void Initialize(Model* model, const Vector3& position,const Vector3&velocity);

	void Update();

	void Draw(const ViewProjection&viewProjection);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	//速度
	Vector3 velocity_;
};
