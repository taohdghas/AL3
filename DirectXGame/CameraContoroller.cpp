#define NOMINMAX 
#include "Player.h"
#include "CameraContoroller.h"
#include "mymath.h"

void CameraContoroller::Initialize() { viewProjection_.Initialize(); }

void CameraContoroller::Update() {
	const Vector3& targetVelocity = target_->GetVelocity();
	//追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	//追従対象とオフセットと追従対象の速度からカメラの目標座標を計算
	targetPos = targetWorldTransform.translation_ + targetOffset_ + targetVelocity * kVelocityBias;
	//座標補間によりゆったり追従
	viewProjection_.translation_ = Lerp(viewProjection_.translation_, targetPos, kInterpolationRate);
	//追従対象が画面外に出ないように補正
	viewProjection_.translation_.x = std::max(viewProjection_.translation_.x,targetPos.x + margin.left);
	viewProjection_.translation_.x = std::min(viewProjection_.translation_.x, targetPos.x + margin.right);
	viewProjection_.translation_.y = std::max(viewProjection_.translation_.y, targetPos.y + margin.bottom);
	viewProjection_.translation_.y = std::min(viewProjection_.translation_.y, targetPos.y + margin.top);
	//移動範囲制限
	viewProjection_.translation_.x = std::min(viewProjection_.translation_.x, movableArea_.left);
	viewProjection_.translation_.x = std::max(viewProjection_.translation_.x, movableArea_.right);
	viewProjection_.translation_.y = std::min(viewProjection_.translation_.y, movableArea_.bottom);
	viewProjection_.translation_.y = std::max(viewProjection_.translation_.y, movableArea_.top);

	viewProjection_.UpdateMatrix();
}

void CameraContoroller::Reset() {
	//追従対象のワールドトランスフォーム
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	//追従対象とオフセットからカメラの座標を計算
	viewProjection_.translation_ = targetWorldTransform.translation_ + targetOffset_;
}