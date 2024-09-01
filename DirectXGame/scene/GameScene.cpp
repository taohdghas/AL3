#include "GameScene.h"
#include "CameraController.h"
#include "TextureManager.h"
#include "WorldTransform.h"
#include "aabb.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete modelBlocks_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
	delete debugCamera_;
	delete modelEnemy_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	delete modelParticles_;
	delete deathParticles_;
	delete modelSkydome_;
	delete player_;
	delete skydome_;
	delete mapChipField_;
	delete cameraController_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	modelBlocks_ = Model::CreateFromOBJ("block", true);
	viewProjection_.Initialize();
	// マップチップフィールドの生成
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/map.csv");
	// 自キャラの生成
	player_ = new Player();
	// 自キャラの生成(モデル)
	modelPlayer_ = Model::CreateFromOBJ("player", true);
	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(3, 17);
	// 自キャラの初期化
	player_->Initialize(modelPlayer_, &viewProjection_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	// 敵モデル
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);
	// 敵の生成
	Vector3 enemyPositions[] = {
	    {24.0f, 1.0f, 0.0f},
        {30.0f, -21.0f, 0.0f},
        {50.0f, -21.0f, 0.0f},
        {60.0f, -21.0f, 0.0f},
        {80.0f, -21.0f, 0.0f}
    };
	int32_t numEnemies = 5; 
	for (int32_t i = 0; i < numEnemies; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = enemyPositions[i]; // 配列から座標を取得
		newEnemy->Initialize(modelEnemy_, &viewProjection_, enemyPosition, mapChipField_);
		enemies_.push_back(newEnemy);
	}


	// パーティクルモデル
	modelParticles_ = Model::CreateFromOBJ("deathParticle", true);
	// 仮の生成
	deathParticles_ = new DeathParticles;
	deathParticles_->Initialize(modelParticles_, &viewProjection_, playerPosition);

	//  3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &viewProjection_);
	// デバックカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	GenerateBlocks();
	// カメラコントローラの初期化
	cameraController_ = new CameraController();
	cameraController_->Initialize(&viewProjection_, movableArea);
	cameraController_->SetTarget(player_);
	cameraController_->Reset();

	// ゲームプレイフェーズから開始
	phase_ = Phase::kPlay;
}

void GameScene::Update() {
	ChangePhase();
	// デバックカメラの更新
	debugCamera_->Update();
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_SPACE)) {
		if (isDebugCameraActive_ == false) {
			isDebugCameraActive_ = true;
		} else {
			isDebugCameraActive_ = false;
		}
	}
