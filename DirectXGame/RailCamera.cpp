#include "RailCamera.h"

void RailCamera::Initialize(const Vector3 position, const Vector3 rotation) {
	// ワールドトランスフォームの初期設定
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = rotation;
	// ビュープロジェクションの初期化
	//viewProjection_.farZ = 1000;
	viewProjection_.Initialize();
	worldTransform_.Initialize();
}

void RailCamera::Update() { 
	//速度
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);
	//回転
	worldTransform_.rotation_ = Add(worldTransform_.rotation_, rotation_);

	worldTransform_.UpdateMatrix();

	//カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = Inverse(worldTransform_.matWorld_);
	/*
	//カメラの座標を画面表示する処理
	ImGui::Begin("Camera");
	ImGui::DragFloat3("Translation", &worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("Rotation", &worldTransform_.rotation_.x, 0.01f);
	ImGui::End();
	*/
}
