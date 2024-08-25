#pragma once
#include "Vector3.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

struct Rect {
	float left = 0.0f;   // 左端
	float right = 1.0f;  // 右端
	float bottom = 0.0f; // 下端
	float top = 1.0f;    // 上端
};
class Player;
class CameraContoroller {
public:
	void Initialize();

	void Update();

	void SetTarget(Player* target) { target_ = target; }

	void Reset();

	void SetMovableArea(Rect area) { movableArea_ = area; }

	const ViewProjection& GetViewProjection() const { return viewProjection_; }

	private:
	// ビュープロジェクション
	ViewProjection viewProjection_;
	Player* target_ = nullptr;
	// 追従対象とカメラの座標の差
	Vector3 targetOffset_ = {5.0f, 6.0f, -30.0f};
	//カメラ移動範囲
	Rect movableArea_ = {};
	//カメラの目標座標
	Vector3 targetPos;
	//座標補間割合
	static inline const float kInterpolationRate = 0.8f;
	//速度掛け率
	static inline const float kVelocityBias = 5.0f;
	//追従対象の各方向へのカメラ移動範囲
	static inline const Rect margin = {-5.0, 5.0, -5.0, 5.0};
};