#endif
	switch (phase_) {
	case Phase::kPlay:
		// ゲームプレイフェーズの処理

		//  天球の更新
		skydome_->Update();
		// 自キャラの更新
		player_->Update();
		// プレイヤーの位置を取得
		Vector3 playerPosition = player_->GetWorldPosition();
		if (playerPosition.x >= 100.0f) {
			finished_ = true; 
		}
		// プレイヤーが特定の位置に来たかをチェック
		if (!enemySpawned_ && playerPosition.x > 20.0f && playerPosition.x < 25.0f && playerPosition.y > 12.5f && playerPosition.y < 16.0f) {
			// 新しい敵を生成
			Enemy* newEnemy = new Enemy();
			Vector3 enemySpawnPosition = {26.0f, 18.0f, 0.0f}; // スポーン位置を指定
			newEnemy->Initialize(modelEnemy_, &viewProjection_, enemySpawnPosition, mapChipField_);
			enemies_.push_back(newEnemy);
			enemySpawned_ = true;
		}
		if (!enemySpawned_2 && playerPosition.x > 50.0f && playerPosition.x < 52.0f) {
			// 新しい敵を生成
			Enemy* newEnemy = new Enemy();
			Vector3 enemySpawnPosition = {65.0f, 15.0f, 0.0f}; // スポーン位置を指定
			newEnemy->Initialize(modelEnemy_, &viewProjection_, enemySpawnPosition, mapChipField_);
			enemies_.push_back(newEnemy);
			enemySpawned_2 = true;
		}
		if (!enemySpawned_3 && playerPosition.x > 85.0f && playerPosition.x < 90.0f) {
			// 新しい敵を生成
			Enemy* newEnemy = new Enemy();
			Vector3 enemySpawnPosition = {89.0f, 11.0f, 0.0f}; // スポーン位置を指定
			newEnemy->Initialize(modelEnemy_, &viewProjection_, enemySpawnPosition, mapChipField_);
			enemies_.push_back(newEnemy);
			enemySpawned_3 = true;
		}
		if (!enemySpawned_4 && playerPosition.x > 85.0f && playerPosition.x < 90.0f) {
			// 新しい敵を生成
			Enemy* newEnemy = new Enemy();
			Vector3 enemySpawnPosition = {93.0f, 11.0f, 0.0f}; // スポーン位置を指定
			newEnemy->Initialize(modelEnemy_, &viewProjection_, enemySpawnPosition, mapChipField_);
			enemies_.push_back(newEnemy);
			enemySpawned_4 = true;
		}
		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}
		// 削除フラグが立っている敵を削除する処理を追加
		enemies_.erase(
		    std::remove_if(
		        enemies_.begin(), enemies_.end(),
		        [](Enemy* enemy) {
			        if (enemy->IsDead()) {
				        delete enemy;
				        return true;
			        }
			        return false;
		        }),
		    enemies_.end());
		// カメラコントローラの更新
		cameraController_->Update();
		// カメラの処理
		if (isDebugCameraActive_) {
			debugCamera_->Update();
			viewProjection_.matView = debugCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の転送
			viewProjection_.TransferMatrix();
		} else {
			// ビュープロジェクション行列の更新と転送
			viewProjection_.UpdateMatrix();
		}
		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				// アフィン変換行列の作成
				worldTransformBlock->UpdateMatrix();
			}
		}
		// 全ての当たり判定
		CheckAllCollisions();
		break;

	case Phase::kDeath:
		//デス演出フェーズの処理
		//   天球の更新
		skydome_->Update();
		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}
		// パーティクルの更新
		if (deathParticles_) {
			deathParticles_->Update();
		}
		// カメラの処理
		if (isDebugCameraActive_) {
			debugCamera_->Update();
			viewProjection_.matView = debugCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の転送
			viewProjection_.TransferMatrix();
		} else {
			// ビュープロジェクション行列の更新と転送
			viewProjection_.UpdateMatrix();
		}
		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				// アフィン変換行列の作成
				worldTransformBlock->UpdateMatrix();
			}
		}

		break;
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// ブロックの描画
	switch (phase_) {
	case Phase::kPlay:// 自キャラの描画
		player_->Draw();
		break;
	case Phase::kDeath:
		break;
	}
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			modelBlocks_->Draw(*worldTransformBlock, viewProjection_);
		}
	}
	// 天球の描画
	skydome_->Draw();
	
	// 敵の描画
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}
	
		// パーティクル描画
		if (deathParticles_) {
			deathParticles_->Draw();
		}
	
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
void GameScene::GenerateBlocks() {
	{
		// 要素数
		uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
		uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();
		// 要素数を変更する
		// 列数を設定(縦方向のブロック数)
		worldTransformBlocks_.resize(20);
		for (uint32_t i = 0; i < 20; ++i) {
			// 一列の要素数を設定(横ブロック数)
			worldTransformBlocks_[i].resize(numBlockHorizontal);
		}
		// ブロックの生成
		for (uint32_t i = 0; i < numBlockVirtical; ++i) {
			for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
				if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
					WorldTransform* worldTransform = new WorldTransform();
					worldTransform->Initialize();
					worldTransformBlocks_[i][j] = worldTransform;
					worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
				}
			}
		}
	}
}

// 全ての当たり判定
void GameScene::CheckAllCollisions() {
#pragma region 自キャラと敵キャラの当たり判定
	// 判定対象1と2の座標
	AABB aabb1, aabb2;
	// 自キャラの座標
	aabb1 = player_->GetAABB();
	// 自キャラと敵弾全ての当たり判定
	for (Enemy* enemy : enemies_) {
		/// 敵弾の座標
		aabb2 = enemy->GetAABB();
		if (AABB::IsCollision(aabb1, aabb2)) {
			// 自キャラの衝突時コールバックを呼び出す
			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}
#pragma endregion
}

//フェーズ切り替え関数
void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kPlay:
		if (player_->IsDead()) {
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;
			// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();
			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(modelParticles_, &viewProjection_, deathParticlesPosition);
		}
		break;
	case Phase::kDeath:
		// デス演出フェーズの処理
		if (deathParticles_ && deathParticles_->IsFinished()) {
			finished_ = true;
		}
		break;
	}
}