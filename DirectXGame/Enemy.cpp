#include "Enemy.h"
#include "mymath.h"
#include <numbers>

Enemy::Enemy() {}

Enemy::~Enemy() {}

void Enemy::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position, MapChipField* mapChipField) { 
	model_ = model;
	viewProjection_ = viewProjection;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / -2.0f;
	velocity_ = {-kWalkSpeed, 0, 0};
	walkTimer_ = 0.0f;
	mapChipField_ = mapChipField;
}

void Enemy::Update() {
	// 移動
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	CheckMapCollision();
	//高さが一定以下に鳴ったら削除フラグオン
	if (worldTransform_.translation_.y < -10.0f) {
		isDead_ = true;
	}
	// タイマーを加算
	walkTimer_ += 1.0f / 60.0f;
    //回転アニメーション
	float param = std::sin(2.0f * std::numbers::pi_v<float> * walkTimer_ / kWalklMotionTime);
	float radian = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.z = radian * (std::numbers::pi_v<float> / 180.0f);
	// 行列計算                 
	worldTransform_.UpdateMatrix();
}
void Enemy::Draw() { model_->Draw(worldTransform_, *viewProjection_); }

//衝突応答
void Enemy::OnCollision(const Player* player) { 
	(void)player;
}
void Enemy::CheckMapCollision() {
	// 現在の位置を取得
	Vector3 currentPosition = GetWorldPosition();

	// 現在位置からマップチップのインデックスを取得
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(currentPosition);

	// マップチップの範囲内か確認
	if (indexSet.xIndex < 0 || indexSet.xIndex >= mapChipField_->GetNumBlockHorizontal() || indexSet.yIndex < 0 || indexSet.yIndex >= mapChipField_->GetNumBlockVirtical()) {
		return;
	}

	// 現在のマップチップのタイプを取得
	MapChipType chipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	// 衝突する場合の応答
	if (chipType == MapChipType::kBlock) {
		// 衝突応答の処理（例えば、逆方向に移動するなど）
		velocity_.x = -velocity_.x;
		worldTransform_.translation_.x += velocity_.x; // 衝突後に位置を少し戻す
	}

	// 敵の下の位置を確認
	Vector3 belowPosition = currentPosition;
	belowPosition.y -= kHeight / 2.0f + 0.1f; // 敵の底辺より少し下をチェック
	MapChipField::IndexSet belowIndexSet = mapChipField_->GetMapChipIndexSetByPosition(belowPosition);

	// マップチップの範囲内か確認
	if (belowIndexSet.xIndex < 0 || belowIndexSet.xIndex >= mapChipField_->GetNumBlockHorizontal() || belowIndexSet.yIndex < 0 || belowIndexSet.yIndex >= mapChipField_->GetNumBlockVirtical()) {
		return;
	}

	// 下のマップチップのタイプを取得
	MapChipType belowChipType = mapChipField_->GetMapChipTypeByIndex(belowIndexSet.xIndex, belowIndexSet.yIndex);

	// 下が空洞であれば落下
	if (belowChipType == MapChipType::kBlank) {
		velocity_.y -= kGravity; // 重力を適用
		if (velocity_.y < -kMaxFallSpeed) {
			velocity_.y = -kMaxFallSpeed; // 最大落下速度を制限
		}
		worldTransform_.translation_.y += velocity_.y; // Y方向の速度を位置に適用
	} else {
		// 地面にいる場合は落下速度をリセット
		velocity_.y = 0;
	}
}

//ワールド座標を取得
Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

//AABB取得関数
AABB Enemy::GetAABB() { 
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
 }