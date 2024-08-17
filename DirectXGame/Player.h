#include "Model.h"
#include "WorldTransform.h"
#include "assert.h"
#include "Input.h"
#include "mymath.h"
#include "ImGuiManager.h"
#include "PlayerBullet.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include <list>
#pragma once
/// <summary>
/// 自キャラ
/// </summary>
class Player {
public:

	Player();

	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model*model,uint32_t textureHandle,Vector3 &position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update( const ViewProjection&viewProjection);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection&viewProjection);

	//旋回
	void Rotate();

	//攻撃
	void Attack();

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	void SetParent(const WorldTransform* parent);

	//UI描画関数
	void DrawUI();

	//ワールド座標を取得
	Vector3 GetWorldPosition();

	//弾リストを取得
	const std::list<PlayerBullet*>& Getbullets() const { return bullets_; }

private:
	//ワールド変換データ
	WorldTransform worldTransform_;
	//モデル
	Model* model_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	//レティクルテクスチャハンドル
	//uint32_t ReticletextureHandle_ = 0u;
	//キーボード入力
	Input* input_ = nullptr;
	//弾
	std::list<PlayerBullet*> bullets_;
	//3Dレティクル用ワールドトランスフォーム
	WorldTransform worldTransform3DReticle_;
	//2Dレティクル用スプライト
	Sprite* sprite2dReticle_ = nullptr;
};
