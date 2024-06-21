#include "Player.h"
#include "DebugText.h"
#include "MapChipField.h"
#include "mymath.h"

Player::Player() {}

Player::~Player() {}

void Player::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
	assert(model);
	model_ = model;
	viewProjection_ = viewProjection;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}
void Player::Update() {
	// 移動入力
	move();

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.moveAmount = velocity_;

	// マップ衝突チェック
	MapCollision(collisionMapInfo);
	// 判定結果を反映して移動させる
	ResultMove(collisionMapInfo);
	//天井に接触
	CeilingHit(collisionMapInfo);
	//接地状態
	OnGround(collisionMapInfo);
	//壁に接触
	WallHit(collisionMapInfo);
	//旋回制御
	TurnPlayer();

	// 行列計算
	worldTransform_.UpdateMatrix();
}
void Player::Draw() { model_->Draw(worldTransform_, *viewProjection_); }

void Player::move() {
	// 移動入力
	//  接地状態
	if (onGround_) {
		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
				acceleration.x += kAcceleration;
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				// 右移動中の左入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
				acceleration.x -= kAcceleration;
			}
			// 加速/減速
			velocity_ += acceleration;
			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			// 非入力時は移動減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ初速
			velocity_ = Add(velocity_, Vector3(0, kJumpAcceleration, 0));
		}
		// 空中
	} else {
		// 落下速度
		velocity_ = velocity_ + Vector3(0, -kGravityAcceleration, 0);
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);

		landing = false;

		// 地面との当たり判定
		// 下降中？
		if (velocity_.y < 0) {
			// Y座標が地面以下になったら着地
			if (worldTransform_.translation_.y <= 2.0f) {
				landing = true;
			}
		}
	}
	// 移動
	worldTransform_.translation_ = worldTransform_.translation_ + velocity_;
	if (onGround_) {
		// 着地判定
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			onGround_ = false;
		}
	} else {
		// 着地
		if (landing) {
			// めり込み排斥
			worldTransform_.translation_.y = 1.0f;
			// 摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAttenuationLanding);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
			// 接地状態に移行
			onGround_ = true;
		}
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {kWidth / 2.0f,  -kHeight / 2.0f, 0}, //  kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kLeftBottom
	    {kWidth / 2.0f,  kHeight / 2.0f,  0}, //  kRightTop
	    {-kWidth / 2.0f, kHeight / 2.0f,  0}  //  kLeftTop
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}

void Player::MapCollision(CollisionMapInfo& info) {
	MapTopCollision(info);
	MapBottomCollision(info);
	MapRightCollision(info);
	MapLeftCollision(info);
}

// マップ衝突判定上
void Player::MapTopCollision(CollisionMapInfo& info) {
	// 上昇あり？
	if (info.moveAmount.y <= 0) {
		return;
	}
	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionNew;
	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.moveAmount, static_cast<Corner>(i));
	}
	MapChipType mapChipType;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	IndexSet indexSet;
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapchipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右上点の判定
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapchipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapchipField_->GetMapChipIndexSetByPosition({worldTransform_.translation_.x + 0, worldTransform_.translation_.y + (kHeight / 2.0f), worldTransform_.translation_.z + 0});
		// めり込み先ブロックの範囲矩形
		Rect rect = mapchipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.moveAmount.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		// 天井に当たったことを記録する
		info.ceilCollision = true;
	}
}

