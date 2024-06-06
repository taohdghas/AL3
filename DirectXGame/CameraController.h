#pragma once
#include "ViewProjection.h"
/// <summary>
/// カメラコントローラ
/// </summary>

struct Rect {
	float left = 0.0f;   // 左端
	float right = 1.0f;  // 右端
	float bottom = 0.0f; // 下端
	float top = 1.0f;    // 上端
};
//前方宣言
class Player;
class CameraController {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ViewProjection*viewProjection);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	void SetTarget(Player* target) { target_ = target; }
	void Reset();
	void SetMovableArea() private:
	//ビュープロジェクション
	ViewProjection* viewProjection_;
	Player* target_ = nullptr;
	//追従対象とカメラの座標の差(オフセット)
	Vector3 targetOffset_ = {0, 0, -15.0f};
	//カメラ移動範囲
	Rect movableArea_ = {0, 100, 0, 100};
};

