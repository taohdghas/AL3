#pragma once
#define NOMINMAX
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include <numbers>
#include "Input.h"
#include <algorithm>
#include <cassert>
/// <summary>
/// 自キャラ
/// </summary>
class MapChipField;

//左右
enum class LRDirection {
	kRight,
	kLeft,
};
//角
enum Corner {
	kRightBottom,//右下
	kLeftBottom,//左下
	kRightTop,//右上
	kLeftTop,//左上

	kNumCorner//要素数
};
//マップとの当たり判定情報
struct CollisionMapInfo {
	//天井衝突フラグ
	bool ceilCollision = false;
	//着地フラグ
	bool onLanding = false;
	//壁接触フラグ
	bool wallContact = false;
	//移動量
	Vector3 moveAmount;
};
class Player {
public:
	Player();

	~Player();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, ViewProjection* viewProjection,const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	const WorldTransform& GetWorldTransform() { return worldTransform_; };
	const Vector3& Getvelocity() const { return velocity_; }
	void SetMapChipField(MapChipField* mapChipField) { mapchipField_ = mapChipField; }
	//移動関数
	void move();
	//マップ衝突判定関数
	void MapCollision(CollisionMapInfo& info);
	void MapTopCollision(CollisionMapInfo& info);
	void MapBottomCollision(CollisionMapInfo& info);
	void MapLightCollision(CollisionMapInfo& info);
	void MapLeftCollision(CollisionMapInfo& info);
	//判定結果を反映して移動させる
	void ResultMove(const CollisionMapInfo& info);
	//天井に接触している場合の処理
	void CeilingContact(const CollisionMapInfo& info);
	Vector3 CornerPosition(const Vector3& center, Corner corner);

private:
	static inline const float kAcceleration = 0.1f;
	static inline const float kAttenuation = 0.5f;
	static inline const float kLimitRunSpeed = 1.0f;
	//旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;
	//重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.1f;
	//最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 1.0f;
	//ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 1.0f;
	static inline const float kAttenuationLanding = 0.5f;

	static inline const float kBlank = 1.0f;

	//キャラクターの当たり判定サイズ
	static inline const float kWidth = 1.0f;
	static inline const float kHeight = 1.0f;
	
	//旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	//旋回タイマー
	float turnTimer_ = 0.0f;
	//接地状態フラグ
	bool onGround_ = true;
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// ビュープロジェクション
	ViewProjection* viewProjection_ = nullptr;
	Vector3 velocity_ = {};
	LRDirection lrDirection_ = LRDirection::kRight;
	//マップチップによるフィールド
	MapChipField* mapchipField_ = nullptr;
};