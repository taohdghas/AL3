#pragma once
#define NOMINMAX
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include <numbers>
#include "Input.h"
#include <algorithm>
#include <cassert>
/// <summary>
/// 自キャラ
/// </summary>
//左右
enum class LRDirection {
	kRight,
	kLeft,
};
class Player {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, ViewProjection* viewProjection,const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	static inline const float kAcceleration = 0.1f;
	static inline const float kAttenuation = 0.5f;
	static inline const float kLimitRunSpeed = 1.0f;
	//旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;
	//重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.1f;
	//最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 1.0f;
	//ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 1.0f;
	//旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	//旋回タイマー
	float turnTimer_ = 0.0f;
	//接地状態フラグ
	bool onGround_ = true;
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// ビュープロジェクション
	ViewProjection* viewProjection_ = nullptr;
	Vector3 velocity_ = {};
	LRDirection lrDirection_ = LRDirection::kRight;
};