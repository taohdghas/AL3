#pragma once
#include "Model.h"
#include "Vector3.h"
#include "aabb.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "MapChipField.h"
class Player;
class Enemy {
public:
	Enemy();
	~Enemy();

	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position, MapChipField* mapChipField);

	void Update();

	void Draw();

	//衝突応答
	void OnCollision(const Player* player);
	void CheckMapCollision();
	// ワールド座標を取得
	Vector3 GetWorldPosition();

	//AABB取得関数
	AABB GetAABB();
	bool IsDead() const { return isDead_; }

private:
	// 敵の当たり判定サイズ
	static inline const float kWidth = 1.0f;
	static inline const float kHeight = 1.0f;

	// 補講の速さ
	static inline const float kWalkSpeed = 0.03f;
	//最初の角度[度]
	static inline const float kWalkMotionAngleStart = 5.0f;
	//最後の角度[度]
	static inline const float kWalkMotionAngleEnd = -5.0f;
	//アニメーションの周期となる時間[秒]
	static inline const float kWalklMotionTime = 1.0f;
	// 重力加速度
	static inline const float kGravity = 0.001f;
	// 最大落下速度
	static inline const float kMaxFallSpeed = 0.1f;

	//経過時間
	float walkTimer_ = 0.0f;
	//敵の削除フラグ
	bool isDead_ = false;
	// 速度
	Vector3 velocity_ = {};
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// ビュープロジェクション
	ViewProjection* viewProjection_ = nullptr;
	MapChipField* mapChipField_ = nullptr;
};
