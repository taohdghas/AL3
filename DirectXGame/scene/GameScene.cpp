#include "GameScene.h"
#include "TextureManager.h"
#include "AxisIndicator.h"
#include "mymath.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete debugCamera_; 
	//ブロックの解放
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	textureHandle_ = TextureManager::Load("mario.png");
	//3Dモデルの生成
	//model_.reset(Model::CreateFromOBJ("player",true));
	model_.reset(Model::Create());
	//天球のモデル生成
	modelSkydome_.reset(Model::CreateFromOBJ("skydome", true));
	//地面のモデル生成
//	modelGround_.reset(Model::CreateFromOBJ("ground", true));
	//ブロックモデルの生成
	modelBlock_.reset(Model::CreateFromOBJ("cube", true));
	viewProjection_.Initialize();
	worldTransform_.Initialize();
	// デバックカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
	// マップチップフィールドの生成
	mapchipField_ = std::make_unique<MapChipField>();
	// マップチップ読み込み
	mapchipField_->LoadMapChipCsv("Resources/map.csv");
	//ブロックの基準位置
	Vector3 basePosition(0.0f, 0.0f, 0.0f);
	//座標をマップチップ番号で指定
	Vector3 playerPosition =mapchipField_->GetMapChipPositionByIndex(1,18);
	//自キャラの生成
	player_ = std::make_unique<Player>();
	//自キャラの初期化
	player_->Initialize(model_.get(),&viewProjection_,playerPosition);
	player_->SetMapChipField(mapchipField_.get());
	//天球の生成
	skydome_ = std::make_unique<Skydome>();
	//天球の初期化
	skydome_->Initialize(modelSkydome_.get(), &viewProjection_);
	/*
	//地面の生成
	ground_ = std::make_unique<Ground>();
	//地面の初期化
	ground_->Initialize(modelGround_.get(), &viewProjection_);
	*/
	//カメラコントローラの生成
	cameraContoroller_ = std::make_unique<CameraContoroller>();
	//カメラコントローラの初期化
	cameraContoroller_->Initialize();
	cameraContoroller_->SetTarget(player_.get());
	cameraContoroller_->Reset();
	//移動範囲指定
	movableArea_ = {100.0f, -20.0f, 5.0f,-5.0f};
	cameraContoroller_->SetMovableArea(movableArea_);
	//ブロック生成
	GenerateBlocks();
}

void GameScene::Update() {

	//自キャラの更新
	player_->Update();
	//天球の更新
	skydome_->Update();
	//カメラコントローラの更新
	cameraContoroller_->Update();
	//ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			// アフィン変換行列の作成
			worldTransformBlock->UpdateMatrix();
		}
	}

		// カメラの処理
	if (isDebugCameraActive_) {
		// デバックカメラの更新
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {
		viewProjection_.matView = cameraContoroller_->GetViewProjection().matView;
		viewProjection_.matProjection = cameraContoroller_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の更新と転送
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
	// 自キャラの描画
	player_->Draw();
	// 天球の描画
	skydome_->Draw(viewProjection_);
	//地面の描画
	//ground_->Draw(viewProjection_);
	//ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			modelBlock_->Draw(*worldTransformBlock, viewProjection_);
		}
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

//ブロックの生成
void GameScene::GenerateBlocks() {
	//初期座標
	Vector3 basePosition(0.0f, 0.0f, 0.0f);
	// 要素数
	uint32_t numBlockVirtical = mapchipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapchipField_->GetNumBlockHorizontal();
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
			if (mapchipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				//worldTransformBlocks_[i][j]->translation_ = mapchipField_->GetMapChipPositionByIndex(j, i);
				worldTransformBlocks_[i][j]->translation_ =  mapchipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

