#include "Player.h"
#include "mymath.h"
#include <cassert>

void Player::Initialize(Model*model,uint32_t textureHandle,ViewProjection*viewProjection) {

//NULLポインタチェック
	assert(model);

	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	viewProjection_ = viewProjection;
}

void Player::Update() {
	//移動
	Move();
	//行列更新
	worldTransform_.UpdateMatrix();
}

void Player::Draw() { 
	//3Dモデルを描画 
	model_->Draw(worldTransform_,*viewProjection_,textureHandle_);
}

//移動
void Player::Move() { 
	XINPUT_STATE joyState; 
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
	//速さ
		const float speed = 0.3f;
		//移動量
		Vector3 move = {
			(float)joyState.Gamepad.sThumbLX / SHRT_MAX,0.0f, 
			(float)joyState.Gamepad.sThumbLY / SHRT_MAX};
		//移動量に速さを反映
		move = Normalize(move) * speed;

		//移動
		worldTransform_.translation_.x += move.x;
		worldTransform_.translation_.z += move.z;
	}

}