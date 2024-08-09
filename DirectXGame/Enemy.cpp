#include "Enemy.h"
#include "Player.h"

Enemy::Enemy() {}

Enemy::~Enemy() {
	// bullet_の解放
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Enemy::Initialize(Model* model, uint32_t textureHandle,const Vector3& velocity) {
	// NULLポインタチェック
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = {3.0f, 1.0f, 60.0f};
	velocity_ = velocity;
	//弾を発射
	Fire();
	//接近フェーズ初期化
	ApproachReset();
}

void Enemy::Update() {
	// デスフラグの立った弾を削除
	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
	switch (phase_) { 
	case Phase::Approach:
	default:
		Approach();
		break;
	case Phase::Leave:
		Leave();
		break;
	}

    // 弾更新
	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}

	//移動
	worldTransform_.translation_ = Add(worldTransform_.translation_,velocity_ );

	worldTransform_.UpdateMatrix(); 
}

void Enemy::Draw(const ViewProjection& viewProjection) { 
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
	// 弾の描画
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}

//接近フェーズ
void Enemy::Approach() {
	//発射タイマーカウントダウン
	fireTimer--;
	//指定時間に達した
	if (fireTimer <= 0) {
	//弾を発射
		Fire();
		//発射タイマーを初期化
		fireTimer = kFireInterval;
	}
	// 移動(ベクトルを加算)
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);
	// 既定の位置に到達したら離脱
	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}
}

//離脱フェーズ
void Enemy::Leave() {
	velocity_ = {-0.1f, 0.1f, 0.0f};
	// 移動(ベクトルを加算)
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);
}

void Enemy::Fire() {
     assert(player_);
	// 弾の速度
	const float kBulletSpeed = 1.0f;

	Vector3 distance = player_->GetWorldPosition() -worldTransform_.translation_;

    distance = Normalize(distance);

	Vector3 velocity = {Multiply( kBulletSpeed,distance)};

	// 速度ベクトルを自機の向きに合わせて回転させる
	velocity = TransformNormal(velocity, worldTransform_.matWorld_);

	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);

	// 弾を登録する
	bullets_.push_back(newBullet);
}

//接近フェーズ初期化
void Enemy::ApproachReset() {
	//発射タイマーを初期化
	fireTimer = kFireInterval;
}

//ワールド座標を取得
Vector3 Enemy::GetWorldPosition() {
	//ワールド座標を入れる変数
	Vector3 worldPos;
	//ワールド行列の平行移動成分を取得
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}