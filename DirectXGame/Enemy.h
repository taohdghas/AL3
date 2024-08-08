#include "Model.h"
#include "WorldTransform.h"
#include "assert.h"
#include "TextureManager.h"
#include "mymath.h"
#include "BaseEnemyState.h"
#include "EnemyStateApproach.h"
#pragma once
/// <summary>
/// 敵
/// </summary>

//行動フェーズ
enum class Phase {
	Approach,//接近する
	Leave,//離脱する
};
class Enemy {
public:
	void Initialize(Model* model, uint32_t textureHandle, const Vector3& velocity);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	//ワールドのゲッター
	const Vector3& GetWorld() const { return worldTransform_.translation_; }
	//ワールドのセッター
	void SetWorld(const Vector3& translation) { worldTransform_.translation_ = translation; }

	//速度のゲッター
	const Vector3& GetVelocity() const { return velocity_; }
    //速度のセッター
	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }

private:
	//メンバ関数ポインタのテーブル
	static void (Enemy::*spPhase[])();
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	//移動
	Vector3 velocity_ = {};
    //フェーズ
	Phase phase_ = Phase::Approach;
	//状態
	std::unique_ptr<BaseEnemyState> state_;
};
