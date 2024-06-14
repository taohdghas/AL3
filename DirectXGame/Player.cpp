#include "Player.h"
#include "MapChipField.h"
#include "mymath.h"
#include "DebugText.h"

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
	//移動入力
	move();

	//衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	//移動量に速度の値をコピー
	collisionMapInfo.moveAmount = velocity_;

	//マップ衝突チェック
	MapCollision(collisionMapInfo);

	ResultMove(collisionMapInfo);

	CeilingContact(collisionMapInfo);

	// 移動
	worldTransform_.translation_ = worldTransform_.translation_ + velocity_;
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
			velocity_ = Add(velocity_, acceleration);
			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
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
		velocity_ = Add(velocity_, Vector3(0, -kGravityAcceleration, 0));
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
	// 着地フラグ
	bool landing = false;
	// 地面との当たり判定
	// 下降中？
	if (velocity_.y < 0) {
		// Y座標が地面以下になったら着地
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}
	// 着地判定
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			onGround_ = false;
		}
	} else {
		// 着地
		if (landing) {
			// めり込み排斥
			worldTransform_.translation_.y = 2.0f;
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
	    {kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kLeftBottom
	    {kWidth / 2.0f, kHeight / 2.0f, 0}, //  kRightTop
	    {-kWidth / 2.0f, kHeight / 2.0f, 0}  //  kLeftTop
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}

//マップ衝突判定上
void Player::MapTopCollision(CollisionMapInfo& info) {
	//上昇あり？
	if (info.moveAmount.y <= 0) {
		return;
	}
	//移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionNew;
	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.moveAmount, static_cast<Corner>(i));
	}
	MapChipType mapChipType;
	//真上の当たり判定を行う
	bool hit = false;
	//左上点の判定
	IndexSet indexSet;
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapchipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	//右上点の判定
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapchipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	//ブロックにヒット？
	if (hit) {
	   //めり込みを排除する方向に移動量を設定する
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
        //めり込み先ブロックの範囲矩形
		Rect rect = mapchipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.moveAmount.y = std::max(
			0.0f,rect.bottom - worldTransform_.translation_.y 
			
			);
		//天井に当たったことを記録する
		info.ceilCollision = true;
	}
}

//マップ衝突判定下
void Player::MapBottomCollision(CollisionMapInfo& info) {
	std::array<Vector3, kNumCorner> positionNew; 
	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.moveAmount, static_cast<Corner>(i));
	}
}
//マップ衝突判定右
void Player::MapLightCollision(CollisionMapInfo& info) {
	std::array<Vector3, kNumCorner> positionNew;
	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.moveAmount, static_cast<Corner>(i));
	}
}
//マップ衝突判定左
void Player::MapLeftCollision(CollisionMapInfo& info) {
	std::array<Vector3, kNumCorner> positionNew;
	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.moveAmount, static_cast<Corner>(i));
	}
}

void Player::MapCollision(CollisionMapInfo& info) { 
	MapTopCollision(info);
	MapBottomCollision(info);
	MapLightCollision(info);
	MapLeftCollision(info);
}
//判定結果を反映して移動させる
void Player::ResultMove(const CollisionMapInfo& info) {
	//移動
	worldTransform_.translation_ += info.moveAmount;
}

// 天井に接触している場合の処理
void Player::CeilingContact(const CollisionMapInfo& info) {
	//天井に当たった？
	if (info.ceilCollision) {
		//DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;
	}
}