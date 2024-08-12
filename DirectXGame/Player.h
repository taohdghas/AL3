#include "Model.h"
#include "WorldTransform.h"
#include "assert.h"
#include "Input.h"
#include "mymath.h"
#include "ImGuiManager.h"
#include "PlayerBullet.h"
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
	void Update();

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
	//キーボード入力
	Input* input_ = nullptr;
	//弾
	std::list<PlayerBullet*> bullets_;
};
