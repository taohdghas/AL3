#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "mymath.h"
#include "imgui.h"
/// <summary>
/// レールカメラ
/// </summary>
class RailCamera {
public:
	void Initialize(const Vector3 position, const Vector3 rotation);

	void Update();

	//ビュープロジェクション取得関数
	const ViewProjection& GetViewprojection() { return viewProjection_; }
	//ワールド行列取得関数
	const WorldTransform& GetWorldtransform() { return worldTransform_; }

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection viewProjection_;
	// 速度
	Vector3 velocity_ = {0.0f, 0.0f, 0.01f};
	//回転
	Vector3 rotation_ = {0.0f, 0.0f, 0.0f};
};
