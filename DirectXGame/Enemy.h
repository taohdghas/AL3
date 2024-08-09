#include "Model.h"
#include "WorldTransform.h"
#include "assert.h"
#include "TextureManager.h"
#include "mymath.h"
#include "EnemyBullet.h"
#include <list>
#pragma once
/// <summary>
/// 敵
/// </summary>

//自機クラスの前方宣言
class Player;

//行動フェーズ
enum class Phase {
	Approach,//接近する
	Leave,//離脱する
};
class Enemy {
public:
	Enemy();
	~Enemy();

	void Initialize(Model* model, uint32_t textureHandle, const Vector3& velocity);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	//接近フェーズ
	void Approach();

	//離脱フェーズ
	void Leave();

	//弾発射
	void Fire();

	//接近フェーズ初期化
	void ApproachReset();

	void SetPlayer(Player* player) { player_ = player; }

	//ワールド座標を取得
	Vector3 GetWorldPosition();

	// 発射間隔
	static const int kFireInterval = 60;

private:
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
	// 弾
	std::list<EnemyBullet*> bullets_;
	//発射タイマー
	int32_t fireTimer = 0;
	//自キャラ
	Player* player_ = nullptr;
};
