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
	void Initialize(Model*model,uint32_t textureHandle);

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

	//ワールド座標を取得
	Vector3 GetWorldPosition();

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
