#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "input.h"
#include "Vector3.h"

//左右
enum class LRDirection {
	kRight,
	kLeft,
};
//マップとの当たり判定情報
struct CollisionMapInfo {
	//天井衝突
	bool isCeil = false;
	//着地フラグ
	bool isLanding = false;
	//壁接触フラグ
	bool isWall = false;
	//移動量
	Vector3 Move;
};
//角
enum Corner {
	kRightBottom, // 右下
	kLeftBottom,  // 左下
	kRightTop,    // 右上
	kLeftTop,     // 左上

	kNumCorner // 要素数
};
class MapChipField;
class Player {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model*model,ViewProjection*viewProjection,const Vector3&position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void Move();

	void Rotate();

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	//マップ衝突判定
	void MapCollision(CollisionMapInfo& info);
	void MapTopCollision(CollisionMapInfo& info);
	void MapBottomCollision(CollisionMapInfo& info);
	void MapLightCollision(CollisionMapInfo& info);
	void MapLeftCollision(CollisionMapInfo& info);

	// 判定結果を反映して移動させる
	void ResultMove(const CollisionMapInfo& info);
    //天井に接触時の処理
	void IsCeil(const CollisionMapInfo& info);
	//接地状態の切り替え
	void ChangeGround(const CollisionMapInfo& info);

	void IsWall(const CollisionMapInfo& info);

	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	const Vector3& GetVelocity() const { return velocity_; }

	Vector3 CornerPosition(const Vector3& center, Corner corner);

private:
	//ワールド変換データ
	WorldTransform worldTransform_;
	//モデル
	Model* model_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	ViewProjection* viewProjection_ = nullptr;
	//速度
	Vector3 velocity_ = {};
	static inline const float kAcceleration = 0.1f;
	static inline const float kAttenuation = 0.3f;
	static inline const float kLimitRunSpeed = 1.0f;

	LRDirection lrDirection_ = LRDirection::kRight;
    //旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	//旋回タイマー
	float turnTimer_ = 0.0f;
	//旋回時間
	static inline const float kTimeTurn = 0.3f;
	//接地状態フラグ
	bool onGround_ = true;
	//重力加速度
	static inline const float kGravityAcceleration = 0.1f;
	//最大落下速度
	static inline const float kLimitFallSpeed = 1.0f;
	//ジャンプ初速
	static inline const float kJumpAcceleration = 0.9f;
	//マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;
	//キャラクターの当たり判定サイズ
	static inline const float kWidth = 1.0f;
	static inline const float kHeight = 1.0f;

	static inline const float kBlank = 1.0f;
	//着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.1f;
	//微小な数値
	static inline const float kGroundHeight = 0.1f;
	static inline const float kAttenuationWall = 0.1f;
};
