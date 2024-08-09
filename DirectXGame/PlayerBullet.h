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

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	bool IsDead() const { return isDead_; }

	Vector3 GetWorldPosition();

	private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	//速度
	Vector3 velocity_;
	//寿命
	static const int32_t kLifeTime = 60 * 5;
	//デスタイマー
	int32_t deathTimer_ = kLifeTime;
	//デスフラグ
	bool isDead_ = false;
};
