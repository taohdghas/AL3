#include "TitleScene.h"
#include <numbers>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() { 
	delete titlemodel_;
	delete fade_;
}

void TitleScene::Initialize() { 
	dxCommon_ = DirectXCommon::GetInstance();
	titlemodel_ = Model::CreateFromOBJ("title", true);
	titleWorldTransform_.Initialize();
	viewProjection_.Initialize();
	Timer_ = 0.0f;
	fade_ = new Fade();
	fade_->Initialize();
}

void TitleScene::Update() {
	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		finished_ = true;
	}
	Timer_ += 1.0f / 60.0f;
	float param = std::sin(2.0f * std::numbers::pi_v<float> * Timer_ / kWalklMotionTime);
	float radian = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	titleWorldTransform_.rotation_.y = radian * (std::numbers::pi_v<float> / 180.0f);
	// 行列計算
	titleWorldTransform_.UpdateMatrix();
	//フェード更新
	fade_->Update();
}

void TitleScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw(commandList);

	titlemodel_->Draw(titleWorldTransform_, viewProjection_);
	//フェード
	fade_->Draw(commandList);

	Model::PostDraw();
}
