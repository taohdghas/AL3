#pragma once

#include "Audio.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "Enemy.h"
#include "Input.h"
#include "Model.h"
#include "Player.h"
#include "RailCamera.h"
#include "Skydome.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <sstream>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();

	void AddEnemyBullet(EnemyBullet* enemyBullet);

	void EnemyPop(Vector3 positon);

	/// 敵発生データの読み込み
	void LoadEnemyPopData();

	//敵発生コマンドの更新
	void UpdateEnemyPopCommands();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	uint32_t EnemytextureHandle_ = 0;
	// 3Dモデルデータ
	Model* model_ = nullptr;
	// ビュープロジェクション
	ViewProjection viewProjection_;
	// 自キャラ
	Player* player_ = nullptr;
	Enemy* enemy_ = nullptr;
	// 自キャラの半径
	float Playerradius_ = 1.0f;
	// 自弾の半径
	float PlayerBulletradius_ = 1.0f;
	// デバックカメラ有効
	bool isDebugCameraActive_ = false;
	// デバックカメラ
	DebugCamera* debugCamera_ = nullptr;
	// 敵
	std::list<Enemy*> enemys_;
	// 敵の座標
	Vector3 Position = {2.0f, 0.0f, 60.0f};
	// 敵の速度
	Vector3 Velocity_ = {0, 0, -0.1f};
	// 敵の離脱時速度
	Vector3 LeaveVelo_ = {-1.0f, 1.0f, 0.0f};
	// 敵の半径
	float Enemyradius_ = 1.0f;
	// 弾
	std::list<EnemyBullet*> Enemybullets_;
	// 敵の弾の半径
	float EnemyBulletradius_ = 1.0f;
	// 天球
	Skydome* skydome_ = nullptr;
	// 3Dモデル
	Model* modelSkydome_ = nullptr;
	// レールカメラ
	RailCamera* railcamera_ = nullptr;
	// レールカメラの座標
	Vector3 railPosition = {0.0f, 0.0f, -100.0f};
	// レールカメラ回転
	Vector3 railRotation = {0.0f, 0.0f, 0.0f};
	// 敵発生コマンド
	std::stringstream enemyPopCommands;
	//待機中フラグ
	bool waitFlag = false;
	//待機タイマー
	int32_t waitTimer = 0;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
