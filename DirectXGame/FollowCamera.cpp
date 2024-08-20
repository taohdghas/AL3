#include "FollowCamera.h"
#include "mymath.h"

void FollowCamera::Initialize() { 
	viewProjection_.Initialize(); 
}

void FollowCamera::Update() {
	//追従対象がいれば
	if (target_) {
	//追従対象からカメラまでのオフセット
		Vector3 offset = {0.0f, 2.0f, -10.0f};
		//座標をコピーしてオフセット分ずらす
		viewProjection_.translation_ = Add(target_->translation_ , offset);
	}
	viewProjection_.UpdateMatrix();

}
