#pragma once
#include "Sprite.h"
/// <summary>
/// フェード
/// </summary>
class Fade {
public:
	Fade();
	~Fade();

	void Initialize();

	void Update();

	void Draw(ID3D12GraphicsCommandList* commandList);

private:
	//画面横・縦幅
	static inline const float kWindowWidth = 1280.0f;
	static inline const float kWindowHeight = 720.0f;

	Sprite* sprite_ = nullptr;
};
