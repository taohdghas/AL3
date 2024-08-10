#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include "assert.h"
/// <summary>
/// 天球
/// </summary>
class Skydome {
public:
	void Initialize(Model* model,ViewProjection*viewProjection);

	void Update();

	void Draw(const ViewProjection&viewProjection);

private:
	//ワールド変換データ
	WorldTransform worldTransform_;
	//モデル
	Model* model_ = nullptr;
	//ビュープロジェクション
	ViewProjection* viewProjection_;
};
