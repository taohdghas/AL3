#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include "assert.h"
/// <summary>
/// 天球
/// </summary>
class Skydome {
public:
	void Initialize(Model* model,uint32_t textureHandle);

	void Update();

	void Draw(const ViewProjection&viewProjection);

private:
	//ワールド変換データ
	WorldTransform worldTransform_;
	//モデル
	Model* model_ = nullptr;
    //テクスチャハンドル
	uint32_t textureHandle_ = 0u;
};
