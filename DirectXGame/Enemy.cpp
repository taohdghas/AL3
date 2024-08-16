#include "Enemy.h"
#include "Player.h"
#include "GameScene.h"

Enemy::Enemy() {}

Enemy::~Enemy() {
	
}

void Enemy::Initialize(Model* model,const Vector3&position) {
	// NULLポインタチェック
	assert(model);
	model_ = model;
	textureHandle_ = TextureManager::Load("red1x1.png");
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	velocity_ = {0, 0, -0.1f};
	LeaveVelo_ = {-1.0f, 1.0f, 0.0f};
	//弾を発射
	//Fire();
	//接近フェーズ初期化
	ApproachReset();
}

void Enemy::Update() {

	switch (phase_) { 
	case Phase::Approach:
	default:
		Approach();
		break;
	case Phase::Leave:
		Leave();
		break;
	}

	//移動
	//worldTransform_.translation_ = Add(worldTransform_.translation_,velocity_ );

	worldTransform_.UpdateMatrix(); 
}

void Enemy::Draw(const ViewProjection& viewProjection) { 
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
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
	// 移動(ベクトルを加算)
	worldTransform_.translation_ = Add(worldTransform_.translation_,LeaveVelo_);
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

	gameScene_->AddEnemyBullet(newBullet);
}

//接近フェーズ初期化
void Enemy::ApproachReset() {
	//発射タイマーを初期化
	fireTimer = kFireInterval;
}

//衝突時コールバック
void Enemy::OnCollision() { isDead_ = true; }

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