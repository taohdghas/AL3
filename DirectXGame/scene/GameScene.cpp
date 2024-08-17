#include "GameScene.h"
#include "AxisIndicator.h"
#include "TextureManager.h"
#include <cassert>
#include <fstream>

GameScene::GameScene() {}

GameScene::~GameScene() {
	// 3Dモデルデータの解放
	delete model_;
	// 自キャラの解放
	delete player_;
	// デバックカメラの開放
	delete debugCamera_;
	// 敵の解放
	for (Enemy* enemy : enemys_) {

		delete enemy;
	}
	// 敵の弾の解放
	for (EnemyBullet* enemyBullet : Enemybullets_) {
		delete enemyBullet;
	}
	// 天球の解放
	delete skydome_;
	// 3Dモデルの解放
	delete modelSkydome_;
	// レールカメラの解放
	delete railcamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("uvChecker.png");
	//  3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("sky", true);
	// 3Dモデルデータの生成
	model_ = Model::Create();
	//レティクルのテクスチャ
	TextureManager::Load("reticle.png");
	// ビュープロジェクションのfarZを適度に大きい値に変更する
	// viewProjection_.farZ = 1000;
	//  ビュープロジェクションの初期化
	viewProjection_.Initialize();
	// 自キャラの生成
	player_ = new Player();
	Vector3 playerPosition(0, 0, 50);
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, playerPosition);
	// デバックカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, textureHandle_);
	// レールカメラの生成
	railcamera_ = new RailCamera();
	// レールカメラの初期化
	railcamera_->Initialize(railPosition, railRotation);
	// 自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railcamera_->GetWorldtransform());
	LoadEnemyPopData();
}

void GameScene::Update() {
	UpdateEnemyPopCommands();
	// 自キャラの更新
	player_->Update(viewProjection_);

	// 敵の更新
	for (Enemy* enemy : enemys_) {
		enemy->Update();
	}

	// 敵の弾の更新
	for (EnemyBullet* enemyBullet : Enemybullets_) {
		enemyBullet->Update();
	}

	// デスフラグの立った敵の弾を削除
	Enemybullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// デスフラグの立った敵を削除
	enemys_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});

	// 天球の更新
	skydome_->Update();

	// レールカメラの更新
	railcamera_->Update();

	// 衝突判定
	CheckAllCollisions();

	// カメラの処理
	if (isDebugCameraActive_) {
		// デバックカメラの更新
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {

		railcamera_->Update();
		viewProjection_.matView = railcamera_->GetViewprojection().matView;
		viewProjection_.matProjection = railcamera_->GetViewprojection().matProjection;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	}
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_Q)) {
		if (!isDebugCameraActive_) {
			isDebugCameraActive_ = true;
		} else if (isDebugCameraActive_) {
			isDebugCameraActive_ = false;
		}
	}
#endif
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

	// 天球の描画
	skydome_->Draw(viewProjection_);
	// 自キャラの描画
	player_->Draw(viewProjection_);
	// 敵の描画
	for (Enemy* enemy : enemys_) {
		enemy->Draw(viewProjection_);
	}
	// 敵の弾の描画
	for (EnemyBullet* enemyBullet : Enemybullets_) {
		enemyBullet->Draw(viewProjection_);
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
	player_->DrawUI();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheckAllCollisions() {
	// 判定対象AとBの座標
	Vector3 posA, posB;

	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->Getbullets();
	// 敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = Enemybullets_;
#pragma region 自キャラと敵弾の当たり判定
	// 自キャラの座標
	posA = player_->GetWorldPosition();

	// 自キャラと敵弾全ての当たり判定
	for (EnemyBullet* bullet : enemyBullets) {
		// 敵弾の座標
		posB = bullet->GetWorldPosition();
		// 衝突判定
		float length = Length(posB - posA);
		float radius = EnemyBulletradius_ + Playerradius_;
		if (length <= radius) {
			// 自キャラの衝突時コールバックを呼び出す
			player_->OnCollision();
			// 敵弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}
#pragma endregion

#pragma region 自弾と敵キャラの当たり判定
	for (Enemy* enemy : enemys_) {
		for (PlayerBullet* bullet : playerBullets) {
			// 敵キャラの座標
			posA = enemy->GetWorldPosition();
			// 自弾の座標
			posB = bullet->GetWorldPosition();
			// 衝突判定
			float length = Length(posB - posA);
			float radius = PlayerBulletradius_ + Enemyradius_;
			if (length <= radius) {
				// 自弾の衝突時コールバックを呼び出す
				bullet->OnCollision();
				// 敵キャラの衝突時コールバックを呼び出す
				enemy->OnCollision();
			}
		}
	}
#pragma endregion

#pragma region 自弾と敵弾の当たり判定
	// 自弾と敵弾全ての当たり判定
	for (PlayerBullet* playerBullet : playerBullets) {
		for (EnemyBullet* enemyBullet : enemyBullets) {
			// 自弾の座標
			posA = playerBullet->GetWorldPosition();
			// 敵弾の座標
			posB = enemyBullet->GetWorldPosition();
			// 衝突判定
			float length = Length(posB - posA);
			float radius = PlayerBulletradius_ + EnemyBulletradius_;
			if (length <= radius) {
				// 自弾の衝突時コールバックを呼び出す
				playerBullet->OnCollision();
				// 敵弾の衝突時コールバックを呼び出す
				enemyBullet->OnCollision();
			}
		}
	}

#pragma endregion
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	// リストに登録する
	Enemybullets_.push_back(enemyBullet);
}

void GameScene::EnemyPop(Vector3 position) {
	// 敵の生成
	Enemy* newEnemy = new Enemy();
	// 敵キャラに自キャラのアドレスを渡す
	newEnemy->SetPlayer(player_);
	// 敵キャラにゲームシーンを渡す
	newEnemy->SetGameScene(this);
	// 敵の初期化
	newEnemy->Initialize(model_, position);
	enemys_.push_back(newEnemy);
}

void GameScene::LoadEnemyPopData() {
	// ファイルを開く
	std::ifstream file;
	file.open("Resources./enemyPop.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();
	// ファイルを閉じる
	file.close();
}

void GameScene::UpdateEnemyPopCommands() {
	//待機処理
	if (waitFlag) {
		waitTimer--;
		if (waitTimer <= 0) {
		//待機完了
			waitFlag = false;
		}
		return;
	}
	// 1行分の文字列を入れる変数
	std::string line;
	// コマンド実行ループ
	while (std::getline(enemyPopCommands, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);
		std::string word;
		//,区切りで行の先頭文字列を取得
		std::getline(line_stream, word, ',');
		//"//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}
		// POPコマンド
		if (word.find("POP") == 0) {
			// x座標
			std::getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());
			// y座標
			std::getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());
			// z座標
			std::getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());
			//敵を発生させる
			EnemyPop(Vector3(x, y, z));
		}
		//WAITコマンド
		else if (word.find("WAIT") == 0) {
			std::getline(line_stream, word, ',');
			//待ち時間
			int32_t waitTime = atoi(word.c_str());
			//待機時間
			waitFlag = true;
			waitTimer = waitTime;
			//コマンドループを抜ける
			break;
		}
	}
}