// マップ衝突判定下
void Player::MapBottomCollision(CollisionMapInfo& info) {
	// 下降あり？
	if (info.moveAmount.y >= 0) {
		return;
	}
	std::array<Vector3, kNumCorner> positionNew;
	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.moveAmount, static_cast<Corner>(i));
	}
	MapChipType mapChipType;
	// 真下の当たり判定を行う
	bool hit = false;
	// 左下点の判定
	IndexSet indexSet;
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
	mapChipType = mapchipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
	mapChipType = mapchipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒット?
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ - Vector3{kHeight / 2.0f, 0.0f, 0.0f});
		// めり込み先ブロックの範囲矩形
		Rect rect = mapchipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.moveAmount.y = std::min(0.0f, rect.top - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		// 地面に当たったことを記録する
		info.onGround = true;
	}
}
// マップ衝突判定右
void Player::MapRightCollision(CollisionMapInfo& info) {
	// 右移動あり?
	if (info.moveAmount.x <= 0) {
		return;
	}
	std::array<Vector3, kNumCorner> positionNew;
	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.moveAmount, static_cast<Corner>(i));
	}
	MapChipType mapChipType;
	// 右の当たり判定を行う
	bool hit = false;
	// 右上の当たり判定
	IndexSet indexSet;
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapchipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下の当たり判定
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
	mapChipType = mapchipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3{kWidth / 2.0f, 0.0f, 0.0f});
		Rect rect = mapchipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.moveAmount.x = std::min(rect.left - worldTransform_.translation_.x + (kWidth / 2.0f + kBlank), 0.0f);
		// 壁に当たったことを判定結果に記録する
		info.wallhit = true;
	}
}
// マップ衝突判定左
void Player::MapLeftCollision(CollisionMapInfo& info) {
	// 左移動あり?
	if (info.moveAmount.x >= 0) {
		return;
	}
	std::array<Vector3, kNumCorner> positionNew;
	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.moveAmount, static_cast<Corner>(i));
	}
	MapChipType mapChipType;
	// 左の当たり判定おw行う
	bool hit = false;
	// 左上の当たり判定
	IndexSet indexSet;
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapchipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 左下の当たり判定
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
	mapChipType = mapchipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒット?
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ - Vector3{kWidth / 2.0f, 0.0f, 0.0f});
		Rect rect = mapchipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.moveAmount.x = std::max(rect.right - worldTransform_.translation_.x + (-kWidth / 2.0f - kBlank), 0.0f);
		// 壁に当たったことを記録する
		info.wallhit = true;
	}
}

// 判定結果を反映して移動させる
void Player::ResultMove(const CollisionMapInfo& info) {
	// 移動
	worldTransform_.translation_ += info.moveAmount;
}

// 天井に接触している場合の処理
void Player::CeilingHit(const CollisionMapInfo& info) {
	// 天井に当たった？
	if (info.ceilCollision) {
		// DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;
	}
}

// 接地状態の切り替え処理
void Player::OnGround(const CollisionMapInfo& info) {
	// 自キャラが接地状態？
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			std::array<Vector3, kNumCorner> positionNew;
			for (uint32_t i = 0; i < positionNew.size(); ++i) {
				positionNew[i] = CornerPosition(worldTransform_.translation_ + info.moveAmount, static_cast<Corner>(i));
			}
			MapChipType mapChipType;
			// 真下の当たり判定を行う
			bool hit = false;
			// 左下点の判定
			IndexSet indexSet;
			indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom] + Vector3(0.0f, -kBlank, 0.0f));
			mapChipType = mapchipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			// 右下点の判定
			indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom] + Vector3(0.0f, -kBlank, 0.0f));
			mapChipType = mapchipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			// 落下開始
			if (!hit) {
				// 空中状態に切り替える
				onGround_ = false;
			}
		}
	} else {
		// 着地フラグ
		if (info.onGround) {
			// 着地状態に切り替える(落下を止める)
			onGround_ = true;
			// 着地時にX速度を減衰
			velocity_.x *= (1.0f - kAttenuationLanding);
			// Y速度をゼロにする
			velocity_.y = 0.0f;
		}
	}
}

// 壁に衝突してる場合の処理
void Player::WallHit(const CollisionMapInfo& info) {
	// 壁接触による減速
	if (info.wallhit) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

// 旋回制御
void Player::TurnPlayer() {
	// 旋回制御
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;
		// 左右の目キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		float easing = 1 - turnTimer_ / kTimeTurn;
		float nowRotationY = std::lerp(turnFirstRotationY_, destinationRotationY, easing);
		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = nowRotationY;
	}
}