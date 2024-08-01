#include "Fade.h"

Fade::Fade() {}

Fade::~Fade() { delete sprite_; }

void Fade::Initialize() { 
	sprite_ = new Sprite();
	sprite_->SetSize(Vector2(kWindowWidth, kWindowHeight));
	sprite_->SetColor(Vector4(0, 0, 0, 1));
}

void Fade::Update() {

}

void Fade::Draw(ID3D12GraphicsCommandList* commandList) {
	Sprite::PreDraw(commandList);
	sprite_->Draw();
	Sprite::PostDraw();
}
